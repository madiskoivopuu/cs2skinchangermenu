#pragma once

#include "skin_changer/SkinPreference.h"
#include "skin_changer/TextureCache.h"

#include <unordered_map>

namespace skins {
	const int ID_GLOVE_PREFERENCE = 9005;
	const int ID_KNIFE_PREFERENCE = 9006;
}

void ApplySkinsCallback(void* rcx);
