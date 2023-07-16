#pragma once
#include "vpktool/VPK.h"

#include <vector>

typedef struct {
	void* bytes;
	size_t size;
} ImgData;

class TextureCache {
	vpktool::VPKEntry entryLoc;
	std::vector<BYTE> rawImgBytes;
	bool loaded;
	void* texture;

public:
	TextureCache(vpktool::VPKEntry entryLoc);
	TextureCache(std::vector<BYTE> rawImgBytes);
	void* Get();

private:
	ImgData GetRaw();
	ImgData GetFromVPK(std::vector<std::byte>& vtexBytes);
};