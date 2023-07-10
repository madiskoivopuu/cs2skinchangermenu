#pragma once

#include <cstdint>

#define MAX_PLAYERS 64 // keep ent idx smaller than this

class CGameEntitySystem {
public:
	void* pVTable;

	template<typename T>
	T* GetFromEntityList(int index) {
		if (index == -1 || index < 0) // invalid handle or random nonsense
			return nullptr;

		uint64_t bucketNr = (index & 0x7FFF) >> 9;
		uint8_t* pointerToList = reinterpret_cast<uint8_t*>(this) + 8 * bucketNr + 16; // points to a list where the entity is stored at, 0x7FFF just keeps the index below 32k
		if (!pointerToList)
			return nullptr;

		uint64_t entityIdxInBucket = (index & 0x1FF);
		uint8_t* pEntityPtr = *reinterpret_cast<uint8_t**>(pointerToList) + 120 * entityIdxInBucket; // pointer to the address that points to ccsplayercontroller
		if (!pEntityPtr)
			return nullptr;

		// valve seems to be doing a check at this offset to see if controller idx == index
		// disabled it since these checks seem to differ per entity type
		//if ((*reinterpret_cast<int*>(pEntityPtr + 16) & 0x7FFF) != entityIdxInBucket)
		//	return nullptr;

		T* entity = *reinterpret_cast<T**>(pEntityPtr);
		return entity;
	}
};