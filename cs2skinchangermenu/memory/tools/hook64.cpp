#include "pch.h"
#include "hook64.h"
#include "hde64.h"

#include <cstdint>
#include <cmath>
#include <memory>

static_assert(sizeof(destPrepPrologue) + sizeof(absCall) + sizeof(destEpilogue) + MAX_ORIG_INSTRUCTIONS + sizeof(hookPageLeave) + sizeof(absJmpNoRegister) <= MAX_GATEWAY_SIZE_BYTES, "increase gateway size");
static_assert(MAX_GATEWAY_SIZE_BYTES <= INT32_MAX, "gateway size too large");
static_assert(OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <= INT32_MAX, "stack original return address storage offset too large, you will point out of stack");

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
	// add 1 to our gateway page accessor count
	0x48, 0x83, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, // add qword ptr [rip+MAX_GATEWAY_SIZE_BYTES-8], 1 <- rip will point to next instruction 8 bytes away, adding MAX_GATEWAY_SIZE_BYTES will make it not aligned correctly at the gateway size addy

	// start storing registers and original return address somewhere up on our stack
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <- int
	0xFF, 0xB4, 0x24, 0x00, 0x00, 0x00, 0x00, // push qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK]
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
	0x48, 0x81, 0xC4, 0x00, 0x00, 0x00, 0x00 // add rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS*8 + 8 <- int, +8 since we want to overwrite the original return address
};

