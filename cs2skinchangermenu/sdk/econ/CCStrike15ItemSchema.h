#pragma once
#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CPaintKit.h"
#include "sdk/econ/CCStrike15ItemDefinition.h"
#include "sdk/econ/CEconItemSetDefinition.h"

class CCStrike15ItemSchema
{
public:
	CUtlMap<int, CPaintKit*> GetPaintKits();
	CUtlMap<int, CCStrike15ItemDefinition*> GetWeaponDefinitions();
	CUtlMap<char*, CEconItemSetDefinition> GetItemSets();
};

