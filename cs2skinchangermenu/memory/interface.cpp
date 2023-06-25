#include "pch.h"

#include "interface.h"
#include "sdk/interfaces/CSource2Client.h"

#include <Windows.h>
#include <cstdint>
#include <iostream>

// interfaces extern definition
namespace Interface {
	CSource2Client* client = nullptr;
}

void* CreateInterface(const char* moduleName, const char* interfaceName) {
	HMODULE module = GetModuleHandle(moduleName);
	if (module == NULL)
		return nullptr;

	BYTE* pInterfaceFunc = reinterpret_cast<BYTE*>(GetProcAddress(module, "CreateInterface")); // byte* so when we add to it, the address increases by one byte only and not 4/8 bytes
	uint32_t regsOffset = *reinterpret_cast<uint32_t*>(pInterfaceFunc+3); // read the offset to interface regs 3 bytes from the start of func, to avoid MOV r9 bytes

	InterfaceReg* s_pInterfaceRegs = *reinterpret_cast<InterfaceReg**>(pInterfaceFunc + regsOffset + 7);

	// find the desired interface from linked list
	for (InterfaceReg* current = s_pInterfaceRegs; current; current = current->m_pNext) {
		// checks if name contains interfaceName
		if (strstr(current->m_pName, interfaceName)) {
			return current->m_CreateFn();
		}
	}

	return nullptr;
}

bool LoadInterfaces() {
	Interface::client = reinterpret_cast<CSource2Client*>(CreateInterface("client.dll", "Source2Client0"));

	return Interface::client;
}