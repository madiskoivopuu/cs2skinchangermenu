#include "pch.h"
#include "skins_cache.h"
#include "skins.h"
#include "cache.h"
#include "netvars/fnvhash.h"
#include "gui/components/skin_changer_menu/helpers/skins_helper_funcs.h"

#include <format>

namespace skins_cache {
    std::unordered_map<uint32_t, std::vector<uint32_t>> paintkitsForWeapons = {};
    // key: weaponname_paintkitname
    // value: texture for that weapon's skin (_heavy at the end is preserved for well worn etc, large_png.vtex_c isn't)
    std::unordered_map<uint64_t, TextureCache> weaponSkins = {};
    vpktool::VPK skinsPakFile;
    std::vector<SkinPreference> loadoutAllPresets;

    std::unordered_map<uint32_t, SkinPreference*> activeLoadout = {};
}

// NB! LOAD WEAPON TEXTURES FIRST THIS NEEDS THEM
// Map out all released paintkits for each weapon
bool LoadPaintkitsForWeapons(CUtlMap<char*, CEconItemSetDefinition> itemSets) {
    for (const auto& iter : itemSets) {
        CEconItemSetDefinition set = iter.m_value;
        for (int i = 0; i < set.items.Count(); i++) {
            item_list_entry_t weaponKit = set.items[i];
            if (weaponKit.m_nPaintKit == 0 || weaponKit.m_nPaintKit == 0xFFFF) // invalid paintkit
                continue;
            if (!ShouldIncludeWeaponForSkin_ID(weaponKit.m_nItemDef))
                continue;

            skins_cache::paintkitsForWeapons[weaponKit.m_nItemDef].push_back(weaponKit.m_nPaintKit);
        }
    }

    // now we have to MANUALLY add all valid paint kits for gloves
    for (const auto& iter : cache::weaponDefs) {
        CCStrike15ItemDefinition* itemDef = iter.m_value;
        if (!strstr(itemDef->GetSubcategory(), "gloves"))
            continue;

        for (const auto& paintkitIter : cache::paintKits) {
            CPaintKit* paintKit = paintkitIter.m_value;
            std::string formatted = std::format("{}_{}_light", itemDef->GetSubcategory(), paintKit->paintKitName);
            if(skins_cache::weaponSkins.find(fnv::Hash(formatted.c_str())) != skins_cache::weaponSkins.end())
                skins_cache::paintkitsForWeapons[itemDef->m_iItemDefinitionIndex].push_back(paintKit->id);
        }
    }

    return skins_cache::paintkitsForWeapons.size() != 0;
}

std::string GetCsgoVPKPath() {
    char csgoFileLoc[MAX_PATH];
    GetModuleFileName(nullptr, csgoFileLoc, MAX_PATH); // D:\Counter-Strike 2 new build\game\bin\win64\cs2.exe

    std::string fileLoc(csgoFileLoc);
    fileLoc.replace(
        fileLoc.find("bin\\win64\\cs2.exe"),
        17,
        "csgo\\pak01_dir.vpk"
    );

    return fileLoc;
}

bool LoadPak01File() {
    std::string vpkPath = GetCsgoVPKPath();
    std::optional<vpktool::VPK> vpk = vpktool::VPK::open(vpkPath);
    if (!vpk.has_value())
        return false;

    skins_cache::skinsPakFile = std::move(vpk.value());
    return true;
}

// Loads weapon textures into a cache which is accessible by the fnv hashed key weaponname_paintkitname (_light, _medium, _heavy for wear is kept is kept, rest isnt)
bool LoadWeaponTextureThumbnails() {
    if (!LoadPak01File())
        return false;

    const std::unordered_map<std::string, std::vector<vpktool::VPKEntry>>& entries = skins_cache::skinsPakFile.getEntries();
    if (!entries.contains("panorama/images/econ/default_generated"))
        return false;

    // load skins to cache
    skins_cache::weaponSkins.reserve(7000);
    for (vpktool::VPKEntry entry : entries.at("panorama/images/econ/default_generated")) {
        // check if large_png is in the file name
        std::string suffix("_large_png.vtex_c");
        if (entry.filename.size() <= suffix.size() || entry.filename.compare(entry.filename.size() - suffix.size(), suffix.size(), suffix) != 0)
            continue;

        std::string cacheKey = entry.filename.substr(0, entry.filename.size() - suffix.size());
        TextureCache cache = { entry };
        skins_cache::weaponSkins[fnv::Hash(cacheKey.c_str())] = cache;
    }

    // load regular weapons to our cache
    for (vpktool::VPKEntry entry : entries.at("panorama/images/econ/weapons/base_weapons")) {
        // check if large_png is in the file name
        std::string suffix("_png.vtex_c");
        std::string cacheKey = entry.filename.substr(0, entry.filename.size() - suffix.size());
        TextureCache cache = { entry };
        skins_cache::weaponSkins[fnv::Hash(cacheKey.c_str())] = cache;
    }

    // load sticker image data to cache
    for (auto keyValuePair : entries) {
        std::string key = keyValuePair.first;
        if (key.find("panorama/images/econ/stickers") == std::string::npos) continue;

        for (vpktool::VPKEntry entry : entries.at(key)) {
            // check if large_png is in the file name
            std::string suffix("_large_png.vtex_c");
            if (entry.filename.size() <= suffix.size() || entry.filename.compare(entry.filename.size() - suffix.size(), suffix.size(), suffix) != 0)
                continue;

            std::string cacheKey = std::format("{}/{}", 
                key.substr(strlen("panorama/images/")),
                entry.filename.substr(0, entry.filename.size() - suffix.size())
            );
            TextureCache cache = { entry };
            skins_cache::weaponSkins[fnv::Hash(cacheKey.c_str())] = cache;
        }
    }

    return true;
}

SkinPreference* CreateAndActivateNewPreference() {
    SkinPreference pref = { };
    pref.weaponID = 7;
    skins_cache::loadoutAllPresets.push_back(pref);

    skins_cache::activeLoadout[pref.weaponID] = &skins_cache::loadoutAllPresets.back();
    return &skins_cache::loadoutAllPresets.back();
}

void ChangeWeaponForSkinPreference(SkinPreference* pref, int oldPreferenceID, CCStrike15ItemDefinition* newItemDef) {
    skins_cache::activeLoadout.erase(oldPreferenceID);

    // reset paintkit
    pref->paintKitID = -1;
    pref->weaponID = newItemDef->m_iItemDefinitionIndex;

    // enable new preference
    if(strstr(newItemDef->GetSubcategory(), "gloves"))
        skins_cache::activeLoadout[skins::ID_GLOVE_PREFERENCE] = pref;
    else
        skins_cache::activeLoadout[pref->weaponID] = pref;
}

// Load all user's preset skins from file(server) to memory.
// TODO: 
bool LoadUserSkinPreferences() {
    skins_cache::loadoutAllPresets.reserve(1000);
    skins_cache::loadoutAllPresets.push_back(
        SkinPreference{
        4, // weapon id

        278,
        0,
        0.10f,

        true,
        11,
        "",

        {
            Sticker{76},
            Sticker{6627},
            Sticker{6611},
            Sticker{76},
        }
        }
    );

    return false;
}