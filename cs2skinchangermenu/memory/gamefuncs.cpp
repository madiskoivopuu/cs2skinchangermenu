#include "pch.h"
#include "gamefuncs.h"
#include "memory/patterns.h"
#include "memory/tools/patternscan.h"

namespace fn {
	fGetCCStrike15ItemSystem CSource2Client__GetCCStrike15ItemSystem = nullptr;
	fGetCSWeaponDataFromItem GetCSWeaponDataFromItem = nullptr;
	fRegenerateWeaponSkin RegenerateWeaponSkin = nullptr;
	fAllowSkinRegenForWeapon AllowSkinRegenForWeapon = nullptr;
	fCEconItemView__SetAttributeValueByName CEconItemView__SetAttributeValueByName = nullptr;
}

void InitGetCEconItemSystem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETECONITEMSYS_PTR, MASK_GETECONITEMSYS_PTR);
	fn::CSource2Client__GetCCStrike15ItemSystem = reinterpret_cast<fGetCCStrike15ItemSystem>(funcptr);
}

void InitGetCSWeaponDataFromItem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETCSWEAPONDATA_PTR, MASK_GETCSWEAPONDATA_PTR);
	fn::GetCSWeaponDataFromItem = reinterpret_cast<fGetCSWeaponDataFromItem>(funcptr);
}

// initiate allowskinregen and regenweaponskin functions
void InitSkinFunctions() {
	uint8_t* relCallPtr = reinterpret_cast<uint8_t*>(ScanPatternInModule("client.dll", PATTERN_REGENWEAPONSKIN_PTR, MASK_REGENWEAPONSKIN_PTR));
	if (!relCallPtr)
		return;

	// set up allowskinregen func by it's relative call addy
	uint32_t offsetFromInstruction = *reinterpret_cast<int32_t*>(relCallPtr + OFFSETSTART_ALLOWSKINREGEN);
	fn::AllowSkinRegenForWeapon = reinterpret_cast<fAllowSkinRegenForWeapon>(relCallPtr + OFFSETEND_ALLOWSKINREGEN + offsetFromInstruction);

	// set up regenweaponskin func
	int32_t offsetFromInstructionSigned = *reinterpret_cast<int32_t*>(relCallPtr + OFFSETSTART_REGENWEAPONSKIN);
	fn::RegenerateWeaponSkin = reinterpret_cast<fRegenerateWeaponSkin>(relCallPtr + OFFSETEND_REGENWEAPONSKIN + offsetFromInstructionSigned);
}

void InitSetAttribValueByName() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_SETATTRIBVALUEBYNAME_PTR, MASK_SETATTRIBVALUEBYNAME_PTR);
	if (!funcptr)
		return;

	fn::CEconItemView__SetAttributeValueByName = reinterpret_cast<fCEconItemView__SetAttributeValueByName>(funcptr);
}

bool InitializeFunctions() {
	InitGetCEconItemSystem();
	InitSetAttribValueByName();
	InitGetCSWeaponDataFromItem();
	InitSkinFunctions();

	return fn::CSource2Client__GetCCStrike15ItemSystem && fn::GetCSWeaponDataFromItem && fn::RegenerateWeaponSkin && fn::AllowSkinRegenForWeapon && fn::CEconItemView__SetAttributeValueByName;
}