#pragma once

#include "nuklear-gui/nuklear.h"

#include <d3d11.h>

namespace gui {
	extern nk_context* nuklearCtx;

	extern bool isGuiInit;
}

void DrawGUIFrame(IDXGISwapChain* chain);