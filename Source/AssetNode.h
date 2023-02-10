#pragma once

#include "Globals.h"

#include <string>
#include <vector>

struct AssetNode
{
	enum class NodeState {
		DEFAULT = 0,
		RENAME,
		CUT
	};

	AssetNode(UID id, std::string name, AssetNode* parent = nullptr, int type = -1, uint num_parents = 0, uint preview_index = 0)
		:id(id), name(name), parent(parent), type(type), num_parents(num_parents), preview_index(preview_index) {}
	~AssetNode() {};

	bool is_rename = false; //***BITMASK
	bool is_selected = false; //***BITMASK
	bool is_open = false; //*** BITMASK
	bool is_hidden = true; //*** BITMASK

	UID id = 0;
	std::string name = "AssetNode";
	int type = -1;
	uint preview_index = 0;

	uint num_parents = 0;
	AssetNode* parent = nullptr;
	std::vector<AssetNode*> childs;
};
