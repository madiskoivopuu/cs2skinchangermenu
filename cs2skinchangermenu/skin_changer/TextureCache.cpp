#include "pch.h"
#include "TextureCache.h"

#include "gui/gui_setup.h"
#include "skin_changer/skins_cache.h"

#include "DirectXTex.h"
#include <d3d11.h>
#include <algorithm>

static std::vector<std::byte> pngMagicNr { std::byte{0x89}, std::byte{0x50}, std::byte{0x4e}, std::byte{0x47}, std::byte{0x0d}, std::byte{0x0a}, std::byte{0x1a}, std::byte{0x0a}};

TextureCache::TextureCache() {
	this->rawImgBytes = {};
	this->width = 0;
	this->height = 0;
	this->loaded = false;
	this->texture = nullptr;
}

TextureCache::TextureCache(vpktool::VPKEntry entryLoc) {
	this->entryLoc = entryLoc;
	this->rawImgBytes = {};
	this->width = 0;
	this->height = 0;
	this->loaded = false;
	this->texture = nullptr;
}

TextureCache::TextureCache(std::vector<BYTE> rawImgBytes) {
	this->rawImgBytes = rawImgBytes;
	this->width = 0;
	this->height = 0;
	this->loaded = false;
	this->texture = nullptr;
}

int TextureCache::Width() {
	return this->width;
}

int TextureCache::Height() {
	return this->height;
}

ImgData TextureCache::GetFromVPK(std::vector<std::byte>& vtexBytes) {
	vtexBytes = skins_cache::skinsPakFile.readBinaryEntry(this->entryLoc);

	std::vector<std::byte>::iterator resultPos = std::search(vtexBytes.begin(), vtexBytes.end(), pngMagicNr.begin(), pngMagicNr.end());
	if (resultPos == vtexBytes.end())
		return ImgData{ 0 };

	size_t pngSize = vtexBytes.end() - resultPos;

	return ImgData{ &*resultPos, pngSize };
}

ImgData TextureCache::GetRaw() {
	return ImgData{ &*this->rawImgBytes.begin(), this->rawImgBytes.size()};
}

void* TextureCache::Get() {
	if (!this->loaded) {
		ImgData data;
		std::vector<std::byte> vtexBytes;
		if (this->rawImgBytes.size() != 0)
			data = this->GetRaw();
		else
			data = this->GetFromVPK(vtexBytes);

		DirectX::ScratchImage img;
		HRESULT res = DirectX::LoadFromWICMemory(data.bytes, data.size, DirectX::WIC_FLAGS_NONE, nullptr, img);
		if(!SUCCEEDED(res))
			return nullptr;

		HRESULT res2 = DirectX::CreateShaderResourceView(gui::device, img.GetImages(), img.GetImageCount(), img.GetMetadata(), reinterpret_cast<ID3D11ShaderResourceView**>(&this->texture));
		if (!SUCCEEDED(res2))
			return nullptr;

		this->width = static_cast<int>(img.GetMetadata().width);
		this->height = static_cast<int>(img.GetMetadata().height);
		this->loaded = true;
	}

	return this->texture;
}