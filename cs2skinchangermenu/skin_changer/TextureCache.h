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

	int width;
	int height;

	bool loaded;
	void* texture;

public:
	TextureCache();
	TextureCache(vpktool::VPKEntry entryLoc);
	TextureCache(std::vector<BYTE> rawImgBytes);
	void* Get();
	int Width();
	int Height();

private:
	ImgData GetRaw();
	ImgData GetFromVPK(std::vector<std::byte>& vtexBytes);
};