#include "pch.h"
#include "hooks.h"
#include "gui/gui.h"

#include <iostream>

namespace hooks {
	fIDXGISwapChain__Present oIDXGISwapChain__Present = nullptr;
	std::unique_ptr<Hook> scPresentHook;
}



#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
/////////////////////////
// 
// HOOKED FUNCTIONS...
//
/////////////////////////
void __stdcall HookPresent(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags) {

    // initialization phase
    if (!guiInitFromHook) {
        // actual device we want
        chain->GetDevice(__uuidof(device), reinterpret_cast<void**>(&device));
        device->GetImmediateContext(&context);

        D3D11_VIEWPORT vpOld;
        UINT nViewPorts = 1;

		// make gui render at the front
		ID3D11Texture2D* renderTargetTexture = nullptr;
		if (SUCCEEDED(chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&renderTargetTexture))))
		{
			device->CreateRenderTargetView(renderTargetTexture, NULL, &g_pRenderTargetView);
			renderTargetTexture->Release();
		}

        context->RSGetViewports(&nViewPorts, &vpOld);
        nuklearCtx = nk_d3d11_init(device, vpOld.Width, vpOld.Height, MAX_VERTEX_BUFFER, MAX_INDEX_BUFFER);

        struct nk_font_atlas* atlas;
        nk_d3d11_font_stash_begin(&atlas);
        nk_d3d11_font_stash_end();
    }

	MSG msg;
	nk_input_begin(nuklearCtx);
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		nk_d3d11_handle_event(window, msg.message, msg.wParam, msg.lParam);
	}
	nk_input_end(nuklearCtx);

	context->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

    struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

	static int show_menu = nk_true;
	static int titlebar = nk_true;
	static int border = nk_true;
	static int resize = nk_true;
	static int movable = nk_true;
	static int no_scrollbar = nk_false;
	static int scale_left = nk_false;
	static nk_flags window_flags = 0;
	static int minimizable = nk_true;

	/* popups */
	static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
	static int show_app_about = nk_false;

	/* window flags */
	window_flags = 0;
	nuklearCtx->style.window.header.align = header_align;
	if (border) window_flags |= NK_WINDOW_BORDER;
	if (resize) window_flags |= NK_WINDOW_SCALABLE;
	if (movable) window_flags |= NK_WINDOW_MOVABLE;
	if (no_scrollbar) window_flags |= NK_WINDOW_NO_SCROLLBAR;
	if (scale_left) window_flags |= NK_WINDOW_SCALE_LEFT;
	if (minimizable) window_flags |= NK_WINDOW_MINIMIZABLE;

	if (nk_begin(nuklearCtx, "Overview", nk_rect(10, 10, 300, 400), window_flags))
	{
		if (show_app_about)
		{
			/* about popup */
			static struct nk_rect s = { 20, 100, 300, 190 };
			if (nk_popup_begin(nuklearCtx, NK_POPUP_STATIC, "About", NK_WINDOW_CLOSABLE, s))
			{
				nk_layout_row_dynamic(nuklearCtx, 20, 1);
				nk_label(nuklearCtx, "Nuklear", NK_TEXT_LEFT);
				nk_label(nuklearCtx, "By Micha Mettke", NK_TEXT_LEFT);
				nk_label(nuklearCtx, "nuklear is licensed under the public domain License.", NK_TEXT_LEFT);
				nk_popup_end(nuklearCtx);
			}
			else show_app_about = nk_false;
		}

		/* window flags */
		if (nk_tree_push(nuklearCtx, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
			nk_layout_row_dynamic(nuklearCtx, 30, 2);
			nk_checkbox_label(nuklearCtx, "Titlebar", &titlebar);
			nk_checkbox_label(nuklearCtx, "Menu", &show_menu);
			nk_checkbox_label(nuklearCtx, "Border", &border);
			nk_checkbox_label(nuklearCtx, "Resizable", &resize);
			nk_checkbox_label(nuklearCtx, "Movable", &movable);
			nk_checkbox_label(nuklearCtx, "No Scrollbar", &no_scrollbar);
			nk_checkbox_label(nuklearCtx, "Minimizable", &minimizable);
			nk_checkbox_label(nuklearCtx, "Scale Left", &scale_left);
			nk_tree_pop(nuklearCtx);
		}

		if (nk_tree_push(nuklearCtx, NK_TREE_TAB, "Widgets", NK_MINIMIZED))
		{
			enum options { A, B, C };
			static int checkbox;
			static int option;
			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Text", NK_MINIMIZED))
			{
				/* Text Widgets */
				nk_layout_row_dynamic(nuklearCtx, 20, 1);
				nk_label(nuklearCtx, "Label aligned left", NK_TEXT_LEFT);
				nk_label(nuklearCtx, "Label aligned centered", NK_TEXT_CENTERED);
				nk_label(nuklearCtx, "Label aligned right", NK_TEXT_RIGHT);
				nk_label_colored(nuklearCtx, "Blue text", NK_TEXT_LEFT, nk_rgb(0, 0, 255));
				nk_label_colored(nuklearCtx, "Yellow text", NK_TEXT_LEFT, nk_rgb(255, 255, 0));
				nk_text(nuklearCtx, "Text without /0", 15, NK_TEXT_RIGHT);

				nk_layout_row_static(nuklearCtx, 100, 200, 1);
				nk_label_wrap(nuklearCtx, "This is a very long line to hopefully get this text to be wrapped into multiple lines to show line wrapping");
				nk_layout_row_dynamic(nuklearCtx, 100, 1);
				nk_label_wrap(nuklearCtx, "This is another long text to show dynamic window changes on multiline text");
				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Button", NK_MINIMIZED))
			{
				/* Buttons Widgets */
				nk_layout_row_static(nuklearCtx, 30, 100, 3);
				if (nk_button_label(nuklearCtx, "Button"))
					fprintf(stdout, "Button pressed!\n");
				nk_button_set_behavior(nuklearCtx, NK_BUTTON_REPEATER);
				if (nk_button_label(nuklearCtx, "Repeater"))
					fprintf(stdout, "Repeater is being pressed!\n");
				nk_button_set_behavior(nuklearCtx, NK_BUTTON_DEFAULT);
				nk_button_color(nuklearCtx, nk_rgb(0, 0, 255));

				nk_layout_row_static(nuklearCtx, 25, 25, 8);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_CIRCLE_SOLID);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_CIRCLE_OUTLINE);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_RECT_SOLID);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_RECT_OUTLINE);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_TRIANGLE_UP);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_TRIANGLE_DOWN);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_TRIANGLE_LEFT);
				nk_button_symbol(nuklearCtx, NK_SYMBOL_TRIANGLE_RIGHT);

				nk_layout_row_static(nuklearCtx, 30, 100, 2);
				nk_button_symbol_label(nuklearCtx, NK_SYMBOL_TRIANGLE_LEFT, "prev", NK_TEXT_RIGHT);
				nk_button_symbol_label(nuklearCtx, NK_SYMBOL_TRIANGLE_RIGHT, "next", NK_TEXT_LEFT);
				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Basic", NK_MINIMIZED))
			{
				/* Basic widgets */
				static int int_slider = 5;
				static float float_slider = 2.5f;
				static size_t prog_value = 40;
				static float property_float = 2;
				static int property_int = 10;
				static int property_neg = 10;

				static float range_float_min = 0;
				static float range_float_max = 100;
				static float range_float_value = 50;
				static int range_int_min = 0;
				static int range_int_value = 2048;
				static int range_int_max = 4096;
				static const float ratio[] = { 120, 150 };

				nk_layout_row_static(nuklearCtx, 30, 100, 1);
				nk_checkbox_label(nuklearCtx, "Checkbox", &checkbox);

				nk_layout_row_static(nuklearCtx, 30, 80, 3);
				option = nk_option_label(nuklearCtx, "optionA", option == A) ? A : option;
				option = nk_option_label(nuklearCtx, "optionB", option == B) ? B : option;
				option = nk_option_label(nuklearCtx, "optionC", option == C) ? C : option;

				nk_layout_row(nuklearCtx, NK_STATIC, 30, 2, ratio);
				nk_slider_int(nuklearCtx, 0, &int_slider, 10, 1);

				nk_label(nuklearCtx, "Slider float", NK_TEXT_LEFT);
				nk_slider_float(nuklearCtx, 0, &float_slider, 5.0, 0.5f);
				nk_progress(nuklearCtx, &prog_value, 100, NK_MODIFIABLE);

				nk_layout_row(nuklearCtx, NK_STATIC, 25, 2, ratio);
				nk_label(nuklearCtx, "Property float:", NK_TEXT_LEFT);
				nk_property_float(nuklearCtx, "Float:", 0, &property_float, 64.0f, 0.1f, 0.2f);
				nk_label(nuklearCtx, "Property int:", NK_TEXT_LEFT);
				nk_property_int(nuklearCtx, "Int:", 0, &property_int, 100.0f, 1, 1);
				nk_label(nuklearCtx, "Property neg:", NK_TEXT_LEFT);
				nk_property_int(nuklearCtx, "Neg:", -10, &property_neg, 10, 1, 1);

				nk_layout_row_dynamic(nuklearCtx, 25, 1);
				nk_label(nuklearCtx, "Range:", NK_TEXT_LEFT);
				nk_layout_row_dynamic(nuklearCtx, 25, 3);
				nk_property_float(nuklearCtx, "#min:", 0, &range_float_min, range_float_max, 1.0f, 0.2f);
				nk_property_float(nuklearCtx, "#float:", range_float_min, &range_float_value, range_float_max, 1.0f, 0.2f);
				nk_property_float(nuklearCtx, "#max:", range_float_min, &range_float_max, 100, 1.0f, 0.2f);

				nk_property_int(nuklearCtx, "#min:", INT_MIN, &range_int_min, range_int_max, 1, 10);
				nk_property_int(nuklearCtx, "#neg:", range_int_min, &range_int_value, range_int_max, 1, 10);
				nk_property_int(nuklearCtx, "#max:", range_int_min, &range_int_max, INT_MAX, 1, 10);

				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Inactive", NK_MINIMIZED))
			{
				static int inactive = 1;
				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_checkbox_label(nuklearCtx, "Inactive", &inactive);

				nk_layout_row_static(nuklearCtx, 30, 80, 1);
				if (inactive) {
					struct nk_style_button button;
					button = nuklearCtx->style.button;
					nuklearCtx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
					nuklearCtx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
					nuklearCtx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
					nuklearCtx->style.button.border_color = nk_rgb(60, 60, 60);
					nuklearCtx->style.button.text_background = nk_rgb(60, 60, 60);
					nuklearCtx->style.button.text_normal = nk_rgb(60, 60, 60);
					nuklearCtx->style.button.text_hover = nk_rgb(60, 60, 60);
					nuklearCtx->style.button.text_active = nk_rgb(60, 60, 60);
					nk_button_label(nuklearCtx, "button");
					nuklearCtx->style.button = button;
				}
				else if (nk_button_label(nuklearCtx, "button"))
					fprintf(stdout, "button pressed\n");
				nk_tree_pop(nuklearCtx);
			}


			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Selectable", NK_MINIMIZED))
			{
				if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "List", NK_MINIMIZED))
				{
					static int selected[4] = { nk_false, nk_false, nk_true, nk_false };
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					nk_selectable_label(nuklearCtx, "Selectable", NK_TEXT_LEFT, &selected[0]);
					nk_selectable_label(nuklearCtx, "Selectable", NK_TEXT_LEFT, &selected[1]);
					nk_label(nuklearCtx, "Not Selectable", NK_TEXT_LEFT);
					nk_selectable_label(nuklearCtx, "Selectable", NK_TEXT_LEFT, &selected[2]);
					nk_selectable_label(nuklearCtx, "Selectable", NK_TEXT_LEFT, &selected[3]);
					nk_tree_pop(nuklearCtx);
				}
				if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Grid", NK_MINIMIZED))
				{
					int i;
					static int selected[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
					nk_layout_row_static(nuklearCtx, 50, 50, 4);
					for (i = 0; i < 16; ++i) {
						if (nk_selectable_label(nuklearCtx, "Z", NK_TEXT_CENTERED, &selected[i])) {
							int x = (i % 4), y = i / 4;
							if (x > 0) selected[i - 1] ^= 1;
							if (x < 3) selected[i + 1] ^= 1;
							if (y > 0) selected[i - 4] ^= 1;
							if (y < 3) selected[i + 4] ^= 1;
						}
					}
					nk_tree_pop(nuklearCtx);
				}
				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Combo", NK_MINIMIZED))
			{
				static float chart_selection = 8.0f;
				static int current_weapon = 0;
				static int check_values[5];
				static float position[3];
				static struct nk_color combo_color = { 130, 50, 50, 255 };
				static struct nk_colorf combo_color2 = { 0.509f, 0.705f, 0.2f, 1.0f };
				static size_t prog_a = 20, prog_b = 40, prog_c = 10, prog_d = 90;
				static const char* weapons[] = { "Fist","Pistol","Shotgun","Plasma","BFG" };

				char buffer[64];
				size_t sum = 0;

				/* default combobox */
				nk_layout_row_static(nuklearCtx, 25, 200, 1);
				current_weapon = nk_combo(nuklearCtx, weapons, NK_LEN(weapons), current_weapon, 25, nk_vec2(200, 200));

				/* slider color combobox */
				if (nk_combo_begin_color(nuklearCtx, combo_color, nk_vec2(200, 200))) {
					float ratios[] = { 0.15f, 0.85f };
					nk_layout_row(nuklearCtx, NK_DYNAMIC, 30, 2, ratios);
					nk_label(nuklearCtx, "R:", NK_TEXT_LEFT);
					combo_color.r = (nk_byte)nk_slide_int(nuklearCtx, 0, combo_color.r, 255, 5);
					nk_label(nuklearCtx, "G:", NK_TEXT_LEFT);
					combo_color.g = (nk_byte)nk_slide_int(nuklearCtx, 0, combo_color.g, 255, 5);
					nk_label(nuklearCtx, "B:", NK_TEXT_LEFT);
					combo_color.b = (nk_byte)nk_slide_int(nuklearCtx, 0, combo_color.b, 255, 5);
					nk_label(nuklearCtx, "A:", NK_TEXT_LEFT);
					combo_color.a = (nk_byte)nk_slide_int(nuklearCtx, 0, combo_color.a, 255, 5);
					nk_combo_end(nuklearCtx);
				}
				/* complex color combobox */
				if (nk_combo_begin_color(nuklearCtx, nk_rgb_cf(combo_color2), nk_vec2(200, 400))) {
					enum color_mode { COL_RGB, COL_HSV };
					static int col_mode = COL_RGB;
#ifndef DEMO_DO_NOT_USE_COLOR_PICKER
					nk_layout_row_dynamic(nuklearCtx, 120, 1);
					combo_color2 = nk_color_picker(nuklearCtx, combo_color2, NK_RGBA);
#endif

					nk_layout_row_dynamic(nuklearCtx, 25, 2);
					col_mode = nk_option_label(nuklearCtx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
					col_mode = nk_option_label(nuklearCtx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

					nk_layout_row_dynamic(nuklearCtx, 25, 1);
					if (col_mode == COL_RGB) {
						combo_color2.r = nk_propertyf(nuklearCtx, "#R:", 0, combo_color2.r, 1.0f, 0.01f, 0.005f);
						combo_color2.g = nk_propertyf(nuklearCtx, "#G:", 0, combo_color2.g, 1.0f, 0.01f, 0.005f);
						combo_color2.b = nk_propertyf(nuklearCtx, "#B:", 0, combo_color2.b, 1.0f, 0.01f, 0.005f);
						combo_color2.a = nk_propertyf(nuklearCtx, "#A:", 0, combo_color2.a, 1.0f, 0.01f, 0.005f);
					}
					else {
						float hsva[4];
						nk_colorf_hsva_fv(hsva, combo_color2);
						hsva[0] = nk_propertyf(nuklearCtx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
						hsva[1] = nk_propertyf(nuklearCtx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
						hsva[2] = nk_propertyf(nuklearCtx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
						hsva[3] = nk_propertyf(nuklearCtx, "#A:", 0, hsva[3], 1.0f, 0.01f, 0.05f);
						combo_color2 = nk_hsva_colorfv(hsva);
					}
					nk_combo_end(nuklearCtx);
				}
				/* progressbar combobox */
				sum = prog_a + prog_b + prog_c + prog_d;
				sprintf_s(buffer, sizeof(buffer), "%lu", sum);
				if (nk_combo_begin_label(nuklearCtx, buffer, nk_vec2(200, 200))) {
					nk_layout_row_dynamic(nuklearCtx, 30, 1);
					nk_progress(nuklearCtx, &prog_a, 100, NK_MODIFIABLE);
					nk_progress(nuklearCtx, &prog_b, 100, NK_MODIFIABLE);
					nk_progress(nuklearCtx, &prog_c, 100, NK_MODIFIABLE);
					nk_progress(nuklearCtx, &prog_d, 100, NK_MODIFIABLE);
					nk_combo_end(nuklearCtx);
				}

				/* checkbox combobox */
				sum = (size_t)(check_values[0] + check_values[1] + check_values[2] + check_values[3] + check_values[4]);
				sprintf_s(buffer, sizeof(buffer), "%lu", sum);
				if (nk_combo_begin_label(nuklearCtx, buffer, nk_vec2(200, 200))) {
					nk_layout_row_dynamic(nuklearCtx, 30, 1);
					nk_checkbox_label(nuklearCtx, weapons[0], &check_values[0]);
					nk_checkbox_label(nuklearCtx, weapons[1], &check_values[1]);
					nk_checkbox_label(nuklearCtx, weapons[2], &check_values[2]);
					nk_checkbox_label(nuklearCtx, weapons[3], &check_values[3]);
					nk_combo_end(nuklearCtx);
				}

				/* complex text combobox */
				sprintf_s(buffer, sizeof(buffer), "%.2f, %.2f, %.2f", position[0], position[1], position[2]);
				if (nk_combo_begin_label(nuklearCtx, buffer, nk_vec2(200, 200))) {
					nk_layout_row_dynamic(nuklearCtx, 25, 1);
					nk_property_float(nuklearCtx, "#X:", -1024.0f, &position[0], 1024.0f, 1, 0.5f);
					nk_property_float(nuklearCtx, "#Y:", -1024.0f, &position[1], 1024.0f, 1, 0.5f);
					nk_property_float(nuklearCtx, "#Z:", -1024.0f, &position[2], 1024.0f, 1, 0.5f);
					nk_combo_end(nuklearCtx);
				}

				/* chart combobox */
				sprintf_s(buffer, sizeof(buffer), "%.1f", chart_selection);
				if (nk_combo_begin_label(nuklearCtx, buffer, nk_vec2(200, 250))) {
					size_t i = 0;
					static const float values[] = { 26.0f,13.0f,30.0f,15.0f,25.0f,10.0f,20.0f,40.0f, 12.0f, 8.0f, 22.0f, 28.0f, 5.0f };
					nk_layout_row_dynamic(nuklearCtx, 150, 1);
					nk_chart_begin(nuklearCtx, NK_CHART_COLUMN, NK_LEN(values), 0, 50);
					for (i = 0; i < NK_LEN(values); ++i) {
						nk_flags res = nk_chart_push(nuklearCtx, values[i]);
						if (res & NK_CHART_CLICKED) {
							chart_selection = values[i];
							nk_combo_close(nuklearCtx);
						}
					}
					nk_chart_end(nuklearCtx);
					nk_combo_end(nuklearCtx);
				}

				{
					static int time_selected = 0;
					static int date_selected = 0;
					static struct tm sel_time;
					static struct tm sel_date;
					if (!time_selected || !date_selected) {
						/* keep time and date updated if nothing is selected */
						time_t cur_time = time(0);
						struct tm n;
						localtime_s(&n, &cur_time);
						if (!time_selected)
							memcpy(&sel_time, &n, sizeof(struct tm));
						if (!date_selected)
							memcpy(&sel_date, &n, sizeof(struct tm));
					}

					/* time combobox */
					sprintf_s(buffer, sizeof(buffer), "%02d:%02d:%02d", sel_time.tm_hour, sel_time.tm_min, sel_time.tm_sec);
					if (nk_combo_begin_label(nuklearCtx, buffer, nk_vec2(200, 250))) {
						time_selected = 1;
						nk_layout_row_dynamic(nuklearCtx, 25, 1);
						sel_time.tm_sec = nk_propertyi(nuklearCtx, "#S:", 0, sel_time.tm_sec, 60, 1, 1);
						sel_time.tm_min = nk_propertyi(nuklearCtx, "#M:", 0, sel_time.tm_min, 60, 1, 1);
						sel_time.tm_hour = nk_propertyi(nuklearCtx, "#H:", 0, sel_time.tm_hour, 23, 1, 1);
						nk_combo_end(nuklearCtx);
					}

					/* date combobox */
					sprintf_s(buffer, sizeof(buffer), "%02d-%02d-%02d", sel_date.tm_mday, sel_date.tm_mon + 1, sel_date.tm_year + 1900);
					if (nk_combo_begin_label(nuklearCtx, buffer, nk_vec2(350, 400)))
					{
						int i = 0;
						const char* month[] = { "January", "February", "March",
							"April", "May", "June", "July", "August", "September",
							"October", "November", "December" };
						const char* week_days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
						const int month_days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
						int year = sel_date.tm_year + 1900;
						int leap_year = (!(year % 4) && ((year % 100))) || !(year % 400);
						int days = (sel_date.tm_mon == 1) ?
							month_days[sel_date.tm_mon] + leap_year :
							month_days[sel_date.tm_mon];

						/* header with month and year */
						date_selected = 1;
						nk_layout_row_begin(nuklearCtx, NK_DYNAMIC, 20, 3);
						nk_layout_row_push(nuklearCtx, 0.05f);
						if (nk_button_symbol(nuklearCtx, NK_SYMBOL_TRIANGLE_LEFT)) {
							if (sel_date.tm_mon == 0) {
								sel_date.tm_mon = 11;
								sel_date.tm_year = NK_MAX(0, sel_date.tm_year - 1);
							}
							else sel_date.tm_mon--;
						}
						nk_layout_row_push(nuklearCtx, 0.9f);
						sprintf_s(buffer, sizeof(buffer), "%s %d", month[sel_date.tm_mon], year);
						nk_label(nuklearCtx, buffer, NK_TEXT_CENTERED);
						nk_layout_row_push(nuklearCtx, 0.05f);
						if (nk_button_symbol(nuklearCtx, NK_SYMBOL_TRIANGLE_RIGHT)) {
							if (sel_date.tm_mon == 11) {
								sel_date.tm_mon = 0;
								sel_date.tm_year++;
							}
							else sel_date.tm_mon++;
						}
						nk_layout_row_end(nuklearCtx);

						/* good old week day formula (double because precision) */
						{int year_n = (sel_date.tm_mon < 2) ? year - 1 : year;
						int y = year_n % 100;
						int c = year_n / 100;
						int y4 = (int)((float)y / 4);
						int c4 = (int)((float)c / 4);
						int m = (int)(2.6 * (double)(((sel_date.tm_mon + 10) % 12) + 1) - 0.2);
						int week_day = (((1 + m + y + y4 + c4 - 2 * c) % 7) + 7) % 7;

						/* weekdays  */
						nk_layout_row_dynamic(nuklearCtx, 35, 7);
						for (i = 0; i < (int)NK_LEN(week_days); ++i)
							nk_label(nuklearCtx, week_days[i], NK_TEXT_CENTERED);

						/* days  */
						if (week_day > 0) nk_spacing(nuklearCtx, week_day);
						for (i = 1; i <= days; ++i) {
							sprintf_s(buffer, sizeof(buffer), "%d", i);
							if (nk_button_label(nuklearCtx, buffer)) {
								sel_date.tm_mday = i;
								nk_combo_close(nuklearCtx);
							}
						}}
						nk_combo_end(nuklearCtx);
					}
				}

				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Input", NK_MINIMIZED))
			{
				static const float ratio[] = { 120, 150 };
				static char field_buffer[64];
				static char text[9][64];
				static int text_len[9];
				static char box_buffer[512];
				static int field_len;
				static int box_len;
				nk_flags active;

				nk_layout_row(nuklearCtx, NK_STATIC, 25, 2, ratio);
				nk_label(nuklearCtx, "Default:", NK_TEXT_LEFT);

				nk_edit_string(nuklearCtx, NK_EDIT_SIMPLE, text[0], &text_len[0], 64, nk_filter_default);
				nk_label(nuklearCtx, "Int:", NK_TEXT_LEFT);
				nk_edit_string(nuklearCtx, NK_EDIT_SIMPLE, text[1], &text_len[1], 64, nk_filter_decimal);
				nk_label(nuklearCtx, "Float:", NK_TEXT_LEFT);
				nk_edit_string(nuklearCtx, NK_EDIT_SIMPLE, text[2], &text_len[2], 64, nk_filter_float);
				nk_label(nuklearCtx, "Hex:", NK_TEXT_LEFT);
				nk_edit_string(nuklearCtx, NK_EDIT_SIMPLE, text[4], &text_len[4], 64, nk_filter_hex);
				nk_label(nuklearCtx, "Octal:", NK_TEXT_LEFT);
				nk_edit_string(nuklearCtx, NK_EDIT_SIMPLE, text[5], &text_len[5], 64, nk_filter_oct);
				nk_label(nuklearCtx, "Binary:", NK_TEXT_LEFT);
				nk_edit_string(nuklearCtx, NK_EDIT_SIMPLE, text[6], &text_len[6], 64, nk_filter_binary);

				nk_label(nuklearCtx, "Password:", NK_TEXT_LEFT);
				{
					int i = 0;
					int old_len = text_len[8];
					char buffer[64];
					for (i = 0; i < text_len[8]; ++i) buffer[i] = '*';
					nk_edit_string(nuklearCtx, NK_EDIT_FIELD, buffer, &text_len[8], 64, nk_filter_default);
					if (old_len < text_len[8])
						memcpy(&text[8][old_len], &buffer[old_len], (nk_size)(text_len[8] - old_len));
				}

				nk_label(nuklearCtx, "Field:", NK_TEXT_LEFT);
				nk_edit_string(nuklearCtx, NK_EDIT_FIELD, field_buffer, &field_len, 64, nk_filter_default);

				nk_label(nuklearCtx, "Box:", NK_TEXT_LEFT);
				nk_layout_row_static(nuklearCtx, 180, 278, 1);
				nk_edit_string(nuklearCtx, NK_EDIT_BOX, box_buffer, &box_len, 512, nk_filter_default);

				nk_layout_row(nuklearCtx, NK_STATIC, 25, 2, ratio);
				active = nk_edit_string(nuklearCtx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, text[7], &text_len[7], 64, nk_filter_ascii);
				if (nk_button_label(nuklearCtx, "Submit") ||
					(active & NK_EDIT_COMMITED))
				{
					text[7][text_len[7]] = '\n';
					text_len[7]++;
					memcpy(&box_buffer[box_len], &text[7], (nk_size)text_len[7]);
					box_len += text_len[7];
					text_len[7] = 0;
				}
				nk_tree_pop(nuklearCtx);
			}
			nk_tree_pop(nuklearCtx);
		}

		if (nk_tree_push(nuklearCtx, NK_TREE_TAB, "Popup", NK_MINIMIZED))
		{
			static struct nk_color color = { 255,0,0, 255 };
			static int select[4];
			static int popup_active;
			const struct nk_input* in = &nuklearCtx->input;
			struct nk_rect bounds;

			/* menu contextual */
			nk_layout_row_static(nuklearCtx, 30, 160, 1);
			bounds = nk_widget_bounds(nuklearCtx);
			nk_label(nuklearCtx, "Right click me for menu", NK_TEXT_LEFT);

			if (nk_contextual_begin(nuklearCtx, 0, nk_vec2(100, 300), bounds)) {
				static size_t prog = 40;
				static int slider = 10;

				nk_layout_row_dynamic(nuklearCtx, 25, 1);
				nk_checkbox_label(nuklearCtx, "Menu", &show_menu);
				nk_progress(nuklearCtx, &prog, 100, NK_MODIFIABLE);
				nk_slider_int(nuklearCtx, 0, &slider, 16, 1);
				if (nk_contextual_item_label(nuklearCtx, "About", NK_TEXT_CENTERED))
					show_app_about = nk_true;
				nk_selectable_label(nuklearCtx, select[0] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[0]);
				nk_selectable_label(nuklearCtx, select[1] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[1]);
				nk_selectable_label(nuklearCtx, select[2] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[2]);
				nk_selectable_label(nuklearCtx, select[3] ? "Unselect" : "Select", NK_TEXT_LEFT, &select[3]);
				nk_contextual_end(nuklearCtx);
			}

			/* color contextual */
			nk_layout_row_begin(nuklearCtx, NK_STATIC, 30, 2);
			nk_layout_row_push(nuklearCtx, 100);
			nk_label(nuklearCtx, "Right Click here:", NK_TEXT_LEFT);
			nk_layout_row_push(nuklearCtx, 50);
			bounds = nk_widget_bounds(nuklearCtx);
			nk_button_color(nuklearCtx, color);
			nk_layout_row_end(nuklearCtx);

			if (nk_contextual_begin(nuklearCtx, 0, nk_vec2(350, 60), bounds)) {
				nk_layout_row_dynamic(nuklearCtx, 30, 4);
				color.r = (nk_byte)nk_propertyi(nuklearCtx, "#r", 0, color.r, 255, 1, 1);
				color.g = (nk_byte)nk_propertyi(nuklearCtx, "#g", 0, color.g, 255, 1, 1);
				color.b = (nk_byte)nk_propertyi(nuklearCtx, "#b", 0, color.b, 255, 1, 1);
				color.a = (nk_byte)nk_propertyi(nuklearCtx, "#a", 0, color.a, 255, 1, 1);
				nk_contextual_end(nuklearCtx);
			}

			/* popup */
			nk_layout_row_begin(nuklearCtx, NK_STATIC, 30, 2);
			nk_layout_row_push(nuklearCtx, 100);
			nk_label(nuklearCtx, "Popup:", NK_TEXT_LEFT);
			nk_layout_row_push(nuklearCtx, 50);
			if (nk_button_label(nuklearCtx, "Popup"))
				popup_active = 1;
			nk_layout_row_end(nuklearCtx);

			if (popup_active)
			{
				static struct nk_rect s = { 20, 100, 220, 90 };
				if (nk_popup_begin(nuklearCtx, NK_POPUP_STATIC, "Error", 0, s))
				{
					nk_layout_row_dynamic(nuklearCtx, 25, 1);
					nk_label(nuklearCtx, "A terrible error as occured", NK_TEXT_LEFT);
					nk_layout_row_dynamic(nuklearCtx, 25, 2);
					if (nk_button_label(nuklearCtx, "OK")) {
						popup_active = 0;
						nk_popup_close(nuklearCtx);
					}
					if (nk_button_label(nuklearCtx, "Cancel")) {
						popup_active = 0;
						nk_popup_close(nuklearCtx);
					}
					nk_popup_end(nuklearCtx);
				}
				else popup_active = nk_false;
			}

			/* tooltip */
			nk_layout_row_static(nuklearCtx, 30, 150, 1);
			bounds = nk_widget_bounds(nuklearCtx);
			nk_label(nuklearCtx, "Hover me for tooltip", NK_TEXT_LEFT);
			if (nk_input_is_mouse_hovering_rect(in, bounds))
				nk_tooltip(nuklearCtx, "This is a tooltip");

			nk_tree_pop(nuklearCtx);
		}

		if (nk_tree_push(nuklearCtx, NK_TREE_TAB, "Layout", NK_MINIMIZED))
		{
			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Widget", NK_MINIMIZED))
			{
				float ratio_two[] = { 0.2f, 0.6f, 0.2f };
				float width_two[] = { 100, 200, 50 };

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Dynamic fixed column layout with generated position and size:", NK_TEXT_LEFT);
				nk_layout_row_dynamic(nuklearCtx, 30, 3);
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "static fixed column layout with generated position and size:", NK_TEXT_LEFT);
				nk_layout_row_static(nuklearCtx, 30, 100, 3);
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Dynamic array-based custom column layout with generated position and custom size:", NK_TEXT_LEFT);
				nk_layout_row(nuklearCtx, NK_DYNAMIC, 30, 3, ratio_two);
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Static array-based custom column layout with generated position and custom size:", NK_TEXT_LEFT);
				nk_layout_row(nuklearCtx, NK_STATIC, 30, 3, width_two);
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Dynamic immediate mode custom column layout with generated position and custom size:", NK_TEXT_LEFT);
				nk_layout_row_begin(nuklearCtx, NK_DYNAMIC, 30, 3);
				nk_layout_row_push(nuklearCtx, 0.2f);
				nk_button_label(nuklearCtx, "button");
				nk_layout_row_push(nuklearCtx, 0.6f);
				nk_button_label(nuklearCtx, "button");
				nk_layout_row_push(nuklearCtx, 0.2f);
				nk_button_label(nuklearCtx, "button");
				nk_layout_row_end(nuklearCtx);

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Static immediate mode custom column layout with generated position and custom size:", NK_TEXT_LEFT);
				nk_layout_row_begin(nuklearCtx, NK_STATIC, 30, 3);
				nk_layout_row_push(nuklearCtx, 100);
				nk_button_label(nuklearCtx, "button");
				nk_layout_row_push(nuklearCtx, 200);
				nk_button_label(nuklearCtx, "button");
				nk_layout_row_push(nuklearCtx, 50);
				nk_button_label(nuklearCtx, "button");
				nk_layout_row_end(nuklearCtx);

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Static free space with custom position and custom size:", NK_TEXT_LEFT);
				nk_layout_space_begin(nuklearCtx, NK_STATIC, 60, 4);
				nk_layout_space_push(nuklearCtx, nk_rect(100, 0, 100, 30));
				nk_button_label(nuklearCtx, "button");
				nk_layout_space_push(nuklearCtx, nk_rect(0, 15, 100, 30));
				nk_button_label(nuklearCtx, "button");
				nk_layout_space_push(nuklearCtx, nk_rect(200, 15, 100, 30));
				nk_button_label(nuklearCtx, "button");
				nk_layout_space_push(nuklearCtx, nk_rect(100, 30, 100, 30));
				nk_button_label(nuklearCtx, "button");
				nk_layout_space_end(nuklearCtx);

				nk_layout_row_dynamic(nuklearCtx, 30, 1);
				nk_label(nuklearCtx, "Row template:", NK_TEXT_LEFT);
				nk_layout_row_template_begin(nuklearCtx, 30);
				nk_layout_row_template_push_dynamic(nuklearCtx);
				nk_layout_row_template_push_variable(nuklearCtx, 80);
				nk_layout_row_template_push_static(nuklearCtx, 80);
				nk_layout_row_template_end(nuklearCtx);
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");
				nk_button_label(nuklearCtx, "button");

				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Group", NK_MINIMIZED))
			{
				static int group_titlebar = nk_false;
				static int group_border = nk_true;
				static int group_no_scrollbar = nk_false;
				static int group_width = 320;
				static int group_height = 200;

				nk_flags group_flags = 0;
				if (group_border) group_flags |= NK_WINDOW_BORDER;
				if (group_no_scrollbar) group_flags |= NK_WINDOW_NO_SCROLLBAR;
				if (group_titlebar) group_flags |= NK_WINDOW_TITLE;

				nk_layout_row_dynamic(nuklearCtx, 30, 3);
				nk_checkbox_label(nuklearCtx, "Titlebar", &group_titlebar);
				nk_checkbox_label(nuklearCtx, "Border", &group_border);
				nk_checkbox_label(nuklearCtx, "No Scrollbar", &group_no_scrollbar);

				nk_layout_row_begin(nuklearCtx, NK_STATIC, 22, 3);
				nk_layout_row_push(nuklearCtx, 50);
				nk_label(nuklearCtx, "size:", NK_TEXT_LEFT);
				nk_layout_row_push(nuklearCtx, 130);
				nk_property_int(nuklearCtx, "#Width:", 100, &group_width, 500, 10, 1);
				nk_layout_row_push(nuklearCtx, 130);
				nk_property_int(nuklearCtx, "#Height:", 100, &group_height, 500, 10, 1);
				nk_layout_row_end(nuklearCtx);

				nk_layout_row_static(nuklearCtx, (float)group_height, group_width, 2);
				if (nk_group_begin(nuklearCtx, "Group", group_flags)) {
					int i = 0;
					static int selected[16];
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					for (i = 0; i < 16; ++i)
						nk_selectable_label(nuklearCtx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
					nk_group_end(nuklearCtx);
				}
				nk_tree_pop(nuklearCtx);
			}
			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Tree", NK_MINIMIZED))
			{
				static int root_selected = 0;
				int sel = root_selected;
				if (nk_tree_element_push(nuklearCtx, NK_TREE_NODE, "Root", NK_MINIMIZED, &sel)) {
					static int selected[8];
					int i = 0, node_select = selected[0];
					if (sel != root_selected) {
						root_selected = sel;
						for (i = 0; i < 8; ++i)
							selected[i] = sel;
					}
					if (nk_tree_element_push(nuklearCtx, NK_TREE_NODE, "Node", NK_MINIMIZED, &node_select)) {
						int j = 0;
						static int sel_nodes[4];
						if (node_select != selected[0]) {
							selected[0] = node_select;
							for (i = 0; i < 4; ++i)
								sel_nodes[i] = node_select;
						}
						nk_layout_row_static(nuklearCtx, 18, 100, 1);
						for (j = 0; j < 4; ++j)
							nk_selectable_symbol_label(nuklearCtx, NK_SYMBOL_CIRCLE_SOLID, (sel_nodes[j]) ? "Selected" : "Unselected", NK_TEXT_RIGHT, &sel_nodes[j]);
						nk_tree_element_pop(nuklearCtx);
					}
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					for (i = 1; i < 8; ++i)
						nk_selectable_symbol_label(nuklearCtx, NK_SYMBOL_CIRCLE_SOLID, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_RIGHT, &selected[i]);
					nk_tree_element_pop(nuklearCtx);
				}
				nk_tree_pop(nuklearCtx);
			}
			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Notebook", NK_MINIMIZED))
			{
				static int current_tab = 0;
				struct nk_rect bounds;
				float step = (2 * 3.141592654f) / 32;
				enum chart_type { CHART_LINE, CHART_HISTO, CHART_MIXED };
				const char* names[] = { "Lines", "Columns", "Mixed" };
				float id = 0;
				int i;

				/* Header */
				nk_style_push_vec2(nuklearCtx, &nuklearCtx->style.window.spacing, nk_vec2(0, 0));
				nk_style_push_float(nuklearCtx, &nuklearCtx->style.button.rounding, 0);
				nk_layout_row_begin(nuklearCtx, NK_STATIC, 20, 3);
				for (i = 0; i < 3; ++i) {
					/* make sure button perfectly fits text */
					const struct nk_user_font* f = nuklearCtx->style.font;
					float text_width = f->width(f->userdata, f->height, names[i], nk_strlen(names[i]));
					float widget_width = text_width + 3 * nuklearCtx->style.button.padding.x;
					nk_layout_row_push(nuklearCtx, widget_width);
					if (current_tab == i) {
						/* active tab gets highlighted */
						struct nk_style_item button_color = nuklearCtx->style.button.normal;
						nuklearCtx->style.button.normal = nuklearCtx->style.button.active;
						current_tab = nk_button_label(nuklearCtx, names[i]) ? i : current_tab;
						nuklearCtx->style.button.normal = button_color;
					}
					else current_tab = nk_button_label(nuklearCtx, names[i]) ? i : current_tab;
				}
				nk_style_pop_float(nuklearCtx);

				/* Body */
				nk_layout_row_dynamic(nuklearCtx, 140, 1);
				if (nk_group_begin(nuklearCtx, "Notebook", NK_WINDOW_BORDER))
				{
					nk_style_pop_vec2(nuklearCtx);
					switch (current_tab) {
					default: break;
					case CHART_LINE:
						nk_layout_row_dynamic(nuklearCtx, 100, 1);
						bounds = nk_widget_bounds(nuklearCtx);
						if (nk_chart_begin_colored(nuklearCtx, NK_CHART_LINES, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
							nk_chart_add_slot_colored(nuklearCtx, NK_CHART_LINES, nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
							for (i = 0, id = 0; i < 32; ++i) {
								nk_chart_push_slot(nuklearCtx, (float)fabs(sin(id)), 0);
								nk_chart_push_slot(nuklearCtx, (float)cos(id), 1);
								id += step;
							}
						}
						nk_chart_end(nuklearCtx);
						break;
					case CHART_HISTO:
						nk_layout_row_dynamic(nuklearCtx, 100, 1);
						bounds = nk_widget_bounds(nuklearCtx);
						if (nk_chart_begin_colored(nuklearCtx, NK_CHART_COLUMN, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
							for (i = 0, id = 0; i < 32; ++i) {
								nk_chart_push_slot(nuklearCtx, (float)fabs(sin(id)), 0);
								id += step;
							}
						}
						nk_chart_end(nuklearCtx);
						break;
					case CHART_MIXED:
						nk_layout_row_dynamic(nuklearCtx, 100, 1);
						bounds = nk_widget_bounds(nuklearCtx);
						if (nk_chart_begin_colored(nuklearCtx, NK_CHART_LINES, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
							nk_chart_add_slot_colored(nuklearCtx, NK_CHART_LINES, nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
							nk_chart_add_slot_colored(nuklearCtx, NK_CHART_COLUMN, nk_rgb(0, 255, 0), nk_rgb(0, 150, 0), 32, 0.0f, 1.0f);
							for (i = 0, id = 0; i < 32; ++i) {
								nk_chart_push_slot(nuklearCtx, (float)fabs(sin(id)), 0);
								nk_chart_push_slot(nuklearCtx, (float)fabs(cos(id)), 1);
								nk_chart_push_slot(nuklearCtx, (float)fabs(sin(id)), 2);
								id += step;
							}
						}
						nk_chart_end(nuklearCtx);
						break;
					}
					nk_group_end(nuklearCtx);
				}
				else nk_style_pop_vec2(nuklearCtx);
				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Simple", NK_MINIMIZED))
			{
				nk_layout_row_dynamic(nuklearCtx, 300, 2);
				if (nk_group_begin(nuklearCtx, "Group_Without_Border", 0)) {
					int i = 0;
					char buffer[64];
					nk_layout_row_static(nuklearCtx, 18, 150, 1);
					for (i = 0; i < 64; ++i) {
						sprintf_s(buffer, sizeof(buffer), "0x%02x", i);
					}
					nk_group_end(nuklearCtx);
				}
				if (nk_group_begin(nuklearCtx, "Group_With_Border", NK_WINDOW_BORDER)) {
					int i = 0;
					char buffer[64];
					nk_layout_row_dynamic(nuklearCtx, 25, 2);
					for (i = 0; i < 64; ++i) {
						sprintf_s(buffer, sizeof(buffer), "%08d", ((((i % 7) * 10) ^ 32)) + (64 + (i % 2) * 2));
						nk_button_label(nuklearCtx, buffer);
					}
					nk_group_end(nuklearCtx);
				}
				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Complex", NK_MINIMIZED))
			{
				int i;
				nk_layout_space_begin(nuklearCtx, NK_STATIC, 500, 64);
				nk_layout_space_push(nuklearCtx, nk_rect(0, 0, 150, 500));
				if (nk_group_begin(nuklearCtx, "Group_left", NK_WINDOW_BORDER)) {
					static int selected[32];
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					for (i = 0; i < 32; ++i)
						nk_selectable_label(nuklearCtx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
					nk_group_end(nuklearCtx);
				}

				nk_layout_space_push(nuklearCtx, nk_rect(160, 0, 150, 240));
				if (nk_group_begin(nuklearCtx, "Group_top", NK_WINDOW_BORDER)) {
					nk_layout_row_dynamic(nuklearCtx, 25, 1);
					nk_button_label(nuklearCtx, "#FFAA");
					nk_button_label(nuklearCtx, "#FFBB");
					nk_button_label(nuklearCtx, "#FFCC");
					nk_button_label(nuklearCtx, "#FFDD");
					nk_button_label(nuklearCtx, "#FFEE");
					nk_button_label(nuklearCtx, "#FFFF");
					nk_group_end(nuklearCtx);
				}

				nk_layout_space_push(nuklearCtx, nk_rect(160, 250, 150, 250));
				if (nk_group_begin(nuklearCtx, "Group_buttom", NK_WINDOW_BORDER)) {
					nk_layout_row_dynamic(nuklearCtx, 25, 1);
					nk_button_label(nuklearCtx, "#FFAA");
					nk_button_label(nuklearCtx, "#FFBB");
					nk_button_label(nuklearCtx, "#FFCC");
					nk_button_label(nuklearCtx, "#FFDD");
					nk_button_label(nuklearCtx, "#FFEE");
					nk_button_label(nuklearCtx, "#FFFF");
					nk_group_end(nuklearCtx);
				}

				nk_layout_space_push(nuklearCtx, nk_rect(320, 0, 150, 150));
				if (nk_group_begin(nuklearCtx, "Group_right_top", NK_WINDOW_BORDER)) {
					static int selected[4];
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					for (i = 0; i < 4; ++i)
						nk_selectable_label(nuklearCtx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
					nk_group_end(nuklearCtx);
				}

				nk_layout_space_push(nuklearCtx, nk_rect(320, 160, 150, 150));
				if (nk_group_begin(nuklearCtx, "Group_right_center", NK_WINDOW_BORDER)) {
					static int selected[4];
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					for (i = 0; i < 4; ++i)
						nk_selectable_label(nuklearCtx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
					nk_group_end(nuklearCtx);
				}

				nk_layout_space_push(nuklearCtx, nk_rect(320, 320, 150, 150));
				if (nk_group_begin(nuklearCtx, "Group_right_bottom", NK_WINDOW_BORDER)) {
					static int selected[4];
					nk_layout_row_static(nuklearCtx, 18, 100, 1);
					for (i = 0; i < 4; ++i)
						nk_selectable_label(nuklearCtx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_CENTERED, &selected[i]);
					nk_group_end(nuklearCtx);
				}
				nk_layout_space_end(nuklearCtx);
				nk_tree_pop(nuklearCtx);
			}

			if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Splitter", NK_MINIMIZED))
			{
				const struct nk_input* in = &nuklearCtx->input;
				nk_layout_row_static(nuklearCtx, 20, 320, 1);
				nk_label(nuklearCtx, "Use slider and spinner to change tile size", NK_TEXT_LEFT);
				nk_label(nuklearCtx, "Drag the space between tiles to change tile ratio", NK_TEXT_LEFT);

				if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Vertical", NK_MINIMIZED))
				{
					static float a = 100, b = 100, c = 100;
					struct nk_rect bounds;

					float row_layout[5];
					row_layout[0] = a;
					row_layout[1] = 8;
					row_layout[2] = b;
					row_layout[3] = 8;
					row_layout[4] = c;

					/* header */
					nk_layout_row_static(nuklearCtx, 30, 100, 2);
					nk_label(nuklearCtx, "left:", NK_TEXT_LEFT);
					nk_slider_float(nuklearCtx, 10.0f, &a, 200.0f, 10.0f);

					nk_label(nuklearCtx, "middle:", NK_TEXT_LEFT);
					nk_slider_float(nuklearCtx, 10.0f, &b, 200.0f, 10.0f);

					nk_label(nuklearCtx, "right:", NK_TEXT_LEFT);
					nk_slider_float(nuklearCtx, 10.0f, &c, 200.0f, 10.0f);

					/* tiles */
					nk_layout_row(nuklearCtx, NK_STATIC, 200, 5, row_layout);

					/* left space */
					if (nk_group_begin(nuklearCtx, "left", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
						nk_layout_row_dynamic(nuklearCtx, 25, 1);
						nk_button_label(nuklearCtx, "#FFAA");
						nk_button_label(nuklearCtx, "#FFBB");
						nk_button_label(nuklearCtx, "#FFCC");
						nk_button_label(nuklearCtx, "#FFDD");
						nk_button_label(nuklearCtx, "#FFEE");
						nk_button_label(nuklearCtx, "#FFFF");
						nk_group_end(nuklearCtx);
					}

					/* scaler */
					bounds = nk_widget_bounds(nuklearCtx);
					nk_spacing(nuklearCtx, 1);
					if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
						nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
						nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
					{
						a = row_layout[0] + in->mouse.delta.x;
						b = row_layout[2] - in->mouse.delta.x;
					}

					/* middle space */
					if (nk_group_begin(nuklearCtx, "center", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
						nk_layout_row_dynamic(nuklearCtx, 25, 1);
						nk_button_label(nuklearCtx, "#FFAA");
						nk_button_label(nuklearCtx, "#FFBB");
						nk_button_label(nuklearCtx, "#FFCC");
						nk_button_label(nuklearCtx, "#FFDD");
						nk_button_label(nuklearCtx, "#FFEE");
						nk_button_label(nuklearCtx, "#FFFF");
						nk_group_end(nuklearCtx);
					}

					/* scaler */
					bounds = nk_widget_bounds(nuklearCtx);
					nk_spacing(nuklearCtx, 1);
					if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
						nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
						nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
					{
						b = (row_layout[2] + in->mouse.delta.x);
						c = (row_layout[4] - in->mouse.delta.x);
					}

					/* right space */
					if (nk_group_begin(nuklearCtx, "right", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
						nk_layout_row_dynamic(nuklearCtx, 25, 1);
						nk_button_label(nuklearCtx, "#FFAA");
						nk_button_label(nuklearCtx, "#FFBB");
						nk_button_label(nuklearCtx, "#FFCC");
						nk_button_label(nuklearCtx, "#FFDD");
						nk_button_label(nuklearCtx, "#FFEE");
						nk_button_label(nuklearCtx, "#FFFF");
						nk_group_end(nuklearCtx);
					}

					nk_tree_pop(nuklearCtx);
				}

				if (nk_tree_push(nuklearCtx, NK_TREE_NODE, "Horizontal", NK_MINIMIZED))
				{
					static float a = 100, b = 100, c = 100;
					struct nk_rect bounds;

					/* header */
					nk_layout_row_static(nuklearCtx, 30, 100, 2);
					nk_label(nuklearCtx, "top:", NK_TEXT_LEFT);
					nk_slider_float(nuklearCtx, 10.0f, &a, 200.0f, 10.0f);

					nk_label(nuklearCtx, "middle:", NK_TEXT_LEFT);
					nk_slider_float(nuklearCtx, 10.0f, &b, 200.0f, 10.0f);

					nk_label(nuklearCtx, "bottom:", NK_TEXT_LEFT);
					nk_slider_float(nuklearCtx, 10.0f, &c, 200.0f, 10.0f);

					/* top space */
					nk_layout_row_dynamic(nuklearCtx, a, 1);
					if (nk_group_begin(nuklearCtx, "top", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
						nk_layout_row_dynamic(nuklearCtx, 25, 3);
						nk_button_label(nuklearCtx, "#FFAA");
						nk_button_label(nuklearCtx, "#FFBB");
						nk_button_label(nuklearCtx, "#FFCC");
						nk_button_label(nuklearCtx, "#FFDD");
						nk_button_label(nuklearCtx, "#FFEE");
						nk_button_label(nuklearCtx, "#FFFF");
						nk_group_end(nuklearCtx);
					}

					/* scaler */
					nk_layout_row_dynamic(nuklearCtx, 8, 1);
					bounds = nk_widget_bounds(nuklearCtx);
					nk_spacing(nuklearCtx, 1);
					if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
						nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
						nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
					{
						a = a + in->mouse.delta.y;
						b = b - in->mouse.delta.y;
					}

					/* middle space */
					nk_layout_row_dynamic(nuklearCtx, b, 1);
					if (nk_group_begin(nuklearCtx, "middle", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
						nk_layout_row_dynamic(nuklearCtx, 25, 3);
						nk_button_label(nuklearCtx, "#FFAA");
						nk_button_label(nuklearCtx, "#FFBB");
						nk_button_label(nuklearCtx, "#FFCC");
						nk_button_label(nuklearCtx, "#FFDD");
						nk_button_label(nuklearCtx, "#FFEE");
						nk_button_label(nuklearCtx, "#FFFF");
						nk_group_end(nuklearCtx);
					}

					{
						/* scaler */
						nk_layout_row_dynamic(nuklearCtx, 8, 1);
						bounds = nk_widget_bounds(nuklearCtx);
						if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
							nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
							nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
						{
							b = b + in->mouse.delta.y;
							c = c - in->mouse.delta.y;
						}
					}

					/* bottom space */
					nk_layout_row_dynamic(nuklearCtx, c, 1);
					if (nk_group_begin(nuklearCtx, "bottom", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
						nk_layout_row_dynamic(nuklearCtx, 25, 3);
						nk_button_label(nuklearCtx, "#FFAA");
						nk_button_label(nuklearCtx, "#FFBB");
						nk_button_label(nuklearCtx, "#FFCC");
						nk_button_label(nuklearCtx, "#FFDD");
						nk_button_label(nuklearCtx, "#FFEE");
						nk_button_label(nuklearCtx, "#FFFF");
						nk_group_end(nuklearCtx);
					}
					nk_tree_pop(nuklearCtx);
				}
				nk_tree_pop(nuklearCtx);
			}
			nk_tree_pop(nuklearCtx);
		}
	}
    nk_end(nuklearCtx);

    nk_d3d11_render(context, NK_ANTI_ALIASING_OFF);

    return; // don't have to do magic here
}


/////////////////////////
// 
// Initialization
//
/////////////////////////
bool InitializeHooks() {
    hooks::scPresentHook = CreateTrampHook64_Advanced(
        reinterpret_cast<BYTE*>(VirtualFunction(swap_chain, 8)),
        reinterpret_cast<BYTE*>(&HookPresent)
    );

    //device->Release();

    return hooks::scPresentHook.get();
}

