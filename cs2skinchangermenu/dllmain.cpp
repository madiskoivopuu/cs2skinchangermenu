// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "memory/gamefuncs.h"
#include "memory/interface.h"
#include "memory/tools/hook64.h"

#include "sdk/econ/CCStrike15ItemDefinition.h"
#include "sdk/KeyValues/ValveKeyValues.h"

// nuklear
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
#include "gui/gui.h"

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <conio.h>

int ExitRoutine(HMODULE hModule, FILE* f) {
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

void HookTestFn(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    std::cout << std::hex << a << " " << b << " " << c << " " << d << std::dec << std::endl;
}

int SomeRandomFunc(void* ptr1, void* ptr2) {
    printf("%d", ptr1);
    return 15;
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
    
    if (!LoadInterfaces() || !InitializeFunctions())
        return ExitRoutine(hModule, f);

    CCStrike15ItemSchema* schema = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema();
    if(!schema)
        return ExitRoutine(hModule, f);

    CUtlMap<int, CCStrike15ItemDefinition*> weaponDefs = schema->GetWeaponDefinitions();
    CUtlMap<int, CPaintKit*> paintKits = schema->GetPaintKits();
    CUtlMap<char*, CEconItemSetDefinition> itemSets = schema->GetItemSets();

    //std::unordered_map<std::string, std::string> englishTranslations = ReadEnglishTranslation();
    std::unordered_map<uint32_t, std::vector<uint32_t>> weaponPaintKits = GetPaintkitsForWeapons(itemSets);

    std::unique_ptr<Hook> memAllocHook = CreateHook64Standalone(
        reinterpret_cast<BYTE*>(GetProcAddress(GetModuleHandle("tier0.dll"), "UtlMemory_Alloc")),
        reinterpret_cast<BYTE*>(&HookTestFn),
        0
    );
    //Sleep(8000);
    memAllocHook.get()->Enable();

    /*InitGUI();
    
    struct nk_font_atlas* atlas;
    nk_d3d11_font_stash_begin(&atlas);
    nk_d3d11_font_stash_end();
    
    struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    while (true) {
        if (nk_begin(nuklearCtx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum { EASY, HARD };
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(nuklearCtx, 30, 80, 1);
            if (nk_button_label(nuklearCtx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(nuklearCtx, 30, 2);
            if (nk_option_label(nuklearCtx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(nuklearCtx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(nuklearCtx, 22, 1);
            nk_property_int(nuklearCtx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(nuklearCtx, 20, 1);
            nk_label(nuklearCtx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(nuklearCtx, 25, 1);
            if (nk_combo_begin_color(nuklearCtx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(nuklearCtx), 400))) {
                nk_layout_row_dynamic(nuklearCtx, 120, 1);
                bg = nk_color_picker(nuklearCtx, bg, NK_RGBA);
                nk_layout_row_dynamic(nuklearCtx, 25, 1);
                bg.r = nk_propertyf(nuklearCtx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(nuklearCtx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(nuklearCtx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(nuklearCtx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(nuklearCtx);
            }
        }
        nk_end(nuklearCtx);
        Sleep(1);
    }*/

    //kv.data.keys["lang"].keys["tokens"];

    while (true) {
        Sleep(10);

        if (GetAsyncKeyState(VK_END) & 1)
            break;
    }

    memAllocHook.get()->Delete();
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

