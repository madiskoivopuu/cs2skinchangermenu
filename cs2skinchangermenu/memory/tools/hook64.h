#pragma once

#include <memory>

#define MAX_ORIG_INSTRUCTIONS 40
#define MAX_STACK_PRESERVE 64*8 // 16 QWORDS, MAKE SURE THIS IS 16 BYTE ALIGNED
#define MAX_GATEWAY_SIZE_BYTES 512 // +16 will be used, 0-7 used to store the original return addy and 8-15 to store the page current use count flag

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
