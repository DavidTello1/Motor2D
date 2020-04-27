#pragma once
#include "Imgui/imgui.h"

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
	bool is_open = false;

	ImGuiTreeNodeFlags flags = 0;
};

//--- FOLDER ---
class NodeFolder : public HierarchyNode
{
public:
	NodeFolder(HierarchyNode* Parent = nullptr) {
		type = NodeType::FOLDER;
		name = "Folder";
		parent = Parent;
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	virtual ~NodeFolder() {};
};

//--- GAMEOBJECT ---
class NodeGameObject : public HierarchyNode
{
public:
	NodeGameObject(GameObject* obj, HierarchyNode* Parent = nullptr) {
		type = NodeType::GAMEOBJECT;
		name = "GameObject";
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
	NodeScene(/*ResourceScene* Scene,*/ HierarchyNode* Parent = nullptr) {
		type = NodeType::SCENE;
		name = "Scene";
		parent = Parent;
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
	NodePrefab(/*Prefab* prefab_,*/ HierarchyNode* Parent = nullptr) {
		type = NodeType::PREFAB;
		name = "Scene";
		parent = Parent;
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		//prefab = prefab_;
	}

	virtual ~NodePrefab() {};

public:
	//Prefab* prefab = nullptr;
};
