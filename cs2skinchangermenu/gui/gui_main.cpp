#include "pch.h"

#define NK_INCLUDE_STANDARD_VARARGS
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024

#include "nuklear-gui/nuklear.h"
#include "nuklear-gui/nuklear_d3d11.h"

#include "nuklear-gui/nuklear.h"
#include "gui/gui_setup.h"
#include "gui/components/components.h"
#include "gui/gui_main.h"

#include <iostream>

namespace gui {
	nk_context* nuklearCtx = nullptr;

	bool isGuiInit = false;
    bool menuOpen = false;
}

void SetupStyle() {
    gui::nuklearCtx->style.text.padding = nk_vec2(8.0f, 5.0f);
}

void InitGUIForDrawing(IDXGISwapChain* chain) {

	chain->GetDevice(__uuidof(gui::device), reinterpret_cast<void**>(&gui::device));
	gui::device->GetImmediateContext(&gui::context);

	// set windowproc to our own window for now
	DXGI_SWAP_CHAIN_DESC sd;
	chain->GetDesc(&sd);
	gui::window = sd.OutputWindow;

	gui::originalWndProc = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc))
	);

	D3D11_VIEWPORT vpOld;
	UINT nViewPorts = 1;

	// make gui render at the front
	ID3D11Texture2D* renderTargetTexture = nullptr;
	if (SUCCEEDED(chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&renderTargetTexture))))
	{
		gui::device->CreateRenderTargetView(renderTargetTexture, NULL, &gui::g_pRenderTargetView);
		renderTargetTexture->Release();
	}

	gui::context->RSGetViewports(&nViewPorts, &vpOld);
	gui::nuklearCtx = nk_d3d11_init(gui::device, vpOld.Width, vpOld.Height, MAX_VERTEX_BUFFER, MAX_INDEX_BUFFER);

	struct nk_font_atlas* atlas;
	nk_d3d11_font_stash_begin(&atlas);
	nk_d3d11_font_stash_end();

    SetupStyle();

	gui::isGuiInit = true;
}

void DrawMainWindow(nk_flags flags) {

	if (nk_begin(gui::nuklearCtx, "godmode", nk_rect(10, 10, 1100, 600), flags)) {
        float sidebarWidth = 0.25 * nk_window_get_width(gui::nuklearCtx);
        nk_layout_row_begin(gui::nuklearCtx, NK_DYNAMIC, nk_window_get_height(gui::nuklearCtx), 2);

        nk_layout_row_push(gui::nuklearCtx, 0.25f);
		DrawSideBar(sidebarWidth);

        // rest is 75%
        // TODO: change currently active menu by sidebar buttons
        nk_layout_row_push(gui::nuklearCtx, 0.75f);
        DrawSkinChangerMenu();

        nk_layout_row_end(gui::nuklearCtx);
	}
	nk_end(gui::nuklearCtx);
}

// Hook callback
void DrawGUIFrame(IDXGISwapChain* chain) {
	// initialization phase
	if (!gui::isGuiInit)
		InitGUIForDrawing(chain);

	if (!gui::menuOpen)
		return;

	gui::context->OMSetRenderTargets(1, &gui::g_pRenderTargetView, NULL);

	nk_flags window_flags = NK_WINDOW_MOVABLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_SCALABLE;
	DrawMainWindow(window_flags);

	nk_d3d11_render(gui::context, NK_ANTI_ALIASING_OFF);

	// this only works after rendering, not before
	MSG msg;
	nk_input_begin(gui::nuklearCtx);
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	nk_input_end(gui::nuklearCtx);
}
