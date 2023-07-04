#include "pch.h"

#include "offsets.h"
#include "memory/patterns.h"
#include "memory/tools/patternscan.h"

#include <iostream>

namespace offsets {
	uint32_t m_bAllowSkinRegeneration = 0;
	uint32_t m_pRegenCount = 0;
}

void GetSkinRegenerationOffset() {
	char* regenSkinPtr = ScanPatternInModule("client.dll", PATTERN_REGENWEP_ALLOWREGENOFFSET, MASK_REGENWEP_ALLOWREGENOFFSET);
	if (!regenSkinPtr)
		return;

	offsets::m_bAllowSkinRegeneration = *reinterpret_cast<uint32_t*>(regenSkinPtr + OFFSETSTART_REGENWEP_ALLOWREGENOFFSET);
	offsets::m_pRegenCount = offsets::m_bAllowSkinRegeneration + 8;
}

bool InitializeOffsets() {
	GetSkinRegenerationOffset();

	return offsets::m_bAllowSkinRegeneration && offsets::m_pRegenCount;
}