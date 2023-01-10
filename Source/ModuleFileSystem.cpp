#include "ModuleFileSystem.h"

#include "SDL/include/SDL.h"
#include "PhysFS/include/physfs.h"

#ifndef _WIN64
#	pragma comment( lib, "PhysFS/libx86/physfs.lib" )
#else
#	pragma comment( lib, "PhysFS/libx64/physfs.lib" )
#endif

//#include <fstream>
//#include <filesystem>

#include "mmgr/mmgr.h"

ModuleFileSystem::ModuleFileSystem(bool start_enabled, const char* game_path) : Module(start_enabled)
{
	// --- Init PhysFS (before main Init so other modules can use it)
	LOG("Loading PhysFS", 'i');
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// --- Workaround VS string Directory Mess
	AddPath(".");
	if (0 && game_path != nullptr)
		AddPath(game_path);

	// --- Get Search Paths
	LOG("FileSystem Operations base is [%s] plus:", GetBasePath(), 'i');
	LOG(GetReadPaths());

	// --- Create Write Directory
	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError(), 'e');

	// --- Create Standard Paths
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
		LIBRARY_SCRIPT_FOLDER,
		LIBRARY_SHADER_FOLDER
	};

	for (uint i = 0; i < sizeof(dirs) / sizeof(const char*); ++i)
	{
		if (!Exists(dirs[i]))
			CreateFolder(dirs[i]);
	}
}

ModuleFileSystem::~ModuleFileSystem()
{
	// --- Close PhysFS
	PHYSFS_deinit();
}

bool ModuleFileSystem::Init()
{
	LOG("Loading File System", 'i');

	// --- Get Write Directory
	char* write_path = SDL_GetPrefPath(TITLE, TITLE);

	//// Turn this on while in game mode
	//if(PHYSFS_setWriteDir(write_path) == 0)
		//LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	// --- Release Write Directory
	SDL_free(write_path);

	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return true;
}

// ---------------------------------------------
// --- MAIN FUNCTIONS ---

// --- Read a whole file and put it in a new buffer ---
uint ModuleFileSystem::Load(const char* file, char** buffer) const
{
	// --- Open File (Read Only)
	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file == nullptr) // Error
	{
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError(), 'e');
		return 0;
	}

	// --- Get File Size
	uint read = 0;
	PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);
	if (size <= 0)
	{
		// --- Read File to Buffer & Get Size
		*buffer = new char[size];
		read = (uint)PHYSFS_read(fs_file, *buffer, 1, size);

		if (read != size) // Error
		{
			LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError(), 'e');
			RELEASE(buffer);
			read = 0;
		}
	}

	// --- Close File
	if (PHYSFS_close(fs_file) == 0)
		LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError(), 'e');

	// --- Return File Size
	return read;
}

// --- Save a whole buffer to disk ---
uint ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	// --- Open File (Append or Write Only)
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file == nullptr) // Error
	{
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError(), 'e');
		return 0;
	}

	// --- Write to Buffer & Get Size
	uint written = (uint)PHYSFS_writeBytes(fs_file, (const void*)buffer, size);

	if (written != size) // Error
	{
		LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError(), 'e');
		written = 0;
	}
	if (append == true) // Append
	{
		LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file, 'i');
	}
	else if (PHYSFS_exists(file) == 0) // New File
	{
		LOG("New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size, 'i');
	}

	// --- Close File
	if (PHYSFS_close(fs_file) == 0) //close file
		LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError(), 'e');


	return written;
}

bool ModuleFileSystem::Remove(const char* file)
{
	// --- Check if File is Valid
	if (file == nullptr)
	{
		LOG("File System error while trying to delete [%s]: file not found", file, 'e');
		return false;
	}

	// --- Delete File
	if (PHYSFS_delete(file) == 0) // Error
	{
		LOG("File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError(), 'e');
		return false;
	}

	// --- Return
	LOG("File deleted: [%s]", file, 'i');
	return true;
}

bool ModuleFileSystem::CopyFromOutside(const char* full_path, const char* destination) // Only place we acces non virtual filesystem
{
	// --- Open Source File
	FILE* source = nullptr;
	fopen_s(&source, full_path, "rb");

	// --- Open Destination File (Write Only)
	PHYSFS_file* dest = PHYSFS_openWrite(destination);

	if (!source && !dest) // Error
	{
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');
		return false;
	}

	// --- Copy Source to Destination File
	char buf[8192];
	PHYSFS_uint32 size;
	while (size = fread_s(buf, 8192, 1, 8192, source))
		PHYSFS_write(dest, buf, 1, size);

	// --- Close Source & Destionation Files
	fclose(source);
	PHYSFS_close(dest);

	// --- Return
	LOG("File System copied file [%s] to [%s]", full_path, destination, 'i');
	return true;
}

bool ModuleFileSystem::Copy(const char* full_path, const char* destination)
{
	// --- Open Source File (Read Only)
	PHYSFS_file* source = PHYSFS_openRead(full_path);

	// --- Open Destination File (Write Only)
	PHYSFS_file* dest = PHYSFS_openWrite(destination);

	PHYSFS_sint32 size;
	if (!source && !dest) // Error
	{
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');
		return false;
	}

	// --- Copy from Source to Destination File
	char buf[8192];
	while (size = (PHYSFS_sint32)PHYSFS_read(source, buf, 1, 8192))
		PHYSFS_write(dest, buf, 1, size);

	// --- Close Source & Destination Files
	PHYSFS_close(source);
	PHYSFS_close(dest);

	// --- Return
	LOG("File System copied file [%s] to [%s]", full_path, destination, 'i');
	return true;
}

// ---------------------------------------------
// --- UTILITY FUNCTIONS ---

// --- Add a new zip file or folder ---
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

bool ModuleFileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

uint64_t ModuleFileSystem::GetLastModTime(const char* path)
{
	return PHYSFS_getLastModTime(path);
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
	{
		*it = tolower(*it);
	}
}

// ---------------------------------------------
// --- FOLDERS ---
bool ModuleFileSystem::IsFolder(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

void ModuleFileSystem::CreateFolder(const char* directory)
{
	PHYSFS_mkdir(directory);
}

void ModuleFileSystem::GetFolderContent(const char* directory, std:: vector<std::string>& file_list, std::vector<std::string>& dir_list) const
{
	// --- Get Base Directory
	char** root = PHYSFS_enumerateFiles(directory);

	// --- Get Content Inside
	for (char** file = root; *file != nullptr; file++)
	{
		// --- Get Child Name
		std::string str = std::string(directory) + std::string("/") + std::string(*file);

		// --- If Folder Add to Directory List
		if (IsFolder(str.c_str()))
		{
			dir_list.push_back(*file);
			continue;
		}

		// --- Add to Files List
		file_list.push_back(*file);
	}

	// --- Release Base Directory
	PHYSFS_freeList(root);
}

// ---------------------------------------------
// --- EXTENSIONS ---
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

	return std::string(buffer);
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

// ---------------------------------------------
// --- PATHS ---
const char* ModuleFileSystem::GetBasePath() const
{
	return PHYSFS_getBaseDir();
}

const char* ModuleFileSystem::GetWritePath() const
{
	return PHYSFS_getWriteDir();
}

const char* ModuleFileSystem::GetReadPaths() const
{
	static char paths[512];
	paths[0] = '\0';

	// --- Get Current Search Path
	for (char** path = PHYSFS_getSearchPath(); *path != nullptr; path++)
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}
	return paths;
}

const char* ModuleFileSystem::GetUserPath() const
{
	return PHYSFS_getUserDir();
}
