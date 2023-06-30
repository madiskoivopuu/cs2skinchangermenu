#include "pch.h"
#include "components.h"
#include "gui/gui_main.h"

#include <iostream>

void DrawSideBar() {
	nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, nk_window_get_height(gui::nuklearCtx), 2);
	//nk_layout_row_push(gui::nuklearCtx, 0.25f);

	//nk_button_label(gui::nuklearCtx, "button");

	nk_layout_row_push(gui::nuklearCtx, 0.25f);
    if (nk_group_begin(gui::nuklearCtx, "1", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
        nk_group_end(gui::nuklearCtx);
    }

	nk_layout_row_push(gui::nuklearCtx, 0.25f);
	nk_label(gui::nuklearCtx, "aaaabb", NK_TEXT_ALIGN_CENTERED);

	//nk_layout_row_end(gui::nuklearCtx);
}
