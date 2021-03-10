#include "HierarchyNode.h"

#include "Icons.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include <queue>

#include "mmgr/mmgr.h"
// ---------------------------------------------------------

int HierarchyNode::FindNode(std::string name, std::vector<std::string> list) const
{
	for (uint i = 0; i < list.size(); ++i)
	{
		if (list[i] == name)
			return i;
	}
	return -1;
}

size_t HierarchyNode::CreateNode(NodeType type_, std::vector<std::string> childs_, std::string name_, std::string parent, int flags_, HN_State state_)
{
	size_t index = data.name.size();

	std::string name = name_;
	switch (type_)
	{
	case NodeType::FOLDER:		name = (name != "") ? name : "Folder"; break;
	case NodeType::GAMEOBJECT:  name = (name != "") ? name : "GameObject"; break;
	case NodeType::PREFAB:		name = (name != "") ? name : "Prefab"; break;
	}
	name = GetNameCount(name);

	data.name.push_back(name);
	data.type.push_back(type_);
	data.state.push_back(state_);
	data.flags.push_back(flags_);

	data.color.push_back(ImVec4(0,0,0,0));
	data.order.push_back(data.name.size());
	data.indent.push_back(0);

	data.parent.push_back(parent);
	data.childs.push_back(childs_);
	
	if (parent != "")
	{
		int parent_index = FindNode(parent, data.name);
		if (parent_index != -1)
		{
			data.parent[index] = data.name[parent_index];
			data.order[index] = RecursivePos(parent_index);
			data.indent[index] = data.indent[parent_index] + 1; //indent = parent indent + 1
			data.childs[parent_index].push_back(name); //add node to parent's child list
			data.flags[parent_index] |= NodeFlags::OPEN; //set parent open
		}
	}

	//ReorderNodes(index);

	return index;
}

void HierarchyNode::DeleteNodes(std::vector<std::string> nodes_list, bool reorder)
{
	int index = 0;
	for (size_t i = 0, size = nodes_list.size(); i < size; ++i)
	{
		//if (reorder) //reorder list by indent (only once)
		//{
		//	nodes_list = SortByIndent(nodes_list);
		//	reorder = false;
		//}

		index = FindNode(nodes_list[i].c_str(), data.name);
		if (index != -1)
		{
			// Delete node from parent's child list
			const char* parent_name = data.parent[index].c_str();
			if (parent_name != "")
			{
				int parent_index = FindNode(parent_name, data.name);
				if (parent_index != -1)
				{
					int child_index = FindNode(data.name[index].c_str(), data.childs[parent_index]);
					if (child_index != -1)
						data.childs[parent_index].erase(data.childs[parent_index].begin() + child_index);
				}
			}

			// Delete childs
			if (!data.childs[index].empty())
				DeleteNodes(data.childs[index]);

			//// Update nodes pos
			//ReorderNodes(index, true);

			// Delete node data
			data.name.erase(data.name.begin() + index);
			data.type.erase(data.type.begin() + index);
			data.state.erase(data.state.begin() + index);
			data.flags.erase(data.flags.begin() + index);
			data.color.erase(data.color.begin() + index);
			data.order.erase(data.order.begin() + index);
			data.indent.erase(data.indent.begin() + index);
			data.parent.erase(data.parent.begin() + index);
			data.childs.erase(data.childs.begin() + index);
		}
	}
	nodes_list.clear();
}

void HierarchyNode::DuplicateNodes(std::vector<std::string> nodes_list, int parent_index)
{		
	// Reorder list by indent (parents first)
	if (nodes_list.size() > 1)
		nodes_list = SortByIndent(nodes_list);

	for (uint i = 0; i < nodes_list.size(); ++i)
	{
		// Get node index in main list
		int index = FindNode(nodes_list[i], data.name);
		if (index == -1)
			continue;

		// Parent
		std::vector<std::string> empty_childs;
		std::string parent_name = "";
		if (parent_index != -1) // if parent is defined
			parent_name = data.name[parent_index];
		else if (parent_index == -1 && data.parent[index] != "") // if parent is not defined and source node has parent, set same parent as source node
			parent_name = data.parent[index];

		// Create Node			
		size_t new_index = CreateNode(data.type[index], empty_childs, data.name[index], parent_name, 0, HN_State::SELECTED);

		// Unselect source node
		data.state[index] = HN_State::IDLE;

		// Childs
		if (!data.childs[index].empty()) // if node has childs, duplicate them
			DuplicateNodes(data.childs[index], new_index);
	}
}

