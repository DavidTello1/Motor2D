#pragma once
#include "Globals.h"
#include "Config.h"

#include "Glew/include/glew.h"
#include <vector>
#include <string>

struct ResourceData
{
	std::vector<UID> ids;
	std::vector<std::string> files_assets;
	std::vector<std::string> files_library;
	std::vector<int> times_loaded;

	// --------------
	void Add(const char* assets_path, const char* library_folder, const char* extension, UID id = 0) 
	{
		UID uid = id;
		if (uid == 0 || (!ids.empty() && GetIndexFromID(id) == -1)) {}
			//uid = GenerateUID()

		ids.push_back(uid);
		files_assets.push_back(assets_path);
		files_library.push_back(library_folder + std::to_string(uid) + extension);
		times_loaded.push_back(0);
	}

	void Remove(size_t index)
	{
		ids.erase(ids.begin() + index);
		files_assets.erase(files_assets.begin() + index);
		files_library.erase(files_library.begin() + index);
		times_loaded.erase(times_loaded.begin() + index);
	}

	int GetIndexFromID(UID id)
	{
		for (size_t index = 0, size = ids.size(); index <= size; ++index)
		{
			if (id == ids[index])
				return index;
		}
		return -1;
	}

	int GetIndexFromPath(const char* path)
	{
		for (size_t index = 0, size = files_assets.size(); index <= size; ++index)
		{
			if (path == files_assets[index])
				return index;
		}
		return -1;
	}
};


//struct ResourceFolder
//{
//	ResourceData data;
//	std::vector<std::vector<UID>> childs;
//
//	//--------------------------
//	void Add(const char* assets_path, const char* library_folder, const char* extension, UID id = 0)
//	{
//		data.Add(assets_path, library_folder, extension, id);
//
//		std::vector<UID> children;
//		childs.push_back(children);
//	}
//
//	void AddChild(size_t index, UID id)
//	{
//		childs[index].push_back(id);
//	}
//};


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
	void Add(const char* assets_path, const char* library_folder, const char* extension, UID id = 0)
	{
		data.Add(assets_path, library_folder, extension, id);

		texture.width.push_back(0);
		texture.height.push_back(0);
		texture.buffer.push_back(0);
	}

	bool Create(const char* path, UID uid = 0, bool save_meta = true);
	void Remove(size_t index);

	bool Import(size_t index) const;
	void SaveMeta(size_t index) const;

	bool Load(size_t index);
	void UnLoad(size_t index);
};
