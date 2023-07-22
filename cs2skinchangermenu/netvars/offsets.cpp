#include "pch.h"

#include "offsets.h"
#include "memory/patterns.h"
#include "memory/tools/patternscan.h"

#include <iostream>

namespace offsets {
	uint32_t m_ppMaterial = 0;
	uint32_t m_bAllowSkinRegeneration = 0;
	uint32_t m_pRegenCount = 0;
	uint32_t m_pWeaponSecondVTable = 0;
	uint32_t m_iMeshGroupMaskMain = 0;
}

// Get's skin regeneration, weapon material pointer and regen count pointer offset
void GetSkinRegenerationOffset() {
	char* regenSkinPtr = ScanPatternInModule("client.dll", PATTERN_REGENWEP_ALLOWREGENOFFSET, MASK_REGENWEP_ALLOWREGENOFFSET);
	if (!regenSkinPtr)
		return;

	offsets::m_bAllowSkinRegeneration = *reinterpret_cast<uint32_t*>(regenSkinPtr + OFFSETSTART_REGENWEP_ALLOWREGENOFFSET);
	offsets::m_ppMaterial = offsets::m_bAllowSkinRegeneration - 0x10;
	offsets::m_pRegenCount = offsets::m_bAllowSkinRegeneration + 8;
}

void GetWeapon2ndVtableOffset() {
	char* wepSecondVtablePtr = ScanPatternInModule("client.dll", PATTERN_WEP2NDVTABLE_OFFSET, MASK_WEP2NDVTABLE_OFFSET);
	if (!wepSecondVtablePtr)
		return;

	offsets::m_pWeaponSecondVTable = *reinterpret_cast<uint32_t*>(wepSecondVtablePtr + OFFSETSTART_WEP2NDVTABLE);
}

bool InitializeOffsets() {
	GetSkinRegenerationOffset();
	GetWeapon2ndVtableOffset();

	// TODO: ALWAYS CHECK MANUAL OFFSETS, add warning
	offsets::m_iMeshGroupMaskMain = 0x2e0;

	return offsets::m_bAllowSkinRegeneration && offsets::m_pRegenCount && offsets::m_ppMaterial && offsets::m_pWeaponSecondVTable && offsets::m_iMeshGroupMaskMain;
}