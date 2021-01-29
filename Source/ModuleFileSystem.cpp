#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
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
		LIBRARY_MATERIAL_FOLDER,
		LIBRARY_SCENE_FOLDER,
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
