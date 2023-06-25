#include "pch.h"
#include "OpcodePatcher.h"

#include <Windows.h>

OpcodePatcher::OpcodePatcher(void* location, const char* newOpcodes, int size) {
	this->location = location;
	this->overwriteOpcodes = newOpcodes;
	this->oldOpcodes = std::make_unique<char[]>(size);
	this->size = size;

	this->toggled = false;

	memcpy_s(this->oldOpcodes.get(), this->size, this->location, this->size);
}

void OpcodePatcher::Patch() {
	DWORD oldProtect;
	VirtualProtect(this->location, this->size, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy_s(this->location, this->size, this->overwriteOpcodes, this->size);

	VirtualProtect(this->location, this->size, oldProtect, &oldProtect);

	this->toggled = true;
}
void OpcodePatcher::Reset() {
	DWORD oldProtect;
	VirtualProtect(this->location, this->size, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy_s(this->location, this->size, this->oldOpcodes.get(), this->size);

	VirtualProtect(this->location, this->size, oldProtect, &oldProtect);

	this->toggled = false;
}

void OpcodePatcher::Toggle() {
	if (!this->toggled)
		this->Patch();
	else
		this->Reset();
}