#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "AssetNode.h"

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
	switch (GetResourceType(path))
	{
	case ResourceType::FOLDER:		break;
	case ResourceType::SCENE:		break;
	case ResourceType::PREFAB:		break;
	case ResourceType::TEXTURE:		return textures.Create(path, uid, save_meta);
	case ResourceType::MATERIAL:	break;
	case ResourceType::ANIMATION:	break;
	case ResourceType::TILEMAP:		break;
	case ResourceType::AUDIO:		break;
	case ResourceType::SCRIPT:		break;
	case ResourceType::SHADER:		break;
	}

	LOG("Importing of [%s] FAILED", path, 'e');
	return false;
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
}

void ModuleResources::ImportAllAssets(const char* path)
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");

	// Get All Files
	AssetNode assets = App->file_system->GetAllFiles(path, nullptr, &ignore_ext);

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