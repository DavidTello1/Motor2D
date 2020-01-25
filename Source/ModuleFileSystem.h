#pragma once
#include "Module.h"

#include "PhysFS/include/physfs.h"
#include <vector>

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(const char* game_path = nullptr);
	~ModuleFileSystem();

	bool Init(Config* config = nullptr);
	bool CleanUp();

	// Utility functions
	bool AddPath(const char* path_or_zip);

	bool Exists(const char* file) const { return PHYSFS_exists(file) != 0; }
	bool IsDirectory(const char* file) const { return PHYSFS_isDirectory(file) != 0; }
	void CreateDirectory(const char* directory) { PHYSFS_mkdir(directory); }

	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	bool CopyFromOutsideFS(const char* full_path, const char* destination);
	bool Copy(const char* source, const char* destination);

	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	bool HasExtension(const char* path) const;
	
	void NormalizePath(char* full_path) const;
	void ToLower(char* full_path) const;

	// Open for Read/Write
	unsigned int LoadFromPath(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

	const char* GetBasePath() const { return PHYSFS_getBaseDir(); }
	const char* GetWritePath() const { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths() const;
};
