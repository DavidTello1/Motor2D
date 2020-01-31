#pragma once
#include "Panel.h"

#include <vector>

class GameObject;

struct HierarchyNode {
	std::string name = "node";
	int pos = 0;

	HierarchyNode* parent = nullptr;
	std::vector<HierarchyNode*> childs;

	bool rename = false;
	bool selected = false;
	bool is_folder = false;
	bool first_scene = false;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

	GameObject* object = nullptr;
	//ResourceScene* scene = nullptr;
};

struct Sort
{
	bool operator()(HierarchyNode* const & node1, HierarchyNode* const & node2) //true if pos1 > pos2
	{
		if (node1->pos > node2->pos)
			return true;
		return false;
	}
};

class Hierarchy : public Panel
{
public:
	Hierarchy();
	virtual ~Hierarchy();

	void Draw();
	void DrawNode(HierarchyNode* node);

	HierarchyNode* CreateNode(const char* name, bool is_folder = false, HierarchyNode* parent = nullptr, bool selected = true, GameObject* object = nullptr/*, ResourceScene*scene = nullptr*/);
	void DeleteNodes(std::vector<HierarchyNode*> nodes);
	void DuplicateNodes(std::vector<HierarchyNode*> nodes, HierarchyNode* parent = nullptr);
	void UnSelectAll(HierarchyNode* exception = nullptr);
	void SelectAll();

	int FindNode(HierarchyNode* node, std::vector<HierarchyNode*> list); //returns -1 if not found
	void MoveNode(HierarchyNode* node, int pos);

private:
	bool DrawRightClick(); //only draws if right click is pressed, returns true if drawn
	void OrderHierarchy();

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	std::vector<HierarchyNode*> nodes;
	std::vector<HierarchyNode*> selected_nodes;

	int last_id = 0; //used for creating unique ids (imgui)
};
