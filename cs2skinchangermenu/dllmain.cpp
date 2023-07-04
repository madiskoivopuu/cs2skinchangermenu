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
#include "sdk/KeyValues/ValveKeyValues.h"

#include "skin_changer/skins.h"

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

// TODO: use new lib
std::unordered_map<std::string, std::string> ReadEnglishTranslation() {
    char csgoFileLoc[MAX_PATH];
    GetModuleFileName(nullptr, csgoFileLoc, MAX_PATH); // D:\Counter-Strike 2 new build\game\bin\win64\cs2.exe
    std::string fileContent;

    std::string englishTranslationFileLoc(csgoFileLoc);
    englishTranslationFileLoc.replace(
        englishTranslationFileLoc.find("bin\\win64\\cs2.exe"),
        17,
        "csgo\\resource\\csgo_english.txt"
    );

    Valve::ValveFileFormat::Parser kvParser;
    std::unique_ptr<Valve::ValveFileFormat::Node> kv = kvParser.Parse(englishTranslationFileLoc);

    // error checks
    if (!kv.get())
        return {};

    if (kv.get()->GetChildren().size() == 0)
        return {};
    
    Valve::ValveFileFormat::Node tokens = kv.get()->GetChildren().at(0);
    return tokens.GetProperties();
}

void HookTestFn() {
    return;
}

DWORD WINAPI Main(HMODULE hModule) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    
    if (!LoadInterfaces() || !InitializeFunctions() || !InitializeNetvars() || !InitializeOffsets() || !InitializeGlobals() || !InitGUI() || !InitializeHooks())
        return ExitRoutine(hModule, f);

    CCStrike15ItemSchema* schema = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema();
    if(!schema)
        return ExitRoutine(hModule, f);

    CUtlMap<int, CCStrike15ItemDefinition*> weaponDefs = schema->GetWeaponDefinitions();
    CUtlMap<int, CPaintKit*> paintKits = schema->GetPaintKits();
    CUtlMap<char*, CEconItemSetDefinition> itemSets = schema->GetItemSets();

    std::cout << (*globals::ppLocalPlayer)->m_hPlayerPawn().Get()->m_pWeaponServices()->m_hMyWeapons()[0].Get()->m_hOwnerEntity().Get() << std::endl;

    //LoadWeaponTextureThumbnails();

    //std::unordered_map<std::string, std::string> englishTranslations = ReadEnglishTranslation();
    //std::unordered_map<uint32_t, std::vector<uint32_t>> weaponPaintKits = GetPaintkitsForWeapons(itemSets);
    //hooks::scPresentHook->Enable();

    //kv.data.keys["lang"].keys["tokens"];

    while (true) {
        Sleep(10);

        if (GetAsyncKeyState(VK_END) & 1)
            break;
    }

    hooks::scPresentHook->Disable();

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

