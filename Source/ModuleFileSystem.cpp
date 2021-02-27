#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

//#include "ModuleEditor.h"
#include "AssetNode.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )
#include "PhysFS/include/physfs.h"
#include "SDL/include/SDL.h"
//#include <fstream>
#include <filesystem>

#include "mmgr/mmgr.h"

ModuleFileSystem::ModuleFileSystem(const char* game_path) : Module("FileSystem", true)
{
	// needs to be created before Init so other modules can use it
	LOG("Loading PhysFS", 'd');
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// workaround VS string directory mess
	AddPath(".");
	if (0 && game_path != nullptr)
		AddPath(game_path);

	LOG("FileSystem Operations base is [%s] plus:", GetBasePath(), 'd');
	LOG(GetReadPaths());

	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError(), 'e');

	// Create Standard Paths
	const char* dirs[] = {
		SETTINGS_FOLDER,
		ASSETS_FOLDER,
		LIBRARY_FOLDER,

		LIBRARY_SCENE_FOLDER,
		LIBRARY_PREFAB_FOLDER,
		LIBRARY_TEXTURE_FOLDER,
		LIBRARY_MATERIAL_FOLDER,
		LIBRARY_ANIMATION_FOLDER,
		LIBRARY_TILEMAP_FOLDER,
		LIBRARY_AUDIO_FOLDER,
		LIBRARY_SCRIPT_FOLDER
	};

	for (uint i = 0; i < sizeof(dirs) / sizeof(const char*); ++i)
	{
		if (!Exists(dirs[i]))
			CreateFolder(dirs[i]);
	}
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

// Called before render is available
bool ModuleFileSystem::Init(Config* config)
{
	LOG("Loading File System");
	bool ret = true;

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(App->GetOrganizationName(), App->GetAppName());

	// Trun this on while in game mode
	//if(PHYSFS_setWriteDir(write_path) == 0)
		//LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	SDL_free(write_path);

	return ret;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
	return true;
}

// Read a whole file and put it in a new buffer
uint ModuleFileSystem::Load(const char* file, char** buffer) const
{
	uint ret = 0;
	PHYSFS_file* fs_file = PHYSFS_openRead(file); //open file (only read)

	if (fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file); //get file size

		if (size > 0)
		{
			*buffer = new char[size];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size); //read file to buffer and get size
			if (readed != size) //check for error
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError(), 'e');
				RELEASE(buffer);
			}
			else
				ret = readed;
		}

		if (PHYSFS_close(fs_file) == 0) //close file
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError(), 'e');
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError(), 'e');

	return ret; //return size of file
}

// Save a whole buffer to disk
uint ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	bool overwrite = PHYSFS_exists(file) != 0; //check if file already exists in file system
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file); //open file (append or write only)

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size); //write file to buffer and get size
		if (written != size) //check for error
		{
			LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError(), 'e');
		}
		else
		{
			if (append == true) //if append
			{
				LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file, 'd');
			}
			else if (overwrite == false) //if is new file
				LOG("New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size, 'd');

			if (PHYSFS_close(fs_file) == 0) //close file
				LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError(), 'e');

			return written;
		}

		if (PHYSFS_close(fs_file) == 0) //close file
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError(), 'e');
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError(), 'e');

	return 0;
}

bool ModuleFileSystem::Remove(const char* file)
{
	if (file != nullptr) //check if file is valid
	{
		if (PHYSFS_delete(file) != 0) //delete file
		{
			LOG("File deleted: [%s]", file, 'd');
			return true;
		}
		else
		{
			const char* error = PHYSFS_getLastError();
			LOG("File System error while trying to delete [%s]: %s", file, error, 'e');
		}
	}
	return false;
}

bool ModuleFileSystem::CopyFromOutside(const char* full_path, const char* destination) // Only place we acces non virtual filesystem
{
	char buf[8192];
	size_t size;

	FILE* source = nullptr;
	fopen_s(&source, full_path, "rb"); //open source file
	PHYSFS_file* dest = PHYSFS_openWrite(destination); //open destination file (only write)

	if (source && dest) //check for error
	{
		while (size = fread_s(buf, 8192, 1, 8192, source))
			PHYSFS_write(dest, buf, 1, size); //copy source file to destination file

		fclose(source); //close source file
		PHYSFS_close(dest); //close destination file

		LOG("File System copied file [%s] to [%s]", full_path, destination, 'd');
		return true;
	}
	else
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');

	return false;
}

bool ModuleFileSystem::Copy(const char* full_path, const char* destination)
{
	char buf[8192];

	PHYSFS_file* source = PHYSFS_openRead(full_path); //open source file (only read)
	PHYSFS_file* dest = PHYSFS_openWrite(destination); //open destination file (only write)

	PHYSFS_sint32 size;
	if (source && dest) //check for error
	{
		while (size = (PHYSFS_sint32)PHYSFS_read(source, buf, 1, 8192))
			PHYSFS_write(dest, buf, 1, size); //copy from source to destination file

		PHYSFS_close(source); //close source file
		PHYSFS_close(dest); //close destination file

		LOG("File System copied file [%s] to [%s]", full_path, destination, 'd');
		return true;
	}
	else
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');

	return false;
}

