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


BYTE stackPreserveAsm[] = {
	0x50, // push rax
	0x41, 0x50, // push r8
	0x52, //push rdx
	0x51, // push rcx
	// memcpy()
	0x49, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, // mov r8d, 0x00 00 00 00 <- size
	0x48, 0x8D, 0x94, 0x24, 0xB0, 0x00, 0x00, 0x00, // lea rdx, [rsp + 168+8] <- src, excluding all the registers we pushed + original ret address
	0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rcx, 0x00 00 00 00 00 00 00 00 <- dst
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, 0x00 00 00 00 00 00 00 00
	0xFF, 0xD0, // call rax
	// memcpy() end
	0x59, // pop rcx
	0x5A, // pop rdx
	0x41, 0x58, // pop r8
	0x58, // pop rax
};

BYTE destPrepPrologue[] = {
	// checks if the stack is 16-byte aligned
	0x50, // push rax
	0x48, 0x89, 0xE0, // mov rax, rsp
	0x48, 0x83, 0xC0, 0x08, // add rax, 8
	0x24, 0x0F, // and al, 0xF
	0x3C, 0x00, // cmp al, 0
	0x58, // pop rax
	0x74, 0x17, // je 23 <- jump over the if statement

	// IF(stack not aligned)
	0x50, // push rax
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, 0x000000000000000
	0x48, 0xC7, 0x00, 0x01, 0x00, 0x00, 0x00, // mov qword ptr [rax], 1
	0x58, // pop rax
	0x48, 0x83, 0xEC, 0x08, // sub rsp, 8
	// ENDIF

	// sets up the stack for a memcpy of our original
	0x50, // push rax  <- needed for our destination
	0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, // sub rsp, 0x00000000
	0x50, // push rax
	0x41, 0x50, // push r8
	0x52, // push rdx
	0x51, // push rcx      50th byte after this
	0x49, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, // mov r8, 0x00000000
	0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// mov rdx, 0x0000000000000000
	0x48, 0x8D, 0x4C, 0x24, 0x20, // lea rcx, [rsp+32] <- because we pushed 4 variables to the stack, dont overwrite those
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// mov rax, 0x0000000000000000
	0xFF, 0xD0, // call rax
	0x59, // pop rcx
	0x5A, // pop rdx
	0x41, 0x58, // pop r8
	0x58 // pop rax
	// memcpy end
};

