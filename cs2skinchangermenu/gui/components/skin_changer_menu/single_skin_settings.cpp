#include "pch.h"
#include "functions.h"

#include "nuklear-gui/nuklear.h"
#include "gui/gui_setup.h"
#include "gui/gui_main.h"

#include "netvars/fnvhash.h"
#include "helpers/skins_helper_funcs.h"
#include "skin_changer/skins_cache.h"
#include "skin_changer/skins.h"
#include "cache.h"

#include <Shlwapi.h>

void LeftPanelMainInfoText(bool bIsGloves) {
	std::string displayName = CreateWeaponDisplayName(*stateSkinsOverviewMenu.currSkinPref);
	int PREFERENCE_ID = !bIsGloves ? stateSkinsOverviewMenu.currSkinPref->weaponID : skins::ID_GLOVE_PREFERENCE; // will be used to figure out where to place the skin if it is enabled

	nk_style_push_color(gui::nuklearCtx, &gui::nuklearCtx->style.window.fixed_background.data.color, nk_color(48, 48, 48, 255));
	if (nk_group_begin_titled(gui::nuklearCtx, "skin_main_info", displayName.c_str(), NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR)) {
		nk_style_pop_color(gui::nuklearCtx);

		struct nk_rect bounds = nk_layout_widget_bounds(gui::nuklearCtx);
		nk_layout_row_static(gui::nuklearCtx, 30, bounds.w - 20, 1);

		// skin enabled checkbox
		auto mapIter = skins_cache::activeLoadout.find(PREFERENCE_ID);
		bool bEnabled = mapIter != skins_cache::activeLoadout.end() && mapIter->second == stateSkinsOverviewMenu.currSkinPref;
		int bEnabledNow = static_cast<int>(!bEnabled); // nk_true is 0 for some shitty reason

		if (nk_checkbox_text(gui::nuklearCtx, "Skin enabled", strlen("Skin enabled"), &bEnabledNow)) {
			if (!bEnabled && !static_cast<bool>(bEnabledNow))
				skins_cache::activeLoadout[PREFERENCE_ID] = stateSkinsOverviewMenu.currSkinPref;

			if (bEnabled && static_cast<bool>(bEnabledNow) && mapIter != skins_cache::activeLoadout.end())
				skins_cache::activeLoadout.erase(mapIter);
		}

		// enabled for which team
		nk_layout_row_dynamic(gui::nuklearCtx, 30, 2);
		int bEnabledCT = static_cast<int>(!stateSkinsOverviewMenu.currSkinPref->enabledCT);
		int bEnabledT = static_cast<int>(!stateSkinsOverviewMenu.currSkinPref->enabledT);
		if (nk_checkbox_label(gui::nuklearCtx, "CT", &bEnabledCT))
			stateSkinsOverviewMenu.currSkinPref->enabledCT = !stateSkinsOverviewMenu.currSkinPref->enabledCT;

		if (nk_checkbox_label(gui::nuklearCtx, "T", &bEnabledT))
			stateSkinsOverviewMenu.currSkinPref->enabledT = !stateSkinsOverviewMenu.currSkinPref->enabledT;

		if (!bIsGloves) {
			nk_layout_row_static(gui::nuklearCtx, 30, bounds.w, 1);
			int bOverridePickedUpSkin = static_cast<int>(!stateSkinsOverviewMenu.currSkinPref->overridePickedUpWeaponSkin);
			if (nk_checkbox_text(gui::nuklearCtx, "Override picked up skins", strlen("Override picked up skins"), &bOverridePickedUpSkin))
				stateSkinsOverviewMenu.currSkinPref->overridePickedUpWeaponSkin = !stateSkinsOverviewMenu.currSkinPref->overridePickedUpWeaponSkin;
		}

		// weapon change list, includes gloves
		std::string weaponTranslatedName = GetTranslatedItemNameFromID(stateSkinsOverviewMenu.currSkinPref->weaponID);

		nk_layout_row_static(gui::nuklearCtx, 30, bounds.w, 1);
		if (nk_combo_begin_label(gui::nuklearCtx, weaponTranslatedName.c_str(), nk_vec2(bounds.w, 200))) {
			for (auto& weaponIter : cache::weaponDefs) {
				CCStrike15ItemDefinition* weaponDef = weaponIter.m_value;
				if (!weaponDef || !ShouldIncludeWeaponForSkin(weaponDef->GetMainCategory(), weaponDef->GetSubcategory()))
					continue;

				std::string weaponTranslatedName = GetTranslatedItemNameFromID(weaponDef->m_iItemDefinitionIndex);
				nk_layout_row_dynamic(gui::nuklearCtx, 25, 1);
				if (nk_combo_item_label(gui::nuklearCtx, weaponTranslatedName.c_str(), NK_TEXT_ALIGN_LEFT))
					ChangeWeaponForSkinPreference(stateSkinsOverviewMenu.currSkinPref, PREFERENCE_ID, weaponDef);
			}

			nk_combo_end(gui::nuklearCtx);
		}

		nk_group_end(gui::nuklearCtx);
	}
}

