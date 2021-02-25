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
	bool ImportFromExplorer(std::string path, UID uid = 0);

private:
	void LoadAllAssets(const char* path);
	bool ImportFromAssets(const char* path, UID uid = 0, bool save_meta = true); // Creates file in /Library and a .meta in /Assets (if bool is true)

	void CleanMeta(); // Remove .meta files of resources that no longer exist in /Assets
	void CleanLibrary(); // Remove Library-Resources that no longer exist in /Assets

	ResourceType GetResourceType(const char* path);

public:
	//std::vector<UID> resources;
	//std::vector<UID> loaded_resources;

	ResourceTexture textures;
};
