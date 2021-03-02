#pragma once
#include "Module.h"

#include "PhysFS/include/physfs.h"
#include <vector>

struct AssetNode;
enum class ResourceType;

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(const char* game_path = nullptr);
	~ModuleFileSystem();

	bool Init(Config* config = nullptr);
	bool CleanUp();

	// Main functions
	unsigned int Load(const char* file, char** buffer) const;
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);
	bool CopyFromOutside(const char* full_path, const char* destination);
	bool Copy(const char* source, const char* destination);

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const { return PHYSFS_exists(file) != 0; }
	uint64_t GetLastModTime(const char* path) { return PHYSFS_getLastModTime(path); }
	void NormalizePath(std::string& full_path) const;
	void ToLower(std::string& full_path) const;

	// Folders
	bool IsFolder(const char* file) const { return PHYSFS_isDirectory(file) != 0; }
	void CreateFolder(const char* directory) { PHYSFS_mkdir(directory); }
	void GetFolderContent(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;

	// Extensions
	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	bool CheckExtension(const char* path, std::vector<std::string> extensions) const; //check if extension matches any of the list

	// Paths
	const char* GetBasePath() const { return PHYSFS_getBaseDir(); }
	const char* GetWritePath() const { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths() const;
	const char* GetActualPath() const { return PHYSFS_getUserDir(); }

	// Assets ***MOVE TO MODULE RESOURCES
	AssetNode GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr); //filter if you only want specific extensions or ignore if you want to ignore specific extensions
	ResourceType GetType(const char* path) const; //get node type
};
