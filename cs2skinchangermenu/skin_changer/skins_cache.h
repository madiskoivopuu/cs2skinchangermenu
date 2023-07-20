#pragma once

#include <unordered_map>
#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "TextureCache.h"
#include "SkinPreference.h"

namespace skins_cache {
    extern std::unordered_map<uint32_t, std::vector<uint32_t>> paintkitsForWeapons;
    extern std::unordered_map<uint64_t, TextureCache> weaponSkins;
    extern vpktool::VPK skinsPakFile;
    extern std::unordered_map<uint32_t, SkinPreference*> activeLoadout;
	extern std::vector<SkinPreference> loadoutAllPresets;
}

SkinPreference* CreateAndActivateNewPreference();
bool LoadPaintkitsForWeapons(CUtlMap<char*, CEconItemSetDefinition> itemSets);
bool LoadWeaponTextureThumbnails();
bool LoadUserSkinPreferences();
void ChangeWeaponForSkinPreference(SkinPreference* pref, int newWeaponID);