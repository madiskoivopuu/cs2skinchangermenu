#pragma once

#include <array>

class Sticker {
public:
	int id;
	float wear;
};

class SkinPreference {
public:
	int weaponID;

	int paintKitID;
	int seed;
	float wearValue;

	bool useStattrak;
	int stattrakKills;

	char nametag[64];

	// TODO: add stickers in the future
	std::array<Sticker, 4> stickers;
};