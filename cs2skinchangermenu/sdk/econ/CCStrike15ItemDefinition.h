#pragma once
#include <cstdint>

class CCStrike15ItemDefinition {
public:
	const char* ItemName() {
		return *reinterpret_cast<const char**>(reinterpret_cast<uint8_t*>(this) + 0x1F8);
	}

	void* pVTable;
	void* pKVItem;
	uint16_t m_iItemDefinitionIndex;
};