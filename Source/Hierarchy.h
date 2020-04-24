#pragma once
#include "Panel.h"

#include <string>
#include <vector>

class GameObject;

struct HierarchyNode {
	std::string name = "node";
	int indent = -1;

	HierarchyNode* parent = nullptr;
	std::vector<HierarchyNode*> childs;

	bool rename = false;
	bool selected = false;
	bool is_folder = false;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	GameObject* object = nullptr;
	//ResourceScene* scene = nullptr;
};

struct IndentSort {
	bool operator()(HierarchyNode* const & node1, HierarchyNode* const & node2) //true if indent1 < indent2
	{
		if (node1->indent >= node2->indent)
			return false;
		else
			return true;
	}
};

class Hierarchy : public Panel
{
public:
	Hierarchy();
	virtual ~Hierarchy();

	void Draw();
	void Shortcuts();

	void DrawNode(HierarchyNode* node);
	HierarchyNode* CreateNode(const char* name, bool is_folder = false, HierarchyNode* parent = nullptr, bool selected = true, GameObject* object = nullptr/*, ResourceScene*scene = nullptr*/);
	void DeleteNodes(std::vector<HierarchyNode*> nodes, bool reorder = true);
	void DuplicateNodes(std::vector<HierarchyNode*> nodes, HierarchyNode* parent = nullptr);
	void SelectAll();
	void UnSelectAll();

	int FindNode(HierarchyNode* node, std::vector<HierarchyNode*> list); //returns -1 if not found
	//void MoveNode(HierarchyNode* node, int pos);

private:
	HierarchyNode* NodeParams(HierarchyNode* node); //init node with params (leaf, selected and type)
	uint CountNode(const char* name); //get number of nodes with same name

	std::vector<HierarchyNode*> SortByIndent(std::vector<HierarchyNode*> list); //order by indent (lower to higher)
	bool DrawRightClick(); //only draws if right click is pressed, returns true if drawn

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	std::vector<HierarchyNode*> nodes;
	std::vector<HierarchyNode*> selected_nodes;
};
