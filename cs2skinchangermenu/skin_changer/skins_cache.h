#pragma once

#include <unordered_map>
#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "TextureCache.h"

void LoadPaintkitsForWeapons(CUtlMap<char*, CEconItemSetDefinition> itemSets);
std::vector<TextureCache> LoadWeaponTextureThumbnails();