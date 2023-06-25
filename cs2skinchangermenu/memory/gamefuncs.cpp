#include "pch.h"
#include "gamefuncs.h"
#include "memory/patterns.h"
#include "memory/tools/patternscan.h"

namespace fn {
	fGetCCStrike15ItemSystem CSource2Client__GetCCStrike15ItemSystem = nullptr;
}

void InitGetCEconItemSystem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETECONITEMSYS_PTR, MASK_GETECONITEMSYS_PTR);
	fn::CSource2Client__GetCCStrike15ItemSystem = (fGetCCStrike15ItemSystem)funcptr;
}

bool InitializeFunctions() {
	InitGetCEconItemSystem();

	return fn::CSource2Client__GetCCStrike15ItemSystem != nullptr;
}