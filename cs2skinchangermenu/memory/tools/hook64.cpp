#include "pch.h"
#include "hook64.h"
#include "hde64.h"

#include <cstdint>
#include <cmath>
#include <memory>

BYTE absJmpNoRegister[] = {
	0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp [rip] 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // 0x???????? <- rip already points here, thus an absolute jump will still occur correctly
};

// simulates a call instruction
BYTE absCall[] = {
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, 0x00 00 00 00 00 00 00 00 
	0xFF, 0xD0 // call rax
};

BYTE destPrepPrologue[] = {
	// start storing registers and original return address somewhere up on our stack
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK-8 <- int, -8 since we want to store the return address exactly at offset OFFSET_STORE_REGS_AND_RETADDR_ON_STACK
	0xFF, 0xB4, 0x24, 0x00, 0x00, 0x00, 0x00, // push qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK-8] <- -8 due to some weird rsp updating shit
	0x50, // push rax
	0x53, // push rbx
	0x51, // push rcx
	0x52, // push rdx
	0x56, // push rsi
	0x57, // push rdi
	0x55, // push rbp
	0x41, 0x50, // push r8
	0x41, 0x51, // push r9
	0x41, 0x52, // push r10
	0x41, 0x53, // push r11
	0x41, 0x54, // push r12
	0x41, 0x55, // push r13
	0x41, 0x56, // push r14
	0x41, 0x57, // push r15

	// replace the return address with our gateway epilogue start addy
	0x48, 0x81, 0xC4, 0x00, 0x00, 0x00, 0x00, // add rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS*8 <- int
	0x50, // push rax
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, // movabs rax, 0x0000000000000000
	0x48, 0x89, 0x44, 0x24, 0x08, // mov [rsp+8], rax
	0x58 // pop rax
};

BYTE destEpilogue[] = {
	// rsp will be higher by 8 bytes now due to ret, so we will compensate for it

	// restore the original ret addr
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, 8 + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <- int
	0x48, 0x8B, 0x04, 0x24, // mov rax, [rsp]
	0x48, 0x89, 0x84, 0x24, 0x00, 0x00, 0x00, 0x00, // mov qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK], rax
	// restore registers
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, NUM_STORED_REGISTERS*8 <- int
	0x41, 0x5F, // pop r15
	0x41, 0x5E, // pop r14
	0x41, 0x5D, // pop r13
	0x41, 0x5C, // pop r12
	0x41, 0x5B, // pop r11
	0x41, 0x5A, // pop r10
	0x41, 0x59, // pop r9
	0x41, 0x58, // pop r8
	0x5D, // pop rbp
	0x5F, // pop rdi
	0x5E, // pop rsi
	0x5A, // pop rdx
	0x59, // pop rcx
	0x5B, // pop rbx
	0x58, // pop rax

	// put stack pointer to the original return address
	0x48, 0x81, 0xC4, 0x00, 0x00, 0x00, 0x10 // add rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK
};

BYTE hookPageEnter[] = {
	// add 1 to our gateway page accessor count
	0xF0, 0x48, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, // lock inc qword ptr [rip+MAX_GATEWAY_SIZE_BYTES-8] <- rip will point to next instruction 8 bytes away, adding MAX_GATEWAY_SIZE_BYTES will make it not aligned correctly at the gateway size addy
};

BYTE hookPageLeave[] = {
	0xF0, 0x48, 0xFF, 0x0D, 0x00, 0x00, 0x00, 0x00 // lock dec qword ptr [rip + (offset to gateway page accessor count)]
};

static_assert(sizeof(destPrepPrologue) + sizeof(absCall) + sizeof(destEpilogue) + MAX_ORIG_INSTRUCTIONS + sizeof(hookPageLeave) + sizeof(absJmpNoRegister) <= MAX_GATEWAY_SIZE_BYTES, "increase gateway size");
static_assert(MAX_GATEWAY_SIZE_BYTES <= INT32_MAX, "gateway size too large");
static_assert(OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <= INT32_MAX, "stack original return address storage offset too large, you will point out of stack");

Hook::Hook(BYTE* targetJmpPlacementLoc, BYTE* hookGatewayLoc, BYTE targetOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int numOriginalOpcodes) {
	this->targetJmpPlacementLoc = targetJmpPlacementLoc;
	this->hookGatewayLoc = hookGatewayLoc;

	// target original opcodes
	memcpy(this->hookTargetOriginalOpcodes, targetOriginalOpcodes, numOriginalOpcodes);
	this->numOriginalOpcodes = numOriginalOpcodes;
}

Hook::~Hook() {
	this->Delete();
}

