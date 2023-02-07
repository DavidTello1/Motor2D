#include "ModuleResources.h"

#include "MessageBus.h"

#include "Application.h"
#include "ModuleFileSystem.h"

//#include "ResourceManager.h"
#include "ResourceFolder.h"
#include "ResourceTexture.h"

#include "mmgr/mmgr.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
	// --- Create Resource Managers
	folder_mgr		= new ResourceManager<ResourceFolder>();
	texture_mgr		= new ResourceManager<ResourceTexture>();
	//audio_mgr		= new ResourceManager<ResourceAudio>(MAX_AUDIOS);
	//textfile_mgr	= new ResourceManager<ResourceTextfile>(MAX_TEXTFILES);
	//script_mgr		= new ResourceManager<ResourceScript>(MAX_SCRIPTS);
	//shader_mgr		= new ResourceManager<ResourceShader>(MAX_SHADERS);
	//scene_mgr		= new ResourceManager<ResourceScene>(MAX_SCENES);
	//prefab_mgr		= new ResourceManager<ResourcePrefab>(MAX_PREFABS);
	//material_mgr	= new ResourceManager<ResourceMaterial>(MAX_MATERIALS);
	//animation_mgr	= new ResourceManager<ResourceAnimation>(MAX_ANIMATIONS);
	//tilemap_mgr		= new ResourceManager<ResourceTilemap>(MAX_TILEMAPS);

	// --- Create Resource Loaders
	folder_loader		= new LoaderFolder();
	texture_loader		= new LoaderTexture();
	//audio_loader		= new LoaderAudio();
	//textfile_loader		= new LoaderTextfile();
	//script_loader		= new LoaderScript();
	//shader_loader		= new LoaderShader();
	//scene_loader		= new LoaderScene();
	//prefab_loader		= new LoaderPrefab();
	//material_loader		= new LoaderMaterial();
	//animation_loader	= new LoaderAnimation();
	//tilemap_loader		= new LoaderTilemap();

	// --- Clean Library & Meta Files
	//CleanMeta();
	//CleanLibrary();

	return true;
}

bool ModuleResources::Start()
{
	//// Import & Load Engine Assets
	//ImportAllAssets("Settings/Icons/Assets");
	//for (size_t index = 0; index <= RESERVED_RESOURCES; ++index)
	//{
	//	if (textures.data.ids.size() <= index)
	//		break;
	//	textures.Load(index);
	//}

	// --- Import Assets
	ImportAllResources(ASSETS_FOLDER);
	App->message->Publish(new OnResourcesImported());

	return true;
}

bool ModuleResources::CleanUp()
{
	// --- Release Resource Managers
	RELEASE(folder_mgr);
	RELEASE(texture_mgr);
	//RELEASE(audio_mgr);
	//RELEASE(textfile_mgr);
	//RELEASE(script_mgr);
	//RELEASE(shader_mgr);
	//RELEASE(scene_mgr);
	//RELEASE(prefab_mgr);
	//RELEASE(material_mgr);
	//RELEASE(animation_mgr);
	//RELEASE(tilemap_mgr);

	// --- Release Resource Loaders
	RELEASE(folder_loader);
	RELEASE(texture_loader);
	//RELEASE(audio_loader);
	//RELEASE(textfile_loader);
	//RELEASE(script_loader);
	//RELEASE(shader_loader);
	//RELEASE(scene_loader);
	//RELEASE(prefab_loader);
	//RELEASE(material_loader);
	//RELEASE(animation_loader);
	//RELEASE(tilemap_loader);

	// --- Clean Library & Meta Files
	//CleanMeta();
	//CleanLibrary();

	return true;
}

// ---------------------------------------------
// --- IMPORTING RESOURCES ---
void ModuleResources::ImportAllResources(const char* directory)
{
	if (!App->filesystem->Exists(directory))
		return;

	std::vector<std::string> path_list = GetAllFilesExcept(directory, (int)ResourceType::META);

	for (int i = 0; i < path_list.size(); ++i)
	{
		ImportResource(path_list[i].c_str());
	}
}

UID ModuleResources::ImportResource(const char* path)
{
	if (!App->filesystem->Exists(path))
		return 0;

	const char* path_meta = std::string(path + std::string(".meta")).c_str();

	if (App->filesystem->Exists(path_meta))
		return CreateResourceFromMeta(path, path_meta);
	
	return CreateResource(path);
}

