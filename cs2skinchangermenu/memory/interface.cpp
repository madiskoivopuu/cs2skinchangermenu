#include "pch.h"

#include "interface.h"


#include "memory/tools/patternscan.h"
#include "patterns.h"

#include <Windows.h>
#include <cstdint>
#include <iostream>

// interfaces extern definition
namespace Interface {
	CSource2Client* client = nullptr;
	CGameEntitySystem* entities = nullptr;
	CSchemaSystem* schema = nullptr;
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

CGameEntitySystem* PatternScanForEntitySystem() {
	uint8_t* instrPtr = reinterpret_cast<uint8_t*>(ScanPatternInModule("client.dll", PATTERN_ENTSYS_PTRPTR1, MASK_ENTSYS_PTRPTR1));
	if (!instrPtr)
		return nullptr;

	// lea ..., g_pCCSPlayerController || we get the g_p... part
	uint32_t offsetFromInstruction = *reinterpret_cast<uint32_t*>(instrPtr + OFFSETSTART_ENTSYS_PTRPTR1); // read 3 bytes from the start of scan to get the offset

	return *reinterpret_cast<CGameEntitySystem**>(instrPtr + OFFSETEND_ENTSYS_PTRPTR1 + offsetFromInstruction); //+7 since the offset is from the end of instruction
}

bool LoadInterfaces() {
	Interface::client = reinterpret_cast<CSource2Client*>(CreateInterface("client.dll", "Source2Client0"));
	Interface::entities = PatternScanForEntitySystem();
	Interface::schema = reinterpret_cast<CSchemaSystem*>(CreateInterface("schemasystem.dll", "SchemaSystem_"));

	return Interface::client && Interface::schema;
}