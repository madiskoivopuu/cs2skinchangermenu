#include "pch.h"
#include "hooks.h"
#include "gui/gui.h"

#include <iostream>

namespace hooks {
	fIDXGISwapChain__Present oIDXGISwapChain__Present = nullptr;
	std::unique_ptr<Hook> scPresentHook;
}

/////////////////////////
// 
// HOOKED FUNCTIONS...
//
/////////////////////////
void __stdcall HookPresent(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags) {
    std::cout << "test" << std::endl;
    return; // don't have to do magic here
}


/////////////////////////
// 
// Initialization
//
/////////////////////////
bool InitializeHooks() {
    hooks::scPresentHook = CreateTrampHook64_Advanced(
        reinterpret_cast<BYTE*>(VirtualFunction(swap_chain, 5)),
        reinterpret_cast<BYTE*>(&HookPresent)
    );

    return hooks::scPresentHook.get();
}

