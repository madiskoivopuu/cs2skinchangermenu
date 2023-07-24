#pragma once

#include "skin_changer/SkinPreference.h"
#include "sdk/econ/CStickerKit.h"
#include <string>

// searches the weapon first by its econ def index
bool ShouldIncludeWeaponForSkin_ID(int itemDefIndex);

bool ShouldIncludeWeaponForSkin(const char* weaponCategoryName, const char* subcategoryName);

bool IsStickerKit(CStickerKit* stickerKit);

// Gets the right wear image for the specified wear value
const char* ImageForFloat(float wear);

// Creates a display name for skin preference.
std::string CreateWeaponDisplayName(SkinPreference pref);

char* GetStickerKitTextureName(CStickerKit* stickerKit);
// Gets translated name for item ID
std::string GetTranslatedItemNameFromID(int id);

std::string GetSkinNameForSkinPreference(SkinPreference pref);

std::string GetSkinImageNameForSkinPreference(SkinPreference pref);