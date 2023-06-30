// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "memory/gamefuncs.h"
#include "memory/interface.h"
#include "memory/hooks.h"
#include "memory/tools/hook64.h"
#include "gui/gui_setup.h"

#include "sdk/econ/CCStrike15ItemDefinition.h"
#include "sdk/KeyValues/ValveKeyValues.h"

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

// Map out all released paintkits for each weapon
std::unordered_map<uint32_t, std::vector<uint32_t>> GetPaintkitsForWeapons(CUtlMap<char*, CEconItemSetDefinition> itemSets) {
    std::unordered_map<uint32_t, std::vector<uint32_t>> weaponPaintKits; // weapon -> [paintkit...]
    
    for (const auto& iter : itemSets) {
        CEconItemSetDefinition set = iter.m_value;
        for (int i = 0; i < set.items.Count(); i++) {
            item_list_entry_t weaponKit = set.items[i];
            if (weaponKit.m_nPaintKit == 0 || weaponKit.m_nPaintKit == 0xFFFF) // invalid paintkit
                continue;
            if (weaponKit.m_nItemDef > 768)
                continue;

            weaponPaintKits[weaponKit.m_nItemDef].push_back(weaponKit.m_nPaintKit);
        }
    }

    return weaponPaintKits;
}

DWORD WINAPI Main(HMODULE hModule) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    
    if (!LoadInterfaces() || !InitializeFunctions() || !InitGUI() || !InitializeHooks())
        return ExitRoutine(hModule, f);

    CCStrike15ItemSchema* schema = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema();
    if(!schema)
        return ExitRoutine(hModule, f);

    CUtlMap<int, CCStrike15ItemDefinition*> weaponDefs = schema->GetWeaponDefinitions();
    CUtlMap<int, CPaintKit*> paintKits = schema->GetPaintKits();
    CUtlMap<char*, CEconItemSetDefinition> itemSets = schema->GetItemSets();

    //std::unordered_map<std::string, std::string> englishTranslations = ReadEnglishTranslation();
    std::unordered_map<uint32_t, std::vector<uint32_t>> weaponPaintKits = GetPaintkitsForWeapons(itemSets);
    hooks::scPresentHook->Enable();

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

