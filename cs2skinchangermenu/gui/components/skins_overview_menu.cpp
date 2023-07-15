#include "pch.h"
#include "components.h"

#include "gui/gui_main.h"
#include "nuklear-gui/nuklear.h"

void DrawSkinsOverview() {
    int i;
    static int selected[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    nk_layout_row_static(gui::nuklearCtx, 50, 50, 4);
    for (i = 0; i < 16; ++i) {
        if (nk_selectable_label(gui::nuklearCtx, "Z", NK_TEXT_CENTERED, &selected[i])) {
            int x = (i % 4), y = i / 4;
            if (x > 0) selected[i - 1] ^= 1;
            if (x < 3) selected[i + 1] ^= 1;
            if (y > 0) selected[i - 4] ^= 1;
            if (y < 3) selected[i + 4] ^= 1;
        }
    }
}