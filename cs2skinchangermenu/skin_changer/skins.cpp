#include "pch.h"
#include "skins.h"

#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "sdk/gameclasses/CCSPlayerController.h"

#include "vpktool/VPK.h"
#include "globals.h"

#include <string>

namespace skins {
	std::array<std::tuple<void*, void*>, 4> wepMaterialPointers;
	std::unordered_map<uint32_t, SkinPreference> loadout = {};
}
bool bInitialized = false;
int initializationFails = 0;

// Makes the necessary pointers for our weapon materials
bool MakePointersToWeaponMaterials() {
	using fUtlMemory_Alloc = uint8_t* (__fastcall*)(void*, void*, size_t, void*);
	fUtlMemory_Alloc UtlMemory_Alloc = (fUtlMemory_Alloc)GetProcAddress(GetModuleHandle("tier0.dll"), "UtlMemory_Alloc");

	uint8_t* startOfPointerChain = UtlMemory_Alloc(0, 0, 4*(3*8), 0);
	if (!startOfPointerChain)
		return false;

	for (int i = 0; i < skins::wepMaterialPointers.size(); i++) {
		*reinterpret_cast<void**>(startOfPointerChain) = startOfPointerChain + 8;
		*reinterpret_cast<void**>(startOfPointerChain + 8) = 0;

		*reinterpret_cast<uintptr_t*>(startOfPointerChain + 16) = 0;

		skins::wepMaterialPointers[i] = std::make_tuple(startOfPointerChain, startOfPointerChain + 16);
		startOfPointerChain += 24;
	}

	return true;
}

// Check whether to update a certain weapon's skin based on the users' settings
bool ShouldUpdateSkin(C_CSPlayerPawn* localPawn, C_WeaponCSBase* weapon) {
	int itemDefIndex = weapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex();
	if (skins::loadout.find(itemDefIndex) == skins::loadout.end()) // skin preference not set
		return false;

	// check if weapon is owned by someone else
	if (weapon->m_hOwnerEntity().Get() != localPawn)
		return false;
}

// actual skin changer logic
void ApplySkins() {
	CCSPlayerController* localPlayer = *globals::ppLocalPlayer;
	if (!localPlayer)
		return;

	C_CSPlayerPawn* pawn = localPlayer->m_hPlayerPawn().Get();
	if (!pawn) 
		return;
	
	CPlayer_WeaponServices* wepServices = pawn->m_pWeaponServices();
	if (!wepServices)
		return;
	
	for (int wepNr = 0; wepNr < wepServices->m_hMyWeapons().Count(); wepNr++) {
		C_WeaponCSBase* weapon = wepServices->m_hMyWeapons()[wepNr].Get();
		if (!ShouldUpdateSkin(pawn, weapon))
			continue;

	}

}

// called for every CreateMove
void ApplySkinsCallback() {
	if (!bInitialized) {
		if (MakePointersToWeaponMaterials())
			bInitialized = true;
	}
	else
		ApplySkins();

}