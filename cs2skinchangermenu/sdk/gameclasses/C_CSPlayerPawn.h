#pragma once
#include "netvars/netvars.h"
#include "EntitySpottedState_t.h"
#include "CPlayer_WeaponServices.h"
#include "CCSPlayer_ViewModelServices.h"

class C_CSPlayerPawn {
public:
	// oddly enough one netvar is from a parent class
	NETVAR(EntitySpottedState_t, pEntitySpottedState, "client.dll!C_CSPlayerPawnBase->m_entitySpottedState");
	NETVAR(CPlayer_WeaponServices*, m_pWeaponServices, "client.dll!C_BasePlayerPawn->m_pWeaponServices");

	NETVAR(CCSPlayer_ViewModelServices*, m_pViewModelServices, "client.dll!C_CSPlayerPawnBase->m_pViewModelServices");

	/*void SetSpotted() {
		EntitySpottedState_t& spottedState = pEntitySpottedState();
		//if (spottedState == nullptr)
		//	return;

		spottedState.m_bSpotted() = true;
		spottedState.m_bSpottedByMask() = 0b101; // bit 3 for servers (deathmatch?), bit 1 for local
	}*/
};