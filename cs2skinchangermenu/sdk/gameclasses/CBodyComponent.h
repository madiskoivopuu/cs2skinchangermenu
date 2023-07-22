#pragma once

#include "netvars/netvars.h"

#include "CBodyComponentSkeletonInstance.h"

class CBodyComponent {
public:
	NETVAR(CBodyComponentSkeletonInstance*, m_pSceneNode, "client.dll!CBodyComponent->m_pSceneNode");
};