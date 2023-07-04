#include "pch.h"
#include "globals.h"
#include "memory/tools/patternscan.h"
#include "memory/patterns.h"

namespace globals {
	CCSPlayerController** ppLocalPlayer = nullptr;
}

void LoadLocalPlayerAddr() {
	uint8_t* instrPtr = reinterpret_cast<uint8_t*>(ScanPatternInModule("client.dll", PATTERN_LOCALPLAYER_PTRPTR1, MASK_LOCALPLAYER_PTRPTR1));
	if (!instrPtr)
		return;

	// lea ..., g_pCCSPlayerController || we get the g_p... part
	uint32_t offsetFromInstruction = *reinterpret_cast<uint32_t*>(instrPtr + OFFSETSTART_LOCALPLAYER_PTRPTR1); // read 5 bytes from the start of scan to get the offset to local player ptr addr

	globals::ppLocalPlayer = reinterpret_cast<CCSPlayerController**>(instrPtr + OFFSETEND_LOCALPLAYER_PTRPTR1 + offsetFromInstruction); //+7 since the offset is from the end of instruction
}

bool InitializeGlobals() {
	LoadLocalPlayerAddr();

	return globals::ppLocalPlayer;
}