#pragma once

#include <memory>

#define MAX_ORIG_INSTRUCTIONS 40
#define MAX_STACK_PRESERVE 64*8 // 16 QWORDS, MAKE SURE THIS IS 16 BYTE ALIGNED
#define MAX_GATEWAY_SIZE_BYTES 512

extern BYTE absJmpNoRegister[];
extern BYTE absCall[];
extern BYTE stackPreserveAsm[];
extern BYTE destPrepPrologue[];
extern BYTE destEpilogue[];
extern BYTE pushAllRegs[];
extern BYTE popAllRegs[];


class Hook {
public:
	BYTE preservedStack[MAX_STACK_PRESERVE];
	bool stackManuallyAligned;

	Hook(BYTE* stackPreservationJmp, BYTE* stackPreserveLoc, BYTE* hookStartJmp, BYTE* hookGatewayLoc, BYTE hookTargetStartOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int actualStartOpcodesInUse, BYTE hookTargetMidFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int actualOpcodesInUse);
	~Hook();

	void SetHookGatewayLoc(BYTE* newGatewayLoc);
	void Enable();
	void Disable();
	bool Delete();

private:
	BYTE* stackPreservationJmp; // TARGET FUNC location where the stack preservation jmp is put
	BYTE targetFuncStackPreserveOriginalOpcodes[MAX_ORIG_INSTRUCTIONS];
	int numStackPreserveOriginalOpcodes;
	BYTE* stackPreserveLoc;

	BYTE* hookStartJmp; // TARGET FUNC location where the gateway jmp is placed
	BYTE* hookGatewayLoc; // gateway location
	BYTE hookTargetMidFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS];
	int actualOpcodesInUse; // necessary so we can actually execute the original target func's opcodes that were replaced by hooking
};

std::unique_ptr<Hook> CreateHook64Standalone(BYTE* targetFunc, BYTE* destinationFunc, int desiredOffsetFromStart);
std::unique_ptr<Hook> CreateHook64Attached(BYTE* targetFunc, BYTE* destinationFunc, int desiredOffsetFromStart);
