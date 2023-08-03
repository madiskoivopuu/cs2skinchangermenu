#include "pch.h"
#include "gloves.h"

#include "skins_cache.h"
#include "skins.h"
#include "memory/gamefuncs.h"

#include "sdk/CUtl/CUtlMap.h"
#include "sdk/econ/CEconItemSetDefinition.h"
#include "sdk/gameclasses/CCSPlayerController.h"

#include <thread>

bool ShouldUpdateGloves(C_CSPlayerPawn* pawn) {
	if (skins_cache::activeLoadout.find(skins::ID_GLOVE_PREFERENCE) == skins_cache::activeLoadout.end())
		return false;

	SkinPreference* pref = skins_cache::activeLoadout[skins::ID_GLOVE_PREFERENCE];
	if (!pref)
		return false;

	if (pref->paintKitID == -1)
		return false;

	if (pawn->m_EconGloves().m_iItemDefinitionIndex() != pref->weaponID)
		return true;

	// no attributes but preference is enabled
	if (pawn->m_EconGloves().m_AttributeList().m_Attributes().Count() == 0)
		return true;

	CAttributeList& attrList = pawn->m_EconGloves().m_AttributeList();
	CEconItemAttribute* paintAttr = attrList.FindAttribute(const_cast<char*>("set item texture prefab"));
	CEconItemAttribute* seedAttr = attrList.FindAttribute(const_cast<char*>("set item texture seed"));
	CEconItemAttribute* wearAttr = attrList.FindAttribute(const_cast<char*>("set item texture wear"));

	// skin
	if (paintAttr && paintAttr->m_flValue() != static_cast<float>(pref->paintKitID))
		return true;
	// pattern id
	if (seedAttr && seedAttr->m_flValue() != static_cast<float>(pref->seed))
		return true;
	// float
	if (wearAttr && wearAttr->m_flValue() != pref->wearValue)
		return true;

	return false;
}

void ForceGlovesUpdate(C_CSGOViewModel* viewModel) {
	int magicNr = 4047747114;
	int64_t offset = fn::GetNextSceneEventIDOffset(&viewModel->m_nNextSceneEventId(), &magicNr, magicNr, false);

	uint8_t* dataLoc = *reinterpret_cast<uint8_t**>(&viewModel->m_nNextSceneEventId()) + offset * 0x10;
	*reinterpret_cast<int*>(dataLoc + 0xc) -= 1;
}
// weird shit to make the gloves update properly
std::atomic_bool gloveApplyThreadRunning;
void forceAsyncUpdate(SkinPreference pref, C_CSPlayerPawn* pawn, C_CSGOViewModel* viewModel) {
	gloveApplyThreadRunning = true;

	for (int i = 0; i < 10; i++) {
		// we should probably try checking if memory addresses have been allocated...
		pawn->m_EconGloves().m_iItemDefinitionIndex() = pref.weaponID; // this will be the gloves id
		pawn->m_EconGloves().SetAttributeValueByName("set item texture prefab", static_cast<float>(pref.paintKitID));
		pawn->m_EconGloves().SetAttributeValueByName("set item texture seed", static_cast<float>(pref.seed));
		pawn->m_EconGloves().SetAttributeValueByName("set item texture wear", static_cast<float>(pref.wearValue));

		pawn->m_EconGloves().m_bInitialized() = true;
		pawn->m_bNeedToReApplyGloves() = true;
		ForceGlovesUpdate(viewModel);
		Sleep(2);
	}

	gloveApplyThreadRunning = false;
}

void ApplyGloves(C_CSPlayerPawn* pawn, C_CSGOViewModel* viewModel) {

	if (!gloveApplyThreadRunning && ShouldUpdateGloves(pawn)) { //skins_cache::activeLoadout.find(skins::ID_GLOVE_PREFERENCE) != skins_cache::activeLoadout.end()) {
		SkinPreference pref = *skins_cache::activeLoadout[skins::ID_GLOVE_PREFERENCE];

		std::thread t1(forceAsyncUpdate, pref, pawn, viewModel);
		t1.detach();
	}
}