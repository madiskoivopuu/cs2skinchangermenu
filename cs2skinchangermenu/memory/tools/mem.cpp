#include "pch.h"
#include "mem.h"

#include <Windows.h>

void PatchMemory(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

void NopMemory(BYTE* dst, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}