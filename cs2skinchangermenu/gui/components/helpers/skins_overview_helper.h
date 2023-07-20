#pragma once

#include <unordered_set>

// Gets the right wear image for the specified wear value
const char* ImageForFloat(float wear) {
	if (wear <= 0.15f)
		return "light";
	else if (wear <= 0.44f)
		return "medium";
	else
		return "heavy";
}

// Creates a display name for skin preference.
std::string CreateWeaponDisplayName(SkinPreference pref) {
	std::string displayName = "";

	std::optional<CCStrike15ItemDefinition*> itemDef = cache::weaponDefs.FindByKey(pref.weaponID);
	if (itemDef.has_value())
		displayName.append(
			cache::englishTranslations[&itemDef.value()->GetHudTranslationTag()[1]]
		);
	else
		displayName.append("Unknown");

	if (pref.paintKitID != -1) {
		displayName.append(" | ");

		std::optional<CPaintKit*> paintkitDef = cache::paintKits.FindByKey(pref.paintKitID);
		if (paintkitDef.has_value())
			displayName.append(
				cache::englishTranslations[&paintkitDef.value()->paintKitNameTag[1]]
			);
		else
			displayName.append("Unknown");
	}

	return displayName;
}

char* GetStickerKitTextureName(CStickerKit* stickerKit) {
	char* actualStickerTextureName = strrchr(stickerKit->stickerKitVpkFile, '/') + 1;
	return actualStickerTextureName;
}

bool ShouldIncludeWeaponInCombobox(const char* weaponCategoryName) {
	static std::unordered_set<uint64_t> bannedWeapons = {
		fnv::HashConst("weapon_knife"),
		fnv::HashConst("weapon_knifegg"),
		fnv::HashConst("weapon_bumpmine"),
		fnv::HashConst("weapon_snowball"),
		fnv::HashConst("weapon_smokegrenade"),
		fnv::HashConst("weapon_hegrenade"),
		fnv::HashConst("weapon_molotov"),
		fnv::HashConst("weapon_melee"),
		fnv::HashConst("weapon_tablet"),
		fnv::HashConst("weapon_breachcharge"),
		fnv::HashConst("weapon_fists"),
		fnv::HashConst("weapon_tagrenade"),
		fnv::HashConst("weapon_healthshot"),
		fnv::HashConst("weapon_incgrenade"),
		fnv::HashConst("weapon_flashbang"),
		fnv::HashConst("weapon_decoy"),
		fnv::HashConst("weapon_shield"),
		fnv::HashConst("weapon_zone_repulsor"),
		fnv::HashConst("weapon_c4")
	};

	if(!strstr(weaponCategoryName, "weapon_"))
		return false;

	if (bannedWeapons.contains(fnv::Hash(weaponCategoryName)))
		return false;

	return true;
}

std::string GetWeaponNameFromID(int id) {
	std::optional<CCStrike15ItemDefinition*> itemDef = cache::weaponDefs.FindByKey(id);
	if (!itemDef.has_value())
		return "N/A";

	return cache::englishTranslations[&itemDef.value()->GetHudTranslationTag()[1]];
}

std::string GetSkinNameForSkinPreference(SkinPreference pref) {
	std::string skinName = "";

	if (pref.paintKitID != -1) {
		std::optional<CPaintKit*> paintkitDef = cache::paintKits.FindByKey(pref.paintKitID);
		if (paintkitDef.has_value())
			skinName = cache::englishTranslations[&paintkitDef.value()->paintKitNameTag[1]];
	}

	return skinName;
}

std::string GetSkinImageNameForSkinPreference(SkinPreference pref) {
	std::string imageName = "";

	std::optional<CCStrike15ItemDefinition*> itemDef = cache::weaponDefs.FindByKey(pref.weaponID);
	if (itemDef.has_value())
		imageName.append(
			itemDef.value()->GetSubcategory()
		);

	if (pref.paintKitID != -1) {
		imageName.append("_");

		std::optional<CPaintKit*> paintkitDef = cache::paintKits.FindByKey(pref.paintKitID);
		if (paintkitDef.has_value())
			imageName.append(
				paintkitDef.value()->paintKitName
			);

		imageName.append("_");
		imageName.append(ImageForFloat(pref.wearValue));
	}

	return imageName;
}