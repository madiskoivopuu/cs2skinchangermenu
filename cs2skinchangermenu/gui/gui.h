#pragma once

#include "nuklear-gui/nuklear.h"
#include "nuklear-gui/nuklear_d3d11.h"
#include <Windows.h>
#include <d3d11.h>

extern WNDCLASSEX windowClass;
extern HWND window;
extern IDXGISwapChain* swap_chain;
extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern ID3D11RenderTargetView* rt_view;
extern nk_context* nuklearCtx;

bool InitGUI();

