#include "pch.h"
#include "gui/gui_setup.h"
#include "gui/components/components.h"
#include "gui/gui_main.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024

namespace gui {
	nk_context* nuklearCtx = nullptr;

	bool isGuiInit = false;
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

	gui::isGuiInit = true;
}

// Hook callback
void DrawGUIFrame(IDXGISwapChain* chain) {
	// initialization phase
	if (!gui::isGuiInit)
		InitGUIForDrawing(chain);

	gui::context->OMSetRenderTargets(1, &gui::g_pRenderTargetView, NULL);

	static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
	static int show_app_about = nk_false;

	nk_flags window_flags = NK_WINDOW_MOVABLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE;
	gui::nuklearCtx->style.window.header.align = header_align;

	if (nk_begin(gui::nuklearCtx, "Overview", nk_rect(10, 10, 300, 400), window_flags)) {
		DrawSideBar();
	}
	nk_end(gui::nuklearCtx);

	nk_d3d11_render(gui::context, NK_ANTI_ALIASING_OFF);
}