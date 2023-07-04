#pragma once

#include <cstdint>

#define OFFSET(type, func_name, offset) type& func_name() \
{ \
	return *reinterpret_cast<type*>(reinterpret_cast<uint8_t*>(this) + offset); \
}

namespace offsets {
	extern uint32_t m_bAllowSkinRegeneration; // Weapons
	extern uint32_t m_pRegenCount;
}

bool InitializeOffsets();