#include "pch.h"

#include "functions.h"

#include "nuklear-gui/nuklear.h"
#include "gui/gui_setup.h"
#include "gui/gui_main.h"

#include "helpers/skins_helper_funcs.h"
#include "skin_changer/skins_cache.h"
#include "cache.h"
#include "netvars/fnvhash.h"

#include <Shlwapi.h>

void DrawSkinsSearch() {
	int STICKERS_PER_ROW = 4;
	int SEARCHBAR_HEIGHT = 25;

	if (nk_group_begin(gui::nuklearCtx, "skins_searchbar", NK_WINDOW_NO_SCROLLBAR)) {
		// header with back button and search
		{
			nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, SEARCHBAR_HEIGHT, 4);
			nk_layout_row_push(gui::nuklearCtx, 0.1f);
			if (nk_button_label(gui::nuklearCtx, "<- Back"))
				stateSkinsOverviewMenu.PrevWindow();

			nk_layout_row_push(gui::nuklearCtx, 0.2f);
			nk_label(gui::nuklearCtx, "", 0);

			nk_layout_row_push(gui::nuklearCtx, 0.1f);
			nk_label(gui::nuklearCtx, "Search", NK_TEXT_ALIGN_LEFT);

			nk_layout_row_push(gui::nuklearCtx, 0.6f);
			nk_edit_string_zero_terminated(gui::nuklearCtx, NK_EDIT_FIELD, stateSkinsOverviewMenu.searchString, sizeof(stateSkinsOverviewMenu.searchString), nk_filter_default);

			nk_layout_row_end(gui::nuklearCtx);
		}

		// skins list
		float listHeight = nk_window_get_height(gui::nuklearCtx) - SEARCHBAR_HEIGHT - 4 * gui::nuklearCtx->style.window.padding.y;
		nk_layout_row_dynamic(gui::nuklearCtx, listHeight, 1);
		if (nk_group_begin(gui::nuklearCtx, "skins_list", NK_WINDOW_MOVABLE)) {
			struct nk_rect bounds = nk_layout_widget_bounds(gui::nuklearCtx);
			float skinBoxSize = bounds.w / STICKERS_PER_ROW;
			nk_layout_row_dynamic(gui::nuklearCtx, skinBoxSize, STICKERS_PER_ROW);

			if (DrawImageBoxBtn(gui::nuklearCtx, "None", notAvailableSymbol)) {
				stateSkinsOverviewMenu.currSkinPref->paintKitID = -1;
				stateSkinsOverviewMenu.PrevWindow();
			}

			std::vector<uint32_t>& paintkitIDs = skins_cache::paintkitsForWeapons[stateSkinsOverviewMenu.currSkinPref->weaponID];
			SkinPreference prefCopy = *stateSkinsOverviewMenu.currSkinPref;
			for (uint32_t id : paintkitIDs) {
				if (id <= 1)
					continue;
				prefCopy.paintKitID = id;

				std::optional<CPaintKit*> paintKit = cache::paintKits.FindByKey(id);
				if (!paintKit.has_value())
					continue;

				std::string paintKitTranslatedName = cache::englishTranslations[&paintKit.value()->paintKitNameTag[1]];
				if (strlen(stateSkinsOverviewMenu.searchString) > 0 && !StrStrIA(paintKitTranslatedName.c_str(), stateSkinsOverviewMenu.searchString))
					continue;

				// display sticker as button
				std::string paintKitTextureName = GetSkinImageNameForSkinPreference(prefCopy);
				TextureCache& stickerTexture = skins_cache::weaponSkins[fnv::Hash(paintKitTextureName.c_str())];
				if (DrawImageBoxBtn(gui::nuklearCtx, paintKitTranslatedName, stickerTexture)) {
					stateSkinsOverviewMenu.currSkinPref->paintKitID = id;
					stateSkinsOverviewMenu.PrevWindow();
				}
			}
			nk_group_end(gui::nuklearCtx);
		}

		nk_group_end(gui::nuklearCtx);
	}
}