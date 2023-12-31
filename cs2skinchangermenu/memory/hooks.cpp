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
    std::unique_ptr<Hook> meshGroupCopyHook;
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
}

void HookPlayerPawn_CreateMove(C_CSPlayerPawn* pawn, void* pUserCMD, void* pUserCMDCopy) {
    return;
}


// rcx <- location valve wants to write to
// rdx <- location valve wants to read the value from
// r8 (r9) <- jump table loc
void HookSetMeshGroupMaskCopy(void* rcx, void* rdx, void* r8) {
    ApplySkinsCallback(rdx);
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
        //reinterpret_cast<BYTE**>(&hooks::oIDXGISwapChain__Present);
    );

    void* funcptr = ScanPatternInModule("client.dll", PATTERN_PLAYERPAWNCREATEMOVE_PTR, MASK_PLAYERPAWNCREATEMOVE_PTR);
    if(funcptr)
        hooks::playerPawnCreateMoveHook = CreateTrampHook64_Advanced(
            reinterpret_cast<BYTE*>(funcptr),
            reinterpret_cast<BYTE*>(&HookPlayerPawn_CreateMove)
        );

    funcptr = ScanPatternInModule("client.dll", PATTERN_MESHGROUPMASKCOPIER_PTR, MASK_MESHGROUPMASKCOPIER_PTR);
    if (funcptr)
        hooks::meshGroupCopyHook = CreateTrampHook64_Advanced(
            reinterpret_cast<BYTE*>(funcptr),
            reinterpret_cast<BYTE*>(&HookSetMeshGroupMaskCopy)
        );

    return hooks::swapChainPresentHook.get() && hooks::playerPawnCreateMoveHook.get() && hooks::meshGroupCopyHook.get();
}

void RemoveHooks() {
    hooks::swapChainPresentHook->Delete();
    hooks::playerPawnCreateMoveHook->Delete();
    hooks::meshGroupCopyHook->Delete();
}
