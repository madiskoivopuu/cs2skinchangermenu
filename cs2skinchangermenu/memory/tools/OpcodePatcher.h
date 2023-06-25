#pragma once
#include <memory>

class OpcodePatcher
{
private:
	void* location;
	const char* overwriteOpcodes;
	std::unique_ptr<char[]> oldOpcodes;
	int size; // size of bytes to nop

	bool toggled;
public:
	OpcodePatcher(void* location, const char* newOpcodes, int size);
	void Patch();
	void Reset();
	void Toggle();
};

