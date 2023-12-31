#pragma once

#include "skin_changer/SkinPreference.h"
#include "skin_changer/TextureCache.h"

enum class Window : int {
	SkinsList,
	SingleSkinSettings,
	StickerSearch,
	SkinSearch
};

inline struct WindowState {
	Window prevWindow = Window::SkinsList;
	Window currActiveWindow = Window::SkinsList;
	SkinPreference* currSkinPref = nullptr;

	char searchString[64] = "";
	int modifyingSickerNum = -1;

	void SetWindow(Window win) {
		this->prevWindow = this->currActiveWindow;
		this->currActiveWindow = win;
	}

	void PrevWindow() {
		this->currActiveWindow = this->prevWindow;
	}
} stateSkinsOverviewMenu;

inline TextureCache plusSymbol({
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x5A,
	0x08, 0x06, 0x00, 0x00, 0x00, 0xDA, 0x74, 0x5A, 0x7B, 0x00, 0x00, 0x00,
	0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00,
	0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
	0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
	0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7, 0x6F, 0xA8, 0x64, 0x00,
	0x00, 0x02, 0xC1, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5E, 0xED, 0x9C, 0xC1,
	0x91, 0xDA, 0x30, 0x14, 0x86, 0x91, 0x0D, 0xB9, 0x71, 0x74, 0x03, 0xB4,
	0x40, 0x03, 0x34, 0x41, 0x19, 0xD4, 0x90, 0xDD, 0x1A, 0x28, 0x83, 0x26,
	0x68, 0xC0, 0x95, 0x70, 0xE4, 0x90, 0x99, 0x04, 0xA4, 0xE8, 0x31, 0xCF,
	0x19, 0xB2, 0xA7, 0x90, 0xDD, 0x7D, 0x1F, 0x3B, 0xFA, 0xBF, 0x19, 0x8D,
	0x31, 0x17, 0x4B, 0x9F, 0x9E, 0x9F, 0x64, 0x8F, 0xE4, 0x99, 0x10, 0x42,
	0x7C, 0x0E, 0xC9, 0x8F, 0xE1, 0xEC, 0xF7, 0xFB, 0xEF, 0xA5, 0x94, 0x17,
	0x3F, 0x0D, 0x21, 0xA5, 0xF4, 0xB2, 0xDB, 0xED, 0x5E, 0xFD, 0x14, 0xA3,
	0xF3, 0xA3, 0x08, 0x44, 0xD2, 0x01, 0x24, 0x1D, 0x40, 0xD2, 0x01, 0x24,
	0x1D, 0x40, 0xD2, 0x01, 0x24, 0x1D, 0x40, 0xD2, 0x01, 0x24, 0x1D, 0x40,
	0xD2, 0x01, 0x24, 0x1D, 0x40, 0xD2, 0x01, 0x5A, 0x94, 0x6E, 0x2F, 0xF9,
	0xB0, 0x17, 0x7D, 0x46, 0x8B, 0xD2, 0x8B, 0x17, 0x8C, 0xA6, 0xA4, 0xE7,
	0x9C, 0x2D, 0xC2, 0xAD, 0xCD, 0x8A, 0xF4, 0x28, 0xBA, 0xAE, 0xC3, 0xA3,
	0xDC, 0x68, 0x31, 0xD2, 0x8D, 0xE6, 0xD2, 0x0B, 0x76, 0x6B, 0xB7, 0x1A,
	0xE9, 0x93, 0x70, 0x44, 0xFC, 0xF5, 0x7A, 0xB5, 0xF6, 0xE2, 0x77, 0x77,
	0x74, 0x05, 0xA6, 0x48, 0x43, 0xA2, 0xAD, 0xEF, 0xFB, 0x5C, 0x0F, 0x56,
	0x50, 0xF0, 0x5E, 0x6F, 0x91, 0x48, 0xE9, 0x76, 0xAD, 0xDE, 0x8F, 0x48,
	0x7A, 0xB9, 0x5C, 0x2E, 0x8B, 0xCD, 0x66, 0x33, 0xF7, 0x53, 0x8C, 0x48,
	0xE9, 0xF7, 0xB7, 0x76, 0xBA, 0x9B, 0x49, 0x34, 0x47, 0x74, 0x7A, 0xB9,
	0xE5, 0xF3, 0x1A, 0x6D, 0x76, 0xDD, 0xE8, 0x6B, 0xDF, 0x38, 0x9F, 0xCF,
	0x78, 0x67, 0x13, 0x0D, 0x2F, 0xC7, 0xE3, 0xF1, 0x52, 0xA7, 0x6F, 0x57,
	0x3F, 0x0F, 0xA3, 0x5E, 0x33, 0xAF, 0x56, 0xAB, 0xE6, 0x06, 0x52, 0x8B,
	0x32, 0x2C, 0xD2, 0xAC, 0xA3, 0x0F, 0x87, 0x43, 0x78, 0x67, 0xBF, 0x25,
	0x3C, 0xBD, 0x6C, 0xB7, 0xDB, 0x6E, 0xBD, 0x5E, 0x2F, 0x7C, 0xCE, 0x1C,
	0x4A, 0x8B, 0x03, 0xE9, 0x8D, 0x1A, 0x69, 0xD3, 0xED, 0x1D, 0x7E, 0xED,
	0x94, 0x52, 0x19, 0x86, 0x61, 0x51, 0x3B, 0xFE, 0x9B, 0xFF, 0x85, 0x10,
	0xDE, 0xF0, 0x4A, 0x19, 0xC7, 0xF1, 0x57, 0x7D, 0x50, 0xB9, 0xF8, 0x79,
	0x18, 0x76, 0xCD, 0xDA, 0xE9, 0x3F, 0x6A, 0xF9, 0xE9, 0x7F, 0x21, 0xFC,
	0xC9, 0xAF, 0xC4, 0x2A, 0xDA, 0x56, 0x78, 0xBB, 0x5A, 0x98, 0x88, 0xF4,
	0xE6, 0x91, 0x74, 0x00, 0x49, 0x07, 0x90, 0x74, 0x00, 0x49, 0x07, 0x90,
	0x74, 0x00, 0x49, 0x07, 0x90, 0x74, 0x00, 0x49, 0x07, 0x90, 0x74, 0x00,
	0x49, 0x07, 0x90, 0x74, 0x00, 0x49, 0x07, 0x90, 0x74, 0x00, 0x49, 0x07,
	0x90, 0x74, 0x00, 0x49, 0x07, 0x90, 0x74, 0x00, 0x49, 0x07, 0x90, 0x74,
	0x00, 0x49, 0x07, 0x90, 0x74, 0x00, 0x49, 0x07, 0x90, 0x74, 0x00, 0x49,
	0x07, 0x90, 0x74, 0x00, 0x6C, 0xD9, 0xB2, 0x3E, 0x86, 0x19, 0xC3, 0x6D,
	0x89, 0xB4, 0x15, 0x5B, 0xAE, 0x4C, 0x2C, 0x95, 0xFE, 0x00, 0xA6, 0xF5,
	0xF5, 0x56, 0xAC, 0xFE, 0xD3, 0x1E, 0xAA, 0x87, 0x88, 0x6C, 0x78, 0x1E,
	0xC7, 0xF1, 0x6A, 0xC5, 0x76, 0x62, 0xF8, 0xF6, 0xC2, 0xAF, 0x44, 0xB2,
	0xB5, 0xF5, 0x35, 0x60, 0x7A, 0x2B, 0x76, 0x5E, 0xCB, 0xFD, 0x3E, 0xAA,
	0x7F, 0x26, 0x3A, 0xDA, 0x72, 0xAD, 0xB8, 0x55, 0xDE, 0x2A, 0xFD, 0xD5,
	0x28, 0xB6, 0x8B, 0xA3, 0x06, 0x8C, 0x15, 0x6B, 0x87, 0xFF, 0xFD, 0x38,
	0xE1, 0xB7, 0xB8, 0x55, 0xFC, 0x74, 0x3A, 0xA5, 0x9A, 0x5E, 0xF0, 0x1D,
	0x11, 0xEF, 0xA1, 0xB6, 0x63, 0xDA, 0x80, 0xFC, 0xF0, 0xB8, 0x18, 0x2E,
	0xBD, 0xD2, 0x3D, 0xC3, 0x0E, 0xB7, 0x77, 0x60, 0xB2, 0xA7, 0xB4, 0xF2,
	0x5F, 0xBB, 0xBF, 0x09, 0xE9, 0x0F, 0x57, 0xF2, 0xA3, 0xC8, 0x39, 0xF7,
	0xCF, 0x90, 0xDA, 0xC2, 0xA5, 0xDB, 0x60, 0xB4, 0x5C, 0x2E, 0xA7, 0x68,
	0x09, 0xA5, 0x4A, 0xBF, 0x4F, 0x0B, 0x18, 0x44, 0x4E, 0xCF, 0xC3, 0x30,
	0x50, 0x0D, 0xB7, 0xD9, 0x07, 0x71, 0x77, 0xFF, 0x05, 0x52, 0x01, 0x13,
	0x4F, 0x4C, 0x19, 0xEB, 0xC3, 0xD1, 0x6D, 0xF6, 0xE1, 0xA7, 0x18, 0x54,
	0xAF, 0x23, 0xDF, 0x06, 0xB0, 0x2D, 0x8D, 0xFE, 0x13, 0x85, 0x92, 0x5E,
	0xFC, 0x2B, 0x43, 0x4D, 0x42, 0x48, 0x37, 0xD9, 0xCD, 0x0A, 0x37, 0xF0,
	0x41, 0x05, 0x00, 0xEF, 0xF0, 0xA6, 0xA4, 0x97, 0x52, 0x6C, 0x1C, 0xC1,
	0x1F, 0xCC, 0x30, 0xE9, 0x1A, 0x48, 0x01, 0x88, 0x81, 0xD4, 0x23, 0x1D,
	0xA7, 0xA9, 0xF4, 0xE2, 0xCF, 0x06, 0xCA, 0xE9, 0x2D, 0xD2, 0x94, 0x74,
	0x1F, 0x47, 0xAC, 0xCD, 0x68, 0x9A, 0xC1, 0xA4, 0x13, 0x03, 0xA9, 0x63,
	0xE9, 0x05, 0x4D, 0x31, 0x94, 0x74, 0x32, 0xD2, 0x9A, 0xCD, 0xE9, 0x89,
	0x98, 0x49, 0x3C, 0xCB, 0xAB, 0x07, 0x2C, 0xBD, 0x3C, 0xCB, 0xF4, 0x8D,
	0x80, 0x92, 0x9E, 0xE7, 0xF3, 0x39, 0xFE, 0x8A, 0x95, 0x02, 0x8B, 0xF4,
	0x96, 0x91, 0x74, 0x00, 0x49, 0x17, 0x42, 0x7C, 0x0A, 0xB3, 0xD9, 0x6F,
	0xAC, 0x42, 0xDD, 0xA8, 0x17, 0x4F, 0x95, 0xCA, 0x00, 0x00, 0x00, 0x00,
	0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82
	});
