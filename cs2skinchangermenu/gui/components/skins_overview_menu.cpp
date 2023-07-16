#include "pch.h"
#include "components.h"

#include "gui/gui_setup.h"
#include "gui/gui_main.h"
#include "skin_changer/skins_cache.h"

#include "nuklear-gui/nuklear.h"

enum class Window : int {
    SkinsList,
    SingleSkinSettings
};
struct {
    Window currActiveWindow = Window::SkinsList;
} windowState;

void DrawSingleSkinSettings() {

}

void DrawSkinsOverview() {
    if (nk_group_begin(gui::nuklearCtx, "sbar", 0)) {
        // 5 rectangular items per row
        float skinPrefBoxSize = (0.75f * nk_window_get_width(gui::nuklearCtx)) / 5;
        nk_layout_row_dynamic(gui::nuklearCtx, skinPrefBoxSize, 4);

        void* loadedTex = skins_cache::weaponSkins[2547].Get();
        if (loadedTex)
            DrawWeaponSkinButton(gui::nuklearCtx, nk_image_ptr(loadedTex));

        //nk_select_image_label(gui::nuklearCtx, nk_image_ptr(loadedTex), "select this", NK_TEXT_ALIGN_CENTERED, false);
    /*static int selected[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    nk_layout_row_dynamic(gui::nuklearCtx, skinPrefBoxSize, 5);
    for (int i = 0; i < 16; ++i) {
        if (nk_selectable_label(gui::nuklearCtx, "Z", NK_TEXT_CENTERED, &selected[i])) {
            int x = (i % 4), y = i / 4;
            if (x > 0) selected[i - 1] ^= 1;
            if (x < 3) selected[i + 1] ^= 1;
            if (y > 0) selected[i - 4] ^= 1;
            if (y < 3) selected[i + 4] ^= 1;
        }
    }*/

        nk_group_end(gui::nuklearCtx);
    }
}

void DrawSkinChangerMenu() {
    switch (windowState.currActiveWindow) {
    case Window::SkinsList:
        DrawSkinsOverview();
        break;
    case Window::SingleSkinSettings:

        break;
    }
}