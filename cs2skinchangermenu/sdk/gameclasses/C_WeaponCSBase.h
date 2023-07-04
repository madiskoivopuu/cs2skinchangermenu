#pragma once
#include "netvars/netvars.h"
#include "netvars/offsets.h"

#include "C_AttributeContainer.h"
#include "CHandle.h"
#include "C_CSPlayerPawn.h"


class C_CSPlayerPawn; // forwarddecl

class C_WeaponCSBase {
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
	OFFSET(bool, m_bAllowSkinRegeneration, offsets::m_bAllowSkinRegeneration);
	OFFSET(uintptr_t, m_pRegenCount, offsets::m_pRegenCount);
};