void HierarchyNode::SetState(HN_State state, std::vector<std::string> index_list)
{
	for (std::string name : index_list)
	{
		int index = FindNode(name, data.name);
		if (index == -1)
			continue;

		data.state[index] = state;
	}
}

 ////--- MAIN HELPERS ---
//void HierarchyNode::MoveNode(HierarchyNode* node, HierarchyNode* parent, HierarchyNode* pos, int indent)
//{
//	// If new parent is child of node (error handling)
//	if (IsChildOf(node, parent) || node == parent)
//		return;
//
//	// Erase node and childs from nodes list
//	std::vector<HierarchyNode*> childs_list = GetAllChilds(node);
//	int position = FindNode(node, nodes);
//	if (position != -1)
//	{
//		nodes.erase(nodes.begin() + position); //erase from nodes list (data is mantained in *node)
//		ReorderNodes(node, true);
//	}
//	if (!childs_list.empty())
//	{
//		for (uint i = 0; i < childs_list.size(); ++i) //erase childs from nodes list
//		{
//			position = FindNode(childs_list[i], nodes);
//			if (position != -1)
//			{
//				childs_list[i]->parent->childs.erase(childs_list[i]->parent->childs.begin() + FindNode(childs_list[i], childs_list[i]->parent->childs));
//				nodes.erase(nodes.begin() + position);
//				ReorderNodes(childs_list[i], true);
//			}
//		}
//	}
//
//	// If node has parent, delete node from parent's child list
//	if (node->parent != nullptr)
//		node->parent->childs.erase(node->parent->childs.begin() + FindNode(node, node->parent->childs));
//
//	// Set indent
//	if (indent == -1)
//	{
//		node->indent = parent->indent + 1;
//		if (parent->type == HierarchyNode::NodeType::SCENE)
//			node->indent = 1;
//	}
//	else
//		node->indent = indent;
//
//	// Set pos
//	if (pos == nullptr)
//	{
//		if (parent->childs.empty())
//			node->pos = parent->pos + 1;
//		else
//			node->pos = GetLastChild(parent)->pos + 1;
//	}
//	else
//		node->pos = pos->pos;
//
//	// Add to nodes list and Reorder all nodes
//	nodes.push_back(node);
//	ReorderNodes(node);
//
//	// Set parent and add to child list
//	if (parent != nullptr)
//	{
//		node->parent = parent;
//		parent->childs.push_back(node);
//		parent->childs = SortByPosition(parent->childs); //order childs (needed for reordering correctly)
//	}
//
//	// Move Childs
//	if (!childs_list.empty())
//	{
//		for (uint i = 0; i < childs_list.size(); ++i)
//		{
//			UpdateNode(childs_list[i]); //update node's pos and indent to new parent's pos
//			childs_list[i]->parent->childs.push_back(childs_list[i]); //add node back to parent's child list
//			nodes.push_back(childs_list[i]); //add node to nodes_list
//			ReorderNodes(childs_list[i]); //reorder nodes
//		}
//	}
//}

std::string HierarchyNode::GetNameCount(const std::string name) const
{
	if (data.name.empty())
		return name;

	bool found = false;
	uint count = 0;
	std::string new_name = name;

	while (found == false)
	{
		for (size_t i = 0, size = data.name.size(); i < size; ++i)
		{
			if (new_name == data.name[i])
			{
				count++;
				new_name = name + (" (") + std::to_string(count) + (")");
				break;
			}
			else if (i == data.name.size() - 1)
			{
				found = true;
				break;
			}
		}
	}
	return new_name;
}

//// --- NODE POS ---
//void HierarchyNode::ReorderNodes(size_t index, bool is_delete)
//{
//	for (uint i = data.pos[index]; i < nodes.size(); ++i)
//	{
//		if ( == node)
//			continue;
//		else
//		{
//			if (is_delete)
//				data.pos[i]--;
//			else
//				data.pos[i]++;
//		}
//	}
//	SortByPosition(data.name);
//}

uint HierarchyNode::RecursivePos(size_t index)
{
	if (data.childs[index].empty())
		return data.order[index] + 1;
	else
		return RecursivePos(FindNode(data.childs[index].back(), data.name));
}

//void HierarchyNode::UpdateNode(size_t index)
//{
//	if (node->parent->childs.empty())
//		node->pos = node->parent->pos + 1;
//	else
//		node->pos = GetLastChild(node->parent)->pos + 1;
//	node->indent = node->parent->indent + 1;
//}

