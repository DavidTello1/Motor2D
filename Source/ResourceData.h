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

	int GetIndexFromName(std::string name) const
	{
		for (size_t index = 0, size = names.size(); index < size; ++index)
		{
			if (name == names[index])
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
