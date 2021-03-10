#pragma once
#include "Globals.h"
#include <string>
#include <vector>

struct ImVec4;

struct PositionSort {
	bool operator()(int const& pos1, int const& pos2) //true if pos1 > pos2
	{
		return (pos1 > pos2) ? true : false;
	}
};

struct IndentSort {
	bool operator()(std::pair<std::string, int> const& indent1, std::pair<std::string, int> const& indent2) //true if indent1 < indent2
	{
		return (indent1.second > indent2.second) ? true : false;
	}
};

enum class NodeType {
	FOLDER,
	GAMEOBJECT,
	PREFAB
};

enum NodeFlags {
	OPEN = 1 << 0,
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
	void DeleteNodes(std::vector<std::string> nodes, bool reorder = true);
	void DuplicateNodes(std::vector<std::string> nodes, int parent_index = -1);
	//void MoveNode(size_t index, size_t parent_index, int pos = -1, int indent = -1); //move node (if pos or indent is -1 they will be set according to parent)

	// --- UTILS ---
	int FindNode(std::string name, std::vector<std::string> list) const; //get node pos in list (returns -1 if not found)
	void SetState(HN_State state, std::vector<std::string> list);
	std::string GetNameCount(const std::string name) const; //get name count

	// --- ORDER ---
	uint RecursivePos(size_t index); //set node pos in CreateNode()
	//void ReorderNodes(size_t index, bool is_delete = false); //update nodes pos
	//void UpdateNode(size_t index); //update nodes pos and indent

	//// --- CHILDS (CONNECTOR LINES) ---
	//std::vector<std::string> GetHiddenNodes() const; //get all hidden nodes in hierarchy
	//std::vector<std::string> GetAllChilds(size_t node) const; //get childs (including childs of childs)
	//std::vector<std::string> GetClosedChilds(size_t node) const; //returns all closed childs (including childs of childs)
	//std::string GetLastChild(size_t node) const; //get last child (including childs of childs)
	//bool IsChildOf(size_t parent, size_t node) const; //check if node is child of parent (including if it is child of childs)

	//// --- SORTERS ---
	//void SortByPosition() const; //order by position (smaller to bigger)
	std::vector<std::string> SortByIndent(std::vector<std::string> list) const; //order list by indent (lower to higher)
};