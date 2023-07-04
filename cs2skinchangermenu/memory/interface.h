#pragma once

#include "sdk/interfaces/CSource2Client.h"
#include "sdk/gameclasses/CGameEntitySystem.h"
#include "sdk/schema.h"

namespace Interface {
	extern CSource2Client* client;
	extern CGameEntitySystem* entities;
	extern CSchemaSystem* schema;
};

// Used by Source internally to register classes.
class InterfaceReg
{
public:
	typedef void* (*InstantiateInterfaceFn)();
	InstantiateInterfaceFn	m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext; // For the global list.
};

// Find a specific client class inside a .dll, nullptr if not found
void* CreateInterface(const char* moduleName, const char* interfaceName);
// Get all necessary interfaces for cheat, store them
bool LoadInterfaces();