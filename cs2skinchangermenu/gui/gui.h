#pragma once

#include "nuklear-gui/nuklear.h"
#include "nuklear-gui/nuklear_d3d11.h"
#include "D3D11StateSaver.h"
#include <Windows.h>
#include <d3d11.h>

extern WNDCLASSEX windowClass;
extern HWND window;
extern IDXGISwapChain* swap_chain;
extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern ID3D11RenderTargetView* rt_view;
extern nk_context* nuklearCtx;
extern D3D11StateSaver stateSaver;
extern ID3D11RenderTargetView* g_pRenderTargetView;

extern bool guiInitFromHook;
extern bool restoreState;


bool InitGUI();
void set_swap_chain_size(int width, int height);

