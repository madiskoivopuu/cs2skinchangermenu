#pragma once
#include "vpktool/VPK.h"

class TextureCache {
	vpktool::VPKEntry entryLoc;
	bool loaded;
	void* texture;

public:
	TextureCache(vpktool::VPKEntry entryLoc);
	void* Get();
};