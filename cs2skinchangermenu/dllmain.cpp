// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "memory/gamefuncs.h"
#include "memory/interface.h"
#include "memory/hooks.h"
#include "memory/tools/hook64.h"
#include "netvars/netvars.h"
#include "netvars/offsets.h"
#include "globals.h"
#include "gui/gui_setup.h"

#include "sdk/econ/CCStrike15ItemDefinition.h"

#include "cache.h"

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <conio.h>

int ExitRoutine(HMODULE hModule, FILE* f) {
    DestroyGUI();

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

void PreExitRoutine() {
    RemoveHooks();
}

DWORD WINAPI Main(HMODULE hModule) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    
    if (!LoadInterfaces() || !InitializeFunctions() || !InitializeNetvars() || !InitializeOffsets() || !InitializeGlobals() || !InitGUI() || !InitializeHooks())
        return ExitRoutine(hModule, f);

    LoadCache();
    //std::unordered_map<uint32_t, std::vector<uint32_t>> weaponPaintKits = GetPaintkitsForWeapons(itemSets);

    hooks::swapChainPresentHook->Enable();
    hooks::playerPawnCreateMoveHook->Enable();
    //hooks::engineFrameBoundary->Enable();

    //kv.data.keys["lang"].keys["tokens"];

    while (true) {
        //ApplySkinsCallback();
        Sleep(1);

        if (GetAsyncKeyState(VK_END) & 1)
            break;
    }

    PreExitRoutine();

    ExitRoutine(hModule, f);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);

        HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Main), hModule, 0, nullptr);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

