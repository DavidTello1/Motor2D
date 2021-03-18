#pragma once
#include "Globals.h"
#include <string>
#include <vector>

struct ImVec4;

struct PositionSort {
	bool operator()(std::pair<std::string, int> const& pos1, std::pair<std::string, int> const& pos2)
	{
		return (pos1.second > pos2.second) ? true : false;
	}
};

struct IndentSort {
	bool operator()(std::pair<std::string, int> const& indent1, std::pair<std::string, int> const& indent2)
	{
		return (indent1.second > indent2.second) ? true : false;
	}
};

enum class NodeType {
	FOLDER,
	//LAYER,
	GAMEOBJECT,
	PREFAB
};

enum NodeFlags {
	CLOSED = 1 << 0,
	HIDDEN = 1 << 1,
};

enum class HN_State {
	IDLE,
	SELECTED,
	DRAGGING,
	RENAME,
	CUT
};

struct HN_Data // Data
{
	std::vector<std::string> name;
	std::vector<NodeType> type;
	std::vector<HN_State> state;
	std::vector<int> flags;

	std::vector<ImVec4> color;
	std::vector<int> order;
	std::vector<int> indent;

	std::vector<std::string> parent;
	std::vector<std::vector<std::string>> childs;
};

struct HierarchyNode // Node
{
	HN_Data data;

	//----------------------------

	// --- MAIN ---
	size_t CreateNode(NodeType type, std::vector<std::string> childs, std::string name = "", std::string parent = "", int flags = 0, HN_State state = HN_State::IDLE);
	void RemoveNode(size_t index);
	void DeleteNodes(std::vector<std::string> nodes);
	void DuplicateNodes(std::vector<std::string> nodes, int parent_index = -1);
	void MoveNode(std::string name, std::string parent_name, int order = -1, int indent = -1); //move node (if order or indent is -1 they will be set according to parent)

	// --- UTILS ---
	int FindNode(std::string name, std::vector<std::string> list) const; //get node pos in list (returns -1 if not found)
	std::string GetNameCount(const std::string name) const;
	void SetState(HN_State state, std::vector<std::string> list);
	void SelectNodesInRangeByPos(size_t index1, size_t index2); //for shift selection
	void SwitchHidden(size_t index); //switch hidden flag, including childs;

	// --- ORDER ---
	uint RecursivePos(size_t index); //set node order in CreateNode()
	void ReorderNodes(size_t index, bool is_delete = false); //update nodes order
	void ReorderNodes(std::vector<std::string> exceptions); //update nodes order with exceptions (used for Drag&Drop)

	// --- CHILDS ---
	std::vector<std::string> GetHiddenNodes() const; //get all hidden nodes
	std::vector<std::string> GetClosedChilds(size_t index) const; //returns all closed childs (including childs of childs) used for GetHiddenNodes()
	std::vector<std::string> GetAllChilds(size_t index) const; //get childs (including childs of childs) used for MoveNode()
	size_t GetLastChild(size_t index) const; //get last child (including childs of childs)
	bool IsChildOf(size_t index, size_t parent_index) const; //check if node is child of parent (including if it is child of childs)

	// --- SORTERS ---
	std::vector<std::string> SortByPosition(std::vector<std::string> list) const; //order by position (smaller to bigger)
	std::vector<std::string> SortByIndent(std::vector<std::string> list) const; //order list by indent (lower to higher)
};