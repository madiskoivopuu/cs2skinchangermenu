#pragma once

#include <cstdint>
#include <vector>

// dereferences a multi offset pointer
template<typename T>
T* DereferenceMultilevelPointer(uintptr_t startAddy, std::vector<intptr_t> offsets) {
	uintptr_t currentAddy = (intptr_t)startAddy;
	for (size_t i = 0; i < offsets.size(); i++) {
		intptr_t offset = offsets.at(i);

		if (offset < 0)
			currentAddy = *reinterpret_cast<uintptr_t*>(static_cast<intptr_t>(currentAddy) + offset);
		else
			currentAddy = *reinterpret_cast<uintptr_t*>(currentAddy + static_cast<uintptr_t>(offset));
	}

	return reinterpret_cast<T*>(currentAddy);
}

// set's the destination bytes to given bytes
void PatchMemory(BYTE* dst, BYTE* src, unsigned int size);

// sets certain size of memory to 0x90 -> NOP instruction
void NopMemory(BYTE* dst, unsigned int size);