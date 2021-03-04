#pragma once
#include "Module.h"
#include "Resource.h"

#define RESERVED_RESOURCES 11 // Icons for Panel Assets (10) + Engine Icon (1)

struct AssetNode;
enum class ResourceType;

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

	// --- Resources ---
	bool ImportFromExplorer(std::string path, UID uid = 0); // Importing

	size_t GetResourceIndex(ResourceType type, const char* path); //***public for resources panel
	ResourceType GetResourceType(const char* path); //***DELETE

	// --- Assets ---
	AssetNode GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr); //filter if you only want specific extensions or ignore if you want to ignore specific extensions
	ResourceType GetType(const char* path) const; //get node type
	void SaveMeta(ResourceData data, size_t index) const;

private:
	void ImportAllAssets(const char* path);
	bool ImportFromAssets(const char* path, UID uid = 0, bool save_meta = true); // Creates file in /Library and a .meta in /Assets (if bool is true)

	//void CleanMeta(); // Remove .meta files of resources that no longer exist in /Assets
	//void CleanLibrary(); // Remove Library-Resources that no longer exist in /Assets

public:
	ResourceTexture textures;

	//std::vector<UID> loaded_resources;
};
