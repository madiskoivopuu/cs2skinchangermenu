#include "pch.h"
#include "skins.h"
#include "skins_cache.h"

#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "sdk/gameclasses/CCSPlayerController.h"

#include "vpktool/VPK.h"
#include "globals.h"
#include "memory/gamefuncs.h"

#include <string>
#include <iostream>
#include <format>

namespace skins {
	const int ID_GLOVE_PREFERENCE = 9005;
	const int ID_KNIFE_PREFERENCE = 9006;
	bool regenViewmodel = false;
}

// Check whether to update a certain weapon's skin based on the users' settings
// Returns false at the end of the function if any rigorous checks didn't go through
bool ShouldUpdateSkin(C_CSPlayerPawn* localPawn, C_WeaponCSBase* weapon) {
	int itemDefIndex = weapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex();
	if (skins_cache::activeLoadout.find(itemDefIndex) == skins_cache::activeLoadout.end()) // skin preference not set
		return false;

	// if the weapon doesn't have vdata set then we do not want to update, as some of the following functions need vdata
	if (weapon->m_pWeaponVData() == nullptr)
		return false;

	// ignore grenades and anything higher than slot 2
	if (weapon->m_pWeaponVData()->m_GearSlot() > 2)
		return false;

	// check if weapon is owned by someone else
	if (weapon->m_hOwnerEntity().GetEnt() != localPawn)
		return false;

	// check if there are no attributes even though we have a skin set
	if (weapon->m_AttributeManager().m_Item().m_AttributeList().m_Attributes().Count() == 0)
		return true;

	// check if weapon already has the same skin or float
	SkinPreference skinPref = *skins_cache::activeLoadout.at(itemDefIndex);

	CAttributeList& attrs = weapon->m_AttributeManager().m_Item().m_AttributeList();
	for (int i = 0; i < attrs.m_Attributes().Count(); i++) {
		CEconItemAttribute attr = attrs.m_Attributes().Element(i);
		// skin
		if (attr.m_iAttributeDefinitionIndex() == 6 && attr.m_flValue() != static_cast<float>(skinPref.paintKitID))
			return true;
		// pattern id
		if (attr.m_iAttributeDefinitionIndex() == 7 && attr.m_flValue() != static_cast<float>(skinPref.seed))
			return true;
		// float
		if (attr.m_iAttributeDefinitionIndex() == 8 && attr.m_flValue() != skinPref.wearValue)
			return true;
		// kill eater aka stattrak counter
		if (attr.m_iAttributeDefinitionIndex() == 80 && attr.m_flValue() != static_cast<float>(skinPref.stattrakKills))
			return true;
	}

	// check for stattrak add or removal
	if (skinPref.useStattrak && weapon->m_hStattrakEntity().IsInvalid())
		return true;

	if (!skinPref.useStattrak && !weapon->m_hStattrakEntity().IsInvalid())
		return true;

	// check for nametag add or removal
	if (strlen(skinPref.nametag) != 0 && weapon->m_hNametagEntity().IsInvalid())
		return true;

	if (strlen(skinPref.nametag) == 0 && !weapon->m_hNametagEntity().IsInvalid())
		return true;

	if (strcmp(&weapon->m_AttributeManager().m_Item().m_szCustomName(), skinPref.nametag) != 0)
		return true;

	// check for sticker changes
	for (int i = 0; i < skinPref.stickers.size(); i++) {
		std::string formattedName = std::format("sticker slot {} id", i);
		CEconItemAttribute* attr = weapon->m_AttributeManager().m_Item().m_AttributeList().FindAttribute(const_cast<char*>(formattedName.c_str()));

		// no sticker on weapon but sticker is set
		if (!attr && skinPref.stickers[i].id != -1)
			return true;

		// sticker on weapon when it is not supposed to be
		if (attr && skinPref.stickers[i].id == -1)
			return true;
	}

	return false;
}

void ForceStattrakUpdate(C_CSGOViewModel* viewModel) {
	int magicNr = 3369543006;
	int64_t offset = fn::GetNextSceneEventIDOffset(&viewModel->m_nNextSceneEventId(), &magicNr, magicNr, false);

	uint8_t* dataLoc = *reinterpret_cast<uint8_t**>(&viewModel->m_nNextSceneEventId()) + offset * 0x10;
	*reinterpret_cast<int*>(dataLoc + 0xc) += 1;
}

// Sets the stattrak on our weapon depending whether it is enabled or not
void SetStattrak(C_CSGOViewModel* viewModel, C_WeaponCSBase* weapon, SkinPreference* pref) {
	C_EconItemView& weaponEconItem = weapon->m_AttributeManager().m_Item();

	if (!pref->useStattrak)
		return weapon->m_hStattrakEntity().Set(-1);


	weaponEconItem.SetAttributeValueByName(const_cast<char*>("kill eater"), static_cast<float>(pref->stattrakKills));
	weaponEconItem.SetAttributeValueByName(const_cast<char*>("kill eater score type"), 0.0f);

	//Sleep(100);
	if (weapon->m_hStattrakEntity().IsInvalid())
		fn::SpawnAndSetStattrakEnt(&weapon->m_hStattrakEntity());

	// TODO: find better method for this
	//ForceStattrakUpdate(viewModel);
}

