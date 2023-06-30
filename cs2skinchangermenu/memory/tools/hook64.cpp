#include "pch.h"
#include "hook64.h"
#include "hde64.h"

#include <cstdint>
#include <cmath>
#include <memory>
#include <chrono>
#include <thread>

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
	// start storing registers except rsp
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

	// subtract the required amount of space for the stack preservation
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, NUM_PRESERVE_STACK_QWORDS*8
	// preserve variables used by movsq instruction, copy stack over to new location
	0x56, // push rsi
	0x57, // push rdi
	0x51, // push rcx
	0x48, 0x8D, 0xB4, 0x24, 0x00, 0x00, 0x00, 0x00, // lea rsi, [rsp + 24 + NUM_PRESERVE_STACK_QWORDS*8 + NUM_STORED_REGISTERS*8 + 8] <- +24 to skip over our preserved regs, +8 to skip over original ret addr
	0x48, 0x8D, 0x7C, 0x24, 0x18, // lea rdi, [rsp+24]
	0x48, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00, // mov rcx, NUM_PRESERVE_STACK_QWORDS
	0xF3, 0x48, 0xA5, // rep movsq
	0x59, // pop rcx
	0x5F, // pop rdi
	0x5E, // pop rsi

	// write new return address to the stack top
	0x6A, 0x00, // push 0 <- replaced later by ret addr
	0x50, // push rax
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, NEW_RET_ADDR
	0x48, 0x89, 0x44, 0x24, 0x08, // mov [rsp+8], rax
	0x58 // pop rax*/
};

BYTE destEpilogue[] = {
	// return stack pointer back to our original addy
	0x48, 0x81, 0xC4, 0x00, 0x00, 0x00, 0x10, // add rsp, NUM_PRESERVE_STACK_QWORDS*8 <- int
	// restore registers
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
	//this->Delete();
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

	while (*reinterpret_cast<uint64_t*>(this->hookGatewayLoc + MAX_GATEWAY_SIZE_BYTES) != 0) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		continue;
	}

	std::this_thread::sleep_for(std::chrono::microseconds(10));

	return VirtualFree(this->hookGatewayLoc, 0, MEM_RELEASE);
}

/*
* Formats the destination call prologue instructions to reset the manual stack alignment bool and if needed, restore the stack to is previous non 16-byte alignment.
* @param opcodeStorage Location to store the formatted instructions at
* @param newRetAddr Return address pointing to the next instruction after absolute jmp to destination
*/
void FormatDestinationPrologue(BYTE* opcodeStorage, BYTE* newRetAddr) {
	memcpy(opcodeStorage, destPrepPrologue, sizeof(destPrepPrologue));

	// set up new stack
	*reinterpret_cast<int*>(opcodeStorage + 26) = NUM_PRESERVE_STACK_QWORDS*8; // sub rsp, NUM_PRESERVE_STACK_QWORDS*8;

	// stack copy setup
	*reinterpret_cast<int*>(opcodeStorage + 37) = 24 + NUM_PRESERVE_STACK_QWORDS*8 + NUM_STORED_REGISTERS*8 + 8;// lea rsi, [rsp + 24 + NUM_PRESERVE_STACK_QWORDS*8 + NUM_STORED_REGISTERS*8]
	*reinterpret_cast<int*>(opcodeStorage + 49) = NUM_PRESERVE_STACK_QWORDS; // mov rax, NUM_PRESERVE_STACK_QWORDS

	*reinterpret_cast<BYTE**>(opcodeStorage + 64) = newRetAddr; // mov rax, NUM_PRESERVE_STACK_QWORDS
}

/*
* Formats the destination call epilogue instructions needed to align the stack and copy the original contents of it, so that our destination function doesn't overwrite something important.
* @param opcodeStorage Location to store the formatted instructions at
*/
void FormatDestinationEpilogue(BYTE* opcodeStorage) {
	memcpy(opcodeStorage, destEpilogue, sizeof(destEpilogue));

	*reinterpret_cast<int*>(opcodeStorage + 3) = NUM_PRESERVE_STACK_QWORDS*8; // sub rsp, NUM_PRESERVE_STACK_QWORDS*8 <- int

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

/*
* Finds a good spot to place our JMP at without overwriting any of the target function's own jmp or call offsets.
*
* @param targetFunc Pointer to the start of our desired function.
*
* @return Pointer to the place most suitable to place the hook
*/
BYTE* FindGoodHookPlacementLoc(BYTE* targetFunc) {
	BYTE* start = targetFunc;
	BYTE* end = start;

	// find an offset that is suitable to avoid corrupting asm instructions
	while (static_cast<uint64_t>(end-start) < sizeof(absJmpNoRegister)) {
		hde64s hdeState = { 0 };
		if (DoesInstructionNeedRecalculating(end)) {
			int size = hde64_disasm(start, &hdeState);
			end += size;
			start = end;
			continue;
		}

		int size = hde64_disasm(start, &hdeState);
		end += size;

		if (hdeState.flags & F_ERROR)
			return nullptr;
	}

	return start;
}


// Creates a hook inside our target function without interfereing with the target function's work.
// This means that even though our destination function will receive the target function's arguments, we will not be able to modify anything inside of the target function (including return value etc)
std::unique_ptr<Hook> CreateTrampHook64_Advanced(BYTE* targetFunc, BYTE* destinationFunc) {
	targetFunc = FindGoodHookPlacementLoc(targetFunc);
	
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
	FormatDestinationPrologue(destPrologueOpcodes, gatewayInstructionPtr+sizeof(destPrepPrologue)+sizeof(absJmpNoRegister));
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
	targetFunc = FindGoodHookPlacementLoc(targetFunc);
	
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
	memcpy(gatewayInstructionPtr, hookLeaveOpcodes, sizeof(hookLeaveOpcodes));
	gatewayInstructionPtr += sizeof(hookLeaveOpcodes);

	// 6. jmp to the next instruction after our hook bytes in the target func
	BYTE jmpBack[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(jmpBack, hookEnd);
	memcpy(gatewayInstructionPtr, jmpBack, sizeof(jmpBack));
	gatewayInstructionPtr += sizeof(jmpBack);

	// all done, now initialize our hook
	return std::make_unique<Hook>(hookStart, gateway, originalInstructions, numOriginalOpcodes);
}