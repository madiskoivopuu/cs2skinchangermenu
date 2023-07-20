#pragma once

#include "netvars/netvars.h"

#include "CModelState.h"

class CSkeletonInstance {
public:
	NETVAR(CModelState, m_modelState, "client.dll!CSkeletonInstance->m_modelState");
};