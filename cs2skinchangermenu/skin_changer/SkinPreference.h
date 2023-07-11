#pragma once

#include <array>

class Sticker {
	int stickerID;
	float wear;
};

class SkinPreference {
public:
	int paintKitID;
	int seed;
	float wearValue;

	bool useStattrak;
	int stattrakKills;

	char nametag[64];

	// TODO: add stickers in the future
	std::array<Sticker, 4> stickers;
};