//// --- CONNECTOR LINES ---
//std::vector<HierarchyNode*> HierarchyNode::GetHiddenNodes()
//{
//	std::vector<HierarchyNode*> hidden;
//	for (uint i = 0; i < nodes.size(); ++i)
//	{
//		if (nodes[i]->type == HierarchyNode::NodeType::SCENE)
//		{
//			if (!nodes[i]->is_open) //if closed add all childs to list
//			{
//				std::vector<HierarchyNode*> tmp_list = GetAllChilds(nodes[i]);
//				hidden.insert(hidden.end(), tmp_list.begin(), tmp_list.end());
//			}
//			else // add closed childs' childs to list
//			{
//				std::vector<HierarchyNode*> tmp_list = GetClosedChilds(nodes[i]);
//				hidden.insert(hidden.end(), tmp_list.begin(), tmp_list.end()); //add tmp_list to hidden_childs
//			}
//		}
//	}
//	return hidden;
//}
//
//std::vector<HierarchyNode*> HierarchyNode::GetAllChilds(HierarchyNode* node)
//{
//	std::vector<HierarchyNode*> num_childs;
//	for (uint i = 0; i < node->childs.size(); ++i)
//	{
//		num_childs.push_back(node->childs[i]);
//
//		if (!node->childs[i]->childs.empty()) //if child has childs add them to list
//		{
//			std::vector<HierarchyNode*> tmp_list = GetAllChilds(node->childs[i]); //get all childs
//			num_childs.insert(num_childs.end(), tmp_list.begin(), tmp_list.end()); //add childs list to hidden_childs
//		}
//	}
//	return num_childs;
//}
//
//std::vector<HierarchyNode*> HierarchyNode::GetClosedChilds(HierarchyNode* node)
//{
//	std::vector<HierarchyNode*> hidden_childs;
//	for (uint i = 0; i < node->childs.size(); ++i)
//	{
//		if (!node->childs[i]->is_open) //if node is closed add childs to list
//		{
//			std::vector<HierarchyNode*> tmp_list = GetAllChilds(node->childs[i]); //get all childs
//			hidden_childs.insert(hidden_childs.end(), tmp_list.begin(), tmp_list.end()); //add childs list to hidden_childs
//		}
//		else
//		{
//			std::vector<HierarchyNode*> tmp_list = GetClosedChilds(node->childs[i]); //if node is open check if any child is closed
//			hidden_childs.insert(hidden_childs.end(), tmp_list.begin(), tmp_list.end()); //add tmp_list to hidden_childs
//		}
//	}
//	return hidden_childs;
//}
//
//bool HierarchyNode::IsChildOf(HierarchyNode* parent, HierarchyNode* node) const
//{
//	if (parent != nullptr && node != nullptr && node->parent != nullptr)
//	{
//		if (node->parent == parent)
//			return true;
//		else
//			return IsChildOf(parent, node->parent);
//	}
//	return false;
//}
//
//std::string HierarchyNode::GetLastChild(size_t index) const
//{
//	if (data.childs[index].empty())
//		return data.name[index];
//	else
//	{
//		int pos = FindNode(data.childs[index].back(), data.name);
//		if (pos == -1)
//			return "";
//		else
//			return GetLastChild(pos);
//	}
//}

//// --- SORTERS ---
//void HierarchyNode::SortByPosition() const
//{
//	std::priority_queue<HierarchyNode*, std::vector<HierarchyNode*>, PositionSort> ListOrder;
//
//	for (HierarchyNode* node : list) //push nodes into Ordered List
//		ListOrder.push(node);
//
//	list.clear(); //clear list
//
//	while (ListOrder.empty() == false) //push Ordered List into New List
//	{
//		list.push_back(ListOrder.top());
//		ListOrder.pop();
//	}
//}

std::vector<std::string> HierarchyNode::SortByIndent(std::vector<std::string> list) const
{
	std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, IndentSort> ListOrder;

	for (std::string name : list) //push nodes into Ordered List
	{
		int pos = FindNode(name, data.name);
		if (pos == -1)
			continue;

		ListOrder.push({ name, data.indent[pos] });
	}

	list.clear(); //clear list

	while (ListOrder.empty() == false) //push Ordered List into New List
	{
		list.push_back(ListOrder.top().first);
		ListOrder.pop();
	}

	return list;
}
