#include "pch.h"
#include "cache.h"

#include "sdk/econ/CCStrike15ItemSchema.h"
#include "sdk/KeyValues/ValveKeyValues.h"
#include "memory/interface.h"

#include "skin_changer/skins_cache.h"

namespace cache {
    std::unordered_map<std::string, std::string> englishTranslations;
    CUtlMap<int, CCStrike15ItemDefinition*> weaponDefs;
    CUtlMap<int, CPaintKit*> paintKits;
    CUtlMap<char*, CEconItemSetDefinition> itemSets;
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

bool LoadCache() {
    CCStrike15ItemSchema* schema = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema();
    if (!schema)
        return false;

    cache::weaponDefs = schema->GetWeaponDefinitions();
    cache::paintKits = schema->GetPaintKits();
    cache::itemSets = schema->GetItemSets();

    if(!cache::englishTranslations.size())
        cache::englishTranslations = ReadEnglishTranslation();

    // skins cache setup
    if (!LoadWeaponTextureThumbnails())
        return false;
    if (!LoadPaintkitsForWeapons(cache::itemSets))
        return false;

    return cache::weaponDefs.m_size && cache::paintKits.m_size && cache::itemSets.m_size && cache::englishTranslations.size();
}