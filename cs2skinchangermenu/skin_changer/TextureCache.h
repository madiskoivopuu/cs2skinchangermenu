#pragma once
#include "vpktool/VPK.h"

struct TextureCache {
	bool loaded;
	vpktool::VPK& vpk;
	vpktool::VPKEntry entryLoc;
	void* texture;
};