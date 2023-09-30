#pragma once

#include <cstdint>
#include "netvars/netvars.h"
#include "sdk/gameclasses/CAttributeList.h"
#include "sdk/gameclasses/CWeaponCSBaseVData.h"

class C_EconItemView {
public:
	NETVAR(uint32_t, m_iAccountID, "client.dll!C_EconItemView->m_iAccountID");
	NETVAR(uint32_t, m_iInventoryPosition, "client.dll!C_EconItemView->m_iInventoryPosition")
	NETVAR(uint16_t, m_iItemDefinitionIndex, "client.dll!C_EconItemView->m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityQuality, "client.dll!C_EconItemView->m_iEntityQuality");
	NETVAR(uint32_t, m_iEntityLevel, "client.dll!C_EconItemView->m_iEntityLevel");
	NETVAR(uint64_t, m_iItemID, "client.dll!C_EconItemView->m_iItemID");
	NETVAR(uint32_t, m_iItemIDHigh, "client.dll!C_EconItemView->m_iItemIDHigh");
	NETVAR(uint32_t, m_iItemIDLow, "client.dll!C_EconItemView->m_iItemIDLow");
	NETVAR(bool, m_bInitialized, "client.dll!C_EconItemView->m_bInitialized");
	NETVAR(char, m_szCustomName, "client.dll!C_EconItemView->m_szCustomName");

	NETVAR(CAttributeList, m_AttributeList, "client.dll!C_EconItemView->m_AttributeList");
	NETVAR(CAttributeList, m_NetworkedDynamicAttributes, "client.dll!C_EconItemView->m_NetworkedDynamicAttributes");

public:
	CCSWeaponBaseVData* GetCSWeaponDataFromItem();
	void SetAttributeValueByName(const char* name, float value);
	void SetAttributeValueIntByName(const char* name, int value);
};