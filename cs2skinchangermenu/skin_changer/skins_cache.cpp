#include "pch.h"
#include "skins_cache.h"

namespace skins_cache {
    std::unordered_map<uint32_t, std::vector<uint32_t>> paintkitsForWeapons;
    vpktool::VPK skinsPakFile;
}

// Map out all released paintkits for each weapon
void LoadPaintkitsForWeapons(CUtlMap<char*, CEconItemSetDefinition> itemSets) {
    for (const auto& iter : itemSets) {
        CEconItemSetDefinition set = iter.m_value;
        for (int i = 0; i < set.items.Count(); i++) {
            item_list_entry_t weaponKit = set.items[i];
            if (weaponKit.m_nPaintKit == 0 || weaponKit.m_nPaintKit == 0xFFFF) // invalid paintkit
                continue;
            if (weaponKit.m_nItemDef > 768)
                continue;

            skins_cache::paintkitsForWeapons[weaponKit.m_nItemDef].push_back(weaponKit.m_nPaintKit);
        }
    }
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

std::vector<TextureCache> LoadWeaponTextureThumbnails() {
    std::vector<TextureCache> thumbnails;
    if (!LoadPak01File())
        return thumbnails;

    const std::unordered_map<std::string, std::vector<vpktool::VPKEntry>>& entries = skins_cache::skinsPakFile.getEntries();
    if (!entries.contains("panorama/images/econ/default_generated"))
        return thumbnails;

    // load skins to cache
    thumbnails.reserve(5000);
    for (vpktool::VPKEntry entry : entries.at("panorama/images/econ/default_generated")) {
        // check if large_png is in the file name
        std::string suffix("large_png.vtex_c");
        if (entry.filename.size() <= strlen("large_png.vtex_c") || entry.filename.compare(entry.filename.size() - suffix.size(), suffix.size(), suffix) != 0)
            continue;

        TextureCache cache = { false, skins_cache::skinsPakFile, entry, nullptr };
        thumbnails.push_back(cache);
    }

    return thumbnails;
}