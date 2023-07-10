#pragma once

#include "sdk/gameclasses/CHandle.h"
#include "netvars/netvars.h"
#include "sdk/gameclasses/C_CSPlayerPawn.h"

class CCSPlayerController {
public:
	void* pVTable;

	NETVAR(CHandle<C_CSPlayerPawn>, m_hPlayerPawn, "client.dll!CCSPlayerController->m_hPlayerPawn");
	NETVAR(bool, m_bPawnIsAlive, "client.dll!CCSPlayerController->m_bPawnIsAlive");
};