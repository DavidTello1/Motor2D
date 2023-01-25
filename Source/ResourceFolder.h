#pragma once
#include "Resource.h"

#include <vector>

struct ResourceFolder : public Resource
{
	ResourceFolder() {};
	ResourceFolder(UID id, const char* name, const char* path_assets, const char* path_library)
		: Resource(id, (int)ResourceType::FOLDER, name, path_assets, path_library) {};

	~ResourceFolder() {};

	UID parent = 0;
	std::vector<UID> childs;
};

class LoaderFolder
{
public:
	bool Load(ResourceFolder* data) { return false; }
	bool Unload(ResourceFolder* data) { return false; }
};
