#pragma once
#include "ResourceData.h"
#include "GameObject.h"

struct ResourceScene
{
	ResourceData data;
	GameObjects objects;

	//--------------------------
	void Add(const char* assets_path, const char* name, const char* library_folder, const char* extension, UID id = 0);

	bool Create(const char* path, const char* name, UID uid = 0);
	void Remove(size_t index);

	bool Save() const;
	bool Load(size_t index);
	void UnLoad(size_t index);
};