void SingleSkinSettingsLeftPanel(bool bIsGloves) {
	// LEFT SIDE DRAWING | skin, weapon name, quick/important settings
	if (nk_group_begin(gui::nuklearCtx, "singleskin_mainsettings", NK_WINDOW_NO_SCROLLBAR)) {
		struct nk_rect bounds = nk_layout_widget_bounds(gui::nuklearCtx);
		// skin view row (1 item)
		nk_layout_row_dynamic(gui::nuklearCtx, bounds.w, 1);

		std::string weaponTextureName = GetSkinImageNameForSkinPreference(*stateSkinsOverviewMenu.currSkinPref);
		TextureCache& weaponTexture = skins_cache::weaponSkins[fnv::Hash(weaponTextureName.c_str())];
		DrawImageBox(gui::nuklearCtx, weaponTexture);

		// next row, big box with weapon name and important settings
		LeftPanelMainInfoText(bIsGloves);

		nk_group_end(gui::nuklearCtx);
	}
}

void SingleSkinSettingsMiddleStickerPanel() {
	int MAX_STICKER_COUNT = 4;
	if (nk_group_begin(gui::nuklearCtx, "singleskin_stickers", 0)) {
		struct nk_rect bounds = nk_layout_widget_bounds(gui::nuklearCtx);

		for (int i = 0; i < MAX_STICKER_COUNT; i++) {
			nk_layout_row_static(gui::nuklearCtx, bounds.w, bounds.w, 1);
			Sticker sticker = stateSkinsOverviewMenu.currSkinPref->stickers[i];
			TextureCache* texture = &notAvailableSymbol;
			if (sticker.id != -1) {
				std::optional<CStickerKit*> stickerKit = cache::stickerKits.FindByKey(sticker.id);
				if (stickerKit.has_value())
					texture = &skins_cache::weaponSkins[fnv::Hash(GetStickerKitTextureName(stickerKit.value()))];
			}

			if (DrawImageBoxBtn(gui::nuklearCtx, "", *texture)) {
				stateSkinsOverviewMenu.modifyingSickerNum = i;
				stateSkinsOverviewMenu.SetWindow(Window::StickerSearch);
			}
		}

		nk_group_end(gui::nuklearCtx);
	}
}

