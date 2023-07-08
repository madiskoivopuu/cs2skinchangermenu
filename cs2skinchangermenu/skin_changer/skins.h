#pragma once

#include "skin_changer/SkinPreference.h"
#include "skin_changer/TextureCache.h"

#include <unordered_map>

namespace skins {
	extern std::unordered_map<uint32_t, SkinPreference> loadout;
}

void ApplySkinsCallback();