// TODO:
void Hook::Enable() {
	DWORD oldProtect;
	VirtualProtect(this->targetJmpPlacementLoc, this->numOriginalOpcodes, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(this->targetJmpPlacementLoc, absJmpNoRegister, sizeof(absJmpNoRegister));
	// write jmp address
	*reinterpret_cast<BYTE**>(this->targetJmpPlacementLoc + 6) = this->hookGatewayLoc;
	VirtualProtect(this->targetJmpPlacementLoc, this->numOriginalOpcodes, oldProtect, &oldProtect);
}

// TODO:
void Hook::Disable() {
	DWORD oldProtect;
	VirtualProtect(this->targetJmpPlacementLoc, this->numOriginalOpcodes, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(this->targetJmpPlacementLoc, this->hookTargetOriginalOpcodes, this->numOriginalOpcodes);
	VirtualProtect(this->targetJmpPlacementLoc, this->numOriginalOpcodes, oldProtect, nullptr);
}

bool Hook::Delete() {
	this->Disable();

	while (*reinterpret_cast<uint64_t*>(this->hookGatewayLoc + MAX_GATEWAY_SIZE_BYTES) != 0)
		continue;

	return VirtualFree(this->hookGatewayLoc, 0, MEM_RELEASE);
}

/*
* Formats the destination call prologue instructions to reset the manual stack alignment bool and if needed, restore the stack to is previous non 16-byte alignment.
* @param opcodeStorage Location to store the formatted instructions at
* @param gateway Pointer to the start of the gateway
*/
void FormatDestinationPrologue(BYTE* opcodeStorage, BYTE* gateway) {
	memcpy(opcodeStorage, destPrepPrologue, sizeof(destPrepPrologue));

	*reinterpret_cast<int*>(opcodeStorage + 3) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK - 8; // RSP offset |  sub rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK-8 <- int
	*reinterpret_cast<int*>(opcodeStorage + 10) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK - 8; // RSP offset to original ret addr | push qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK-8]

	*reinterpret_cast<int*>(opcodeStorage + 40) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS * 8; // RSP point stack to original loc, +8 since the last push will offset the register by 8 too high of the original pos | add rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS*8 <- int

	*reinterpret_cast<BYTE**>(opcodeStorage + 47) = gateway + sizeof(hookPageEnter) + sizeof(destPrepPrologue) + sizeof(absJmpNoRegister);
}

/*
* Formats the destination call epilogue instructions needed to align the stack and copy the original contents of it, so that our destination function doesn't overwrite something important.
* @param opcodeStorage Location to store the formatted instructions at
*/
void FormatDestinationEpilogue(BYTE* opcodeStorage) {
	memcpy(opcodeStorage, destEpilogue, sizeof(destEpilogue));

	*reinterpret_cast<int*>(opcodeStorage + 3) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + 8; // RSP offset to top of stack for our stored original ret addr | sub rsp, 8 + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <- int
	*reinterpret_cast<int*>(opcodeStorage + 15) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK; // RSP offset to the original/modified return address | mov qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK], rax

	*reinterpret_cast<int*>(opcodeStorage + 22) = NUM_STORED_REGISTERS * 8; // RSP offset to our preserved regs | mov qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK], rax
	*reinterpret_cast<int*>(opcodeStorage + 52) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK; // RSP point stack to original retaddr loc | add rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS*8 + 8 <- int

}

/*
* Formats absolute jmp code to jump to our desired address
* @param opcodeStorage Location to store the formatted instructions at
* @param desiredJmpAddress Pointer to our desired jmp location
*/
void FormatAbsoluteJmpCode(BYTE* opcodeStorage, BYTE* desiredJmpAddress) {
	memcpy(opcodeStorage, absJmpNoRegister, sizeof(absJmpNoRegister));

	*reinterpret_cast<BYTE**>(opcodeStorage + 6) = desiredJmpAddress;
}

/*
* Formats absolute jmp code to jump to our desired address
* @param opcodeStorage Location to store the formatted instructions at
* @param desiredCallAddress Pointer to our desired call location
*/
void FormatAbsoluteCallCode(BYTE* opcodeStorage, BYTE* desiredCallAddress) {
	memcpy(opcodeStorage, absCall, sizeof(absCall));

	*reinterpret_cast<BYTE**>(opcodeStorage + 2) = desiredCallAddress;
}

/*
* Formats hook page enter code to add 1 to the amount of current gateway memory page accessors.
* @param opcodeStorage Location to store the formatted instructions at
*/
void FormatHookPageEnter(BYTE* opcodeStorage) {
	memcpy(opcodeStorage, hookPageEnter, sizeof(hookPageEnter));

	*reinterpret_cast<int*>(opcodeStorage + 4) = MAX_GATEWAY_SIZE_BYTES - 8; // RIP offset | add qword ptr [rip+MAX_GATEWAY_SIZE_BYTES-8]
}

/*
* Formats hook page leave code to subtract 1 from the amount of current gateway memory page accessors.
* @param opcodeStorage Location to store the formatted instructions at
* @param offset Offset to the end of the gateway, pointing to address gateway+MAX_GATEWAY_SIZE_BYTES
*/
void FormatHookPageLeave(BYTE* opcodeStorage, uint64_t offset) {
	memcpy(opcodeStorage, hookPageLeave, sizeof(hookPageLeave));

	*reinterpret_cast<int*>(opcodeStorage + 4) = MAX_GATEWAY_SIZE_BYTES - offset - 8; // -8 since rip already points to next instruction, it wont store it at the correct offset
}

// Checks whether an instruction inside our desired hook area is a relative JMP or CALL. If so, returns true, since we would have to recalculate those CALL and JMP destinations.
bool DoesInstructionNeedRecalculating(BYTE* instructionPtr) {
	// excludes relative JMP and CALL
	if (0xE8 <= *instructionPtr && *instructionPtr <= 0xEB)
		return true;

	// relative JO, JNE etc.. instructions
	if (0x70 <= *instructionPtr && *instructionPtr <= 0x7F)
		return true;

	// another type of relative JNE, JGE etc...
	if (*instructionPtr == 0x0F && 0x80 <= *(instructionPtr + 1) && *(instructionPtr + 1) <= 0x8F)
		return true;

	return false;
}

/*
* Returns the amount of bytes we would need to jump over, if we were to replace them with an absJmpNoRegister, such that we don't get invalid/incorrect instructions.
*
* @param targetFunc Pointer to the start of our desired function.
* 
* @return Amount of bytes we need to jump from the function start to not get invalid instructions.
*/
int AbsJmpNumBytesToSave(BYTE* targetFunc) {
	int currentOffset = 0;

	// find an offset that is suitable to avoid corrupting asm instructions
	while (currentOffset < sizeof(absJmpNoRegister)) {
		hde64s hdeState = { 0 };
		int size = hde64_disasm(targetFunc, &hdeState);
		currentOffset += size;
		targetFunc += size;

		if (hdeState.flags & F_ERROR)
			return -1;
	}

	return currentOffset;
}


// Creates a hook inside our target function without interfereing with the target function's work.
// This means that even though our destination function will receive the target function's arguments, we will not be able to modify anything inside of the target function (including return value etc)
std::unique_ptr<Hook> CreateTrampHook64_Advanced(BYTE* targetFunc, BYTE* destinationFunc) {
	int offsetValidOpcode = AbsJmpNumBytesToSave(targetFunc);
	if (offsetValidOpcode == -1)
		return nullptr;

	BYTE* hookStart = targetFunc;
	BYTE* hookEnd = targetFunc + offsetValidOpcode;
	uint64_t numOriginalOpcodes = hookEnd - hookStart;

	// copy target func opcodes
	BYTE originalOpcodes[MAX_ORIG_INSTRUCTIONS] = { 0 };
	memcpy(originalOpcodes, hookStart, numOriginalOpcodes);

	// allocate enough memory to preserve all registers
	// call our hook func, pop all registers and return
	// to our target func at the **appropriate** location
	BYTE* gateway = static_cast<BYTE*>(VirtualAlloc(0, MAX_GATEWAY_SIZE_BYTES+16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)); // will still allocate a whole page
	if (!gateway)
		return nullptr;

	// start copying instructions to our gateway
	BYTE* gatewayInstructionPtr = gateway;

	// 1. hook page enter code
	BYTE hookEnterOpcodes[sizeof(hookPageEnter)] = { 0 };
	FormatHookPageEnter(hookEnterOpcodes);
	memcpy(gatewayInstructionPtr, hookEnterOpcodes, sizeof(hookEnterOpcodes));
	gatewayInstructionPtr += sizeof(hookEnterOpcodes);

	// 2. destination call preparation, prologue
	BYTE destPrologueOpcodes[sizeof(destPrepPrologue)] = { 0 };
	FormatDestinationPrologue(destPrologueOpcodes, gateway);
	memcpy(gatewayInstructionPtr, destPrologueOpcodes, sizeof(destPrologueOpcodes));
	gatewayInstructionPtr += sizeof(destPrologueOpcodes);

	// 3. call destination func
	BYTE jmpDestOpcodes[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(jmpDestOpcodes, destinationFunc);
	memcpy(gatewayInstructionPtr, jmpDestOpcodes, sizeof(jmpDestOpcodes));
	gatewayInstructionPtr += sizeof(jmpDestOpcodes);

	// 4. destination call cleanup, epilogue
	BYTE epilogueOpcodes[sizeof(destEpilogue)] = { 0 };
	FormatDestinationEpilogue(epilogueOpcodes);
	memcpy(gatewayInstructionPtr, epilogueOpcodes, sizeof(epilogueOpcodes));
	gatewayInstructionPtr += sizeof(epilogueOpcodes);

	// 5. execute original opcodes
	memcpy(gatewayInstructionPtr, originalOpcodes, numOriginalOpcodes);
	gatewayInstructionPtr += numOriginalOpcodes;

	// 6. hook page leave code
	BYTE hookLeaveOpcodes[sizeof(hookPageLeave)] = { 0 };
	FormatHookPageLeave(hookLeaveOpcodes, gatewayInstructionPtr - gateway);
	memcpy(gatewayInstructionPtr, hookLeaveOpcodes, sizeof(hookLeaveOpcodes));
	gatewayInstructionPtr += sizeof(hookLeaveOpcodes);

	// 7. jmp to our target after our jmp instruction
	BYTE jmpOpcodes[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(jmpOpcodes, hookEnd);
	memcpy(gatewayInstructionPtr, jmpOpcodes, sizeof(jmpOpcodes));
	gatewayInstructionPtr += sizeof(jmpOpcodes);

	return std::make_unique<Hook>(hookStart, gateway, originalOpcodes, numOriginalOpcodes);
}

// Creates a classic trampoline hook from our target function to our destination function. Return values can be modified.
std::unique_ptr<Hook> CreateTrampHook64(BYTE* targetFunc, BYTE* destinationFunc, BYTE** pCallOriginalFunc) {
	int offsetValidOpcode = AbsJmpNumBytesToSave(targetFunc);
	if (offsetValidOpcode == -1)
		return nullptr;

	BYTE* hookStart = targetFunc;
	BYTE* hookEnd = targetFunc + offsetValidOpcode;
	uint64_t numOriginalOpcodes = hookEnd - hookStart;

	// copy target func opcodes
	BYTE originalInstructions[MAX_ORIG_INSTRUCTIONS] = { 0 };
	memcpy(originalInstructions, hookStart, numOriginalOpcodes);

	BYTE* gateway = static_cast<BYTE*>(VirtualAlloc(0, MAX_GATEWAY_SIZE_BYTES, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)); // will still allocate a whole page
	if (!gateway)
		return nullptr;

	// start writing opcodes to gateway
	BYTE* gatewayInstructionPtr = gateway;
	// 1. increase current page accessor count
	BYTE hookEnterOpcodes[sizeof(hookPageEnter)] = { 0 };
	FormatHookPageEnter(hookEnterOpcodes);
	memcpy(gatewayInstructionPtr, hookEnterOpcodes, sizeof(hookEnterOpcodes));
	gatewayInstructionPtr += sizeof(hookEnterOpcodes);

	// 2. jmp to our own function
	BYTE jmpToDest[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(jmpToDest, destinationFunc);
	memcpy(gatewayInstructionPtr, absJmpNoRegister, sizeof(absJmpNoRegister));
	*reinterpret_cast<BYTE**>(gatewayInstructionPtr + 6) = destinationFunc;
	gatewayInstructionPtr += sizeof(absJmpNoRegister);

	// 3. padding
	int paddingSize = 16 - (reinterpret_cast<uint64_t>(gatewayInstructionPtr) & 0xF);
	if (paddingSize != 16) {
		memset(gatewayInstructionPtr, 0xCC, paddingSize);
		gatewayInstructionPtr += paddingSize;
	}

	// 4. function prologue for our hooked function
	if (pCallOriginalFunc)
		*pCallOriginalFunc = gatewayInstructionPtr;
	
	memcpy(gatewayInstructionPtr, originalInstructions, numOriginalOpcodes);
	gatewayInstructionPtr += numOriginalOpcodes;
	// 5. hook page leave code
	BYTE hookLeaveOpcodes[sizeof(hookPageLeave)] = { 0 };
	FormatHookPageLeave(hookLeaveOpcodes, gatewayInstructionPtr - gateway);

	// 6. jmp to the next instruction after our hook bytes in the target func
	BYTE jmpBack[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(jmpBack, hookEnd);
	memcpy(gatewayInstructionPtr, jmpBack, sizeof(jmpBack));
	gatewayInstructionPtr += sizeof(jmpBack);

	// all done, now initialize our hook
	return std::make_unique<Hook>(hookStart, gateway, originalInstructions, numOriginalOpcodes);
}