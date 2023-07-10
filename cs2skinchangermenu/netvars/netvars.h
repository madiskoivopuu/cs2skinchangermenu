#pragma once
#include <unordered_map>
#include "fnvhash.h"


namespace Netvars {
	extern std::unordered_map<uint32_t, uint32_t> list;
}

#define NETVAR(type, func_name, netvar) type& func_name() \
{ \
	uint32_t key = fnv::HashConst(netvar); \
	uint32_t offset = Netvars::list[key]; \
	return *reinterpret_cast<type*>(reinterpret_cast<uint8_t*>(this) + offset); \
}

#define NETVAR_WITH_OFFSET(type, func_name, netvar, additional_offset) type& func_name() \
{ \
	uint32_t key = fnv::HashConst(netvar); \
	uint32_t offset = Netvars::list[key]; \
	return *reinterpret_cast<type*>(reinterpret_cast<uint8_t*>(this) + offset + additional_offset); \
}

#define NETVAR_PTR(type, func_name, netvar) type* func_name() \
{ \
	uint32_t key = fnv::HashConst(netvar); \
	uint32_t offset = Netvars::list[key]; \
	return reinterpret_cast<type*>(reinterpret_cast<uint8_t*>(this) + offset); \
}


// Load all netvars to Netvars::list
bool InitializeNetvars();