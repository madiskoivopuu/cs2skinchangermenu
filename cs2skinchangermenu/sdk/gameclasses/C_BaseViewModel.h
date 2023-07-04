#pragma once

#include "netvars/netvars.h"
#include "CHandle.h"
#include "C_ViewmodelWeapon.h"

class C_BaseViewModel {
public:
	NETVAR(CHandle<C_ViewmodelWeapon>, m_hWeaponModel, "client.dll!C_BaseViewModel->m_hWeaponModel")
};