#include "pch.h"

// nuklear impl
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D11_IMPLEMENTATION
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
#include "gui_setup.h"
#include "gui_main.h"
#include "nuklear-gui/nuklear.h"
#include "nuklear-gui/nuklear_d3d11.h"
#include <iostream>

#ifdef _DEBUG
#include <assert.h>
#endif

namespace gui {
	WNDCLASSEX windowClass = {};
	HWND window = NULL;
	IDXGISwapChain* swap_chain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* rt_view = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
	WNDPROC originalWndProc = nullptr;
}

bool CreateWindowClass(const char* className) {
	gui::windowClass.cbSize = sizeof(WNDCLASSEX);
	gui::windowClass.style = CS_HREDRAW | CS_VREDRAW;
	gui::windowClass.lpfnWndProc = DefWindowProc;
	gui::windowClass.cbClsExtra = 0;
	gui::windowClass.cbWndExtra = 0;
	gui::windowClass.hInstance = GetModuleHandle(NULL);
	gui::windowClass.hIcon = NULL;
	gui::windowClass.hCursor = NULL;
	gui::windowClass.hbrBackground = NULL;
	gui::windowClass.lpszMenuName = NULL;
	gui::windowClass.lpszClassName = className;
	gui::windowClass.hIconSm = NULL;

	// register
	if (!RegisterClassEx(&gui::windowClass))
		return false;

	return true;
}

bool CreateDevice() {
	HMODULE d3d11 = GetModuleHandle("d3d11.dll");

	using CreateDeviceFn = HRESULT(__stdcall*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
	CreateDeviceFn CreateDeviceD3D11 = reinterpret_cast<CreateDeviceFn>(GetProcAddress(d3d11, "D3D11CreateDeviceAndSwapChain"));

	if (!CreateDeviceD3D11)
		return false;
	
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_chain_desc.OutputWindow = gui::window;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swap_chain_desc.BufferDesc.Width = 1920;
	//swap_chain_desc.BufferDesc.Height = 1080;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 244;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;

	if (CreateDeviceD3D11(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
		&swap_chain_desc, &gui::swap_chain, &gui::device, NULL, &gui::context) < 0)
		return false;

	return true;
}

bool InitGUI() {
	if (!CreateWindowClass("skc"))
		return false;

	gui::window = CreateWindow(
		gui::windowClass.lpszClassName,
		"skin changer",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		10000,
		10000,
		0,
		0,
		gui::windowClass.hInstance,
		0
	);
	if (!gui::window)
		return false;

	// get direct 3d and create device
	if (!CreateDevice())
		return false;

	// our own created stuff
	//swap_chain->Release();
	//context->Release();
	//rt_view->Release();

	return true;
}

void DestroyGUI() {
	UnregisterClass(gui::windowClass.lpszClassName, gui::windowClass.hInstance);
	if (gui::window)
		DestroyWindow(gui::window);

	SetWindowLongPtr(
		gui::window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(gui::originalWndProc)
	);
}

// Window Process callback
LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (gui::menuOpen && nk_d3d11_handle_event(wnd, msg, wparam, lparam))
		return 1L;

	//nk_d3d11_handle_event(wnd, msg, wparam, lparam);
	return CallWindowProc(gui::originalWndProc, wnd, msg, wparam, lparam);
}