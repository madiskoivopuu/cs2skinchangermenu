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

void DrawStickerSearch() {
	int STICKERS_PER_ROW = 4;
	int SEARCHBAR_HEIGHT = 25;

	if (nk_group_begin(gui::nuklearCtx, "sticker_searchbar", NK_WINDOW_NO_SCROLLBAR)) {
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

		// stickers list
		float listHeight = nk_window_get_height(gui::nuklearCtx) - SEARCHBAR_HEIGHT - 4 * gui::nuklearCtx->style.window.padding.y;
		nk_layout_row_dynamic(gui::nuklearCtx, listHeight, 1);
		if (nk_group_begin(gui::nuklearCtx, "stickers_list", NK_WINDOW_MOVABLE)) {
			struct nk_rect bounds = nk_layout_widget_bounds(gui::nuklearCtx);
			float stickerBoxSize = bounds.w / STICKERS_PER_ROW;
			nk_layout_row_dynamic(gui::nuklearCtx, stickerBoxSize, STICKERS_PER_ROW);

			if (DrawImageBoxBtn(gui::nuklearCtx, "None", notAvailableSymbol)) {
				stateSkinsOverviewMenu.currSkinPref->stickers[stateSkinsOverviewMenu.modifyingSickerNum].id = -1;
				stateSkinsOverviewMenu.PrevWindow();
			}

			for (CUtlMap<int, CStickerKit*>::Node_t stickerKitNode : cache::stickerKits) {
				CStickerKit* stickerKit = stickerKitNode.m_value;
				if (stickerKit->id <= 1)
					continue;

				std::string stickerKitTranslatedName = cache::englishTranslations[&stickerKit->stickerKitNameTranslationTag[1]];
				if (strlen(stateSkinsOverviewMenu.searchString) > 0 && !StrStrIA(stickerKitTranslatedName.c_str(), stateSkinsOverviewMenu.searchString))
					continue;

				if (!IsStickerKit(stickerKit))
					continue;

				// display sticker as button

				TextureCache& stickerTexture = skins_cache::weaponSkins[fnv::Hash(GetStickerKitTextureName(stickerKit))];
				if (DrawImageBoxBtn(gui::nuklearCtx, stickerKitTranslatedName, stickerTexture)) {
					stateSkinsOverviewMenu.currSkinPref->stickers[stateSkinsOverviewMenu.modifyingSickerNum].id = stickerKit->id;
					stateSkinsOverviewMenu.PrevWindow();
				}
			}
			nk_group_end(gui::nuklearCtx);
		}

		nk_group_end(gui::nuklearCtx);
	}
}