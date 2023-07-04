#pragma once
// Patterns for necessary CSGO functions

// CSource2Client::GetCEconItemSystem()  (new build vtable 110)
constexpr auto PATTERN_GETECONITEMSYS_PTR = "\x48\x83\xec\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x85\xc0\x75\x00\x48\x89\x5c\x24\x00\x8d\x48\x00\x48\x89\x7c\x24\x00\xe8\x00\x00\x00\x00\x33\xff\x48\x8b\xd8\x48\x85\xc0\x74\x00\x48\x8d\x05\x00\x00\x00\x00\x48\x89\x7b\x00\xb9\x00\x00\x00\x00\x48\x89\x03\xe8\x00\x00\x00\x00\x48\x85\xc0\x74\x00\x48\x8b\xc8\xe8\x00\x00\x00\x00\x48\x8b\xf8\x48\x8d\x05\x00\x00\x00\x00\x48\x89\x7b\x00\x48\x89\x03\xeb\x00\x48\x8b\xdf\x48\x8b\x7c\x24";
constexpr auto MASK_GETECONITEMSYS_PTR = "xxx?xxx????xxxx?xxxx?xx?xxxx?x????xxxxxxxxx?xxx????xxx?x????xxxx????xxxx?xxxx????xxxxxx????xxx?xxxx?xxxxxxx";


// local player ptr
constexpr auto PATTERN_LOCALPLAYER_PTRPTR1 = "\x48\x63\xC3\x48\x8D\x0D\x00\x00\x00\x00\x48\x89\x34\xC1";
constexpr auto MASK_LOCALPLAYER_PTRPTR1 = "xxxxxx????xxxx";
constexpr auto OFFSETSTART_LOCALPLAYER_PTRPTR1 = 6;
constexpr auto OFFSETEND_LOCALPLAYER_PTRPTR1 = 3 + 7;

// csource2client::getcgameentitysystem pattern scan
// 48 8B 05 F9 6F 10 01                          mov     rax, cs:qword_1817CD560
// 48 83 C0 10                                   add     rax, 10h
// C3 ret
constexpr auto PATTERN_ENTSYS_PTRPTR1 = "\x48\x8B\x05\x00\x00\x00\x00\x48\x83\xC0\x10\xC3";
constexpr auto MASK_ENTSYS_PTRPTR1 = "xxx????xxxxx";
constexpr auto OFFSETSTART_ENTSYS_PTRPTR1 = 3;
constexpr auto OFFSETEND_ENTSYS_PTRPTR1 = 7;

// *RegenerateWeaponSkins() -> if check to allow regeneration
constexpr auto PATTERN_REGENWEP_ALLOWREGENOFFSET = "\x48\x85\xC0\x74\x00\x39\xB0\x00\x00\x00\x00\x7f\x00\x39\xB0\x00\x00\x00\x00\x7e\x00";
constexpr auto MASK_REGENWEP_ALLOWREGENOFFSET = "xxxx?xx????x?xx????x?";
constexpr auto OFFSETSTART_REGENWEP_ALLOWREGENOFFSET = 15; // uint32 offset