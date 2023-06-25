#pragma once

#include <cstdint>
#include "sdk/CUtl/CUtlVector.h"

class item_list_entry_t {
public:
	int m_nItemDef;

	// Paint kit applied to it
	int m_nPaintKit;
	int m_nPaintKitSeed;
	float m_flPaintKitWear;

	// Sticker kit applied to it
	uint32_t m_nStickerKit;

	// music kit applied to it
	uint32_t m_nMusicKit;

	bool m_bIsNestedList;
	bool m_bIsUnusualList;
	mutable bool m_bAlreadyUsedInRecursiveCreation;
};

class CEconItemSetDefinition {
public:
	void* pVTable;
	const char* setName;
	const char* setTranslationTag;

	char pad2[0x10];
	CUtlVector<item_list_entry_t> items;
	char pad3[0x28];
};