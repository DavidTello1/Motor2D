#pragma once
#include "Panel.h"
#include "HierarchyNode.h"

#include <string>
#include <vector>

struct PositionSort {
	bool operator()(HierarchyNode* const& node1, HierarchyNode* const& node2) //true if pos1 > pos2
	{
		if (node1->pos > node2->pos)
			return true;
		else
			return false;
	}
};

struct IndentSort {
	bool operator()(HierarchyNode* const& node1, HierarchyNode* const& node2) //true if indent1 < indent2
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
	HierarchyNode* CreateNode(HierarchyNode::NodeType type, HierarchyNode* parent = nullptr, std::string name = "");
	void DeleteNodes(std::vector<HierarchyNode*> nodes, bool reorder = true);
	void DuplicateNodes(std::vector<HierarchyNode*> nodes, HierarchyNode* parent = nullptr);
	void SelectAll();
	void UnSelectAll();

private:
	// --- MAIN HELPERS ---
	HierarchyNode* HandleSelection(HierarchyNode* node); //selection states
	bool DrawRightClick(); //only draws if right click is pressed, returns true if drawn
	bool ShowSceneOptions(HierarchyNode* node); //popup when options button is clicked
	void DrawConnectorLines(HierarchyNode* node, ImDrawList* draw_list); //draw connector lines when node is open
	int FindNode(HierarchyNode* node, std::vector<HierarchyNode*> list); //get node pos in list (returns -1 if not found)
	//void MoveNode(HierarchyNode* node, int pos);

	// --- NODE CREATION ---
	void CreateMenu(); //imgui menu for creating nodes (folder, gameobject, scene, etc)
	std::string CreateName(const char* name); //get name (with count of same names)

	// --- NODE POS ---
	void ReorderNodes(HierarchyNode* node, bool is_delete = false); //update nodes pos
	uint RecursivePos(HierarchyNode* node, bool is_duplicate = false); //set node pos in CreateNode()

	// --- CONNECTOR LINES ---
	std::vector<HierarchyNode*> GetHiddenNodes(); //get all hidden nodes in hierarchy
	std::vector<HierarchyNode*> GetAllChilds(HierarchyNode* node); //get childs (including childs of childs)
	std::vector<HierarchyNode*> GetClosedChilds(HierarchyNode* node); //returns all closed childs (including childs of childs)
	bool IsChildOf(HierarchyNode* parent, HierarchyNode* node); //check if node is child of parent (including if it is child of childs)

	// --- SORTERS ---
	std::vector<HierarchyNode*> SortByPosition(std::vector<HierarchyNode*> list); //order by position (smaller to bigger)
	std::vector<HierarchyNode*> SortByIndent(std::vector<HierarchyNode*> list); //order by indent (lower to higher)

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	std::vector<HierarchyNode*> nodes;
	std::vector<HierarchyNode*> selected_nodes;

	HierarchyNode* current_scene = nullptr;
};
