#pragma once

#include "netvars/netvars.h"
#include "CHandle.h"
#include "C_ViewmodelWeapon.h"

class C_CSGOViewModel {
public:
	NETVAR(uint32_t, m_nNextSceneEventId, "client.dll!C_BaseFlex->m_nNextSceneEventId");
	NETVAR(CHandle<void*>, m_iCameraAttachment, "client.dll!C_BaseViewModel->m_iCameraAttachment");
};