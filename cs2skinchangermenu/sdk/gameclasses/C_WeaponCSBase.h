#pragma once
#include "netvars/netvars.h"
#include "netvars/offsets.h"

#include "C_AttributeContainer.h"
#include "CHandle.h"
#include "C_CSPlayerPawn.h"
#include "CWeaponCSBaseVData.h"
#include "CBodyComponentSkeletonInstance.h"
#include "CBodyComponent.h"


class C_CSPlayerPawn; // forwarddecl

class C_WeaponCSBase {
	//char size[0x1750];
public:
	NETVAR(CHandle<C_CSPlayerPawn>, m_hOwnerEntity, "client.dll!C_BaseEntity->m_hOwnerEntity");

	NETVAR(uint32_t, m_nViewModelIndex, "client.dll!C_WeaponCSBase->m_nViewModelIndex");

	// econ ent
	NETVAR(C_AttributeContainer, m_AttributeManager, "client.dll!C_EconEntity->m_AttributeManager");
	NETVAR(uint32_t, m_OriginalOwnerXuidLow, "client.dll!C_EconEntity->m_OriginalOwnerXuidLow");
	NETVAR(uint32_t, m_OriginalOwnerXuidHigh, "client.dll!C_EconEntity->m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackPaintKit, "client.dll!C_EconEntity->m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "client.dll!C_EconEntity->m_nFallbackSeed");
	NETVAR(float, m_flFallbackWear, "client.dll!C_EconEntity->m_flFallbackWear");
	NETVAR(int32_t, m_nFallbackStatTrak, "client.dll!C_EconEntity->m_nFallbackStatTrak");

	// skin changer stuff
	NETVAR_WITH_OFFSET(CWeaponCSBaseVData*, m_pWeaponVData, "client.dll!C_BaseEntity->m_nSubclassID", +8);
	NETVAR_WITH_OFFSET(CHandle<void*>, m_hStattrakEntity, "client.dll!C_WeaponCSBase->m_iNumEmptyAttacks", +4);
	NETVAR_WITH_OFFSET(CHandle<void*>, m_hNametagEntity, "client.dll!C_WeaponCSBase->m_iNumEmptyAttacks", +20);

	NETVAR(CBodyComponentSkeletonInstance*, m_pGameSceneNode, "client.dll!C_BaseEntity->m_pGameSceneNode");
	NETVAR(CBodyComponent*, m_CBodyComponent, "client.dll!C_BaseEntity->m_CBodyComponent");

	// skin changer stuff 2
	OFFSET_PTR(void, m_pWeaponSecondVTable, offsets::m_pWeaponSecondVTable); // retarded compiler problems with using OFFSET() reference, it dereferences it thinking the vtable itself is the object.....
	OFFSET(void*, m_ppMaterial, offsets::m_ppMaterial);
	OFFSET(bool, m_bAllowSkinRegeneration, offsets::m_bAllowSkinRegeneration);
	OFFSET(void*, m_pRegenCount, offsets::m_pRegenCount);
};