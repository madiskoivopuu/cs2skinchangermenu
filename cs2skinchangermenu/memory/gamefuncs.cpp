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
	fGetNextSceneEventIDOffset GetNextSceneEventIDOffset = nullptr;
	fCGameSceneNode__SetMeshGroupMask CGameSceneNode__SetMeshGroupMask = nullptr;
	fCPaintKit__IsUsingLegacyModel CPaintKit__IsUsingLegacyModel = nullptr;
	fCRenderGameSystem__GetNthViewMatrix CRenderGameSystem__GetNthViewMatrix = nullptr;
}

void InitNextSceneIDOffset() {
	uint8_t* relCallPtr = reinterpret_cast<uint8_t*>(ScanPatternInModule("client.dll", PATTERN_GETNEXTSCENEEVENTOFFSET_PTR_OFFSET, MASK_GETNEXTSCENEEVENTOFFSET_PTR_OFFSET));
	if (!relCallPtr)
		return;

	// set up allowskinregen func by it's relative call addy
	int32_t offsetFromInstruction = *reinterpret_cast<int32_t*>(relCallPtr + OFFSETSTART_GETNEXTSCENEEVENTOFFSET);
	fn::GetNextSceneEventIDOffset = reinterpret_cast<fGetNextSceneEventIDOffset>(relCallPtr + OFFSETEND_GETNEXTSCENEEVENTOFFSET + offsetFromInstruction);
}

void InitGetCEconItemSystem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETECONITEMSYS_PTR, MASK_GETECONITEMSYS_PTR);
	fn::CSource2Client__GetCCStrike15ItemSystem = static_cast<fGetCCStrike15ItemSystem>(funcptr);
}

void InitGetCSWeaponDataFromItem() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETCSWEAPONDATA_PTR, MASK_GETCSWEAPONDATA_PTR);
	fn::GetCSWeaponDataFromItem = static_cast<fGetCSWeaponDataFromItem>(funcptr);
}

// initiate allowskinregen and regenweaponskin functions, legacy model check func
void InitSkinFunctions() {
	// set up regenerate all weapon skins func
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_REGENALLWEAPONSKINS_PTR, MASK_REGENALLWEAPONSKINS_PTR);
	if (!funcptr)
		return;
	fn::RegenerateAllWeaponSkins = (fRegenerateAllWeaponSkins)funcptr;

	uint8_t* relCallPtr = reinterpret_cast<uint8_t*>(ScanPatternInModule("client.dll", PATTERN_REGENWEAPONSKIN_PTR, MASK_REGENWEAPONSKIN_PTR));
	if (relCallPtr) {
		// set up allowskinregen func by it's relative call addy
		int32_t offsetFromInstruction = *reinterpret_cast<int32_t*>(relCallPtr + OFFSETSTART_ALLOWSKINREGEN);
		fn::AllowSkinRegenForWeapon = reinterpret_cast<fAllowSkinRegenForWeapon>(relCallPtr + OFFSETEND_ALLOWSKINREGEN + offsetFromInstruction);

		// set up regenweaponskin func
		int32_t offsetFromInstructionSigned = *reinterpret_cast<int32_t*>(relCallPtr + OFFSETSTART_REGENWEAPONSKIN);
		fn::RegenerateWeaponSkin = reinterpret_cast<fRegenerateWeaponSkin>(relCallPtr + OFFSETEND_REGENWEAPONSKIN + offsetFromInstructionSigned);
	}

	// set up is using legacy model stuff
	void* funcptr2 = ScanPatternInModule("client.dll", PATTERN_ISUSINGLEGACYMODEL_PTR, MASK_ISUSINGLEGACYMODEL_PTR);
	if (funcptr2)
		fn::CPaintKit__IsUsingLegacyModel = reinterpret_cast<fCPaintKit__IsUsingLegacyModel>(funcptr2);
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
	fn::SpawnAndSetStattrakEnt = static_cast<fSpawnAndSetStattrakEnt>(funcptr);

	funcptr = ScanPatternInModule("client.dll", PATTERN_SPAWNSETNAMETAG_PTR, MASK_SPAWNSETNAMETAG_PTR);
	fn::SpawnAndSetNametagEnt = static_cast<fSpawnAndSetNametagEnt>(funcptr);

	funcptr = ScanPatternInModule("client.dll", PATTERN_UPDATEVIEWMODELSTATTRAKATTACHMENTS_PTR, MASK_UPDATEVIEWMODELSTATTRAKATTACHMENTS_PTR);
	fn::UpdateViewmodelAttachments = static_cast<fUpdateViewmodelAttachments>(funcptr);
}

void InitSetMeshGroupMask() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_SETMESHGROUPMASK_PTR, MASK_SETMESHGROUPMASK_PTR);
	if (!funcptr)
		return;

	fn::CGameSceneNode__SetMeshGroupMask = reinterpret_cast<fCGameSceneNode__SetMeshGroupMask>(funcptr);
}

void InitViewMatrixGetter() {
	void* funcptr = ScanPatternInModule("client.dll", PATTERN_GETNTHVIEWMATRIX_PTR, MASK_GETNTHVIEWMATRIX_PTR);

	fn::CRenderGameSystem__GetNthViewMatrix = static_cast<fCRenderGameSystem__GetNthViewMatrix>(funcptr);
}

bool InitializeFunctions() {
	InitGetCEconItemSystem();
	InitAttribFunctions();
	InitGetCSWeaponDataFromItem();
	InitSkinFunctions();
	InitSkinAttachmentFunctions();
	InitNextSceneIDOffset();
	InitSetMeshGroupMask();
	InitViewMatrixGetter();

	return fn::CSource2Client__GetCCStrike15ItemSystem && fn::GetCSWeaponDataFromItem && fn::RegenerateWeaponSkin && fn::AllowSkinRegenForWeapon && fn::CEconItemView__SetAttributeValueByName
		&& fn::UpdateViewmodelAttachments && fn::SpawnAndSetStattrakEnt && fn::CEconItemSchema__GetAttributeDefinitionByName && fn::RegenerateAllWeaponSkins && fn::GetNextSceneEventIDOffset
		&& fn::CGameSceneNode__SetMeshGroupMask && fn::CPaintKit__IsUsingLegacyModel && fn::CRenderGameSystem__GetNthViewMatrix;
}