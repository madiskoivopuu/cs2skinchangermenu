#pragma once

#include <array>

class Sticker {
public:
	int id;
	float wear;
};

class SkinPreference {
public:
	int weaponID = 1;

	int paintKitID = -1;
	int seed = -1;
	float wearValue = 0.0f;

	bool useStattrak = false;
	int stattrakKills = 0;

	char nametag[64];

	// TODO: add stickers in the future
	std::array<Sticker, 4> stickers;
};