#pragma once

#include "netvars/netvars.h"

class CEconItemAttribute {
public:
	char size[0x48];

	NETVAR(uint16_t, m_iAttributeDefinitionIndex, "client.dll!CEconItemAttribute->m_iAttributeDefinitionIndex");
	NETVAR(float, m_flValue, "client.dll!CEconItemAttribute->m_flValue");
	NETVAR(float, m_flInitialValue, "client.dll!CEconItemAttribute->m_flInitialValue");

	NETVAR(int, m_ilValue, "client.dll!CEconItemAttribute->m_flValue");
	NETVAR(int, m_iInitialValue, "client.dll!CEconItemAttribute->m_flInitialValue");

	NETVAR(int32_t, m_nRefundableCurrency, "client.dll!CEconItemAttribute->m_nRefundableCurrency");
	NETVAR(bool, m_bSetBonus, "client.dll!CEconItemAttribute->m_bSetBonus");
};