BYTE destEpilogue[] = {
	// rsp will be higher by 8 bytes now due to ret, so we will compensate for it

	// restore the original ret addr
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, 8 + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <- int
	0x48, 0x8B, 0x04, 0x24, // mov rax, [rsp]
	0x48, 0x89, 0xA4, 0x24, 0x00, 0x00, 0x00, 0x00, // mov qword ptr [rsp + OFFSET_STORE_REGS_AND_RETADDR_ON_STACK], rax
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

BYTE hookPageLeave[] = {
	0x48, 0x83, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x01 // sub qword ptr [rip + (offset to gateway page accessor count)], 1
};

Hook::Hook(BYTE* targetJmpPlacementLoc, BYTE* hookGatewayLoc, BYTE targetOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int numOriginalOpcodes) {
	this->targetJmpPlacementLoc = targetJmpPlacementLoc;
	this->hookGatewayLoc = hookGatewayLoc;

	// target original opcodes
	memcpy(targetOriginalOpcodes, targetOriginalOpcodes, numOriginalOpcodes);
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
	memcpy(this->targetJmpPlacementLoc, this->hookTargetMidFuncOriginalOpcodes, this->numOriginalOpcodes);
	VirtualProtect(this->targetJmpPlacementLoc, this->numOriginalOpcodes, oldProtect, nullptr);
}

bool Hook::Delete() {
	this->Disable();

	return VirtualFree(this->hookGatewayLoc, 0, MEM_RELEASE);
}

/*
* Formats the destination call prologue instructions to reset the manual stack alignment bool and if needed, restore the stack to is previous non 16-byte alignment.
* @param opcodeStorage Location to store the formatted instructions at
* @param manualAlignment Pointer to bool which shows whether we altered the stack such that it would be 16-byte aligned.
*/
void FormatDestinationPrologue(BYTE* opcodeStorage) {
	memcpy(opcodeStorage, destPrepPrologue, sizeof(destPrepPrologue));

	*reinterpret_cast<int*>(opcodeStorage + 3) = MAX_GATEWAY_SIZE_BYTES - 8; // RIP offset | add qword ptr [rip+MAX_GATEWAY_SIZE_BYTES-8]

	*reinterpret_cast<int*>(opcodeStorage + 11) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK; // RSP offset |  sub rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK <- int
	*reinterpret_cast<int*>(opcodeStorage + 18) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK; // RSP offset to original ret addr | sub rsp, NUM_STORED_REGISTERS*8 <- int

	*reinterpret_cast<int*>(opcodeStorage + 48) = OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS * 8 + 8; // RSP point stack to original loc | add rsp, OFFSET_STORE_REGS_AND_RETADDR_ON_STACK + NUM_STORED_REGISTERS*8 + 8 <- int
}

/*
* Formats the destination call epilogue instructions needed to align the stack and copy the original contents of it, so that our destination function doesn't overwrite something important.
* @param opcodeStorage Location to store the formatted instructions at
* @param stackPreservationLoc Pointer to the location where the stack has been stored at
* @param manualAlignment Pointer to bool which shows whether we altered the stack such that it would be 16-byte aligned.
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
* Formats hook page leave code to subtract 1 from the amount of current gateway memory page accessors.
* @param opcodeStorage Location to store the formatted instructions at
* @param gatewayStart Pointer to the start of the gateway
*/
void FormatHookPageLeave(BYTE* opcodeStorage) {
	memcpy(opcodeStorage, hookPageLeave, sizeof(hookPageLeave));

	*reinterpret_cast<int*>(opcodeStorage + 3) = MAX_GATEWAY_SIZE_BYTES - sizeof(destPrepPrologue) - sizeof(absCall) - sizeof(destEpilogue) - 8; // -8 since rip already points to next instruction, it wont store it at the correct offset
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
std::unique_ptr<Hook> CreateHook64Standalone(BYTE* targetFunc, BYTE* destinationFunc) {
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

	std::unique_ptr<Hook> hook = std::make_unique<Hook>(hookStart, gateway, originalOpcodes, numOriginalOpcodes);

	// start copying instructions to our gateway
	BYTE* gatewayInstructionPtr = gateway;

	// 1. destination call preparation, prologue
	BYTE destPrologueOpcodes[sizeof(destPrepPrologue)] = { 0 };
	FormatDestinationPrologue(destPrologueOpcodes);
	memcpy(gatewayInstructionPtr, destPrologueOpcodes, sizeof(destPrologueOpcodes));
	gatewayInstructionPtr += sizeof(destPrologueOpcodes);

	// 2. call destination func
	BYTE callOpcodes[sizeof(absCall)];
	FormatAbsoluteCallCode(absCall, destinationFunc);
	memcpy(gatewayInstructionPtr, callOpcodes, sizeof(callOpcodes));
	gatewayInstructionPtr += sizeof(callOpcodes);

	// 3. destination call cleanup, epilogue
	BYTE epilogueOpcodes[sizeof(destEpilogue)];
	FormatDestinationEpilogue(epilogueOpcodes);
	memcpy(gatewayInstructionPtr, epilogueOpcodes, sizeof(epilogueOpcodes));
	gatewayInstructionPtr += sizeof(epilogueOpcodes);

	// 4. hook page leave code
	BYTE hookLeaveOpcodes[sizeof(hookPageLeave)];
	FormatHookPageLeave(hookLeaveOpcodes);
	memcpy(gatewayInstructionPtr, hookLeaveOpcodes, sizeof(hookLeaveOpcodes));
	gatewayInstructionPtr += sizeof(hookLeaveOpcodes);

	// 5. jmp to our target after our jmp instruction
	BYTE jmpOpcodes[sizeof(absJmpNoRegister)];
	FormatAbsoluteJmpCode(jmpOpcodes, hookEnd);

	return std::make_unique<Hook>(hookStart, gateway, originalOpcodes, numOriginalOpcodes);
}

// Creates a hook inside our target function which jumps to our own & lets us modify the return values etc.
std::unique_ptr<Hook> CreateHook64Attached(BYTE* targetFunc, BYTE* destinationFunc, BYTE** gatewayToCallOriginalTarget, int desiredOffsetFromStart) {
	return nullptr;

	/*int alignedOffset = FindAlignedOffsetFromStart(targetFunc, desiredOffsetFromStart);
	if (alignedOffset == -1)
		return nullptr;
	// didn't find a good place for hook placement
	std::tuple<BYTE*, BYTE*> hookPointers = GetHookStartAndEndAddress(targetFunc, alignedOffset);
	if (std::get<0>(hookPointers) == nullptr)
		return nullptr;
	BYTE* hookStart = std::get<0>(hookPointers);
	BYTE* hookEnd = std::get<1>(hookPointers);
	// copy target func opcodes
	uint64_t opcodesBetween = hookEnd - hookStart;
	BYTE originalInstructions[MAX_ORIG_INSTRUCTIONS] = { 0 };
	memcpy(originalInstructions, hookStart, opcodesBetween);
	// TODO:
	int paddingSize = roundToNearestMultiple(8, sizeof(absJmpNoRegister));
	uint64_t gatewaySize = sizeof(absJmpNoRegister) + paddingSize + opcodesBetween + sizeof(absJmpNoRegister);
	BYTE* gateway = static_cast<BYTE*>(VirtualAlloc(0, gatewaySize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)); // will still allocate a whole page
	if (!gateway)
		return nullptr;
	BYTE* gatewayInstructionPtr = gateway;
	// start writing opcodes to gateway
	// 1. jmp to our own function
	memcpy(gatewayInstructionPtr, absJmpNoRegister, sizeof(absJmpNoRegister));
	*reinterpret_cast<BYTE**>(gatewayInstructionPtr + 6) = destinationFunc;
	gatewayInstructionPtr += sizeof(absJmpNoRegister);
	// 2. padding
	memset(gatewayInstructionPtr, 0xCC, paddingSize);
	gatewayInstructionPtr += paddingSize;
	// 3. function prologue for our hooked function
	if (gatewayToCallOriginalTarget)
		*gatewayToCallOriginalTarget = gatewayInstructionPtr;
	memcpy(gatewayInstructionPtr, originalInstructions, opcodesBetween);
	gatewayInstructionPtr += opcodesBetween;
	// 4. jmp to the next instruction after our hook bytes in the target func
	memcpy(gatewayInstructionPtr, absJmpNoRegister, sizeof(absJmpNoRegister));
	*reinterpret_cast<BYTE**>(gatewayInstructionPtr + 6) = hookEnd;
	// all done, now initialize our hook
	return std::make_unique<Hook>(hookStart, gateway, gatewaySize, originalInstructions, opcodesBetween);*/
}