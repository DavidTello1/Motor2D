#include "HierarchyNode.h"

#include "Icons.h"
#include "Imgui/imgui.h"
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
			data.order[index] = RecursivePos(parent_index);
			data.indent[index] = data.indent[parent_index] + 1; //indent = parent indent + 1
			data.childs[parent_index].push_back(name); //add node to parent's child list
		}
	}

	ReorderNodes(index);

	return index;
}

void HierarchyNode::DeleteNodes(std::vector<std::string> nodes_list)
{
	// Reorder list by indent (parents first)
	if (nodes_list.size() > 1)
		nodes_list = SortByIndent(nodes_list);

	int index = 0;
	for (size_t i = 0, size = nodes_list.size(); i < size; ++i)
	{

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

			// Update nodes pos
			ReorderNodes(index, true);

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

void HierarchyNode::SelectNodesInRangeByPos(size_t index1, size_t index2)
{
	int first = (data.order[index1] < data.order[index2]) ? data.order[index1] : data.order[index2];
	int second = (data.order[index1] < data.order[index2]) ? data.order[index2] : data.order[index1];

	for (size_t i = 0, size = data.order.size(); i < size; ++i)
	{
		if (data.order[i] >= first && data.order[i] <= second)
			data.state[i] = HN_State::SELECTED;
	}

}

void HierarchyNode::SwitchHidden(size_t index)
{
	std::vector<std::string> childs = GetAllChilds(index);
	if (data.flags[index] & NodeFlags::HIDDEN)
	{
		data.flags[index] &= ~NodeFlags::HIDDEN;

		for (std::string child : childs)
		{
			int child_index = FindNode(child, data.name);
			if (child_index != -1)
				data.flags[child_index] &= ~NodeFlags::HIDDEN;
		}
	}
	else
	{
		data.flags[index] |= NodeFlags::HIDDEN;

		for (std::string child : childs)
		{
			int child_index = FindNode(child, data.name);
			if (child_index != -1)
				data.flags[child_index] |= NodeFlags::HIDDEN;
		}
	}
}

//--- MAIN HELPERS ---
void HierarchyNode::MoveNode(std::string name, std::string parent_name, int order, int indent)
{
	// Error handling
	int index = FindNode(name, data.name);
	int parent_index = FindNode(parent_name, data.name);
	if (index == -1 || parent_index == -1 || index == parent_index)
		return;

	// If node has parent, delete node from parent's child list
	if (data.parent[index] != "")
	{
		int parent = FindNode(data.parent[index], data.name);
		if (parent != -1)
			data.childs[parent].erase(data.childs[parent].begin() + FindNode(name, data.childs[parent]));
	}

	// Set indent
	if (indent == -1)
	{
		data.indent[index] = data.indent[parent_index] + 1;
		//if (data.parent[parent_index] == "")
		//	data.indent[index] = 1;
	}
	else
		data.indent[index] = indent;

	// Set order
	if (order == -1)
	{
		if (data.childs[parent_index].empty())
			data.order[index] = data.order[parent_index] + 1;
		else
			data.order[index] = data.order[GetLastChild(parent_index)] +1;
	}
	else
		data.order[index] = order;

	ReorderNodes(index); //***SHOULD HAVE EXCEPTIONS

	// Set parent and add to child list
	data.parent[index] = parent_name;
	if (parent_index != -1)
		data.childs[parent_index].push_back(data.name[index]);
	data.childs[parent_index] = SortByPosition(data.childs[parent_index]); //order childs (needed for reordering correctly)

	// Move Childs
	if (!data.childs[index].empty())
	{
		for (uint i = 0; i < data.childs[index].size(); ++i)
		{
			int child_index = FindNode(data.childs[index][i], data.name);
			if (child_index == -1)
				continue;

			MoveNode(data.name[child_index], name);
			ReorderNodes(child_index); //reorder nodes
		}
	}
}

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

// --- NODE POS ---
void HierarchyNode::ReorderNodes(size_t index, bool is_delete)
{
	for (size_t i = 0, size = data.order.size(); i < size; ++i)
	{
		if (i == index)
			continue;
		else if (data.order[i] >= data.order[index])
		{
			if (is_delete)
				data.order[i]--;
			else
				data.order[i]++;
		}
	}
}

void HierarchyNode::ReorderNodes(std::vector<std::string> exceptions)
{
	size_t first = 0;
	bool is_first = false;

	// Get indexes
	std::vector<size_t> indexes;
	for (std::string name : exceptions)
	{
		int pos = FindNode(name, data.name);
		if (pos != -1)
			indexes.push_back(pos);
	}

	// Order Nodes without exceptions
	for (size_t index : indexes)
	{
		for (size_t i = 0, size = data.order.size(); i < size; ++i)
		{
			if (index == i)
				continue;
			else if (data.order[index] < data.order[i])
				data.order[i]--;
		}
	}
}

uint HierarchyNode::RecursivePos(size_t index)
{
	if (data.childs[index].empty())
		return data.order[index] + 1;
	else
		return RecursivePos(FindNode(data.childs[index].back(), data.name));
}

// --- CONNECTOR LINES ---
std::vector<std::string> HierarchyNode::GetHiddenNodes() const
{
	std::vector<std::string> hidden;
	for (size_t index = 0, size = data.name.size(); index < size; ++index)
	{
		if (data.flags[index] & NodeFlags::CLOSED) //if closed add all childs to list
		{
			std::vector<std::string> tmp_list = GetAllChilds(index);
			hidden.insert(hidden.end(), tmp_list.begin(), tmp_list.end());
		}
		else // add closed childs' childs to list
		{
			std::vector<std::string> tmp_list = GetClosedChilds(index);
			hidden.insert(hidden.end(), tmp_list.begin(), tmp_list.end()); //add tmp_list to hidden_childs
		}
	}
	return hidden;
}

std::vector<std::string> HierarchyNode::GetAllChilds(size_t index) const
{
	std::vector<std::string> num_childs;

	for (size_t i = 0, size = data.childs[index].size(); i < size; ++i)
	{
		num_childs.push_back(data.childs[index][i]);

		int child_index = FindNode(data.childs[index][i], data.name);
		if (child_index != -1 && !data.childs[child_index].empty()) //if child has childs add them to list
		{
			std::vector<std::string> tmp_list = GetAllChilds(child_index); //get all childs
			num_childs.insert(num_childs.end(), tmp_list.begin(), tmp_list.end()); //add childs list to hidden_childs
		}
	}
	return num_childs;
}

std::vector<std::string> HierarchyNode::GetClosedChilds(size_t index) const
{
	std::vector<std::string> hidden_childs;
	for (uint i = 0; i < data.childs[index].size(); ++i)
	{
		int child_index = FindNode(data.childs[index][i], data.name);
		if (child_index == -1)
			continue;

		if (data.flags[child_index] & NodeFlags::CLOSED) //if node is closed add childs to list
		{
			std::vector<std::string> tmp_list = GetAllChilds(child_index); //get all childs
			hidden_childs.insert(hidden_childs.end(), tmp_list.begin(), tmp_list.end()); //add childs list to hidden_childs
		}
		else
		{
			std::vector<std::string> tmp_list = GetClosedChilds(child_index); //if node is open check if any child is closed
			hidden_childs.insert(hidden_childs.end(), tmp_list.begin(), tmp_list.end()); //add tmp_list to hidden_childs
		}
	}
	return hidden_childs;
}

bool HierarchyNode::IsChildOf(size_t index, size_t parent_index) const
{
	std::vector<std::string> all_childs = GetAllChilds(parent_index);
	std::string name = data.name[index];

	for (std::string child : all_childs)
	{
		if (child == name)
			return true;
	}
	return false;
}

size_t HierarchyNode::GetLastChild(size_t index) const
{
	if (data.childs[index].empty())
		return index;
	else
		return GetLastChild(FindNode(data.childs[index].back(), data.name));
}

// --- SORTERS ---
std::vector<std::string> HierarchyNode::SortByPosition(std::vector<std::string> list) const
{
	std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, PositionSort> ListOrder;

	for (std::string name : list) //push nodes into Ordered List
	{
		int index = FindNode(name, data.name);
		if (index == -1)
			continue;

		ListOrder.push({ name, data.order[index] });
	}

	list.clear(); //clear list

	while (ListOrder.empty() == false) //push Ordered List into New List
	{
		list.push_back(ListOrder.top().first);
		ListOrder.pop();
	}

	return list;
}

std::vector<std::string> HierarchyNode::SortByIndent(std::vector<std::string> list) const
{
	std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, IndentSort> ListOrder;

	for (std::string name : list) //push nodes into Ordered List
	{
		int index = FindNode(name, data.name);
		if (index == -1)
			continue;

		ListOrder.push({ name, data.indent[index] });
	}

	list.clear(); //clear list

	while (ListOrder.empty() == false) //push Ordered List into New List
	{
		list.push_back(ListOrder.top().first);
		ListOrder.pop();
	}

	return list;
}
