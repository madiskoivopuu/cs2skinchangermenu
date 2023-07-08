#pragma once

#include <d3d11.h>
#include "memory/tools/hook64.h"

namespace hooks {
	using fIDXGISwapChain__Present = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
	extern fIDXGISwapChain__Present oIDXGISwapChain__Present;
	extern std::unique_ptr<Hook> swapChainPresentHook;

	extern std::unique_ptr<Hook> playerPawnCreateMoveHook;
}

constexpr void* VirtualFunction(void* thisptr, size_t index) noexcept
{
	return (*static_cast<void***>(thisptr))[index];
}

bool InitializeHooks();
void RemoveHooks();

