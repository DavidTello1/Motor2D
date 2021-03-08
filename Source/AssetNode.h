#pragma once
#include "Globals.h"
#include <string>
#include <vector>

enum class ResourceType {
	FOLDER,
	SCENE,
	PREFAB,
	TEXTURE,
	MATERIAL,
	ANIMATION,
	TILEMAP,
	AUDIO,
	SCRIPT,
	SHADER,
	UNKNOWN
};

enum class AN_State {
	IDLE,
	SELECTED,
	DRAGGING,
	RENAME,
	CUT
};

struct AssetNode
{
	std::vector<std::string> name;
	std::vector<std::string> path;
	std::vector<ResourceType> type;
	std::vector<AN_State> state;
	std::vector<bool> open;

	std::vector<std::string> parent;
	std::vector <std::vector<std::string>> childs;

	//----------------------------
	size_t Add(std::string path_, std::string name_, ResourceType type_, std::vector<std::string> childs_, std::string parent_ = "", AN_State state_ = AN_State::IDLE, bool open_ = false)
	{
		name.push_back(name_);
		path.push_back(path_);
		type.push_back(type_);
		state.push_back(state_);
		open.push_back(open_);
		parent.push_back(parent_);
		childs.push_back(childs_);

		return name.size() - 1;
	}

	void Remove(size_t index)
	{
		name.erase(name.begin() + index);
		path.erase(path.begin() + index);
		type.erase(type.begin() + index);
		state.erase(state.begin() + index);
		open.erase(open.begin() + index);
		parent.erase(parent.begin() + index);
		childs.erase(childs.begin() + index);
	}

	void Clear()
	{
		name.clear();
		path.clear();
		type.clear();
		state.clear();
		open.clear();
		parent.clear();
		childs.clear();

		name.shrink_to_fit();
		path.shrink_to_fit();
		type.shrink_to_fit();
		state.shrink_to_fit();
		open.shrink_to_fit();
		parent.shrink_to_fit();	
		childs.shrink_to_fit();
	}
};