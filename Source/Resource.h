#pragma once
#include "Globals.h"

enum class ResourceType {
	// Base Resources
	META,
	FOLDER,
	TEXTURE,
	AUDIO,
	TEXTFILE,
	SCRIPT,
	SHADER,

	// Engine Resources
	SCENE,
	PREFAB,
	MATERIAL,
	ANIMATION,
	TILEMAP,

	COUNT // used for counting number of types
};

struct Resource {
	Resource() {};
	Resource(UID id, int type, const char* name, const char* path_assets, const char* path_library) : id(id), type(type), name(name), path_assets(path_assets), path_library(path_library) {};
	virtual ~Resource() {};

	UID id = 0;
	int type = -1;
	const char* name = "";
	const char* path_assets = "";
	const char* path_library = "";
	int times_loaded = 0;
};
