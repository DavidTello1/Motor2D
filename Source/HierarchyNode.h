#pragma once
#include "Imgui/imgui.h"

#include "Icons.h"
#include <string>
#include <vector>

class GameObject;
//class ResourceScene;
//class Prefab;

class HierarchyNode
{
public:
	enum class NodeType {
		NONE,
		FOLDER,
		GAMEOBJECT,
		SCENE,
		PREFAB
	};

	HierarchyNode() {};
	virtual ~HierarchyNode() {};

public:
	NodeType type = NodeType::NONE;
	std::string name = "node";
	std::string icon = "";

	HierarchyNode* parent = nullptr;
	std::vector<HierarchyNode*> childs;

	int pos = -1;
	int indent = -1;

	bool rename = false;
	bool selected = false;
	bool is_open = true;
	bool is_shown = true;

	ImVec4 color = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
};

//--- FOLDER ---
class NodeFolder : public HierarchyNode
{
public:
	NodeFolder(HierarchyNode* Parent = nullptr, std::string Name = "Folder") {
		type = NodeType::FOLDER;
		name = Name;
		icon = ICON_FOLDER;
		parent = Parent;
	}

	virtual ~NodeFolder() {};
};

//--- GAMEOBJECT ---
class NodeGameObject : public HierarchyNode
{
public:
	NodeGameObject(GameObject* obj, HierarchyNode* Parent = nullptr, std::string Name = "GameObject") {
		type = NodeType::GAMEOBJECT;
		name = Name;
		icon = ICON_GAMEOBJECT;
		parent = Parent;
		gameobject = obj;
	}

	virtual ~NodeGameObject() {};

public:
	GameObject* gameobject = nullptr;
};

//--- SCENE ---
class NodeScene : public HierarchyNode
{
public:
	NodeScene(/*ResourceScene* Scene,*/ std::string Name = "Scene") {
		type = NodeType::SCENE;
		name = Name;
		icon = ICON_SCENE_OBJ;
		parent = nullptr;
		//scene = Scene;
	}

	virtual ~NodeScene() {};

public:
	//ResourceScene* scene = nullptr;
};

//--- PREFAB ---
class NodePrefab : public HierarchyNode
{
public:
	NodePrefab(/*Prefab* prefab_,*/ HierarchyNode* Parent = nullptr, std::string Name = "Prefab") {
		type = NodeType::PREFAB;
		name = Name;
		icon = ICON_PREFAB;
		parent = Parent;
		//prefab = prefab_;
	}

	virtual ~NodePrefab() {};

public:
	//Prefab* prefab = nullptr;
};
