#pragma once

#include <memory>

#define MAX_ORIG_INSTRUCTIONS 40
#define NUM_PRESERVE_STACK_QWORDS 18 // keep this an even number
#define MAX_GATEWAY_SIZE_BYTES 512 // we will put the number of current page accessors at this location in the gateway
#define OFFSET_STORE_REGS_AND_RETADDR_ON_STACK 32768 // offset of rsp where the original return address and registers are stored at
#define NUM_STORED_REGISTERS 15

extern BYTE absJmpNoRegister[];
extern BYTE absCall[];
extern BYTE stackPreserveAsm[];
extern BYTE destPrepPrologue[];
extern BYTE destEpilogue[];
extern BYTE pushAllRegs[];
extern BYTE popAllRegs[];


class Hook {
public:
	Hook(BYTE* targetJmpPlacementLoc, BYTE* hookGatewayLoc, BYTE targetOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int numOriginalOpcodes);
	~Hook();

	void SetHookGatewayLoc(BYTE* newGatewayLoc);
	void Enable();
	void Disable();
	bool Delete();

private:
	BYTE* targetJmpPlacementLoc; // TARGET FUNC location where the gateway jmp is placed
	BYTE* hookGatewayLoc; // gateway location

	BYTE hookTargetOriginalOpcodes[MAX_ORIG_INSTRUCTIONS];
	int numOriginalOpcodes; // necessary so we can actually execute the original target func's opcodes that were replaced by hooking
};

std::unique_ptr<Hook> CreateTrampHook64_Advanced(BYTE* targetFunc, BYTE* destinationFunc);
std::unique_ptr<Hook> CreateTrampHook64(BYTE* targetFunc, BYTE* destinationFunc, BYTE** pCallOriginalFunc);