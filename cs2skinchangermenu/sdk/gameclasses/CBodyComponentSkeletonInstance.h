#pragma once
#include "netvars/netvars.h"

#include "CSkeletonInstance.h"

class CBodyComponentSkeletonInstance {
public:
	NETVAR(CSkeletonInstance, m_skeletonInstance, "client.dll!CBodyComponentSkeletonInstance->m_skeletonInstance");
};