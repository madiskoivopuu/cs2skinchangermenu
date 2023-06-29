#include "pch.h"
#include "hooks.h"
#include "gui/gui.h"

#include <iostream>

namespace hooks {
	fIDXGISwapChain__Present oIDXGISwapChain__Present = nullptr;
	std::unique_ptr<Hook> scPresentHook;
}



#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
/////////////////////////
// 
// HOOKED FUNCTIONS...
//
/////////////////////////
void HookPresent(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags) {

    OnPresentHookCalled(chain);

    return; // don't have to do magic here
}


/////////////////////////
// 
// Initialization
//
/////////////////////////
bool InitializeHooks() {
    hooks::scPresentHook = CreateTrampHook64_Advanced(
        reinterpret_cast<BYTE*>(VirtualFunction(swap_chain, 8)),
        reinterpret_cast<BYTE*>(&HookPresent)
    );

    return hooks::scPresentHook.get();
}

