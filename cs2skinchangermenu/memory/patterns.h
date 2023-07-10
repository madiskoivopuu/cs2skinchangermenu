#pragma once
// Patterns for necessary CSGO functions

// CSource2Client::GetCEconItemSystem()  (new build vtable 110)
constexpr auto PATTERN_GETECONITEMSYS_PTR = "\x48\x83\xec\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x85\xc0\x75\x00\x48\x89\x5c\x24\x00\x8d\x48\x00\x48\x89\x7c\x24\x00\xe8\x00\x00\x00\x00\x33\xff\x48\x8b\xd8\x48\x85\xc0\x74\x00\x48\x8d\x05\x00\x00\x00\x00\x48\x89\x7b\x00\xb9\x00\x00\x00\x00\x48\x89\x03\xe8\x00\x00\x00\x00\x48\x85\xc0\x74\x00\x48\x8b\xc8\xe8\x00\x00\x00\x00\x48\x8b\xf8\x48\x8d\x05\x00\x00\x00\x00\x48\x89\x7b\x00\x48\x89\x03\xeb\x00\x48\x8b\xdf\x48\x8b\x7c\x24";
constexpr auto MASK_GETECONITEMSYS_PTR = "xxx?xxx????xxxx?xxxx?xx?xxxx?x????xxxxxxxxx?xxx????xxx?x????xxxx????xxxx?xxxx????xxxxxx????xxx?xxxx?xxxxxxx";

// csource2client::getcgameentitysystem pattern scan
// 48 8B 05 F9 6F 10 01                          mov     rax, cs:qword_1817CD560
// 48 83 C0 10                                   add     rax, 10h
// C3 ret
constexpr auto PATTERN_ENTSYS_PTRPTR1 = "\x48\x8B\x05\x00\x00\x00\x00\x48\x83\xC0\x10\xC3";
constexpr auto MASK_ENTSYS_PTRPTR1 = "xxx????xxxxx";
constexpr auto OFFSETSTART_ENTSYS_PTRPTR1 = 3;
constexpr auto OFFSETEND_ENTSYS_PTRPTR1 = 7;

// !! Weapons do have their vdata pointer somewhere close to offset 0x360
// GetCSWeaponDataFromItem
// found by string:   FX_FireBullets: GetCSWeaponDataFromItem failed for weapon %s\n
// go up to the matching if check to find it
constexpr auto PATTERN_GETCSWEAPONDATA_PTR = "\x48\x81\xec\x00\x00\x00\x00\x48\x85\xc9\x75\x00\x33\x00\x48\x81\xc4\x00\x00\x00\x00\xc3\x48\x89\x9c\x24";
constexpr auto MASK_GETCSWEAPONDATA_PTR = "xxx????xxxx?x?xxx????xxxxx";

// local player ptr
constexpr auto PATTERN_LOCALPLAYER_PTRPTR1 = "\x48\x63\xC3\x48\x8D\x0D\x00\x00\x00\x00\x48\x89\x34\xC1";
constexpr auto MASK_LOCALPLAYER_PTRPTR1 = "xxxxxx????xxxx";
constexpr auto OFFSETSTART_LOCALPLAYER_PTRPTR1 = 6;
constexpr auto OFFSETEND_LOCALPLAYER_PTRPTR1 = 3 + 7;

// C_CSPlayerPawn::CreateMove(CUserCmd*, CUserCmd*) (potentially)
// found by: looking inside CClientInput virtual function, where there's a string "cl: %d -------------------"
// player pawn call is above a lot of if checks with |= assignments
constexpr auto PATTERN_PLAYERPAWNCREATEMOVE_PTR = "\x48\x89\x5c\x24\x00\x57\x48\x83\xec\x00\x8b\x42\x00\x48\x8b\xda\x48\xc1\xe8";
constexpr auto MASK_PLAYERPAWNCREATEMOVE_PTR = "xxxx?xxxx?xx?xxxxxx";

