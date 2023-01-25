#pragma once
#include "Resource.h"

struct ResourceTexture : public Resource 
{
	ResourceTexture() {};
	ResourceTexture(UID id, const char* name, const char* path_assets, const char* path_library) 
		: Resource(id, (int)ResourceType::TEXTURE, name, path_assets, path_library) {};

	~ResourceTexture() {};

	uint tex_index = 0;
	int width = 0;
	int height = 0;
};


class LoaderTexture
{
public:
	bool Load(ResourceTexture* data);
	bool Unload(ResourceTexture* data);
};
