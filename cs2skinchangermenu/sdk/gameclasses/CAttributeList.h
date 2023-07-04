#pragma once

#include "netvars/netvars.h"
#include "sdk/CUtl/CUtlVector.h"
#include "sdk/gameclasses/CEconItemAttribute.h"

class CAttributeList {
public:
	NETVAR(CUtlVector<CEconItemAttribute>, m_Attributes, "client.dll!CAttributeList->m_Attributes")
};