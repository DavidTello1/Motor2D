#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Config.h"
#include "AssetNode.h"
#include "Icons.h"

#include "Imgui/imgui.h"
#include <windows.h>

#include "mmgr/mmgr.h"

ModuleResources::ModuleResources(bool start_enabled) : Module("Resources", start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(Config* config)
{
	//CleanMeta();
	//CleanLibrary();

	return true;
}

bool ModuleResources::Start(Config* config) 
{
	// Import & Load Engine Assets
	ImportAllAssets("Settings/Icons/Assets");
	for (size_t index = 0; index < RESERVED_RESOURCES; ++index)
	{
		if (textures.data.ids.size() <= index)
			break;
		textures.Load(index);
	}

	// Load Assets
	ImportAllAssets("Assets");

	return true;
}

bool ModuleResources::CleanUp()
{
	//CleanMeta();
	//CleanLibrary();

	return true;
}

void ModuleResources::Save(Config* config) const
{
}

void ModuleResources::Load(Config* config)
{
}

bool ModuleResources::ImportFromExplorer(std::string path, UID uid)
{
	// Normalize Path
	std::string full_path = path;
	App->file_system->NormalizePath(full_path);

	// Get File Name
	full_path = ASSETS_FOLDER + App->file_system->GetFileName(full_path.c_str());
	App->file_system->NormalizePath(full_path);

	// Copy file to Assets Folder & Import
	if (App->file_system->CopyFromOutside(path.c_str(), full_path.c_str()) == true)
		return ImportFromAssets(full_path.c_str(), uid);

	return false;
}

bool ModuleResources::ImportFromAssets(const char* path, UID uid, bool save_meta)
{
	bool ret = false;
	ResourceData data;
	switch (GetResourceType(path))
	{
	case ResourceType::FOLDER:		ret = true; save_meta = false; break;
	case ResourceType::SCENE:		break;
	case ResourceType::PREFAB:		break;
	case ResourceType::TEXTURE:		ret = textures.Create(path, uid); data = textures.data; break;
	case ResourceType::MATERIAL:	break;
	case ResourceType::ANIMATION:	break;
	case ResourceType::TILEMAP:		break;
	case ResourceType::AUDIO:		break;
	case ResourceType::SCRIPT:		break;
	case ResourceType::SHADER:		break;
	}

	if (ret == false)
	{
		LOG("Importing of [%s] FAILED", path, 'e');
	}
	else if (save_meta == true)
	{
		SaveMeta(textures.data, data.ids.size() - 1);
	}
	return ret;
}

AssetNode ModuleResources::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext)
{
	AssetNode nodes;

	if (App->file_system->Exists(directory)) //check if directory exists
	{
		// Create AssetNode and initialize
		ResourceType type = GetResourceType(directory);
		std::vector<std::string> empty_childs;

		std::string parent = directory;
		parent = parent.substr(0, parent.find_last_of("/"));
		parent = parent.substr(parent.find_last_of("/") + 1);

		std::string name = App->file_system->GetFileName(directory);
		if (name == "")
		{
			name = directory;
			parent = "";
		}
		size_t index = nodes.Add(directory, name, type, empty_childs, parent);

		// Get Folder Content
		std::vector<std::string> file_list, dir_list;
		App->file_system->GetFolderContent(directory, file_list, dir_list);

		//Adding all child directories
		for (std::string dir : dir_list)
		{
			std::string str = directory + std::string("/") + dir;
			AssetNode child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);

			for (size_t i = 0, size = child.name.size(); i < size; ++i)
			{
				nodes.Add(child.path[i], child.name[i], child.type[i], child.childs[i], child.parent[i]);

				if (child.parent[i] == nodes.name[index])
					nodes.childs[index].push_back(child.name[i]);
			}
		}

		//Adding all child files
		for (std::string file : file_list)
		{
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
				filter = App->file_system->CheckExtension(file.c_str(), *filter_ext); //check if file_ext == filter_ext
			else if (ignore_ext != nullptr)
				discard = App->file_system->CheckExtension(file.c_str(), *ignore_ext); //check if file_ext == ignore_ext

			if (filter == true && discard == false)
			{
				std::string str = directory + std::string("/") + file;
				AssetNode child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);

				for (size_t i = 0, size = child.name.size(); i < size; ++i)
				{
					nodes.Add(child.path[i], child.name[i], child.type[i], child.childs[i], child.parent[i]);

					if (child.parent[i] == nodes.name[index])
						nodes.childs[index].push_back(child.name[i]);
				}
			}
		}
	}
	else
		LOG("Error retrieving files", 'e');

	return nodes;
}

