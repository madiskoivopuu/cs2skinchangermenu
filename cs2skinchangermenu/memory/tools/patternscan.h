#pragma once
#include <cstdint>

char* ScanPattern(char* startAddr, uint64_t bytesToScan, const char* pattern, const char* mask);
char* ScanPatternInModule(const char* moduleName, const char* pattern, const char* mask);
char* ScanPatternEntireMemory(const char* pattern, const char* mask);

int ScanPatternInModuleCount(const char* moduleName, const char* pattern, const char* mask);