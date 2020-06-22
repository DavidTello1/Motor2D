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

	HierarchyNode* parent = nullptr;
	std::vector<HierarchyNode*> childs;

	int pos = -1;
	int indent = -1;

	bool rename = false;
	bool selected = false;
	bool is_open = true;

	ImGuiTreeNodeFlags flags = 0;
};

//--- FOLDER ---
class NodeFolder : public HierarchyNode
{
public:
	NodeFolder(HierarchyNode* Parent = nullptr, std::string Name = "Folder") {
		type = NodeType::FOLDER;
		name = ICON_FOLDER + std::string(" ") + Name;
		parent = Parent;
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	virtual ~NodeFolder() {};
};

//--- GAMEOBJECT ---
class NodeGameObject : public HierarchyNode
{
public:
	NodeGameObject(GameObject* obj, HierarchyNode* Parent = nullptr, std::string Name = "GameObject") {
		type = NodeType::GAMEOBJECT;
		name = ICON_GAMEOBJECT + std::string(" ") + Name;
		parent = Parent;
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
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
		name = ICON_SCENE_OBJ + std::string(" ") + Name;
		parent = nullptr;
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;
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
		name = ICON_PREFAB + std::string(" ") + Name;
		parent = Parent;
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		//prefab = prefab_;
	}

	virtual ~NodePrefab() {};

public:
	//Prefab* prefab = nullptr;
};
