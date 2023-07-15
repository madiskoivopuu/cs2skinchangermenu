#include "pch.h"
#include "components.h"
#include "gui/gui_main.h"

#include <iostream>

void DrawSideBar() {
	float sidebarWidth = 0.25 * nk_window_get_width(gui::nuklearCtx);

    nk_layout_row_static(gui::nuklearCtx, nk_window_get_height(gui::nuklearCtx), sidebarWidth, 1);
	if (nk_group_begin(gui::nuklearCtx, "sbar", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_layout_row_static(gui::nuklearCtx, 25, sidebarWidth*0.95, 1);

        nk_button_label(gui::nuklearCtx, "Skin Changer");
		nk_group_end(gui::nuklearCtx);
	}

	// layout_row continues
}
