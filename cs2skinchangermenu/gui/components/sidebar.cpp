#include "pch.h"
#include "components.h"
#include "gui/gui_main.h"
#include "skin_changer/skins_cache.h"

#include "nuklear-gui/nuklear.h"

#include <iostream>

void DrawSideBar(float sidebarWidth) {
	if (nk_group_begin(gui::nuklearCtx, "sbar", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_layout_row_static(gui::nuklearCtx, 25, sidebarWidth*0.95, 1);

        nk_button_label(gui::nuklearCtx, "Skin Changer");
		nk_group_end(gui::nuklearCtx);
	}

	// layout_row continues
}
