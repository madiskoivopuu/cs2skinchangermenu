#include "pch.h"
#include "cache.h"

#include "sdk/econ/CCStrike15ItemSchema.h"
#include "sdk/KeyValues/vdf_parser.hpp"
#include "memory/interface.h"

#include "skin_changer/skins_cache.h"

namespace cache {
    std::unordered_map<std::string, std::string> englishTranslations;
    CUtlMap<int, CCStrike15ItemDefinition*> weaponDefs;
    CUtlMap<int, CPaintKit*> paintKits;
    CUtlMap<char*, CEconItemSetDefinition> itemSets;
    CUtlMap<int, CStickerKit*> stickerKits;
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

    std::ifstream file(englishTranslationFileLoc);
    file.seekg(3);
    char c = file.get();
    if (c != '\"')
        throw std::runtime_error("problem with translation file");

    // read file to string
    file.seekg(3, std::ios::end);
    size_t size = file.tellg();
    std::string buffer(size, ' ');
    file.seekg(3);
    file.read(&buffer[0], size);

    tyti::vdf::object kv = tyti::vdf::read(std::cbegin(buffer), std::cend(buffer));

    if (kv.childs.size() == 0)
        return {};

    return kv.childs["Tokens"]->attribs;
}

bool LoadCache() {
    CCStrike15ItemSchema* schema = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema();
    if (!schema)
        return false;

    cache::weaponDefs = schema->GetWeaponDefinitions();
    cache::paintKits = schema->GetPaintKits();
    cache::itemSets = schema->GetItemSets();
    cache::stickerKits = schema->GetStickerKits();

    if(!cache::englishTranslations.size())
        cache::englishTranslations = ReadEnglishTranslation();

    // skins cache setup
    if (!LoadWeaponTextureThumbnails())
        return false;
    if (!LoadPaintkitsForWeapons(cache::itemSets))
        return false;

    return cache::weaponDefs.m_size && cache::paintKits.m_size && cache::itemSets.m_size && cache::englishTranslations.size();
}