void SingleSkinSettingsRightPanel(bool bIsGloves) {
	if (nk_group_begin(gui::nuklearCtx, "singleskin_settings", 0)) {
		// current paintkit row
		{
			nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, 25, 3);

			nk_layout_row_push(gui::nuklearCtx, 0.25f);
			nk_label(gui::nuklearCtx, "Current skin", NK_TEXT_ALIGN_LEFT);

			nk_layout_row_push(gui::nuklearCtx, 0.5f);
			std::string skinName = GetSkinNameForSkinPreference(*stateSkinsOverviewMenu.currSkinPref);
			int len = static_cast<int>(skinName.length()) + 1;
			nk_edit_string(gui::nuklearCtx, NK_TEXT_EDIT_MODE_VIEW, const_cast<char*>(skinName.c_str()), &len, len, nk_filter_default);

			nk_layout_row_push(gui::nuklearCtx, 0.25f);
			if (nk_button_label(gui::nuklearCtx, "Change")) {
				stateSkinsOverviewMenu.SetWindow(Window::SkinSearch);
			}

			nk_layout_row_end(gui::nuklearCtx);
		}

		// seed
		{
			nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, 25, 2);

			nk_layout_row_push(gui::nuklearCtx, 0.4f);
			nk_label(gui::nuklearCtx, "Skin pattern (seed)", NK_TEXT_ALIGN_LEFT);

			nk_layout_row_push(gui::nuklearCtx, 0.6f);
			char patternSeed[7] = { 0 }; // probably bottlenecks somewhere
			std::string pattern = std::to_string(stateSkinsOverviewMenu.currSkinPref->seed);
			strcpy_s(patternSeed, sizeof(patternSeed), pattern.c_str());

			nk_edit_string_zero_terminated(gui::nuklearCtx, NK_EDIT_FIELD, patternSeed, sizeof(patternSeed), nk_filter_decimal);
			if (strlen(patternSeed) > 0 && isdigit(patternSeed[0])) // fix for no characters or negative numbers
				stateSkinsOverviewMenu.currSkinPref->seed = std::stoi(patternSeed);
			else
				stateSkinsOverviewMenu.currSkinPref->seed = 0;

			nk_layout_row_end(gui::nuklearCtx);
		}

		// current wear
		{
			nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, 25, 3);

			nk_layout_row_push(gui::nuklearCtx, 0.2f);
			nk_label(gui::nuklearCtx, "Skin wear", NK_TEXT_ALIGN_LEFT);

			nk_layout_row_push(gui::nuklearCtx, 0.7f);
			nk_slider_float(gui::nuklearCtx, 0.0f, &stateSkinsOverviewMenu.currSkinPref->wearValue, 1.0f, 0.01f);

			nk_layout_row_push(gui::nuklearCtx, 0.1f);
			std::string wearStr = std::to_string(stateSkinsOverviewMenu.currSkinPref->wearValue);
			wearStr.resize(4, '0');
			nk_label(gui::nuklearCtx, wearStr.c_str(), NK_TEXT_ALIGN_CENTERED);

			nk_layout_row_end(gui::nuklearCtx);
		}

		if(!bIsGloves) {
			// stattrak
			nk_layout_row_static(gui::nuklearCtx, 25, 125, 1);

			int bEnabled = static_cast<int>(!stateSkinsOverviewMenu.currSkinPref->useStattrak);
			if (nk_checkbox_label(gui::nuklearCtx, "Enable StatTrak™", &bEnabled))
				stateSkinsOverviewMenu.currSkinPref->useStattrak = !stateSkinsOverviewMenu.currSkinPref->useStattrak;

			// kills count, if stattrak is enabled
			if (stateSkinsOverviewMenu.currSkinPref->useStattrak) {
				nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, 25, 2);

				nk_layout_row_push(gui::nuklearCtx, 0.3f);
				nk_label(gui::nuklearCtx, "StatTrak™ kills", NK_TEXT_ALIGN_LEFT);

				nk_layout_row_push(gui::nuklearCtx, 0.7f);
				char killsCount[7] = { 0 }; // probably bottlenecks somewhere
				std::string kills = std::to_string(stateSkinsOverviewMenu.currSkinPref->stattrakKills);
				strcpy_s(killsCount, sizeof(killsCount), kills.c_str());

				nk_edit_string_zero_terminated(gui::nuklearCtx, NK_EDIT_FIELD, killsCount, sizeof(killsCount), nk_filter_decimal);
				if (strlen(killsCount) > 0 && isdigit(killsCount[0])) // fix for no characters or negative numbers
					stateSkinsOverviewMenu.currSkinPref->stattrakKills = std::stoi(killsCount);
				else
					stateSkinsOverviewMenu.currSkinPref->stattrakKills = 0;

				nk_layout_row_end(gui::nuklearCtx);
			}

			// name tag
			nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, 25, 2);

			nk_layout_row_push(gui::nuklearCtx, 0.3f);
			nk_label(gui::nuklearCtx, "Name tag", NK_TEXT_ALIGN_LEFT);

			nk_layout_row_push(gui::nuklearCtx, 0.7f);
			nk_edit_string_zero_terminated(gui::nuklearCtx, NK_EDIT_FIELD, stateSkinsOverviewMenu.currSkinPref->nametag, sizeof(stateSkinsOverviewMenu.currSkinPref->nametag), nk_filter_default);

			nk_layout_row_end(gui::nuklearCtx);
		}

		nk_group_end(gui::nuklearCtx);
	}
}

void DrawSingleSkinSettings() {
	if (nk_group_begin(gui::nuklearCtx, "singleskin", 0)) {
		nk_layout_row_static(gui::nuklearCtx, 25, 75, 1);
		if (nk_button_label(gui::nuklearCtx, "<- Back"))
			stateSkinsOverviewMenu.SetWindow(Window::SkinsList); // next frame will have the previous window

		std::optional<CCStrike15ItemDefinition*> itemDefOpt = cache::weaponDefs.FindByKey(stateSkinsOverviewMenu.currSkinPref->weaponID);
		if (itemDefOpt.has_value()) {
			float SECOND_ROW_WIDTH = 0.16f;

			CCStrike15ItemDefinition* itemDef = itemDefOpt.value();
			bool bIsGloves = static_cast<bool>(strstr(itemDef->GetSubcategory(), "gloves"));

			nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, nk_window_get_height(gui::nuklearCtx) - 50, 3);
			nk_layout_row_push(gui::nuklearCtx, 0.3f + SECOND_ROW_WIDTH * static_cast<int>(bIsGloves));
			SingleSkinSettingsLeftPanel(bIsGloves);

			// middle panel for stickers
			if (!bIsGloves) {
				nk_layout_row_push(gui::nuklearCtx, SECOND_ROW_WIDTH);
				SingleSkinSettingsMiddleStickerPanel();
			}

			// RIGHT SIDE DRAWING | skin choice, stattrak, wear, float, name
			nk_layout_row_push(gui::nuklearCtx, 0.54f);
			SingleSkinSettingsRightPanel(bIsGloves);

			nk_layout_row_end(gui::nuklearCtx);
		}

		nk_group_end(gui::nuklearCtx);
	}
}