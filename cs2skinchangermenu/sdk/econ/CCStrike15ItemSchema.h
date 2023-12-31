#pragma once
#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CPaintKit.h"
#include "sdk/econ/CCStrike15ItemDefinition.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "sdk/econ/CEconItemAttributeDefinition.h"
#include "sdk/econ/CStickerKit.h"

class CCStrike15ItemSchema
{
public:
	CUtlMap<int, CPaintKit*> GetPaintKits();
	CUtlMap<int, CStickerKit*> GetStickerKits();
	CUtlMap<int, CCStrike15ItemDefinition*> GetWeaponDefinitions();
	CUtlMap<char*, CEconItemSetDefinition> GetItemSets();
	CEconItemAttributeDefinition* GetAttributeDefinitionByName(char* name);
};

