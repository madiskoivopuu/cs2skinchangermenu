#pragma once

#include "netvars/netvars.h"
#include "CHandle.h"
#include "C_ViewmodelWeapon.h"
#include "CBodyComponentSkeletonInstance.h"
#include "CBodyComponent.h"

class C_CSGOViewModel {
public:
	NETVAR(CBodyComponent*, m_CBodyComponent, "client.dll!C_BaseEntity->m_CBodyComponent");
	NETVAR(CBodyComponentSkeletonInstance*, m_pGameSceneNode, "client.dll!C_BaseEntity->m_pGameSceneNode");

	NETVAR(uint32_t, m_nNextSceneEventId, "client.dll!C_BaseFlex->m_nNextSceneEventId");
	NETVAR(CHandle<void*>, m_iCameraAttachment, "client.dll!C_BaseViewModel->m_iCameraAttachment");
};