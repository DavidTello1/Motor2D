#pragma once
#include "Panel.h"
#include "HierarchyNode.h"

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
	~Hierarchy();

	void Draw() override;
	void Shortcuts() override;

	void Save(Config* config) const override;
	void Load(Config* config) override;

	void DrawNode(HierarchyNode* node);
	HierarchyNode* CreateNode(HierarchyNode::NodeType type, HierarchyNode* parent = nullptr, std::string name = "");
	void DeleteNodes(std::vector<HierarchyNode*> nodes_list, bool reorder = true);
	void DuplicateNodes(std::vector<HierarchyNode*> nodes, HierarchyNode* parent = nullptr);
	void SelectAll();
	void UnSelectAll();

private:
	// --- MAIN HELPERS ---
	HierarchyNode* HandleSelection(HierarchyNode* node, bool is_hovered); //selection states
	void MoveNode(HierarchyNode* node, HierarchyNode* parent, HierarchyNode* pos, int indent); //move node (if pos or indent is nullptr or -1 they will be set according to parent)
	bool DrawRightClick(); //only draws if right click is pressed, returns true if drawn
	bool ShowSceneOptions(HierarchyNode* node); //popup when options button is clicked
	void DrawConnectorLines(HierarchyNode* node, ImDrawList* draw_list); //draw connector lines when node is open
	int FindNode(HierarchyNode* node, std::vector<HierarchyNode*> list); //get node pos in list (returns -1 if not found)

	// --- NODE CREATION ---
	void CreateMenu(); //imgui menu for creating nodes (folder, gameobject, scene, etc)
	uint GetNameCount(HierarchyNode* node); //get name count

	// --- NODE POS ---
	void ReorderNodes(HierarchyNode* node, bool is_delete = false); //update nodes pos
	uint RecursivePos(HierarchyNode* node); //set node pos in CreateNode()
	void UpdateNode(HierarchyNode* node); //update nodes pos and indent

	// --- CHILDS (CONNECTOR LINES) ---
	std::vector<HierarchyNode*> GetHiddenNodes(); //get all hidden nodes in hierarchy
	std::vector<HierarchyNode*> GetAllChilds(HierarchyNode* node); //get childs (including childs of childs)
	std::vector<HierarchyNode*> GetClosedChilds(HierarchyNode* node); //returns all closed childs (including childs of childs)
	bool IsChildOf(HierarchyNode* parent, HierarchyNode* node); //check if node is child of parent (including if it is child of childs)
	HierarchyNode* GetLastChild(HierarchyNode* node); //get last child (including childs of childs)

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
	std::vector<HierarchyNode*> hidden_childs;

	HierarchyNode* current_scene = nullptr;
};
