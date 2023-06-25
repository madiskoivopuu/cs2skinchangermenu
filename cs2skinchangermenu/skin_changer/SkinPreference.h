#pragma once

#include <array>

class Sticker {
	int stickerID;
	float wear;
};

class SkinPreference {
	int paintKitID;
	int seed;
	float wearValue;
	int stattrakKills;

	// TODO: add stickers in the future
	std::array<Sticker, 4> stickers;
	char nametag[20];
};