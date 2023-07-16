#pragma once

#include <string>
#include <unordered_map>
#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CCStrike15ItemDefinition.h"
#include "sdk/econ/CPaintKit.h"
#include "sdk/econ/CEconItemSetDefinition.h"

namespace cache {
	extern std::unordered_map<std::string, std::string> englishTranslations;
	extern CUtlMap<int, CCStrike15ItemDefinition*> weaponDefs;
	extern CUtlMap<int, CPaintKit*> paintKits;
	extern CUtlMap<char*, CEconItemSetDefinition> itemSets;
}

// Initializes and loads EVERY cache.
bool LoadCache();
