#pragma once
#include "Module.h"

#include <vector>
#include <string>

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true, const char* game_path = nullptr);
	~ModuleFileSystem();

	bool Init() override;
	bool CleanUp() override;

	// --- Main functions ---
	unsigned int Load(const char* file, char** buffer) const;
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);
	bool CopyFromOutside(const char* full_path, const char* destination);
	bool Copy(const char* source, const char* destination);

	// --- Utility functions ---
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	uint64_t GetLastModTime(const char* path);
	void NormalizePath(std::string& full_path) const;
	void ToLower(std::string& full_path) const;

	// --- Folders ---
	bool IsFolder(const char* file) const;
	void CreateFolder(const char* directory);
	void GetFolderContent(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;

	// --- Extensions ---
	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	bool CheckExtension(const char* path, std::vector<std::string> extensions) const; //check if extension matches any of the list

	// --- Paths ---
	const char* GetBasePath() const;
	const char* GetWritePath() const;
	const char* GetReadPaths() const;
	const char* GetUserPath() const;
};