void SetNametag(C_WeaponCSBase* weapon, SkinPreference* pref) {
	if (strlen(pref->nametag) == 0) {
		weapon->m_hNametagEntity().Set(-1);
		memset(&weapon->m_AttributeManager().m_Item().m_szCustomName(), 0x00, sizeof(pref->nametag));
	}

	weapon->m_hNametagEntity().Set(-1);
	memcpy(&weapon->m_AttributeManager().m_Item().m_szCustomName(), pref->nametag, sizeof(pref->nametag));
	fn::SpawnAndSetNametagEnt(&weapon->m_hNametagEntity());
}

void SetStickers(C_WeaponCSBase* weapon, SkinPreference* pref) {
	C_EconItemView& weaponEconItem = weapon->m_AttributeManager().m_Item();

	for (int i = 0; i < pref->stickers.size(); i++) {
		std::string formatted = std::format("sticker slot {} id", i);
		std::string formattedWear = std::format("sticker slot {} wear", i);

		Sticker sticker = pref->stickers[i];

		if (sticker.id == -1)
			weaponEconItem.m_AttributeList().RemoveAttribute(const_cast<char*>(formatted.c_str()));
		else
			weaponEconItem.SetAttributeValueIntByName(formatted.c_str(), sticker.id);
	}
}

// Adds all necessary attributes etc to the weapon & forcefully updates its skin
void SetAndUpdateSkin(C_CSGOViewModel* viewModel, C_WeaponCSBase* weapon) {
	C_EconItemView& weaponEconItem = weapon->m_AttributeManager().m_Item();
	SkinPreference* pref = skins_cache::activeLoadout.at(weaponEconItem.m_iItemDefinitionIndex());

	// add stattrak, nametag and stickers to weapon OR remove them
	SetStattrak(viewModel, weapon, pref);
	SetNametag(weapon, pref);
	SetStickers(weapon, pref);

	// add skin to weapon
	weaponEconItem.SetAttributeValueByName(const_cast<char*>("set item texture prefab"), static_cast<float>(pref->paintKitID));
	weaponEconItem.SetAttributeValueByName(const_cast<char*>("set item texture seed"), static_cast<float>(pref->seed));
	weaponEconItem.SetAttributeValueByName(const_cast<char*>("set item texture wear"), pref->wearValue);

	// TODO: add if check to see if we need to enable this for legacy paints
	weapon->m_pGameSceneNode()->m_skeletonInstance().m_modelState().m_MeshGroupMask() = 2;
	viewModel->m_pGameSceneNode()->m_skeletonInstance().m_modelState().m_MeshGroupMask() = 2;

	fn::AllowSkinRegenForWeapon(weapon->m_pWeaponSecondVTable(), true); // weird issue with 1st argument being dereferenced incorrectly by the compiler when using a reference to a pointer that has been dereferenced
	fn::RegenerateWeaponSkin(weapon);
	//fn::RegenerateAllWeaponSkins(); // updates stickers
	fn::UpdateViewmodelAttachments(viewModel, weapon);
}

// actual skin changer logic
void ApplySkins() {
	CCSPlayerController* localPlayer = *globals::ppLocalPlayer;
	if (!localPlayer)
		return;

	if (!localPlayer->m_bPawnIsAlive())
		return;

	C_CSPlayerPawn* pawn = localPlayer->m_hPlayerPawn().GetEnt();
	if (!pawn) 
		return;
	
	CPlayer_WeaponServices* wepServices = pawn->m_pWeaponServices();
	if (!wepServices)
		return;

	C_CSGOViewModel* viewModel = pawn->m_pViewModelServices()->m_hViewModel().GetEnt();
	if (!viewModel)
		return;
	
	pawn->m_nNextSceneEventId() += 15;
	pawn->m_EconGloves().m_iItemDefinitionIndex() = 5033;
	pawn->m_EconGloves().m_iItemID() = -1;
	pawn->m_EconGloves().m_iItemIDLow() = -1;
	pawn->m_EconGloves().m_iItemIDHigh() = -1;
	pawn->m_EconGloves().SetAttributeValueByName("set item texture prefab", 0.0f);
	pawn->m_EconGloves().SetAttributeValueByName("set item texture seed", 0.0f);
	pawn->m_EconGloves().SetAttributeValueByName("set item texture wear", 0.8f);

	pawn->m_EconGloves().m_bInitialized() = true;
	pawn->m_bNeedToReApplyGloves() = true;

	C_WeaponCSBase* weapon = wepServices->m_hActiveWeapon().GetEnt();
	if(weapon != nullptr) {
		if (!ShouldUpdateSkin(pawn, weapon))
			return;

		//std::cout << weapon->m_pGameSceneNode() << std::endl;
		//std::cout << &weapon->m_pCBodyComponent()->m_skeletonInstance().m_modelState().m_MeshGroupMask() << std::endl;
		SetAndUpdateSkin(viewModel, weapon);
	}
}

// called for every CreateMove
void ApplySkinsCallback() {
	// initialization code possibly
	ApplySkins();
}