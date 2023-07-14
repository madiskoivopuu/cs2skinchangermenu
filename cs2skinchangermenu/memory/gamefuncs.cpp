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
	fSpawnAndSetStattrakEnt SpawnAndSetStattrakEnt = nullptr;
	fSpawnAndSetNametagEnt SpawnAndSetNametagEnt = nullptr;
	fUpdateViewmodelAttachments UpdateViewmodelAttachments = nullptr;
	fCEconItemSchema__GetAttributeDefinitionByName CEconItemSchema__GetAttributeDefinitionByName = nullptr;
	fRegenerateAllWeaponSkins RegenerateAllWeaponSkins = nullptr;
}

void InitGetCEconItemSystem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETECONITEMSYS_PTR, MASK_GETECONITEMSYS_PTR);
	fn::CSource2Client__GetCCStrike15ItemSystem = static_cast<fGetCCStrike15ItemSystem>(funcptr);
}

void InitGetCSWeaponDataFromItem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETCSWEAPONDATA_PTR, MASK_GETCSWEAPONDATA_PTR);
	fn::GetCSWeaponDataFromItem = static_cast<fGetCSWeaponDataFromItem>(funcptr);
}

// initiate allowskinregen and regenweaponskin functions
void InitSkinFunctions() {
	// set up regenerate all weapon skins func
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_REGENALLWEAPONSKINS_PTR, MASK_REGENALLWEAPONSKINS_PTR);
	if (!funcptr)
		return;

	fn::RegenerateAllWeaponSkins = (fRegenerateAllWeaponSkins)funcptr;

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

void InitAttribFunctions() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_SETATTRIBVALUEBYNAME_PTR, MASK_SETATTRIBVALUEBYNAME_PTR);
	if (!funcptr)
		return;

	fn::CEconItemView__SetAttributeValueByName = static_cast<fCEconItemView__SetAttributeValueByName>(funcptr);

	funcptr = ScanPatternInModule("client.dll", PATTERN_GETATTRIBDEFBYNAME_PTR, MASK_GETATTRIBDEFBYNAME_PTR);
	if (!funcptr)
		return;

	fn::CEconItemSchema__GetAttributeDefinitionByName = static_cast<fCEconItemSchema__GetAttributeDefinitionByName>(funcptr);
}

void InitSkinAttachmentFunctions() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_SPAWNSETSTATTRAK_PTR, MASK_SPAWNSETSTATTRAK_PTR);
	if (!funcptr)
		return;

	fn::SpawnAndSetStattrakEnt = static_cast<fSpawnAndSetStattrakEnt>(funcptr);

	funcptr = ScanPatternInModule("client.dll", PATTERN_SPAWNSETNAMETAG_PTR, MASK_SPAWNSETNAMETAG_PTR);
	if (!funcptr)
		return;

	fn::SpawnAndSetNametagEnt = static_cast<fSpawnAndSetNametagEnt>(funcptr);

	funcptr = ScanPatternInModule("client.dll", PATTERN_UPDATEVIEWMODELSTATTRAKATTACHMENTS_PTR, MASK_UPDATEVIEWMODELSTATTRAKATTACHMENTS_PTR);
	if (!funcptr)
		return;

	fn::UpdateViewmodelAttachments = static_cast<fUpdateViewmodelAttachments>(funcptr);
}

bool InitializeFunctions() {
	InitGetCEconItemSystem();
	InitAttribFunctions();
	InitGetCSWeaponDataFromItem();
	InitSkinFunctions();
	InitSkinAttachmentFunctions();

	return fn::CSource2Client__GetCCStrike15ItemSystem && fn::GetCSWeaponDataFromItem && fn::RegenerateWeaponSkin && fn::AllowSkinRegenForWeapon && fn::CEconItemView__SetAttributeValueByName
		&& fn::UpdateViewmodelAttachments && fn::SpawnAndSetStattrakEnt && fn::CEconItemSchema__GetAttributeDefinitionByName && fn::RegenerateAllWeaponSkins;
}