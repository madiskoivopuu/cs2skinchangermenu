#include "pch.h"

#include "patternscan.h"

#include <cstdint>
#include <Psapi.h>
#include <Windows.h>

char* ScanPattern(char* startAddr, uint64_t bytesToScan, const char* pattern, const char* mask) {
	uint64_t patternLen = strlen(mask);

	for (uint64_t i = 0; i < bytesToScan - patternLen; i++) {
		bool found = true;

		for (uint64_t j = 0; j < patternLen; j++) {
			if (mask[j] != '?' && pattern[j] != *(startAddr + i + j)) {
				found = false;
				break;
			}
		}

		if (found) {
			return (startAddr + i);
		}
	}

	return nullptr;
}

char* ScanPatternInModule(const char* moduleName, const char* pattern, const char* mask) {
	HMODULE module = GetModuleHandle(moduleName);
	if (module == 0) return nullptr;

	MODULEINFO moduleInfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(MODULEINFO));
	if (moduleInfo.lpBaseOfDll == 0) return nullptr;

	// start scanning for the pattern in range
	char* startAddress = static_cast<char*>(moduleInfo.lpBaseOfDll);
	return ScanPattern(startAddress, moduleInfo.SizeOfImage, pattern, mask);
}

// Count the amount of times we found a matching pattern within a module
int ScanPatternInModuleCount(const char* moduleName, const char* pattern, const char* mask) {
	HMODULE module = GetModuleHandle(moduleName);
	if (module == 0) return 0;

	MODULEINFO moduleInfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(MODULEINFO));
	if (moduleInfo.lpBaseOfDll == 0) return 0;

	// start scanning for the pattern in range
	char* startAddress = static_cast<char*>(moduleInfo.lpBaseOfDll);
	char* endAddress = startAddress + moduleInfo.SizeOfImage;
	int count = 0;

	do { // scan for pattern, update count and start addy
		startAddress = static_cast<char*>(ScanPattern(startAddress, static_cast<ptrdiff_t>(endAddress - startAddress), pattern, mask));
		if (startAddress) {
			//std::cout << static_cast<void*>(startAddress) << std::endl;
			startAddress += strlen(pattern);
			count++;
		}

	} while (startAddress != nullptr);

	return count;
}

char* ScanPatternEntireMemory(const char* pattern, const char* mask) {
	char* patternScanResult = 0;

	char* currentAddr = 0;
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	while (VirtualQueryEx(GetCurrentProcess(), currentAddr, &mbi, sizeof(mbi))) {
		// look for only committed & executable memory
		if(mbi.State == MEM_COMMIT && (mbi.Protect & PAGE_EXECUTE || mbi.Protect & PAGE_EXECUTE_READ || mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_EXECUTE_WRITECOPY))

		if ((patternScanResult = ScanPattern(currentAddr, mbi.RegionSize, pattern, mask)))
			return patternScanResult;

		currentAddr += mbi.RegionSize;
	}

	return nullptr;
}
