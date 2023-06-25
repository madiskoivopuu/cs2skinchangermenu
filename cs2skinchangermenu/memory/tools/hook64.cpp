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


BYTE destPrepPrologue[] = {
	// add 1 to our page access
	0x48, 0x83, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, // add qword ptr [rip+MAX_GATEWAY_SIZE_BYTES+8], 1 <- since the rip already points to the next instruction, which is exactly 8 bytes away, we will not need to add 8 when actually formatting offset
	// store original return address
	0x48, 0x8B, 0x04, 0x24, // mov rax, [rsp]
	0x48, 0xA3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs 0x0000000000000000, rax
	// overwrite return address
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, gateway+(whatever offset is after our jmp to destination)
	0x48, 0x89, 0x04, 0x24 // mov [rsp], rax
};

BYTE destEpilogue[] = {
	// restore original return address
	0x48, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, gateway+MAX_GATEWAY_SIZE_BYTES
	0x50, // push rax
	// subtract 1 from page access, here we use absolute address since it's a bit easier
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rax, gateway+MAX_GATEWAY_SIZE_BYTES+8
	0x48, 0x83, 0x28, 0x01 // sub qword ptr [rax], 1
};

// push rax
// push rbx
// push rcx
// push rdx
// push rsi
// push rdi
// push rbp
// push rsp
// push r8
// push r9
// push r10
// push r11
// push r12
// push r13
// push r14
// push r15
BYTE pushAllRegs[] = { 
	0x50, 
	0x53, 
	0x51, 
	0x52, 
	0x56, 
	0x57, 
	0x55, 
//	0x54, 
	0x41, 0x50, 
	0x41, 0x51, 
	0x41, 0x52, 
	0x41, 0x53, 
	0x41, 0x54, 
	0x41, 0x55, 
	0x41, 0x56, 
	0x41, 0x57 
};

// pop r15
// pop r14
// ...
// pop rax
BYTE popAllRegs[] = {
	0x41, 0x5F,
	0x41, 0x5E,
	0x41, 0x5D,
	0x41, 0x5C,
	0x41, 0x5B,
	0x41, 0x5A,
	0x41, 0x59,
	0x41, 0x58,
//	0x5C,
	0x5D,
	0x5F,
	0x5E,
	0x5A,
	0x59,
	0x5B,
	0x58
};

/*
* Formats the destination preparation prologue to save our original return address on the stack to somewhere else and replace it with our gateway.
* @param opcodeStorage Location to store the formatted instructions at
* @param gateway Pointer to the start of our gateway.
*/
void FormatDestinationPrologue(BYTE* opcodeStorage, BYTE* gateway) {
	memcpy(opcodeStorage, destPrepPrologue, sizeof(destPrepPrologue));

	*reinterpret_cast<int*>(opcodeStorage + 3) = MAX_GATEWAY_SIZE_BYTES;
	*reinterpret_cast<BYTE**>(opcodeStorage + 14) = gateway + MAX_GATEWAY_SIZE_BYTES;
	*reinterpret_cast<BYTE**>(opcodeStorage + 24) = gateway + sizeof(destPrepPrologue) + sizeof(absJmpNoRegister); // we want to return to the location after our destination jmp
}

