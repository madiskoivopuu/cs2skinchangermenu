#include "pch.h"
#include "hooks.h"
#include "gui/gui_setup.h"
#include "gui/gui_main.h"

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

    DrawGUIFrame(chain);

    return; // don't have to do magic here
}


/////////////////////////
// 
// Initialization
//
/////////////////////////
bool InitializeHooks() {
    hooks::scPresentHook = CreateTrampHook64_Advanced(
        reinterpret_cast<BYTE*>(VirtualFunction(gui::swap_chain, 8)),
        reinterpret_cast<BYTE*>(&HookPresent)
    );

    return hooks::scPresentHook.get();
}

