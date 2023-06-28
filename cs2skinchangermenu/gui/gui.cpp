#include "pch.h"

// nuklear impl
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D11_IMPLEMENTATION
#include "gui.h"
#include "D3D11StateSaver.h"

#ifdef _DEBUG
#include <assert.h>
#endif

WNDCLASSEX windowClass = {};
HWND window = NULL;
IDXGISwapChain* swap_chain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
ID3D11RenderTargetView* rt_view = nullptr;
nk_context* nuklearCtx = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
D3D11StateSaver stateSaver = { };

bool guiInitFromHook = false;
bool restoreState = false;

/////////////////////////////
//		WINDOW CALLBACK
////////////////////////////
// taken from nuklear d3d11 impl
void set_swap_chain_size(int width, int height)
{
	ID3D11Texture2D* back_buffer;
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	HRESULT hr;

	if (rt_view)
		rt_view->Release();

	context->OMSetRenderTargets(0, NULL, NULL);

	hr = swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR)
	{
		/* to recover from this, you'll need to recreate device and all the resources */
		MessageBoxW(NULL, L"DXGI device is removed or reset!", L"Error", 0);
		exit(0);
	}
	assert(SUCCEEDED(hr));

	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = swap_chain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&back_buffer);
	assert(SUCCEEDED(hr));

	hr = device->CreateRenderTargetView((ID3D11Resource*)back_buffer, &desc, &rt_view);
	assert(SUCCEEDED(hr));

	back_buffer->Release();
}

static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		if (swap_chain)
		{
			int width = LOWORD(lparam);
			int height = HIWORD(lparam);
			set_swap_chain_size(width, height);
			nk_d3d11_resize(context, width, height);
		}
		break;
	}

	if (nk_d3d11_handle_event(wnd, msg, wparam, lparam))
		return 0;

	return DefWindowProcW(wnd, msg, wparam, lparam);
}

bool CreateWindowClass(const char* className) {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = NULL;

	// register
	if (!RegisterClassEx(&windowClass))
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
	swap_chain_desc.OutputWindow = window;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swap_chain_desc.BufferDesc.Width = 1920;
	//swap_chain_desc.BufferDesc.Height = 1080;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 244;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;

	if (CreateDeviceD3D11(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
		&swap_chain_desc, &swap_chain, &device, NULL, &context) < 0)
		return false;

	return true;
}

bool InitGUI() {
	if (!CreateWindowClass("skc"))
		return false;

	window = CreateWindow(
		windowClass.lpszClassName,
		"skin changer",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		10000,
		10000,
		0,
		0,
		windowClass.hInstance,
		0
	);
	if (!window)
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
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
	if (window)
		DestroyWindow(window);
}
