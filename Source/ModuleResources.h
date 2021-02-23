#pragma once
#include "Module.h"
#include "Resource.h"

enum class ResourceType {
	FOLDER,
	SCENE,
	PREFAB,
	TEXTURE,
	MATERIAL,
	ANIMATION,
	TILEMAP,
	AUDIO,
	SCRIPT,
	UNKNOWN
};

class ModuleResources : public Module
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	void Save(Config* config) const override;
	void Load(Config* config) override;

	// Importing
	bool ImportFromPath(std::string path, UID uid = 0);
	bool ImportResource(const char* path, UID uid = 0);

private:
	ResourceType GetResourceType(const char* path);

public:
	//std::vector<UID> resources;
	//std::vector<UID> loaded_resources;

	ResourceTexture textures;
};
