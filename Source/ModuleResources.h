#pragma once
#include "Module.h"
#include "Resource.h"
#include "AssetNode.h"

#define RESERVED_RESOURCES 11 // Icons for Panel Assets (10) + Engine Icon (1)

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

	size_t GetResourceIndex(ResourceType type, const char* path); //***public for resources panel
	ResourceType GetResourceType(const char* path); //***public for module file_system

private:
	void ImportAllAssets(const char* path);
	bool ImportFromAssets(const char* path, UID uid = 0, bool save_meta = true); // Creates file in /Library and a .meta in /Assets (if bool is true)

	//void CleanMeta(); // Remove .meta files of resources that no longer exist in /Assets
	//void CleanLibrary(); // Remove Library-Resources that no longer exist in /Assets


public:
	//std::vector<UID> resources;
	//std::vector<UID> loaded_resources;

	ResourceTexture textures;
};