/*
* Formats the destination call epilogue instructions that restore the original return address & decrease the page access value by 1.
* @param opcodeStorage Location to store the formatted instructions at
* @param gateway Pointer to the start of our gateway.
*/
void FormatDestinationEpilogue(BYTE* opcodeStorage, BYTE* gateway) {
	memcpy(opcodeStorage, destEpilogue, sizeof(destEpilogue));

	*reinterpret_cast<BYTE**>(opcodeStorage + 2) = gateway+MAX_GATEWAY_SIZE_BYTES;
	*reinterpret_cast<BYTE**>(opcodeStorage + 13) = gateway+MAX_GATEWAY_SIZE_BYTES+8; // set up the offset from rip to our page access qword
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

// Returns the amount of bytes we will minimally need to copy so that we can properly execute the target function prologue.
int FindMinValidInstructionsSize(BYTE* targetFunc) {
	int currentOffset = 0;

	// find an offset that is suitable to avoid corrupting asm instructions
	while (currentOffset < sizeof(absJmpNoRegister)) {
		hde64s hdeState = { 0 };
		int size = hde64_disasm(targetFunc, &hdeState);
		currentOffset += size;

		if (hdeState.flags & F_ERROR)
			return -1;
	}

	return currentOffset;
}

// Get the hook start & end address from our desired offset in the target function. Actual placement may differ from offset due to relative JMP and CALL instructions
// which we would have to recalculate in our code-cave.
std::tuple<BYTE*, BYTE*> GetHookStartAndEndAddress(BYTE* targetFunc, int alignedOffset) {
	// get the hook start address
	// and end address that is enough for us to fit our absJmpNoRegister instructions in between
	BYTE* hookStart = targetFunc + alignedOffset;
	BYTE* hookEnd = hookStart;
	while (hookEnd < hookStart + sizeof(absJmpNoRegister) && (hookEnd - (targetFunc + alignedOffset)) <= 0x7FF) {
		hde64s hdeState = { 0 };
		int size = hde64_disasm(hookEnd, &hdeState);
		hookEnd += size;

		if (DoesInstructionNeedRecalculating(hookEnd))
			hookStart = hookEnd; // possible infinite loop, thus the second condition inside while loop

		if (hdeState.flags & F_ERROR)
			return std::make_tuple(nullptr, nullptr);
	}

	if ((hookEnd - (targetFunc + alignedOffset)) > 0x7FF)
		return std::make_tuple(nullptr, nullptr);

	return std::make_tuple(hookStart, hookEnd);
}

// Creates a detached trampoline hook at the start of our target function to our destination function without extra coding needed.
// Hooking this way will make you unable to modify the return value of the target function. Destination function gets executed before target function.
std::unique_ptr<Hook> CreateTrampHook64_Advanced(BYTE* targetFunc, BYTE* destinationFunc) {
	int targetFuncBytes = FindMinValidInstructionsSize(targetFunc);
	if (targetFuncBytes == -1)
		return nullptr;

	BYTE* targetJmpBackAddy = targetFunc + targetFuncBytes;
	// copy the original bytes to our buffer
	BYTE targetFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS];
	memcpy(targetFuncOriginalOpcodes, targetFunc, targetFuncBytes);

	BYTE* gateway = static_cast<BYTE*>(VirtualAlloc(targetFunc, MAX_GATEWAY_SIZE_BYTES + 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!gateway)
		return nullptr;

	// start writing prologue code for our gateway
	BYTE* gatewayInstructionPtr = gateway;

	// 1. destination jmp prologue, preparation
	BYTE destinationPrep[sizeof(destPrepPrologue)] = { 0 };
	FormatDestinationPrologue(destinationPrep, gateway);
	memcpy(gatewayInstructionPtr, destinationPrep, sizeof(destinationPrep));
	gatewayInstructionPtr += sizeof(destinationPrep);

	// 2. jmp to our destination func, it will ret back to our gateway thanks to the prologue
	BYTE destJmp[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(destJmp, destinationFunc);
	memcpy(gatewayInstructionPtr, destJmp, sizeof(absJmpNoRegister));
	gatewayInstructionPtr += sizeof(absJmpNoRegister);

	// 3. destination return epilogue
	BYTE epilogue[sizeof(destEpilogue)] = { 0 };
	FormatDestinationEpilogue(epilogue, gateway);
	memcpy(gatewayInstructionPtr, epilogue, sizeof(epilogue));
	gatewayInstructionPtr += sizeof(epilogue);

	// 4. execute original bytes
	memcpy(gatewayInstructionPtr, targetFuncOriginalOpcodes, targetFuncBytes);
	gatewayInstructionPtr += targetFuncBytes;

	// 5. return to target func
	BYTE jmpBack[sizeof(absJmpNoRegister)] = { 0 };
	FormatAbsoluteJmpCode(jmpBack, targetFunc + targetFuncBytes);
	memcpy(gatewayInstructionPtr, jmpBack, sizeof(jmpBack));

	return std::make_unique<Hook>(gateway, targetFunc, targetFuncOriginalOpcodes, targetFuncBytes);
}




// Hook class code
Hook::Hook(BYTE* gatewayStart, BYTE* targetLoc, BYTE targetFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int numOriginalOpcodes) {
	this->gatewayStart;
	this->targetLoc = targetLoc;
	memcpy(this->targetFuncOriginalOpcodes, targetFuncOriginalOpcodes, numOriginalOpcodes);
	this->numOriginalOpcodes = numOriginalOpcodes;
}

Hook::~Hook() {
	this->Delete();
}

// TODO:
void Hook::Enable() {
	BYTE jmpOpcodes[sizeof(absJmpNoRegister)];
	FormatAbsoluteJmpCode(jmpOpcodes, this->gatewayStart);

	DWORD oldProtect;
	VirtualProtect(this->targetLoc, this->numOriginalOpcodes, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(this->targetLoc, jmpOpcodes, sizeof(jmpOpcodes));
	VirtualProtect(this->targetLoc, this->numOriginalOpcodes, oldProtect, nullptr);
}

// TODO:
void Hook::Disable() {
	DWORD oldProtect;
	VirtualProtect(this->targetLoc, this->numOriginalOpcodes, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(this->targetLoc, this->targetFuncOriginalOpcodes, this->numOriginalOpcodes);
	VirtualProtect(this->targetLoc, this->numOriginalOpcodes, oldProtect, nullptr);
}

bool Hook::Delete() {
	this->Disable();

	while (*reinterpret_cast<uint64_t*>(this->gatewayStart + MAX_GATEWAY_SIZE_BYTES + 8) != 0) // wait for the page to stop being in use
		continue;

	return VirtualFree(this->gatewayStart, 0, MEM_RELEASE);
}