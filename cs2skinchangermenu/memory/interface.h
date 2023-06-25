#pragma once

#include "sdk/interfaces/CSource2Client.h" // TODO: rename to source2client

// Used by Source internally to register classes.
class InterfaceReg
{
public:
	typedef void* (*InstantiateInterfaceFn)();
	InstantiateInterfaceFn	m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext; // For the global list.
};

namespace Interface {
	extern CSource2Client* client;
};

// Find a specific client class inside a .dll, nullptr if not found
void* CreateInterface(const char* moduleName, const char* interfaceName);
// Get all necessary interfaces for cheat, store them
bool LoadInterfaces();