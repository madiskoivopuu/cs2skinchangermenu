#pragma once

#include <unordered_map>
#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "TextureCache.h"

namespace skins_cache {
    extern std::unordered_map<uint32_t, std::vector<uint32_t>> paintkitsForWeapons;
    extern std::vector<TextureCache> weaponSkins;
    extern vpktool::VPK skinsPakFile;
}

bool LoadPaintkitsForWeapons(CUtlMap<char*, CEconItemSetDefinition> itemSets);
bool LoadWeaponTextureThumbnails();