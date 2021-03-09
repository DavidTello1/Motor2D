#pragma once
#include "ResourceData.h"

struct ResourceTexture //***maybe have the functions inside a struct
{
	//enum
	//{
	//	kFlagPosition = 1 << 0,
	//	kFlagSprite = 1 << 1,
	//	kFlagWorldBounds = 1 << 2,
	//	kFlagMove = 1 << 3,
	//};

	ResourceData data;

	struct TextureData {
		std::vector<int> width, height;
		std::vector<uint32_t> buffer;
		//std::vector<int> flags;
	} texture;

	//--------------------------
	void Add(const char* assets_path, const char* name, const char* library_folder, const char* extension, UID id = 0)
	{
		data.Add(assets_path, name, library_folder, extension, id);

		texture.width.push_back(0);
		texture.height.push_back(0);
		texture.buffer.push_back(0);
	}

	bool Create(const char* path, const char* name, UID uid = 0);
	void Remove(size_t index);

	bool Save() const;
	bool Load(size_t index);
	void UnLoad(size_t index);
};
