#pragma once

#include <d3d11.h>
struct nk_context;
typedef unsigned int nk_flags;

namespace gui {
	extern nk_context* nuklearCtx;

	extern bool isGuiInit;
	extern bool menuOpen;
}

void DrawGUIFrame(IDXGISwapChain* chain);
void DrawMainWindow(nk_flags flags);