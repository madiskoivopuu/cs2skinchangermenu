#pragma once

#include <d3d11.h>
struct nk_context;

namespace gui {
	extern nk_context* nuklearCtx;

	extern bool isGuiInit;
}

void DrawGUIFrame(IDXGISwapChain* chain);