#pragma once

#include "netvars/netvars.h"
#include "CHandle.h"
#include "C_ViewmodelWeapon.h"
#include "CBodyComponentSkeletonInstance.h"
#include "CBodyComponent.h"
#include "sdk/Vector.h"

class C_CSGOViewModel {
public:
	NETVAR(CBodyComponent*, m_CBodyComponent, "client.dll!C_BaseEntity->m_CBodyComponent");
	NETVAR(CBodyComponentSkeletonInstance*, m_pGameSceneNode, "client.dll!C_BaseEntity->m_pGameSceneNode");

	NETVAR(Vector3, m_CachedViewTarget, "client.dll!C_BaseFlex->m_CachedViewTarget");
	NETVAR(CHandle<void*>, m_iCameraAttachment, "client.dll!C_BaseViewModel->m_iCameraAttachment");
};