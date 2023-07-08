#include "pch.h"
#include "hooks.h"

#include "gui/gui_setup.h"
#include "gui/gui_main.h"
#include "skin_changer/skins.h"
#include "memory/tools/patternscan.h"
#include "memory/patterns.h"

#include "sdk/gameclasses/C_CSPlayerPawn.h"

#include <iostream>

namespace hooks {
	fIDXGISwapChain__Present oIDXGISwapChain__Present = nullptr;
	std::unique_ptr<Hook> swapChainPresentHook;

    std::unique_ptr<Hook> playerPawnCreateMoveHook;
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

void HookPlayerPawn_CreateMove(C_CSPlayerPawn* pawn, void* pUserCMD, void* pUserCMDCopy) {
    ApplySkinsCallback();
}


/////////////////////////
// 
// Initialization
//
/////////////////////////
bool InitializeHooks() {
    hooks::swapChainPresentHook = CreateTrampHook64_Advanced(
        reinterpret_cast<BYTE*>(VirtualFunction(gui::swap_chain, 8)),
        reinterpret_cast<BYTE*>(&HookPresent)
    );

    void* funcptr = ScanPatternInModule("client.dll", PATTERN_PLAYERPAWNCREATEMOVE_PTR, MASK_PLAYERPAWNCREATEMOVE_PTR);
    if(funcptr)
        hooks::playerPawnCreateMoveHook = CreateTrampHook64_Advanced(
            reinterpret_cast<BYTE*>(funcptr),
            reinterpret_cast<BYTE*>(&HookPlayerPawn_CreateMove)
        );

    return hooks::swapChainPresentHook.get() && hooks::playerPawnCreateMoveHook.get();
}

void RemoveHooks() {
    hooks::swapChainPresentHook->Delete();
    hooks::playerPawnCreateMoveHook->Delete();
}