ResourceType ModuleResources::GetType(const char* path) const
{
	std::string extension = App->file_system->GetExtension(path);

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

void ModuleResources::SaveMeta(ResourceData data, size_t index) const
{
	// Create Config file
	Config config;
	config.AddUID("ID", data.ids[index]);
	config.AddString("AssetsFile", data.files_assets[index].c_str());
	config.AddString("LibraryFile", data.files_library[index].c_str());
	config.AddDouble("Date", App->file_system->GetLastModTime(data.files_assets[index].c_str()));

	// Save as .meta file
	char* buffer = nullptr;
	uint size = config.Save(&buffer, "meta file");
	if (size > 0)
	{
		std::string path = data.files_assets[index] + ".meta";
		App->file_system->Save(path.c_str(), buffer, size);
	}
}

ResourceType ModuleResources::GetResourceType(const char* path) //***put inside ImportResource() to avoid unnecessary branching
{
	std::string extension = App->file_system->GetExtension(path);
	App->file_system->ToLower(extension);

	if (extension == "")								
		return ResourceType::FOLDER;

	else if (extension == "png" || extension == "jpg")	
		return ResourceType::TEXTURE;

	else
	{
		LOG("%s format not supported from %s", extension.c_str(), path, 'e');
		return ResourceType::UNKNOWN;
	}
}

size_t ModuleResources::GetResourceIndex(ResourceType type, const char* path)
{
	switch (type)
	{
	case ResourceType::FOLDER:		break;
	case ResourceType::SCENE:		break;
	case ResourceType::PREFAB:		break;
	case ResourceType::TEXTURE:		return textures.data.GetIndexFromPath(path);
	case ResourceType::MATERIAL:	break;
	case ResourceType::ANIMATION:	break;
	case ResourceType::TILEMAP:		break;
	case ResourceType::AUDIO:		break;
	case ResourceType::SCRIPT:		break;
	case ResourceType::SHADER:		break;
	default:						break;
	}
	return 0;
}

void ModuleResources::ImportAllAssets(const char* path)
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");

	// Get All Files
	AssetNode assets = GetAllFiles(path, nullptr, &ignore_ext);

	for (size_t index = 0, size = assets.name.size(); index < size; ++index)
	{
		std::string meta_file = assets.path[index] + std::string(".meta");

		// Check if .meta file exists
		if (App->file_system->Exists(meta_file.c_str()))
		{
			// Load .meta
			char* buffer = nullptr;
			App->file_system->Load(meta_file.c_str(), &buffer);

			// Get UID from .meta
			Config meta_data(buffer);
			UID uid = meta_data.GetUID("ID");

			// Check if Library File exists
			if (App->file_system->Exists(meta_data.GetString("LibraryFile")))
			{
				// Check if Last Modification Time has changed
				if (App->file_system->GetLastModTime(assets.path[index].c_str()) != meta_data.GetInt("Date"))
					ImportFromAssets(assets.path[index].c_str(), uid); // Import File and Save .meta
			}
			else
				ImportFromAssets(assets.path[index].c_str(), uid, false); // Import File and NOT Save .meta

			RELEASE_ARRAY(buffer);
		}
		else
			ImportFromAssets(assets.path[index].c_str()); // Import File and Save .meta
	}
}