// UpdateViewmodelStattrakAttachments(C_CSGOViewModel*, C_Weapon*)
// found by string in client.dll: CEntitySpawner<class C_ViewmodelAttachmentModel>::Spawn
// found by: cheat engine looking what accesses the address where m_hStattrakAttachment is
constexpr auto PATTERN_UPDATEVIEWMODELSTATTRAKATTACHMENTS_PTR = "\x48\x89\x5c\x24\x00\x55\x56\x57\x41\x55\x41\x56\x48\x8d\x6c\x24 ";
constexpr auto MASK_UPDATEVIEWMODELSTATTRAKATTACHMENTS_PTR = "xxxx?xxxxxxxxxxx";

// SpawnAndSetStattrakAttachment(m_hStattrakAttachment*)
// found by string: CEntitySpawner<class C_WorldModelStattrak>::Spawn
// found by string: weapons/models/shared/stattrak/stattrak_module.vmdl
// preferably two of these strings in the same func
constexpr auto PATTERN_SPAWNSETSTATTRAK_PTR = "\x4c\x8b\xdc\x55\x41\x55\x49\x8d\x6b";
constexpr auto MASK_SPAWNSETSTATTRAK_PTR = "xxxxxxxxx";

// ::SetAttributeValueByName(C_EconItemView*, char* name, float val)
// found by string: set item texture prefab
// look for a lot of the same functions together, preferably 4 such as texture prefab, texture seed, kill eater etc
constexpr auto PATTERN_SETATTRIBVALUEBYNAME_PTR = "\x40\x53\x48\x83\xec\x00\x48\x8b\xd9\x48\x81\xc1\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8b\x8b";
constexpr auto MASK_SETATTRIBVALUEBYNAME_PTR = "xxxxx?xxxxxx????x????xxx";

// *RegenerateAllWeaponSkins() -> offset to allow weapon regeneration [ONLY USED FOR MATERIAL PTR PURPOSES]
// found by string: regenerate_weapon_skins
constexpr auto PATTERN_REGENWEP_ALLOWREGENOFFSET = "\x48\x85\xC0\x74\x00\x39\xB0\x00\x00\x00\x00\x7f\x00\x39\xB0\x00\x00\x00\x00\x7e\x00";
constexpr auto MASK_REGENWEP_ALLOWREGENOFFSET = "xxxx?xx????x?xx????x?";
constexpr auto OFFSETSTART_REGENWEP_ALLOWREGENOFFSET = 15; // uint32 offset

// RegenerateWeaponSkin(weapon*)
// found by string: regenerate_weapon_skins
// second call inside an if check with 3 things to check
// since the call address is relative to rip, we will dereference it for the correct address
constexpr auto PATTERN_REGENWEAPONSKIN_PTR = "\x48\x8d\x88\x00\x00\x00\x00\xb2\x01\xe8\x00\x00\x00\x00\x48\x8b\xcb\xe8\x00\x00\x00\x00";
constexpr auto MASK_REGENWEAPONSKIN_PTR = "xxx????xxx????xxxx????";
constexpr auto OFFSETSTART_REGENWEAPONSKIN = 18; // 
constexpr auto OFFSETEND_REGENWEAPONSKIN = 22;

// AllowSkinRegen(weapon 2nd vtable*, 1)
// found by string: regenerate_weapon_skins
// first call inside an if check with 3 things to check
// since the call address is relative to rip, we will dereference it for the correct address
constexpr auto PATTERN_ALLOWSKINREGEN_PTR = PATTERN_REGENWEAPONSKIN_PTR;
constexpr auto MASK_ALLOWSKINREGEN_PTR = MASK_REGENWEAPONSKIN_PTR;
constexpr auto OFFSETSTART_ALLOWSKINREGEN = 10;
constexpr auto OFFSETEND_ALLOWSKINREGEN = 14;

// *RegenerateAllWeaponSkins() -> C_WeaponName 2nd vtable offset
// found by string: regenerate_weapon_skins
// first call's first argument inside an if check with 3 things to check
// we need this offset so we can pass the proper weapon to AllowSkinRegen
constexpr auto PATTERN_WEP2NDVTABLE_OFFSET = PATTERN_REGENWEAPONSKIN_PTR;
constexpr auto MASK_WEP2NDVTABLE_OFFSET = MASK_REGENWEAPONSKIN_PTR;
constexpr auto OFFSETSTART_WEP2NDVTABLE = 3;