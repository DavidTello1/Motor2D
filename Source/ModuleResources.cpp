#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

ModuleResources::ModuleResources(bool start_enabled) : Module("Resources", start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(Config* config)
{
	return true;
}

bool ModuleResources::Start(Config* config) 
{
	//LoadAllAssets();

	//ImportDefaults();

	return true;
}

bool ModuleResources::CleanUp()
{
	return true;
}

void ModuleResources::Save(Config* config) const
{
}

void ModuleResources::Load(Config* config)
{
}

bool ModuleResources::ImportFromPath(std::string path, UID uid)
{
	// Normalize Path
	std::string full_path = path;
	App->file_system->NormalizePath(full_path);

	// Get File Name
	full_path = ASSETS_FOLDER + App->file_system->GetFileName(full_path.c_str());
	App->file_system->NormalizePath(full_path);

	// Copy file to Assets Folder & Import
	if (App->file_system->CopyFromOutside(path.c_str(), full_path.c_str()) == true)
		return ImportResource(full_path.c_str(), uid);

	return false;
}

bool ModuleResources::ImportResource(const char* path, UID uid)
{
	switch (GetResourceType(path))
	{
	case ResourceType::FOLDER:		break;
	case ResourceType::SCENE:		break;
	case ResourceType::PREFAB:		break;
	case ResourceType::TEXTURE:		return textures.Create(path, uid);
	case ResourceType::MATERIAL:	break;
	case ResourceType::ANIMATION:	break;
	case ResourceType::TILEMAP:		break;
	case ResourceType::AUDIO:		break;
	case ResourceType::SCRIPT:		break;
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