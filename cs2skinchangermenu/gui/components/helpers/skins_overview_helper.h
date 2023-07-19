#pragma once

// Creates a display name for skin preference.
std::string CreateDisplayName(SkinPreference pref) {
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

// Gets the right wear image for the specified wear value
const char* ImageForFloat(float wear) {
	if (wear <= 0.15f)
		return "light";
	else if (wear <= 0.44f)
		return "medium";
	else
		return "heavy";
}

char* GetStickerKitTextureName(CStickerKit* stickerKit) {
	char* actualStickerTextureName = strrchr(stickerKit->stickerKitVpkFile, '/') + 1;
	return actualStickerTextureName;
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