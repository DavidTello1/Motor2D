#pragma once

#include "Resource.h"
#include "ResourceManager.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture() {};
	ResourceTexture(UID id, const char* name, const char* path_assets, const char* path_library)
		: Resource(id, (int)ResourceType::TEXTURE, name, path_assets, path_library) {};

	~ResourceTexture() {};

public:
	uint tex_index = 0;
	int width = 0;
	int height = 0;
};


class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	bool Import();
	bool Save();

	bool Load();
	bool Unload();

private:
	bool LoadToMemory();
	bool UnloadFromMemory();

	bool LoadFromMeta();
	bool SaveToMeta();

private:
	static constexpr int MAX_TEXTURES = 1000;

	std::array<UID, MAX_TEXTURES> ids;
	ResourceManager<ResourceTexture>* textures;
};
