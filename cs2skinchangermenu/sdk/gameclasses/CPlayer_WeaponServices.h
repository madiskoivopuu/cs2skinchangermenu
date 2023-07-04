#pragma once
#include "netvars/netvars.h"
#include "sdk/CUtl/CUtlVector.h"
#include "CHandle.h"
#include "C_WeaponCSBase.h"

class C_WeaponCSBase;

class CPlayer_WeaponServices {
public:
	// actually C_BasePlayerWeapon, but this one works for now
	NETVAR(CUtlVector<CHandle<C_WeaponCSBase>>, m_hMyWeapons, "client.dll!CPlayer_WeaponServices->m_hMyWeapons"); // right after this offset +0x8 there is the pointer to weapons list
	NETVAR(CHandle<C_WeaponCSBase>, m_hActiveWeapon, "client.dll!CPlayer_WeaponServices->m_hActiveWeapon");
};