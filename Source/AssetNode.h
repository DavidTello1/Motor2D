#include "Globals.h"
#include <string>
#include <vector>

class GameObject;
//class Resource;

class AssetNode
{
public:
	enum class NodeType {
		NONE,
		FOLDER,
		SCENE,
		PREFAB,
		TEXTURE,
		SCRIPT
	};

	AssetNode() {};
	virtual ~AssetNode() {};

public:
	NodeType type = NodeType::NONE;
	std::string path = "null path";
	std::string name = "file_name";

	uint icon = 0;
	uint count = 0;

	bool rename = false;
	bool selected = false;
	bool cut = false;

	AssetNode* parent = nullptr;
	std::vector<AssetNode*> childs;
	//Resource* resource;
};