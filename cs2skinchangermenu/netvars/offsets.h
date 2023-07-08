#pragma once

#include <cstdint>

#define OFFSET(type, func_name, offset) type& func_name() \
{ \
	return *reinterpret_cast<type*>(reinterpret_cast<uint8_t*>(this) + offset); \
}

#define OFFSET_PTR(type, func_name, offset) type* func_name() \
{ \
	return reinterpret_cast<type*>(reinterpret_cast<uint8_t*>(this) + offset); \
}

namespace offsets {
	extern uint32_t m_ppMaterial; // 0x9d0
	extern uint32_t m_bAllowSkinRegeneration; // 0x9e0
	extern uint32_t m_pRegenCount; // 0x9e8

	extern uint32_t m_pWeaponSecondVTable; // 0x540
}

bool InitializeOffsets();