UID ModuleResources::CreateResourceFromMeta(const char* path, const char* path_meta)
{
	//char* buffer = nullptr;
	//App->filesystem->Load(path_meta, &buffer);
	//Config meta_data(buffer);

	//const char* name = meta_data.GetString("Name").c_str();
	//int type = meta_data.GetInt("Type");
	//UID id = meta_data.GetUID("UID");
	//const char* path_library = meta_data.GetString("LibraryFile").c_str();

	//if (!App->filesystem->Exists(path_library))
	//	return CreateResource(path, name, type, id, false);

	//if (App->filesystem->GetLastModTime(path_meta) != meta_data.GetInt("Date"))
	//	return CreateResource(path, name, type, id, true);

	//*** SOME OPTIONS CAN RETURN NOTHING
	return 0;
}

UID ModuleResources::CreateResource(const char* path, const char* resource_name, int resource_type, UID uid, bool save_meta)
{
	UID id = uid;
	if (id == 0)
		id = 0; // App->GenerateID();

	int type = resource_type;
	if (type == -1)
		type = (int)GetResourceType(App->filesystem->GetExtension(path));

	std::string name = resource_name;
	if (name == "")
		name = App->filesystem->GetFileName(path);

	const char* library_path = GetLibraryPath((ResourceType)type, name.c_str());

	// --- Add Resource
	Resource resource = { id, type, name.c_str(), path, library_path };
	if (AddResource(resource) == false)
	{
		LOG("Error importing resource: %s", path, 'e');
		return 0;
	}

	//if (save_meta)
	//	SaveMeta(resource);

	return id;
}

