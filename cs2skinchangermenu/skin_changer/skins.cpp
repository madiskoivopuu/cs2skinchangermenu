#include "pch.h"
#include "skins.h"

#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "sdk/gameclasses/CCSPlayerController.h"

#include "vpktool/VPK.h"
#include "globals.h"
#include "memory/gamefuncs.h"

#include <string>
#include <iostream>

namespace skins {
	std::array<std::tuple<void*, void*>, 4> wepMaterialPointers;
	std::unordered_map<uint32_t, SkinPreference> loadout = { 
		// TODO: remove
		{
			7, 
			SkinPreference{
				879,
				0,
				0.10f
			}
		}
	};
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
// Returns false at the end of the function if any rigorous checks didn't go through
bool ShouldUpdateSkin(C_CSPlayerPawn* localPawn, C_WeaponCSBase* weapon) {
	int itemDefIndex = weapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex();
	if (skins::loadout.find(itemDefIndex) == skins::loadout.end()) // skin preference not set
		return false;

	// ignore grenades and anything higher than slot 2
	if (weapon->m_AttributeManager().m_Item().GetCSWeaponDataFromItem()->m_GearSlot() > 2)
		return false;

	// check if weapon is owned by someone else
	if (weapon->m_hOwnerEntity().Get() != localPawn)
		return false;

	// check if the weapon doesn't have a material set, then we should update
	if (weapon->m_ppMaterial() == nullptr)
		return true;

	// check if there are no attributes even though we have a skin set
	if (weapon->m_AttributeManager().m_Item().m_AttributeList().m_Attributes().Count() == 0)
		return true;

	// check if weapon already has the same skin or float
	SkinPreference skinPref = skins::loadout.at(itemDefIndex);

	CAttributeList attrs = weapon->m_AttributeManager().m_Item().m_AttributeList();
	for (int i = 0; i < attrs.m_Attributes().Count(); i++) {
		CEconItemAttribute attr = attrs.m_Attributes().Element(i);
		// skin
		if (attr.m_iAttributeDefinitionIndex() == 6 && attr.m_flValue() != static_cast<float>(skinPref.paintKitID))
			return true;
		// float
		if (attr.m_iAttributeDefinitionIndex() == 8 && attr.m_flValue() != skinPref.wearValue)
			return true;
	}

	// TODO: check for nametag, stattrak
	// TODO: check for sticker changes

	return false;
}

// UJpdates the material pointer and regen count pointer for the weapon if needed
void UpdateMatsIfNeeded(C_WeaponCSBase* weapon) {
	if (!weapon->m_ppMaterial() || !weapon->m_pRegenCount()) {
		CWeaponCSBaseVData* vdata = weapon->m_AttributeManager().m_Item().GetCSWeaponDataFromItem();
		int slot = vdata->m_GearSlot();
		if (weapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex() == 31) // weapon_taser
			slot += 1;

		// set material and regen count ptr
		std::tuple<void*, void*> matAndRegenPtr = skins::wepMaterialPointers[slot];
		weapon->m_ppMaterial() = std::get<0>(matAndRegenPtr);
		weapon->m_pRegenCount() = std::get<1>(matAndRegenPtr);
	}
}

// Adds all necessary attributes etc to the weapon & forcefully updates its skin
void SetAndUpdateSkin(C_WeaponCSBase* weapon) {
	C_EconItemView& weaponEconItem = weapon->m_AttributeManager().m_Item();
	SkinPreference pref = skins::loadout.at(weaponEconItem.m_iItemDefinitionIndex());

	weaponEconItem.SetAttributeValueByName(const_cast<char*>("set item texture prefab"), static_cast<float>(pref.paintKitID));
	weaponEconItem.SetAttributeValueByName(const_cast<char*>("set item texture seed"), static_cast<float>(pref.seed));
	weaponEconItem.SetAttributeValueByName(const_cast<char*>("set item texture wear"), pref.wearValue);

	// TODO: add stattrak and nametag attachments later

	fn::AllowSkinRegenForWeapon(weapon->m_pWeaponSecondVTable(), true); // weird issue with 1st argument being dereferenced incorrectly by the compiler when using a reference to a pointer that has been dereferenced
	//fn::RegenerateWeaponSkin(weapon);
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

		UpdateMatsIfNeeded(weapon);
		SetAndUpdateSkin(weapon);
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