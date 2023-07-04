#pragma once
#include "netvars/netvars.h"

class EntitySpottedState_t {
public:
	char bytes[0x18];

	bool& m_bSpotted() {

		uint32_t offset = Netvars::list[fnv::HashConst("client.dll!EntitySpottedState_t->m_bSpotted")];
		bool& bSpotted = reinterpret_cast<bool&>(this->bytes[offset]);
		return bSpotted;
	}

	uint8_t& m_bSpottedByMask() {

		uint32_t offset = Netvars::list[fnv::HashConst("client.dll!EntitySpottedState_t->m_bSpottedByMask")];
		uint8_t& bSpotted = reinterpret_cast<uint8_t&>(this->bytes[offset]);
		return bSpotted;
	}
};