#pragma once

#include "sdk/econ/CCStrike15ItemSystem.h"
#include "sdk/econ/C_EconItemView.h"
#include "sdk/econ/CEconItemAttributeDefinition.h"
#include "sdk/gameclasses/CWeaponCSBaseVData.h"
#include "sdk/gameclasses/C_WeaponCSBase.h"

// In-game function typedefs
using fGetCCStrike15ItemSystem = CCStrike15ItemSystem* (__fastcall*)(); // CSource2Client::
using fGetCSWeaponDataFromItem = CCSWeaponBaseVData* (__fastcall*)(C_EconItemView*);
using fRegenerateWeaponSkin = void (__fastcall*)(C_WeaponCSBase*);
using fRegenerateAllWeaponSkins = void (__fastcall*)();
using fAllowSkinRegenForWeapon = void (__fastcall*)(void*, bool);
using fCEconItemView__SetAttributeValueByName = void (__fastcall*)(C_EconItemView*, const char*, float);
using fSpawnAndSetStattrakEnt = void (__fastcall*)(void*); // 1st argument is m_hStattrakAttachment*
using fUpdateViewmodelAttachments = void (__fastcall*)(void*, C_WeaponCSBase*); // 1st argument is C_CSGOViewModel*
using fSpawnAndSetNametagEnt = void (__fastcall*)(void*); // 1st argument is m_hNametagAttachment*
using fCEconItemSchema__GetAttributeDefinitionByName = CEconItemAttributeDefinition* (__fastcall*)(void*, char*);
using fGetNextSceneEventIDOffset = int64_t (__fastcall*)(void*, void*, int, bool);
using fCGameSceneNode__SetMeshGroupMask = void (__fastcall*)(void*, uint64_t);
using fCPaintKit__IsUsingLegacyModel = bool (__fastcall*)(const char*);
using fCRenderGameSystem__GetNthViewMatrix = float* (__fastcall*)(void*, int); // argument 1 can be empty as it is not used

namespace fn {
	extern fGetCCStrike15ItemSystem CSource2Client__GetCCStrike15ItemSystem;
	extern fGetCSWeaponDataFromItem GetCSWeaponDataFromItem;
	extern fRegenerateWeaponSkin RegenerateWeaponSkin;
	extern fAllowSkinRegenForWeapon AllowSkinRegenForWeapon;
	extern fCEconItemView__SetAttributeValueByName CEconItemView__SetAttributeValueByName;
	extern fSpawnAndSetStattrakEnt SpawnAndSetStattrakEnt;
	extern fSpawnAndSetNametagEnt SpawnAndSetNametagEnt;
	extern fUpdateViewmodelAttachments UpdateViewmodelAttachments;
	extern fCEconItemSchema__GetAttributeDefinitionByName CEconItemSchema__GetAttributeDefinitionByName;
	extern fRegenerateAllWeaponSkins RegenerateAllWeaponSkins;
	extern fGetNextSceneEventIDOffset GetNextSceneEventIDOffset;
	extern fCGameSceneNode__SetMeshGroupMask CGameSceneNode__SetMeshGroupMask;
	extern fCPaintKit__IsUsingLegacyModel CPaintKit__IsUsingLegacyModel;
	extern fCRenderGameSystem__GetNthViewMatrix CRenderGameSystem__GetNthViewMatrix;
}

bool InitializeFunctions();