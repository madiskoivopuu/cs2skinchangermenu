#pragma once
#include <cstdint>

class CCStrike15ItemDefinition {
public:
	const char* GetMainCategory() {
		return *reinterpret_cast<const char**>(reinterpret_cast<uint8_t*>(this) + 0x1F8);
	}

	// returns the full weapon name with its subtype
	const char* GetSubcategory() {
		return *reinterpret_cast<const char**>(reinterpret_cast<uint8_t*>(this) + 0x210);
	}

	void* pVTable;
	void* pKVItem;
	uint16_t m_iItemDefinitionIndex;
};