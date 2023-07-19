#include "pch.h"
#include "CCStrike15ItemSchema.h"
#include "sdk/CUtl/CUtlMap.h"

#include "memory/gamefuncs.h"

CUtlMap<int, CPaintKit*> CCStrike15ItemSchema::GetPaintKits() {
	return *reinterpret_cast< CUtlMap<int, CPaintKit*>* >(this + 0x2E8);
}


CUtlMap<int, CStickerKit*> CCStrike15ItemSchema::GetStickerKits() {
	return *reinterpret_cast<CUtlMap<int, CStickerKit*>*>(this + 0x310);
}

CUtlMap<int, CCStrike15ItemDefinition*> CCStrike15ItemSchema::GetWeaponDefinitions() {
	return *reinterpret_cast< CUtlMap<int, CCStrike15ItemDefinition*>* > (this + 0x120);
}

CUtlMap<char*, CEconItemSetDefinition> CCStrike15ItemSchema::GetItemSets() {
	return *reinterpret_cast<CUtlMap<char*, CEconItemSetDefinition>*> (this + 0x190); // how tf does not casting to uint8_t* and adding offset work correctly here and not in header`????
}

CEconItemAttributeDefinition* CCStrike15ItemSchema::GetAttributeDefinitionByName(char* name) {
	return fn::CEconItemSchema__GetAttributeDefinitionByName(this, name);
}
