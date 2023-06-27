#pragma once

#include <memory>

#define MAX_ORIG_INSTRUCTIONS 40
#define MAX_GATEWAY_SIZE_BYTES 512
#define OFFSET_ORIGINAL_RET_ADDRESS 0
#define OFFSET_PAGE_ACCESSORS 8
#define OFFSET_RAX_ORIGINAL 16
#define NUM_PRESERVED_REGS 15

extern BYTE absJmpNoRegister[];
extern BYTE destPrepPrologue[];
extern BYTE destEpilogue[];
extern BYTE pushAllRegs[];
extern BYTE popAllRegs[];


class Hook {
public:
	Hook(BYTE* gatewayStart, BYTE* targetLoc, BYTE targetFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS], int numOriginalOpcodes);
	~Hook();

	void Enable();
	void Disable();
	bool Delete();

private:
	BYTE* gatewayStart;
	BYTE* targetLoc;
	BYTE targetFuncOriginalOpcodes[MAX_ORIG_INSTRUCTIONS];
	int numOriginalOpcodes;
};

std::unique_ptr<Hook> CreateTrampHook64_Advanced(BYTE* targetFunc, BYTE* destinationFunc);
