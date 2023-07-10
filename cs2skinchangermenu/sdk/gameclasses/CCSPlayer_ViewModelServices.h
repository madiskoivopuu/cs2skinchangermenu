#pragma once

//#include "CHandle.h"
#include "C_CSGOViewModel.h"

class CCSPlayer_ViewModelServices {
public:
	NETVAR(CHandle<C_CSGOViewModel>, m_hViewModel, "client.dll!CCSPlayer_ViewModelServices->m_hViewModel")
};