BYTE destEpilogue[] = {
	0x50, // push rax
	0x48, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, [0x0000000000000000]
	0x48, 0x83, 0xF8, 0x01, // cmp rax, 1
	0x58, // pop rax
	0x75, 0x17, // jne 23  <- endif

	// IF(stack was manually aligned)
	0x50, // push rax
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, 0x0000000000000000
	0x48, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, // mov qword ptr [rax], 0
	0x58, // pop rax
	0x48, 0x83, 0xC4, 0x08, // add rsp, 8
	// ENDIF
	0x48, 0x81, 0xC4, 0x00, 0x00, 0x00, 0x00, // add rsp, 0x00000000
	0x58 // pop rax  <- restore original value
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

Hook::Hook(BYTE* stackPreservationJmp, BYTE* stackPreserveLoc, BYTE* hookStartJmp, BYTE* hookGatewayLoc, BYTE targetFuncStackPreserveOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int numStackPreserveOriginalOpcodes, BYTE hookTargetMidFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int actualOpcodesInUse) {
	this->stackPreservationJmp = stackPreservationJmp;
	this->stackPreserveLoc = stackPreserveLoc;
	this->hookStartJmp = hookStartJmp;
	this->hookGatewayLoc = hookGatewayLoc;
	// stack preservation opcodes
	memcpy(this->targetFuncStackPreserveOriginalOpcodes, targetFuncStackPreserveOriginalOpcodes, numStackPreserveOriginalOpcodes);
	this->numStackPreserveOriginalOpcodes = numStackPreserveOriginalOpcodes;
	// actual hooked location opcodes, will be all 0x00 if unused
	memcpy(this->hookTargetMidFuncOriginalOpcodes, hookTargetMidFuncOriginalOpcodes, actualOpcodesInUse);
	this->actualOpcodesInUse = actualOpcodesInUse;
}

Hook::~Hook() {
	this->Delete();
}

/*
* Updates the location where our mid function hook will jump to.
*/
void Hook::SetHookGatewayLoc(BYTE* newGatewayLoc) {
	this->hookGatewayLoc = newGatewayLoc;
}

// TODO:
void Hook::Enable() {
	DWORD oldProtect;
	VirtualProtect(this->hookStartJmp, this->actualOpcodesInUse, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(this->hookStartJmp, absJmpNoRegister, sizeof(absJmpNoRegister));
	// write jmp address
	*reinterpret_cast<BYTE**>(this->hookStartJmp + 6) = this->hookGatewayLoc;
	VirtualProtect(this->hookStartJmp, this->actualOpcodesInUse, oldProtect, &oldProtect);

	// conditionally enable hook start jmp aswell, if we are mid function hooking
	if (this->stackPreservationJmp != this->hookStartJmp) {
		VirtualProtect(this->stackPreservationJmp, this->numStackPreserveOriginalOpcodes, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(this->stackPreservationJmp, absJmpNoRegister, sizeof(absJmpNoRegister));
		// write jmp address
		*reinterpret_cast<BYTE**>(this->stackPreservationJmp + 6) = this->stackPreserveLoc;
		VirtualProtect(this->stackPreservationJmp, this->numStackPreserveOriginalOpcodes, oldProtect, nullptr);
	}
}

// TODO:
void Hook::Disable() {
	DWORD oldProtect;
	VirtualProtect(this->hookStartJmp, this->actualOpcodesInUse, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(this->hookStartJmp, this->hookTargetMidFuncOriginalOpcodes, this->actualOpcodesInUse);
	VirtualProtect(this->hookStartJmp, this->actualOpcodesInUse, oldProtect, nullptr);

	if (this->stackPreservationJmp != this->hookStartJmp) {
		VirtualProtect(this->stackPreservationJmp, this->numStackPreserveOriginalOpcodes, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(this->stackPreservationJmp, this->targetFuncStackPreserveOriginalOpcodes, this->numStackPreserveOriginalOpcodes);
		VirtualProtect(this->stackPreservationJmp, this->numStackPreserveOriginalOpcodes, oldProtect, nullptr);
	}
}

bool Hook::Delete() {
	this->Disable();

	return VirtualFree(this->stackPreserveLoc, 0, MEM_RELEASE);
}

/*
* Formats the stack preservation assembly such that the instructions point to the correct addresses.
* @param opcodeStorage Location to store the formatted instructions at
* @param preservationLoc Pointer to the location where the stack will be preserved during runtime
*/
void FormatStackPreservationCode(BYTE* opcodeStorage, BYTE* stackPreservationLoc) {
	memcpy(opcodeStorage, stackPreserveAsm, sizeof(stackPreserveAsm));

	*reinterpret_cast<int*>(opcodeStorage + 8) = MAX_STACK_PRESERVE; // mov r8, MAX_STACK_PRESERVE  <-- memcpy size argument
	*reinterpret_cast<BYTE**>(opcodeStorage + 22) = stackPreservationLoc; // mov rcd, preservationLoc  <-- memcpy dst argument

	*reinterpret_cast<BYTE**>(opcodeStorage + 32) = reinterpret_cast<BYTE*>(&memcpy);
}

/*
* Formats the destination call prologue instructions to reset the manual stack alignment bool and if needed, restore the stack to is previous non 16-byte alignment.
* @param opcodeStorage Location to store the formatted instructions at
* @param manualAlignment Pointer to bool which shows whether we altered the stack such that it would be 16-byte aligned.
*/
void FormatDestinationPrologue(BYTE* opcodeStorage, BYTE* stackPreservationLoc, bool* manualAlignment) {
	memcpy(opcodeStorage, destPrepPrologue, sizeof(destPrepPrologue));

	*reinterpret_cast<bool**>(opcodeStorage + 18) = manualAlignment;

	// set how many bytes to subtract from rsp
	*reinterpret_cast<int*>(opcodeStorage + 42) = MAX_STACK_PRESERVE+16;

	// setting up memcpy arguments
	// size
	*reinterpret_cast<int*>(opcodeStorage + 54) = MAX_STACK_PRESERVE;
	// ptr to src
	*reinterpret_cast<BYTE**>(opcodeStorage + 60) = stackPreservationLoc;
	// ptr to memcpy
	*reinterpret_cast<BYTE**>(opcodeStorage + 75) = reinterpret_cast<BYTE*>(&memcpy);
}

/*
* Formats the destination call epilogue instructions needed to align the stack and copy the original contents of it, so that our destination function doesn't overwrite something important.
* @param opcodeStorage Location to store the formatted instructions at
* @param stackPreservationLoc Pointer to the location where the stack has been stored at
* @param manualAlignment Pointer to bool which shows whether we altered the stack such that it would be 16-byte aligned.
*/
void FormatDestinationEpilogue(BYTE* opcodeStorage, bool* manualAlignment) {
	memcpy(opcodeStorage, destEpilogue, sizeof(destEpilogue));

	*reinterpret_cast<bool**>(opcodeStorage + 3) = manualAlignment;
	*reinterpret_cast<bool**>(opcodeStorage + 21) = manualAlignment;

	*reinterpret_cast<int*>(opcodeStorage + 44) = MAX_STACK_PRESERVE+16;
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

// Returns the offset to a valid instruction from the start of target function, the offset being >= desired offset.
int FindAlignedOffsetFromStart(BYTE* targetFunc, int desiredOffsetFromStart) {
	int currentOffset = 0;

	// find an offset that is suitable to avoid corrupting asm instructions
	while (currentOffset < desiredOffsetFromStart) {
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


// Creates a hook inside our target function without interfereing with the target function's work.
// This means that even though our destination function will receive the target function's arguments, we will not be able to modify anything inside of the target function (including return value etc)
std::unique_ptr<Hook> CreateHook64Standalone(BYTE* targetFunc, BYTE* destinationFunc, int desiredOffsetFromStart) {
	static_assert(3*sizeof(absJmpNoRegister) + 2*sizeof(pushAllRegs) + 2*sizeof(popAllRegs) + 2*MAX_ORIG_INSTRUCTIONS + sizeof(stackPreserveAsm) + sizeof(destPrepPrologue) + sizeof(absCall) + sizeof(destEpilogue) <= MAX_GATEWAY_SIZE_BYTES, "increase gateway size");
	
	int alignedOffset = FindAlignedOffsetFromStart(targetFunc, desiredOffsetFromStart);
	if (alignedOffset == -1)
		return nullptr;

	// didn't find a good place for hook placement
	std::tuple<BYTE*, BYTE*> hookPointers = GetHookStartAndEndAddress(targetFunc, alignedOffset);
	if (std::get<0>(hookPointers) == nullptr)
		return nullptr;

	BYTE* hookStart = std::get<0>(hookPointers);
	BYTE* hookEnd = std::get<1>(hookPointers);
	// will be needed when hooking mid func, jmp from start of our target and preserve the stack and then jump back
	BYTE* stackPreserveStart = hookStart;
	BYTE* stackPreserveEnd = hookEnd;
	uint64_t opcodesInStart = 0;

	// copy target func opcodes
	uint64_t opcodesMidFunc = hookEnd - hookStart;
	BYTE funcStartOpcodes[MAX_ORIG_INSTRUCTIONS] = { 0 };
	BYTE midFuncOpcodes[MAX_ORIG_INSTRUCTIONS] = { 0 };
	memcpy(midFuncOpcodes, hookStart, opcodesMidFunc);

	// stack preservation will not work the exact same way when we are mid func, got to do it at the beginning and then jmp back
	if (hookStart != targetFunc) {
		std::tuple<BYTE*, BYTE*> jmpPointers = GetHookStartAndEndAddress(targetFunc, 0);
		if (std::get<0>(jmpPointers) == nullptr)
			return nullptr;

		stackPreserveStart = std::get<0>(hookPointers);
		stackPreserveEnd = std::get<1>(hookPointers);
		opcodesInStart = stackPreserveEnd - stackPreserveStart;
		memcpy(funcStartOpcodes, stackPreserveStart, opcodesInStart);
	}

	// allocate enough memory to preserve all registers
	// call our hook func, pop all registers and return
	// to our target func at the **appropriate** location
	BYTE* gateway = static_cast<BYTE*>(VirtualAlloc(0, MAX_GATEWAY_SIZE_BYTES, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)); // will still allocate a whole page
	if (!gateway)
		return nullptr;

	std::unique_ptr<Hook> hook = std::make_unique<Hook>(stackPreserveStart, gateway, hookStart, gateway, funcStartOpcodes, opcodesInStart, midFuncOpcodes, opcodesMidFunc);
	BYTE stackPreservationOpcodes[sizeof(stackPreserveAsm)] = { 0 };

	BYTE destCallPrepOpcodes[sizeof(destPrepPrologue)] = { 0 };
	BYTE callOpcodes[sizeof(absCall)] = { 0 };
	BYTE destCallEpilogueOpcodes[sizeof(destEpilogue)] = { 0 };
	BYTE jmpBackToTarget[sizeof(absJmpNoRegister)] = { 0 };

	// start copying instructions to our gateway
	BYTE* gatewayInstructionPtr = gateway;

	// 1. push all registers to the stack (except rsp), preserve their state
	memcpy(gatewayInstructionPtr, pushAllRegs, sizeof(pushAllRegs));
	gatewayInstructionPtr += sizeof(pushAllRegs);

	// 2. stack preservation
	FormatStackPreservationCode(stackPreservationOpcodes, hook.get()->preservedStack);
	memcpy(gatewayInstructionPtr, stackPreservationOpcodes, sizeof(stackPreservationOpcodes));
	gatewayInstructionPtr += sizeof(stackPreservationOpcodes);
	// (CONDITIONAL) 3. original start code, jump back to function
	if (stackPreserveStart != hookStart) {
		BYTE jmpBackAfterStackPreserve[sizeof(absJmpNoRegister)] = { 0 };

		memcpy(gatewayInstructionPtr, stackPreserveStart, opcodesInStart);
		gatewayInstructionPtr += opcodesInStart;

		// pop all regs, restore all registers' previous state
		memcpy(gatewayInstructionPtr, popAllRegs, sizeof(popAllRegs));
		gatewayInstructionPtr += sizeof(popAllRegs);

		// run the original overwritten code
		memcpy(gatewayInstructionPtr, funcStartOpcodes, opcodesInStart);
		gatewayInstructionPtr += opcodesInStart;

		FormatAbsoluteJmpCode(jmpBackAfterStackPreserve, stackPreserveEnd);
		memcpy(gatewayInstructionPtr, jmpBackAfterStackPreserve, sizeof(jmpBackAfterStackPreserve));
		gatewayInstructionPtr += sizeof(jmpBackAfterStackPreserve);

		// add 16-byte alignment
		int alignmentSize = 16 - (reinterpret_cast<uintptr_t>(gatewayInstructionPtr) & 0xF);
		if (alignmentSize != 16) {
			memset(gatewayInstructionPtr, 0xCC, alignmentSize);
			gatewayInstructionPtr += alignmentSize;
		}

		hook.get()->SetHookGatewayLoc(gatewayInstructionPtr);
	}

	// (OPTIONAL) 4. push all registers onto the stack if we already haven't
	if (stackPreserveStart != hookStart) {
		memcpy(gatewayInstructionPtr, pushAllRegs, sizeof(pushAllRegs));
		gatewayInstructionPtr += sizeof(pushAllRegs);
	}

	// 5. copy our hook destination setup prologue
	FormatDestinationPrologue(destCallPrepOpcodes, hook.get()->preservedStack, &hook.get()->stackManuallyAligned);
	memcpy(gatewayInstructionPtr, destCallPrepOpcodes, sizeof(destCallPrepOpcodes));
	gatewayInstructionPtr += sizeof(destCallPrepOpcodes);
	// 6. call our destination function
	FormatAbsoluteCallCode(callOpcodes, destinationFunc);
	memcpy(gatewayInstructionPtr, callOpcodes, sizeof(callOpcodes));
	gatewayInstructionPtr += sizeof(callOpcodes);
	// 7. destination func epilogue, cleanup
	FormatDestinationEpilogue(destCallEpilogueOpcodes, &hook.get()->stackManuallyAligned);
	memcpy(gatewayInstructionPtr, destCallEpilogueOpcodes, sizeof(destCallEpilogueOpcodes));
	gatewayInstructionPtr += sizeof(destCallEpilogueOpcodes);
	// 8. pop all registers from the stack, get their original state
	memcpy(gatewayInstructionPtr, popAllRegs, sizeof(popAllRegs));
	gatewayInstructionPtr += sizeof(popAllRegs);
	// 9. run the original overwritten code
	memcpy(gatewayInstructionPtr, midFuncOpcodes, opcodesMidFunc);
	gatewayInstructionPtr += opcodesMidFunc;
	// 10. jmp back to our mid func location
	FormatAbsoluteJmpCode(jmpBackToTarget, hookEnd);
	memcpy(gatewayInstructionPtr, jmpBackToTarget, sizeof(jmpBackToTarget));
	gatewayInstructionPtr += sizeof(jmpBackToTarget);

	// all done, now initialize our hook
	return hook;
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