#pragma once

#include "netvars/netvars.h"
#include "CHandle.h"
#include "C_ViewmodelWeapon.h"

class C_CSGOViewModel {
public:
	NETVAR(CHandle<void*>, m_iCameraAttachment, "client.dll!C_BaseViewModel->m_iCameraAttachment");
};