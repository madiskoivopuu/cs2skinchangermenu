#pragma once

#include "nuklear-gui/nuklear.h"
#include "nuklear-gui/nuklear_d3d11.h"
#include <Windows.h>
#include <d3d11.h>

namespace gui {
	extern WNDCLASSEX windowClass;
	extern HWND window;
	extern IDXGISwapChain* swap_chain;
	extern ID3D11Device* device;
	extern ID3D11DeviceContext* context;
	extern ID3D11RenderTargetView* rt_view;
	extern ID3D11RenderTargetView* g_pRenderTargetView;
	extern WNDPROC originalWndProc;
}


bool InitGUI();
LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
void OnPresentHookCalled(IDXGISwapChain* chain);

