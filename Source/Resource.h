#pragma once
#include "Application.h"

struct ResourceData
{
	std::vector<UID> ids;
	std::vector<std::string> names;
	std::vector<std::string> files_assets;
	std::vector<std::string> files_library;
	std::vector<int> times_loaded;
	std::vector<int> size;

	// --------------
	void Add(const char* assets_path, const char* name, const char* library_folder, const char* extension, UID id = 0)
	{
		UID uid = id;
		if (uid == 0 || (!ids.empty() && GetIndexFromID(id) == -1))
			uid = App->GenerateUID();

		ids.push_back(uid);
		names.push_back(name);
		files_assets.push_back(assets_path);
		files_library.push_back(library_folder + std::to_string(uid) + extension);
		times_loaded.push_back(0);
		size.push_back(0);
	}

	void Remove(size_t index)
	{
		ids.erase(ids.begin() + index);
		names.erase(names.begin() + index);
		files_assets.erase(files_assets.begin() + index);
		files_library.erase(files_library.begin() + index);
		times_loaded.erase(times_loaded.begin() + index);
		size.erase(size.begin() + index);
	}

	int GetIndexFromID(UID id) const
	{
		for (size_t index = 0, size = ids.size(); index < size; ++index)
		{
			if (id == ids[index])
				return index;
		}
		return -1;
	}

	int GetIndexFromPath(const char* path) const
	{
		for (size_t index = 0, size = files_assets.size(); index < size; ++index)
		{
			if (path == files_assets[index])
				return index;
		}
		return -1;
	}
};

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

struct ResourceScene 
{
	ResourceData data;

	struct SceneData {
		//GameObjects objects;
	} scene;
};