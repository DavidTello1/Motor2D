#pragma once
#include "Globals.h"
#include "Config.h"

#include "Glew/include/glew.h"
#include <vector>
#include <string>

#define EXTENSION_TEXTURE ".dvs_texture"

struct ResourceData
{
	std::vector<UID> ids;
	std::vector<std::string> files_assets;
	std::vector<std::string> files_library;
	std::vector<int> times_loaded;

	void Add(const char* assets_path, const char* library_folder, const char* extension, UID id = 0) 
	{
		UID uid = id;
		if (uid == 0 && !ids.empty() && GetIndex(id) != -1) {}
			//uid = GenerateUID()

		ids.push_back(uid);
		files_assets.push_back(assets_path);
		files_library.push_back(library_folder + std::to_string(uid) + extension);
		times_loaded.push_back(0);
	}

	int GetIndex(UID id)
	{
		for (size_t index = 0, size = ids.size(); index <= size; ++index)
		{
			if (id == ids[index])
				return index;
		}
		return -1;
	}
};

struct ResourceTexture
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

	void Add(const char* assets_path, const char* library_folder, const char* extension, UID id = 0)
	{
		data.Add(assets_path, library_folder, extension, id);

		texture.width.push_back(0);
		texture.height.push_back(0);
		texture.buffer.push_back(0);
	}

	bool Create(const char* path, UID uid = 0);
	bool Remove(size_t index) const;

	bool Import(size_t index) const;
	void SaveMeta(size_t index) const;

	bool Load(size_t index);
	void UnLoad(size_t index);
};