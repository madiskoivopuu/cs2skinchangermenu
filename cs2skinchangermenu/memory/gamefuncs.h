#pragma once

#include "sdk/econ/CCStrike15ItemSystem.h"
#include "sdk/econ/C_EconItemView.h"
#include "sdk/gameclasses/CWeaponCSBaseVData.h"
#include "sdk/gameclasses/C_WeaponCSBase.h"

// In-game function typedefs
using fGetCCStrike15ItemSystem = CCStrike15ItemSystem* (__fastcall*)(); // CSource2Client::
using fGetCSWeaponDataFromItem = CWeaponCSBaseVData* (__fastcall*)(C_EconItemView*);
using fRegenerateWeaponSkin = void (__fastcall*)(C_WeaponCSBase*);
using fAllowSkinRegenForWeapon = void (__fastcall*)(void*, bool);
using fCEconItemView__SetAttributeValueByName = void(__fastcall*)(C_EconItemView*, const char*, float);

namespace fn {
	extern fGetCCStrike15ItemSystem CSource2Client__GetCCStrike15ItemSystem;
	extern fGetCSWeaponDataFromItem GetCSWeaponDataFromItem;
	extern fRegenerateWeaponSkin RegenerateWeaponSkin;
	extern fAllowSkinRegenForWeapon AllowSkinRegenForWeapon;
	extern fCEconItemView__SetAttributeValueByName CEconItemView__SetAttributeValueByName;
}

bool InitializeFunctions();