// Add a new zip file or folder
bool ModuleFileSystem::AddPath(const char* path_or_zip)
{
	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		const char* error = PHYSFS_getLastError();
		LOG("File System error while adding a path or zip: %s\n", error, 'e');
		return false;
	}

	return true;
}

void ModuleFileSystem::NormalizePath(std::string& full_path) const
{
	for (std::string::iterator it = full_path.begin(); it != full_path.end(); ++it)
	{
		if (*it == '\\')
			*it = '/';
	}
}

void ModuleFileSystem::ToLower(std::string& full_path) const
{
	for (std::string::iterator it = full_path.begin(); it != full_path.end(); ++it)
		*it = tolower(*it);
}

void ModuleFileSystem::GetFolderContent(const char* directory, std:: vector<std::string>& file_list, std::vector<std::string>& dir_list) const
{
	char** rc = PHYSFS_enumerateFiles(directory); //get base directory
	char** i;

	std::string dir(directory);
	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsFolder(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

std::string ModuleFileSystem::GetFileName(const char* path) const
{
	const char* file_name = strrchr(path, 128);
	if (file_name == nullptr)
		file_name = (strrchr(path, '/') != nullptr) ? strrchr(path, '/') : "";

	if (file_name != "")
		file_name++;

	return file_name;
}

std::string ModuleFileSystem::GetExtension(const char* path) const
{
	char buffer[32] = "";
	const char* last_dot = strrchr(path, '.');
	if (last_dot != nullptr)
		strcpy_s(buffer, last_dot + 1);

	std::string extension(buffer);
	return extension;
}

bool ModuleFileSystem::CheckExtension(const char* path, std::vector<std::string> extensions) const
{
	std::string ext = GetExtension(path);
	for (std::string extension : extensions)
	{
		if (extension == ext)
			return true;
	}
	return false;
}

const char* ModuleFileSystem::GetReadPaths() const
{
	static char paths[512];
	paths[0] = '\0';

	char** path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++) //get current search path
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}
	return paths;
}

AssetNode ModuleFileSystem::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext)
{
	AssetNode nodes;

	if (Exists(directory)) //check if directory exists
	{
		// Create AssetNode and initialize
		std::string name = GetFileName(directory);
		if (name == "")
			name = directory;

		ResourceType type = GetType(directory);
		std::vector<std::string> empty_childs;
		size_t index = nodes.Add(directory, name, type, empty_childs);

		// Get Folder Content
		std::vector<std::string> file_list, dir_list;
		GetFolderContent(directory, file_list, dir_list);

		//Adding all child directories
		for (std::string dir : dir_list)
		{
			std::string str = directory + std::string("/") + dir;
			AssetNode child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);

			for (size_t i = 0, size = child.name.size(); i < size; ++i)
				nodes.Add(child.path[i], child.name[i], child.type[i], child.childs[i], nodes.name[index]);

			nodes.childs[index].insert(nodes.childs[index].end(), child.name.begin(), child.name.end());
		}

		//Adding all child files
		for (std::string file : file_list)
		{
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
				filter = CheckExtension(file.c_str(), *filter_ext); //check if file_ext == filter_ext
			else if (ignore_ext != nullptr)
				discard = CheckExtension(file.c_str(), *ignore_ext); //check if file_ext == ignore_ext

			if (filter == true && discard == false)
			{
				std::string str = directory + std::string("/") + file;
				AssetNode child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);
				nodes.childs[index] = child.name;
			}
		}
	}
	else
		LOG("Error retrieving files", 'e');

	return nodes;
}

ResourceType ModuleFileSystem::GetType(const char* path) const
{
	std::string extension = GetExtension(path);

	if		(extension == EXTENSION_FOLDER)		return ResourceType::FOLDER;
	else if (extension == EXTENSION_SCENE)		return ResourceType::SCENE;
	else if (extension == EXTENSION_PREFAB)		return ResourceType::PREFAB;
	else if (extension == EXTENSION_TEXTURE)	return ResourceType::TEXTURE;
	else if (extension == EXTENSION_MATERIAL)	return ResourceType::MATERIAL;
	else if (extension == EXTENSION_ANIMATION)	return ResourceType::ANIMATION;
	else if (extension == EXTENSION_TILEMAP)	return ResourceType::TILEMAP;
	else if (extension == EXTENSION_AUDIO)		return ResourceType::AUDIO;
	else if (extension == EXTENSION_SCRIPT)		return ResourceType::SCRIPT;
	else if (extension == EXTENSION_SHADER)		return ResourceType::SHADER;
	else										return ResourceType::UNKNOWN;
}
