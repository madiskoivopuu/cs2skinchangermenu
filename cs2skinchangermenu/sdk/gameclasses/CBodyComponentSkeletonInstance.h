#pragma once
#include "netvars/netvars.h"
#include "netvars/offsets.h"

#include "CSkeletonInstance.h"

class CBodyComponentSkeletonInstance {
public:
	NETVAR(CSkeletonInstance, m_skeletonInstance, "client.dll!CBodyComponentSkeletonInstance->m_skeletonInstance");

	OFFSET(uint64_t, m_iMeshGroupMaskMain, offsets::m_iMeshGroupMaskMain);
};