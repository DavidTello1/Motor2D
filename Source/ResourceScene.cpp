#include "ResourceScene.h"

#include "mmgr/mmgr.h"

void ResourceScene::Add(const char* assets_path, const char* name, const char* library_folder, const char* extension, UID id)
{
	data.Add(assets_path, name, library_folder, extension, id);
}

bool ResourceScene::Create(const char* path, const char* name, UID uid)
{
	Add(path, name, LIBRARY_TEXTURE_FOLDER, EXTENSION_SCENE, uid); // Initialize Data
	Save(data.names.size() - 1); // Save Library File

	return true;
}

void ResourceScene::Remove(size_t index)
{
	UnLoad(index);
	data.Remove(index);

	// remove gameobjects
}

bool ResourceScene::Save(size_t index) const
{
	return true;
}

bool ResourceScene::Load(size_t index)
{
	return true;
}

void ResourceScene::UnLoad(size_t index)
{
}
