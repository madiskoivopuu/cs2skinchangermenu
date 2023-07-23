#include "pch.h"
#include "functions.h"

#include "nuklear-gui/nuklear.h"
#include "gui/gui_setup.h"
#include "gui/gui_main.h"

#include "helpers/skins_helper_funcs.h"
#include "skin_changer/skins_cache.h"
#include "cache.h"
#include "netvars/fnvhash.h"

void DrawSavedSkinsList() {
	int SKINS_PER_ROW = 4;

	if (nk_group_begin(gui::nuklearCtx, "skins_menu", 0)) {
		// 5 rectangular items per row
		struct nk_rect bounds = nk_layout_widget_bounds(gui::nuklearCtx);
		float skinPrefBoxSize = bounds.w / SKINS_PER_ROW;
		nk_layout_row_dynamic(gui::nuklearCtx, skinPrefBoxSize, SKINS_PER_ROW);

		for (SkinPreference& pref : skins_cache::loadoutAllPresets) {
			std::string displayName = CreateWeaponDisplayName(pref);
			std::string weaponTextureName = GetSkinImageNameForSkinPreference(pref);

			TextureCache& loadedTex = skins_cache::weaponSkins[fnv::Hash(weaponTextureName.c_str())];
			if (DrawImageBoxBtn(gui::nuklearCtx, displayName, loadedTex)) {
				stateSkinsOverviewMenu.SetWindow(Window::SingleSkinSettings);
				stateSkinsOverviewMenu.currSkinPref = &pref;
			}
		}

		if (DrawImageBoxBtn(gui::nuklearCtx, "Add new skin", plusSymbol)) {
			stateSkinsOverviewMenu.SetWindow(Window::SingleSkinSettings);
			stateSkinsOverviewMenu.currSkinPref = CreateAndActivateNewPreference();
		}

		nk_group_end(gui::nuklearCtx);
	}
}