// ---------------------------------------------
bool ModuleResources::AddResource(Resource& resource)
{
	int index = -1;
	bool ret = false;

	switch ((ResourceType)resource.type)
	{
	case ResourceType::FOLDER:		index = folder_mgr->GetSize();		ret = folder_mgr->Add(index, (ResourceFolder&)resource);	break;
	case ResourceType::TEXTURE:		index = texture_mgr->GetSize();		ret = texture_mgr->Add(index, (ResourceTexture&)resource);	break;
	//case ResourceType::AUDIO:		index = audio_mgr->GetSize();		ret = audio_mgr->Add(index); break;
	//case ResourceType::TEXTFILE:	index = textfile_mgr->GetSize();	ret = textfile_mgr->Add(index); break;
	//case ResourceType::SCRIPT:		index = script_mgr->GetSize();		ret = script_mgr->Add(index); break;
	//case ResourceType::SHADER:		index = shader_mgr->GetSize();		ret = shader_mgr->Add(index); break;
	//case ResourceType::SCENE:		index = scene_mgr->GetSize();		ret = scene_mgr->Add(index); break;
	//case ResourceType::PREFAB:		index = prefab_mgr->GetSize();		ret = prefab_mgr->Add(index); break;
	//case ResourceType::MATERIAL:	index = material_mgr->GetSize();	ret = material_mgr->Add(index); break;
	//case ResourceType::ANIMATION:	index = animation_mgr->GetSize();	ret = animation_mgr->Add(index); break;
	//case ResourceType::TILEMAP:		index = tilemap_mgr->GetSize();		ret = tilemap_mgr->Add(index); break;

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	//if (ret == false)
	//{
	//	LOG("Error Adding Resource: invalid type - %d", resource.type, 'e');
	//	return false;
	//}

	// Add ResourceHandle to list
	resources[num_resources] = { resource.id, resource.path_assets, resource.type, index };
	num_resources++;

	// --- Create Meta
	//CreateMeta(resource);

	return true;
}

bool ModuleResources::RemoveResource(UID id)
{
	int item = Find(id);
	if (item == -1) {
		return false;
	}

	ResourceType type = (ResourceType)resources[item].type;
	int index = resources[item].index;
	bool ret = false;

	switch (type)
	{
	case ResourceType::FOLDER:		ret = folder_mgr->Remove(index);
	case ResourceType::TEXTURE:		ret = texture_mgr->Remove(index);
	//case ResourceType::AUDIO:		ret = audio_mgr->Remove(index);
	//case ResourceType::TEXTFILE:	ret = textfile_mgr->Remove(index);
	//case ResourceType::SCRIPT:		ret = script_mgr->Remove(index);
	//case ResourceType::SHADER:		ret = shader_mgr->Remove(index);
	//case ResourceType::SCENE:		ret = scene_mgr->Remove(index);
	//case ResourceType::PREFAB:		ret = prefab_mgr->Remove(index);
	//case ResourceType::MATERIAL:	ret = material_mgr->Remove(index);
	//case ResourceType::ANIMATION:	ret = animation_mgr->Remove(index);
	//case ResourceType::TILEMAP:		ret = tilemap_mgr->Remove(index);

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	if (ret == false)
	{
		LOG("Error Removing Resource: invalid type - '%d'", (int)type, 'e');
		return false;
	}

	// --- Delete Meta
	//DeleteMeta(resource);

	return true;
}

bool ModuleResources::LoadResource(UID id)
{
	int item = Find(id);
	if (item == -1) {
		return false;
	}

	ResourceType type = (ResourceType)resources[item].type;
	int index = resources[item].index;

	switch (type)
	{
	case ResourceType::FOLDER:		return folder_loader->Load(&folder_mgr->GetResource(index));
	case ResourceType::TEXTURE:		return texture_loader->Load(&texture_mgr->GetResource(index));
	//case ResourceType::AUDIO:		return audio_loader->Load(index);
	//case ResourceType::TEXTFILE:	return textfile_loader->Load(index);
	//case ResourceType::SCRIPT:		return script_loader->Load(index);
	//case ResourceType::SHADER:		return shader_loader->Load(index);
	//case ResourceType::SCENE:		return scene_loader->Load(index);
	//case ResourceType::PREFAB:		return prefab_loader->Load(index);
	//case ResourceType::MATERIAL:	return material_loader->Load(index);
	//case ResourceType::ANIMATION:	return animation_loader->Load(index);
	//case ResourceType::TILEMAP:		return tilemap_loader->Load(index);

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	LOG("Error Loading Resource: invalid type - '%d'", (int)type, 'e');
	return false;
}

bool ModuleResources::UnloadResource(UID id)
{
	int item = Find(id);
	if (item == -1) {
		return false;
	}

	ResourceType type = (ResourceType)resources[item].type;
	int index = resources[item].index;

	switch (type)
	{
	case ResourceType::FOLDER:		return folder_loader->Unload(&folder_mgr->GetResource(index));
	case ResourceType::TEXTURE:		return texture_loader->Unload(&texture_mgr->GetResource(index));
	//case ResourceType::AUDIO:		return audio_loader->Unload(index);
	//case ResourceType::TEXTFILE:	return textfile_loader->Unload(index);
	//case ResourceType::SCRIPT:		return script_loader->Unload(index);
	//case ResourceType::SHADER:		return shader_loader->Unload(index);
	//case ResourceType::SCENE:		return scene_loader->Unload(index);
	//case ResourceType::PREFAB:		return prefab_loader->Unload(index);
	//case ResourceType::MATERIAL:	return material_loader->Unload(index);
	//case ResourceType::ANIMATION:	return animation_loader->Unload(index);
	//case ResourceType::TILEMAP:		return tilemap_loader->Unload(index);

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	LOG("Error Unloading Resource: invalid type - '%d'", (int)type, 'e');
	return false;
}

Resource* ModuleResources::GetResource(UID id)
{
	int item = Find(id);
	if (item == -1) {
		return nullptr;
	}

	ResourceType type = (ResourceType)resources[item].type;
	int index = resources[item].index;

	switch (type)
	{
	case ResourceType::FOLDER:		return &folder_mgr->GetResource(index);
	case ResourceType::TEXTURE:		return &texture_mgr->GetResource(index);
	//case ResourceType::AUDIO:		return &audio_mgr->GetResource(index);
	//case ResourceType::TEXTFILE:	return &textfile_mgr->GetResource(index);
	//case ResourceType::SCRIPT:		return &script_mgr->GetResource(index);
	//case ResourceType::SHADER:		return &shader_mgr->GetResource(index);
	//case ResourceType::SCENE:		return &scene_mgr->GetResource(index);
	//case ResourceType::PREFAB:		return &prefab_mgr->GetResource(index);
	//case ResourceType::MATERIAL:	return &material_mgr->GetResource(index);
	//case ResourceType::ANIMATION:	return &animation_mgr->GetResource(index);
	//case ResourceType::TILEMAP:		return &tilemap_mgr->GetResource(index);

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	LOG("Error Getting Resource: invalid type - '%d'", (int)type, 'e');
	return nullptr;
}

const Resource* ModuleResources::GetResource(UID id) const
{
	int item = Find(id);
	if (item == -1) {
		return nullptr;
	}

	ResourceType type = (ResourceType)resources[item].type;
	int index = resources[item].index;

	switch (type)
	{
	case ResourceType::FOLDER:		return &folder_mgr->GetResource(index);
	case ResourceType::TEXTURE:		return &texture_mgr->GetResource(index);
	//case ResourceType::AUDIO:		return &audio_mgr->GetResource(index);
	//case ResourceType::TEXTFILE:	return &textfile_mgr->GetResource(index);
	//case ResourceType::SCRIPT:		return &script_mgr->GetResource(index);
	//case ResourceType::SHADER:		return &shader_mgr->GetResource(index);
	//case ResourceType::SCENE:		return &scene_mgr->GetResource(index);
	//case ResourceType::PREFAB:		return &prefab_mgr->GetResource(index);
	//case ResourceType::MATERIAL:	return &material_mgr->GetResource(index);
	//case ResourceType::ANIMATION:	return &animation_mgr->GetResource(index);
	//case ResourceType::TILEMAP:		return &tilemap_mgr->GetResource(index);

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	LOG("Error Getting Resource: invalid type - '%d'", (int)type, 'e');
	return nullptr;
}

bool ModuleResources::SaveResource(UID id, Resource& resource)
{
	int item = Find(id);
	if (item == -1) {
		return false;
	}

	ResourceType type = (ResourceType)resources[item].type;
	int index = resources[item].index;
	bool ret = false;

	switch (type)
	{
	case ResourceType::FOLDER:		ret = folder_mgr->SetResource(index, (ResourceFolder&)resource);
	case ResourceType::TEXTURE:		ret = texture_mgr->SetResource(index, (ResourceTexture&)resource);
	//case ResourceType::AUDIO:		ret = audio_mgr->SetResource(index, (ResourceAudio&)resource);
	//case ResourceType::TEXTFILE:	ret = textfile_mgr->SetResource(index, (ResourceTextfile)resource);
	//case ResourceType::SCRIPT:		ret = script_mgr->SetResource(index, (ResourceScript)resource);
	//case ResourceType::SHADER:		ret = shader_mgr->SetResource(index, (ResourceShader)resource);
	//case ResourceType::SCENE:		ret = scene_mgr->SetResource(index, (ResourceScene)resource);
	//case ResourceType::PREFAB:		ret = prefab_mgr->SetResource(index, (ResourcePrefab)resource);
	//case ResourceType::MATERIAL:	ret = material_mgr->SetResource(index, (ResourceMaterial)resource);
	//case ResourceType::ANIMATION:	ret = animation_mgr->SetResource(index, (ResourceAnimation)resource);
	//case ResourceType::TILEMAP:		ret = tilemap_mgr->SetResource(index, (ResourceTilemap)resource);

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	if (ret == false)
	{
		LOG("Error Saving Resource: invalid type - '%d'", (int)type, 'e');
		return false;
	}

	// --- Save Meta File
	//SaveMeta(resource);

	return true;
}

int ModuleResources::GetReferenceCount(UID id) const
{
	//int item = Find(id);
	//if (item == -1) {
	//	return -1;
	//}

	//ResourceType type = (ResourceType)resources[item].type;
	//int index = resources[item].index;

	//switch (type)
	//{
	////case ResourceType::FOLDER:		return folder_mgr->GetReferenceCount(index);
	////case ResourceType::TEXTURE:		return texture_mgr->GetReferenceCount(index);
	////case ResourceType::AUDIO:		return audio_mgr->GetReferenceCount(index);
	////case ResourceType::TEXTFILE:	return textfile_mgr->GetReferenceCount(index);
	////case ResourceType::SCRIPT:		return script_mgr->GetReferenceCount(index);
	////case ResourceType::SHADER:		return shader_mgr->GetReferenceCount(index);
	////case ResourceType::SCENE:		return scene_mgr->GetReferenceCount(index);
	////case ResourceType::PREFAB:		return prefab_mgr->GetReferenceCount(index);
	////case ResourceType::MATERIAL:	return material_mgr->GetReferenceCount(index);
	////case ResourceType::ANIMATION:	return animation_mgr->GetReferenceCount(index);
	////case ResourceType::TILEMAP:		return tilemap_mgr->GetReferenceCount(index);

	//case ResourceType::COUNT:
	//	break;
	//default:
	//	break;
	//}

	return -1;
}

std::vector<std::string> ModuleResources::GetAllFiles(const char* directory)
{
	if (!App->filesystem->Exists(directory))
	{
		LOG("Error retrieving files: invalid path - '%s'", directory, 'e');
		return std::vector<std::string>();
	}

	std::vector<std::string> all_files, directories, files;
	App->filesystem->GetFolderContent(directory, files, directories);

	// --- Child Directories
	for (size_t i = 0; i < directories.size(); ++i)
	{
		std::string child_path = directory + std::string("/") + directories[i];
		all_files.insert(all_files.end(), child_path);

		std::vector<std::string> children = GetAllFiles(child_path.c_str()); // (Recursive) Get all files inside child directory
		all_files.insert(all_files.end(), children.begin(), children.end());
	}

	// --- Child Files
	for (size_t i = 0; i < files.size(); ++i)
	{
		std::string child_path = directory + std::string("/") + files[i];
		all_files.insert(all_files.end(), child_path);
	}

	return all_files;
}

std::vector<std::string> ModuleResources::GetAllFilesOfType(const char* directory, int type)
{
	if (!App->filesystem->Exists(directory))
	{
		LOG("Error retrieving files: invalid path - '%s'", directory, 'e');
		return std::vector<std::string>();
	}
	if (type < 0 || type >= (int)ResourceType::COUNT)
	{
		LOG("Error retrieving files: invalid type - '%d'", type, 'e');
		return std::vector<std::string>();
	}

	std::vector<std::string> filter_files;
	std::vector<std::string> all_files = GetAllFiles(directory);

	for (size_t i = 0; i < all_files.size(); ++i)
	{
		std::string extension = App->filesystem->GetExtension(all_files[i].c_str());
		if (type == (int)GetResourceType(extension))
			filter_files.push_back(all_files[i]);
	}

	return filter_files;
}

std::vector<std::string> ModuleResources::GetAllFilesExcept(const char* directory, int type)
{
	if (!App->filesystem->Exists(directory))
	{
		LOG("Error retrieving files: invalid path - '%s'", directory, 'e');
		return std::vector<std::string>();
	}
	if (type < 0 || type >= (int)ResourceType::COUNT)
	{
		LOG("Error retrieving files: invalid type - '%d'", type, 'e');
		return std::vector<std::string>();
	}

	std::vector<std::string> filter_files;
	std::vector<std::string> all_files = GetAllFiles(directory);

	for (size_t i = 0; i < all_files.size(); ++i)
	{
		std::string extension = App->filesystem->GetExtension(all_files[i].c_str());
		if (type == (int)GetResourceType(extension))
			continue;
		
		filter_files.push_back(all_files[i]);
	}

	return filter_files;
}

//------------------------------
int ModuleResources::Find(UID id) const
{
	for (uint i = 0; i < num_resources; ++i)
	{
		if (id == resources[i].id)
			return i;
	}

	return -1;
}

const char* ModuleResources::GetLibraryPath(ResourceType type, const char* name) const
{
	std::string library_folder = "";
	switch (type)
	{
	case ResourceType::FOLDER:		break;
	case ResourceType::TEXTURE:		library_folder = LIBRARY_TEXTURE_FOLDER + std::string(name);
	//case ResourceType::AUDIO:		library_folder = LIBRARY_AUDIO_FOLDER + name;
	//case ResourceType::TEXTFILE:	library_folder = LIBRARY_TEXTFILE_FOLDER + name;
	//case ResourceType::SCRIPT:		library_folder = LIBRARY_SCRIPT_FOLDER + name;
	//case ResourceType::SHADER:		library_folder = LIBRARY_SHADER_FOLDER + name;
	//case ResourceType::SCENE:		library_folder = LIBRARY_SCENE_FOLDER + name;
	//case ResourceType::PREFAB:		library_folder = LIBRARY_PREFAB_FOLDER + name;
	//case ResourceType::MATERIAL:	library_folder = LIBRARY_MATERIAL_FOLDER + name;
	//case ResourceType::ANIMATION:	library_folder = LIBRARY_ANIMATION_FOLDER + name;
	//case ResourceType::TILEMAP:		library_folder = LIBRARY_TILEMAP_FOLDER + name;

	case ResourceType::COUNT:
		break;
	default:
		break;
	}

	return library_folder.c_str();
}


ResourceType ModuleResources::GetResourceType(std::string extension) const
{	
	if (extension == "")
		return ResourceType::FOLDER;

	return ResourceType::COUNT;
}