inline TextureCache notAvailableSymbol({
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
		0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x00, 0x00, 0x4B,
		0x08, 0x06, 0x00, 0x00, 0x00, 0x38, 0x4E, 0x7A, 0xEA, 0x00, 0x00, 0x00,
		0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00,
		0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
		0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
		0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7, 0x6F, 0xA8, 0x64, 0x00,
		0x00, 0x06, 0x6F, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5E, 0xD5, 0x9C, 0x49,
		0x6E, 0xE3, 0x46, 0x14, 0x86, 0x6D, 0xEA, 0x0E, 0xBD, 0x92, 0xEE, 0x21,
		0xF7, 0x29, 0x7A, 0x63, 0x03, 0xB9, 0x41, 0x02, 0x78, 0x40, 0xBB, 0xCF,
		0xD1, 0x6D, 0x78, 0x58, 0xE4, 0x08, 0x41, 0x6B, 0x95, 0x4B, 0xD8, 0xBE,
		0x87, 0xB9, 0xCA, 0x11, 0x02, 0xC8, 0xCE, 0xFF, 0xB3, 0x5F, 0x31, 0x24,
		0xC5, 0xE1, 0xBD, 0x9A, 0x48, 0x7D, 0x80, 0x51, 0x45, 0x59, 0x62, 0x95,
		0x7E, 0xBE, 0xA9, 0x8A, 0x92, 0x4E, 0x4F, 0x66, 0xE0, 0xE1, 0xE1, 0x61,
		0x5D, 0x14, 0xC5, 0xE6, 0xE3, 0xE3, 0x63, 0xFB, 0xFE, 0xFE, 0xBE, 0xC1,
		0x43, 0xDB, 0xD3, 0xD3, 0x53, 0xB6, 0xEB, 0xEA, 0x09, 0x6D, 0x4A, 0x3C,
		0xEF, 0x85, 0x1D, 0xBC, 0x86, 0x6D, 0x79, 0x75, 0x75, 0xB5, 0xE3, 0x71,
		0x6E, 0xB2, 0x89, 0xF5, 0xF8, 0xF8, 0x78, 0x0E, 0x61, 0xCE, 0x21, 0xCA,
		0x19, 0x0E, 0xFB, 0x44, 0x31, 0x03, 0x11, 0x77, 0x10, 0x70, 0x97, 0x4B,
		0xBC, 0xA4, 0x62, 0x3D, 0x3D, 0x3D, 0x9D, 0xED, 0xF7, 0xFB, 0xAF, 0x10,
		0xE8, 0x5C, 0x1E, 0x4A, 0x46, 0x0E, 0xE1, 0x92, 0x88, 0x45, 0x2B, 0xC2,
		0xE4, 0x7F, 0xA0, 0x1B, 0xC5, 0x82, 0x2C, 0xD0, 0x65, 0x21, 0xDA, 0x5D,
		0x0A, 0xD1, 0xA2, 0x8A, 0x35, 0xA7, 0x48, 0x3D, 0x94, 0xB0, 0xE8, 0xDB,
		0x98, 0xA2, 0x45, 0x11, 0x8B, 0x01, 0x1B, 0x0D, 0x83, 0xEF, 0x12, 0x44,
		0x6A, 0x41, 0x4B, 0x5B, 0xAD, 0x56, 0xDF, 0x2E, 0x2F, 0x2F, 0xAB, 0x24,
		0x11, 0x42, 0x21, 0xAD, 0x37, 0xF7, 0xF7, 0xF7, 0x3F, 0xD1, 0xBC, 0xE1,
		0x6F, 0x71, 0x42, 0x11, 0x26, 0x14, 0x24, 0x96, 0x67, 0xCC, 0x93, 0x16,
		0x1F, 0x84, 0xB7, 0x65, 0x2D, 0xD9, 0x9A, 0x86, 0xA0, 0x95, 0x41, 0xBC,
		0x8B, 0xEB, 0xEB, 0xEB, 0x52, 0x1E, 0x32, 0xE1, 0x65, 0x59, 0x8C, 0x4D,
		0x68, 0x16, 0x6B, 0x4D, 0x43, 0x48, 0xD9, 0xF2, 0x26, 0xF3, 0x37, 0x63,
		0x16, 0x0B, 0x03, 0x7D, 0xC5, 0x15, 0xA2, 0xEB, 0x1D, 0x2D, 0x9C, 0x3F,
		0xDF, 0x87, 0x1C, 0xAA, 0x31, 0xB9, 0x21, 0xFD, 0x1E, 0x57, 0xC7, 0x3C,
		0xC8, 0x00, 0xAC, 0xCC, 0x69, 0x9D, 0xAF, 0x08, 0xC0, 0x3B, 0xC4, 0x15,
		0xF6, 0x4F, 0xBA, 0x2E, 0xC2, 0x5A, 0x0D, 0xCF, 0x5B, 0xE3, 0xFF, 0x55,
		0x31, 0x8B, 0xF1, 0xA3, 0xD5, 0x6C, 0x38, 0xEF, 0xDD, 0xCD, 0xCD, 0xCD,
		0xAD, 0x1C, 0x4E, 0xA2, 0x16, 0x2B, 0x92, 0x50, 0x14, 0x68, 0x87, 0xF3,
		0xFC, 0xF0, 0x8D, 0x1B, 0x84, 0x6E, 0x24, 0xAB, 0x81, 0x18, 0xC2, 0xFD,
		0x85, 0xB9, 0xFC, 0x26, 0xFD, 0x51, 0x54, 0x62, 0x89, 0xEB, 0x85, 0x64,
		0x93, 0xE8, 0x35, 0x8F, 0x43, 0x84, 0xE3, 0x2A, 0x81, 0x96, 0xE7, 0x05,
		0x5E, 0xFB, 0x37, 0xE6, 0xF6, 0x45, 0x0E, 0x07, 0x99, 0x14, 0x8B, 0x93,
		0x81, 0x50, 0xBE, 0x31, 0x2A, 0x99, 0x48, 0x5D, 0x42, 0x45, 0xC3, 0xEB,
		0xFE, 0xC0, 0x3C, 0xFF, 0x94, 0xC3, 0x5E, 0x46, 0xC5, 0x92, 0xF2, 0xA0,
		0x8A, 0x25, 0x56, 0x44, 0xA4, 0x3B, 0x39, 0xCC, 0x46, 0xA0, 0x17, 0x6C,
		0xC6, 0xC2, 0xC3, 0x68, 0x36, 0xF4, 0xB4, 0x28, 0x0E, 0xB6, 0x99, 0x43,
		0x28, 0xC2, 0x71, 0xB1, 0x36, 0xFC, 0x8C, 0xEE, 0x3F, 0xBF, 0x1E, 0x31,
		0x31, 0x5A, 0xE5, 0x0F, 0x8A, 0x25, 0x01, 0xDD, 0x64, 0xD2, 0x10, 0xF7,
		0x05, 0x57, 0x66, 0xF4, 0xEA, 0xE4, 0x00, 0xF3, 0xD8, 0xA2, 0xF9, 0xF4,
		0xEB, 0xC8, 0xC4, 0x5A, 0x56, 0x24, 0xBD, 0xF4, 0xBA, 0x21, 0xD3, 0x35,
		0xFC, 0xFF, 0x59, 0x0E, 0x55, 0x60, 0x82, 0xA6, 0x34, 0x9C, 0x8A, 0x48,
		0x59, 0xBB, 0xF7, 0x82, 0xF7, 0x5A, 0xD6, 0x7E, 0xBF, 0xFF, 0x2E, 0x5D,
		0x15, 0x0B, 0x12, 0xEA, 0x67, 0x04, 0xA1, 0x48, 0xAF, 0x3B, 0x1E, 0x58,
		0x96, 0x35, 0xFB, 0xD1, 0xF5, 0x20, 0x14, 0x63, 0xC4, 0x6C, 0x30, 0x11,
		0x71, 0xCE, 0xD6, 0xB0, 0x31, 0x06, 0xCE, 0x75, 0xD1, 0xCD, 0xE2, 0x07,
		0x96, 0x85, 0x41, 0x2D, 0x99, 0xA4, 0x5C, 0x82, 0x50, 0x68, 0xB8, 0x40,
		0x8E, 0x26, 0x14, 0xE9, 0xD3, 0xA1, 0x25, 0x16, 0xAD, 0x0A, 0x8D, 0x7A,
		0x71, 0x8C, 0xAC, 0x73, 0x21, 0xDD, 0x59, 0x60, 0x6C, 0x45, 0x93, 0x6A,
		0x41, 0xBF, 0x16, 0x3D, 0x6A, 0x5A, 0x62, 0xB1, 0xA8, 0x93, 0xEE, 0x24,
		0xB8, 0x92, 0xB7, 0x31, 0x36, 0xD4, 0x7C, 0x61, 0x3D, 0x65, 0x4D, 0x42,
		0x56, 0xBA, 0xD6, 0x55, 0x8B, 0x45, 0x15, 0xB5, 0xA6, 0xCC, 0x38, 0x35,
		0x57, 0x1D, 0x45, 0x98, 0xF1, 0x8C, 0xE1, 0xA2, 0x86, 0x73, 0x47, 0xA3,
		0x2D, 0x6D, 0xD6, 0x62, 0xBD, 0x15, 0xB5, 0x58, 0xB8, 0x4A, 0xEA, 0x45,
		0x29, 0x6F, 0x08, 0x48, 0x37, 0x3B, 0x21, 0x19, 0x0F, 0x42, 0x31, 0x6B,
		0x7F, 0xA6, 0x57, 0xC8, 0x43, 0x93, 0xF0, 0xEE, 0x94, 0x74, 0xFF, 0x17,
		0x0B, 0x27, 0x50, 0x89, 0x25, 0x56, 0x95, 0x7C, 0xAD, 0xD7, 0x85, 0x81,
		0x1C, 0x42, 0x3D, 0x6B, 0xE7, 0xD9, 0x85, 0x02, 0xB9, 0xF2, 0x46, 0xE6,
		0xAF, 0xB2, 0xAE, 0xE6, 0x78, 0x95, 0x58, 0xDD, 0x40, 0x36, 0xC6, 0x1C,
		0x56, 0x15, 0x9A, 0xF1, 0xF0, 0x3A, 0x96, 0x01, 0xAD, 0x79, 0x53, 0x3C,
		0xE9, 0x4E, 0xE2, 0xF4, 0xA9, 0xC4, 0xB2, 0xB8, 0x60, 0x6E, 0xAB, 0x0A,
		0xCC, 0x78, 0x25, 0xD7, 0x89, 0x7D, 0x73, 0xB6, 0x58, 0x97, 0xD3, 0xA7,
		0x12, 0xAB, 0x69, 0x6A, 0x63, 0xC0, 0x05, 0xB3, 0x0A, 0x15, 0x98, 0xF1,
		0x28, 0xC4, 0xD9, 0x58, 0xC6, 0xD6, 0xBE, 0x1F, 0x67, 0xD1, 0x75, 0xCC,
		0xD2, 0x80, 0xAB, 0x94, 0x4D, 0xAC, 0xD0, 0x8C, 0xA7, 0x59, 0xD0, 0x43,
		0x04, 0xED, 0xF9, 0xD7, 0x0C, 0x05, 0x85, 0x25, 0x5E, 0xE5, 0x72, 0xC1,
		0x18, 0x19, 0x4F, 0x0E, 0x47, 0x11, 0x31, 0x55, 0xAE, 0x08, 0x43, 0xD9,
		0xD0, 0xB2, 0x54, 0xB1, 0x20, 0x87, 0x0B, 0xC6, 0xCC, 0x78, 0x5A, 0xF0,
		0xBE, 0x54, 0x9B, 0x9B, 0x78, 0xDE, 0xB6, 0x40, 0x4C, 0xD0, 0x66, 0x18,
		0xD5, 0x15, 0xF0, 0x25, 0x45, 0xC6, 0x53, 0xF2, 0x2A, 0xED, 0x28, 0xD0,
		0xA9, 0xB2, 0x2C, 0x15, 0x30, 0xC3, 0x64, 0x4B, 0x9B, 0x54, 0x19, 0x4F,
		0x03, 0x6F, 0xC3, 0x49, 0x77, 0x8A, 0x6D, 0xA1, 0xBD, 0x92, 0x78, 0x5E,
		0x12, 0xCB, 0x4A, 0x9D, 0xF1, 0xA6, 0xC0, 0xD8, 0x2A, 0x37, 0xC4, 0xFB,
		0xD7, 0xC7, 0xAC, 0x14, 0x8B, 0xE6, 0xD0, 0x8C, 0x87, 0xE6, 0x2C, 0xE7,
		0x16, 0xB6, 0xA9, 0x74, 0x88, 0x49, 0x60, 0xC6, 0xDB, 0x31, 0xE3, 0xC5,
		0x10, 0xCA, 0x70, 0x8E, 0x75, 0x76, 0xB1, 0x42, 0x33, 0x1E, 0x84, 0x62,
		0x69, 0x30, 0xCB, 0x3E, 0x5A, 0x56, 0xB1, 0x22, 0x64, 0x3C, 0x73, 0x69,
		0x10, 0x13, 0x8A, 0xA5, 0x32, 0x43, 0x79, 0xA3, 0xDE, 0x84, 0xEE, 0x6A,
		0x4A, 0xC6, 0x9B, 0x6D, 0x6B, 0x88, 0x64, 0xB1, 0xAC, 0x08, 0x19, 0x6F,
		0x93, 0x6A, 0x57, 0xB6, 0xB9, 0xB9, 0x37, 0x41, 0x59, 0x20, 0x06, 0xA8,
		0x52, 0x27, 0xCB, 0x7D, 0xE9, 0x9A, 0x58, 0x7A, 0xC6, 0xC3, 0x18, 0x2A,
		0x4B, 0xE7, 0x5C, 0x68, 0x59, 0xAA, 0x0A, 0x76, 0xBF, 0xDF, 0x9B, 0x03,
		0xF2, 0x52, 0x32, 0xDE, 0x18, 0x86, 0x15, 0x0C, 0x0D, 0xA6, 0x50, 0x59,
		0x16, 0xE0, 0x2D, 0x71, 0x35, 0x47, 0x94, 0xF1, 0x54, 0x96, 0x05, 0x9D,
		0x5E, 0x58, 0xC1, 0xAB, 0x2C, 0x0B, 0xCF, 0x53, 0xB9, 0x21, 0x13, 0x01,
		0xFE, 0xDE, 0xF0, 0xFC, 0xA3, 0xC8, 0x78, 0x86, 0x0B, 0x5A, 0x72, 0x21,
		0xAD, 0xB5, 0xAC, 0x6A, 0x4F, 0x47, 0xFA, 0xBD, 0x48, 0xB0, 0x64, 0x9C,
		0x39, 0x8A, 0x8C, 0x67, 0xDD, 0x9E, 0x2A, 0x24, 0x26, 0xA8, 0xE2, 0x02,
		0xDC, 0x63, 0xF0, 0x8A, 0x73, 0x60, 0xC9, 0x78, 0x3E, 0x42, 0x25, 0xCD,
		0x78, 0x43, 0x60, 0xBE, 0xA6, 0x30, 0x51, 0x95, 0x0E, 0x8C, 0xF4, 0xD5,
		0xD1, 0x04, 0x43, 0x26, 0xCB, 0xD2, 0x00, 0xE7, 0xF0, 0xFA, 0x74, 0xA0,
		0x8C, 0x9D, 0x75, 0x8D, 0xE7, 0xD0, 0xBA, 0x20, 0xE6, 0x58, 0xED, 0x4C,
		0x54, 0x62, 0xC1, 0xFC, 0xB5, 0xDB, 0x14, 0x07, 0xB7, 0xB4, 0x03, 0x4B,
		0x83, 0x2C, 0x19, 0xAF, 0x0F, 0x8B, 0x0B, 0x3A, 0x7D, 0x2A, 0xB1, 0x2C,
		0x7B, 0x41, 0x4D, 0x61, 0x24, 0xE3, 0x85, 0x6C, 0xFF, 0xCE, 0xF6, 0x59,
		0x09, 0xB8, 0xA0, 0x7A, 0xDE, 0x4E, 0x9F, 0xBA, 0x82, 0x77, 0xA6, 0xA6,
		0x80, 0xD6, 0xF5, 0xFB, 0x31, 0x65, 0xBC, 0x2E, 0xB4, 0x2A, 0xED, 0xDC,
		0x9B, 0xBA, 0xD4, 0x62, 0xAD, 0x56, 0x2B, 0x75, 0x16, 0xC2, 0x09, 0x9E,
		0xD0, 0x1C, 0x45, 0xC6, 0xEB, 0xC3, 0x62, 0x55, 0xCD, 0x10, 0x55, 0x8B,
		0x25, 0x99, 0x48, 0x1B, 0x3B, 0x56, 0xD2, 0x5A, 0x98, 0x25, 0xE3, 0x75,
		0x61, 0x32, 0x32, 0x58, 0x55, 0xEB, 0xA3, 0x0A, 0xB5, 0x58, 0x84, 0xEE,
		0x21, 0xDD, 0xA8, 0x70, 0x50, 0x34, 0xB3, 0x64, 0xBC, 0x26, 0xAC, 0x03,
		0x31, 0x17, 0x75, 0x32, 0x82, 0x55, 0xB5, 0x3C, 0xE0, 0xE0, 0x63, 0x92,
		0x8C, 0x45, 0x68, 0xBC, 0x5C, 0xAC, 0x0F, 0xFA, 0xFC, 0x9C, 0x81, 0xBC,
		0x89, 0xF1, 0xBD, 0x95, 0xB8, 0xB8, 0xAD, 0x55, 0x4B, 0xCB, 0xB2, 0x48,
		0x4C, 0xEB, 0x82, 0x50, 0xB3, 0x66, 0xBC, 0x26, 0xCC, 0xDC, 0x68, 0xD4,
		0x46, 0xD0, 0xA7, 0x43, 0xEF, 0x47, 0xBB, 0x63, 0x58, 0x17, 0x07, 0x9B,
		0x3B, 0x90, 0x3B, 0x58, 0x0B, 0x62, 0x3E, 0xEA, 0xA0, 0xCE, 0xB0, 0xC1,
		0xFA, 0x4F, 0x0E, 0x6B, 0x0E, 0x2C, 0x4B, 0xF0, 0x2A, 0x09, 0x1C, 0x4B,
		0xC8, 0x78, 0x0E, 0xAB, 0x50, 0x84, 0xDF, 0xA9, 0x96, 0x6E, 0x8B, 0x5E,
		0xB1, 0x18, 0x88, 0x19, 0x6B, 0xE4, 0xD0, 0xC2, 0xBF, 0xF8, 0x9B, 0x3D,
		0xE3, 0x39, 0x7C, 0x8A, 0x66, 0x86, 0x8E, 0xA1, 0xF9, 0x4F, 0x7D, 0xD1,
		0xC9, 0xC7, 0x1D, 0x79, 0x87, 0xF8, 0x62, 0x4E, 0xC1, 0x30, 0x6F, 0xCE,
		0x99, 0xE3, 0x9B, 0xE6, 0x3E, 0xE4, 0x7E, 0x8E, 0x21, 0x37, 0x74, 0xF8,
		0xB8, 0x23, 0xBF, 0x75, 0xFA, 0xCC, 0x7A, 0x46, 0x8E, 0xB3, 0x22, 0xE3,
		0x7A, 0xC5, 0x5C, 0x58, 0xE1, 0x68, 0x32, 0x1A, 0xB5, 0x2C, 0xC2, 0xA5,
		0x01, 0x14, 0xF7, 0xDE, 0x51, 0x60, 0xAD, 0xD2, 0x2C, 0xEC, 0x52, 0x21,
		0xF3, 0x64, 0x0D, 0xE5, 0x95, 0x98, 0x28, 0xD4, 0xD4, 0x3C, 0x27, 0xC5,
		0x22, 0xBC, 0x5A, 0x32, 0x11, 0x2F, 0x52, 0x8A, 0x16, 0x2A, 0x12, 0x81,
		0x50, 0xAA, 0xCC, 0xAD, 0x12, 0x8B, 0xF8, 0x64, 0x95, 0x3E, 0xF0, 0xC6,
		0x82, 0x7F, 0x60, 0x87, 0x31, 0x09, 0xE7, 0xB9, 0xC5, 0x7C, 0xB8, 0xCD,
		0xE2, 0x2D, 0x12, 0xC1, 0x79, 0x58, 0x0B, 0xAA, 0x6A, 0x4B, 0xB5, 0x58,
		0x24, 0x96, 0x60, 0x0E, 0x0A, 0x87, 0x86, 0x09, 0x81, 0x77, 0xA9, 0xCB,
		0x6E, 0x52, 0x90, 0x40, 0xCD, 0x52, 0x64, 0x23, 0x77, 0x97, 0xC6, 0x7E,
		0x67, 0xCB, 0x8C, 0x45, 0x28, 0x62, 0x12, 0x8B, 0x84, 0xBA, 0xE4, 0x52,
		0x80, 0xE8, 0xE6, 0xA2, 0xD9, 0x2C, 0x16, 0x09, 0x09, 0xFA, 0x4B, 0x00,
		0x42, 0x4D, 0x06, 0xF3, 0x3E, 0xBC, 0xC4, 0x22, 0x12, 0x37, 0xA2, 0x7E,
		0xC7, 0x2F, 0x03, 0xDC, 0xF5, 0xF0, 0xDE, 0xFD, 0xF0, 0x16, 0xCB, 0x11,
		0x3B, 0x8E, 0xA5, 0x82, 0xF1, 0x31, 0x74, 0x51, 0x1F, 0x2C, 0x16, 0xE1,
		0x3E, 0x11, 0xBF, 0x2A, 0xBC, 0x50, 0x2B, 0x0B, 0xB2, 0xA6, 0x26, 0x51,
		0xC4, 0x72, 0xC4, 0xA8, 0x79, 0x22, 0xB2, 0xCC, 0x5F, 0x66, 0xEB, 0x42,
		0xD1, 0xB8, 0xCF, 0x3D, 0x93, 0xA5, 0x25, 0xFB, 0x95, 0x92, 0x24, 0x62,
		0x39, 0x44, 0xB4, 0x58, 0x3F, 0xB0, 0x33, 0x0A, 0x63, 0x12, 0x6F, 0xBA,
		0xA4, 0x5C, 0xC0, 0x27, 0x15, 0xAB, 0x49, 0x02, 0xE1, 0xAA, 0x1F, 0x7B,
		0x0D, 0x5D, 0x0D, 0x58, 0xC8, 0x26, 0x56, 0x17, 0x8A, 0x87, 0xC6, 0xFD,
		0x2E, 0x16, 0x6B, 0x1F, 0xB6, 0xDD, 0x58, 0x57, 0x05, 0x65, 0x88, 0xC2,
		0x5D, 0x84, 0x57, 0x08, 0xC3, 0x7B, 0x95, 0xAF, 0x78, 0xCD, 0x5B, 0xFE,
		0x9B, 0x1F, 0x27, 0x27, 0xFF, 0x01, 0x25, 0x56, 0x00, 0xC0, 0x8D, 0xD8,
		0xBB, 0x35, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42,
		0x60, 0x82
	});

void DrawSingleSkinSettings(); // single_skin_settings.cpp
void DrawStickerSearch(); // sticker_search.cpp
void DrawSavedSkinsList(); // saved_skins_list.cpp
void DrawSkinsSearch(); // skins_search.cpp