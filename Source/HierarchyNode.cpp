#include "HierarchyNode.h"

#include "Icons.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include <queue>

#include "mmgr/mmgr.h"
// ---------------------------------------------------------

void HierarchyNode::DrawNode(size_t index)
{
	// Main Variables
	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;
	static ImGuiStyle* style = &ImGui::GetStyle();
	static ImVec4* colors = style->Colors;
	const float height = g.FontSize + g.Style.FramePadding.y;
	bool is_hovered = false, is_clicked = false;

	// Background
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + ImGui::GetWindowWidth() + ImGui::GetScrollX(), pos.y + height));
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(data.color[index]));

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	if (ImGui::InvisibleButton(std::string(data.name[index] + ICON_SHOW).c_str(), ImVec2(15, height)))
	{
		if (data.flags[index] == NodeFlags::HIDDEN)
			data.flags[index] |= NodeFlags::HIDDEN;
		else
			data.flags[index] &= NodeFlags::HIDDEN;
	}
	ImGui::SameLine();
	ImGui::SetCursorPosX(pos_x);
	if (ImGui::IsItemHovered())
	{
		is_hovered = true;
		data.flags[index] == NodeFlags::HIDDEN ? ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_SHOW) : ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_HIDE);
	}
	else
		data.flags[index] == NodeFlags::HIDDEN ?  ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.0f), ICON_SHOW) : ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), ICON_HIDE);
	ImGui::SameLine();

	//// Drag&Drop selectable (top of node)
	//ImGui::SetCursorPos(ImVec2(pos_x + 15, ImGui::GetCursorPosY() - 3));
	float width = bg.Max.x - 53;
	if (width <= 0.0f) width = 0.01f;
	//if (ImGui::InvisibleButton(std::string(data.name[index] + "dnd").c_str(), ImVec2(width, 3)))
	//{
	//	node->selected = !node->selected;
	//	is_clicked = true;
	//}
	//if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	//{
	//	is_hovered = true;
	//	if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
	//		ImGui::SetWindowFocus();
	//}
	//ImGui::SetItemAllowOverlap();
	//ImGui::SameLine();

	//if (ImGui::BeginDragDropTarget()) // Reordering
	//{
	//	for (uint i = 0; i < selected_nodes.size(); ++i)
	//	{
	//		if (((node->type == HierarchyNode::NodeType::SCENE && selected_nodes[i]->type == HierarchyNode::NodeType::SCENE) ||
	//			(node->type != HierarchyNode::NodeType::SCENE && selected_nodes[i]->type != HierarchyNode::NodeType::SCENE)) &&
	//			(node != selected_nodes[i]))// error handling
	//		{
	//			is_hovered = false;
	//			window->DrawList->AddLine(ImVec2(pos_x + 15, bg.Min.y + 1.5f), ImVec2(pos_x + 15 + width, bg.Min.y + 1.5f), ImColor(255, 255, 255, 255));

	//			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode"))
	//				MoveNode(selected_nodes[i], node->parent, node, node->indent);
	//		}
	//	}
	//	ImGui::EndDragDropTarget();
	//}

	// Selectable
	ImGui::SetCursorPos(ImVec2(pos_x + 15, ImGui::GetCursorPosY() + 3));
	if (data.state[index] == HN_State::RENAME)
		width = 34;
	if (ImGui::InvisibleButton(data.name[index].c_str(), ImVec2(width, height)))
	{
		is_clicked = true;
		if (data.state[index] == HN_State::SELECTED)
			data.state[index] = HN_State::IDLE;
		else
			data.state[index] = HN_State::SELECTED;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		is_hovered = true;
		if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
			ImGui::SetWindowFocus();
	}
	ImGui::SetItemAllowOverlap();
	ImGui::SameLine();

	////Drag and Drop
	//if (!selected_nodes.empty() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) // Source
	//{
	//	ImGui::SetDragDropPayload("HierarchyNode", &name, sizeof(std::string));

	//	if (selected_nodes.size() == 1)
	//	{
	//		std::string node_name = selected_nodes[0]->name;
	//		if (selected_nodes[0]->count != 0)
	//			node_name = node_name + std::string(" (") + std::to_string(selected_nodes[0]->count) + std::string(")");
	//		ImGui::Text(node_name.c_str());
	//	}
	//	else
	//		ImGui::Text(std::to_string(selected_nodes.size()).c_str());

	//	ImGui::EndDragDropSource();
	//}
	//ImGui::SetItemAllowOverlap();

	//if (ImGui::BeginDragDropTarget()) // Reparenting
	//{
	//	for (uint i = 0; i < selected_nodes.size(); ++i)
	//	{
	//		if (selected_nodes[i]->type != HierarchyNode::NodeType::SCENE && IsChildOf(selected_nodes[i], node) == false && node != selected_nodes[i]) // error handling
	//		{
	//			HierarchyNode* last_child = GetLastChild(node);
	//			float w = pos_x + 15;
	//			float h = bg.Min.y - 1.5f + (height + 3) * float(last_child->pos - node->pos + 1);
	//			window->DrawList->AddLine(ImVec2(w, h), ImVec2(w + width, h), ImColor(255, 255, 255, 255));

	//			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode"))
	//				MoveNode(selected_nodes[i], node, nullptr, -1);
	//		}
	//	}
	//	ImGui::EndDragDropTarget();
	//}

	//// Selection
	//HandleSelection(node, is_hovered);

	// Indent
	ImGui::SetCursorPosX(pos_x + 17);
	if (data.indent[index] > 0)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15 * (data.indent[index] - 1));
	if (data.childs[index].empty())
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);
	else
	{
		// Arrow Icon
		pos_x = ImGui::GetCursorPosX();
		if (ImGui::InvisibleButton(std::string(data.name[index] + ICON_ARROW_OPEN).c_str(), ImVec2(15, height)))
		{
			if (data.flags[index] == NodeFlags::OPEN)
				data.flags[index] |= NodeFlags::OPEN;
			else
				data.flags[index] &= NodeFlags::OPEN;
			//hidden_childs = GetHiddenNodes(); //refresh hidden_childs list
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			if (data.flags[index] == NodeFlags::OPEN)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_CLOSED);
		}
		else
		{
			if (data.flags[index] == NodeFlags::OPEN)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_CLOSED);
		}
		ImGui::SameLine(0.0f, 2.0f);
	}

	// Node Icon
	switch (data.type[index])
	{
	case NodeType::FOLDER:		ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), ICON_FOLDER); break;
	case NodeType::PREFAB:		ImGui::TextColored(ImVec4(0.2f, 1.0f, 1.0f, 1.0f), ICON_PREFAB); break;
	case NodeType::GAMEOBJECT:	ImGui::Text(ICON_GAMEOBJECT); break;
	}
	ImGui::SameLine(0.0f, 2.0f);

	// Name
	if (data.state[index] != HN_State::RENAME)
	{
		//if (node->type == HierarchyNode::NodeType::SCENE)
		//{
		//	NodeScene* node_scene = (NodeScene*)node;
		//	if (node_scene->is_saved == false)
		//		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), name.c_str());
		//	else
		//		ImGui::Text(name.c_str());
		//}
		//else
		ImGui::Text(data.name[index].c_str());
	}
	else // Rename
	{
		//char buffer[128];
		//sprintf_s(buffer, 128, "%s", node->name.c_str());

		//ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		//if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		//{
		//	if (strchr(buffer, '(') != nullptr || strchr(buffer, ')') != nullptr)
		//		LOG("Error renaming node, character not valid '()'", 'e')
		//	else
		//	{
		//		node->name = buffer;
		//		node->count = 0;
		//		node->count = GetNameCount(node);
		//		node->rename = false;
		//	}
		//}

		//// Space after input text
		//ImGui::SameLine();
		//ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8);
		//width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 8;
		//if (width == 0.0f) //error handling (invisible button size cannot be 0)
		//	width = 0.1f;
		//if (ImGui::InvisibleButton(std::string(node->name + std::to_string(node->count)).c_str(), ImVec2(width, height)))
		//{
		//	node->rename = false;
		//	is_clicked = true;
		//}
		//if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
		//{
		//	is_hovered = true;
		//	if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
		//		ImGui::SetWindowFocus();
		//}
	}

	// Scene Options & Edit Prefab Buttons
	if (data.type[index] == NodeType::PREFAB)
	{
		ImGui::SameLine();
		ImVec2 limit = ImGui::GetCursorPos();
		pos_x = ImGui::GetWindowWidth() - 37 + ImGui::GetScrollX();
		if (pos_x < limit.x)
			pos_x = limit.x;

		ImGui::SetCursorPos(ImVec2(pos_x, limit.y + 1));
		if (ImGui::InvisibleButton(std::string(data.name[index] + ICON_ARROW_SHOW).c_str(), ImVec2(15, height)))
		{
			//if (node->type == HierarchyNode::NodeType::SCENE) // Scenes
			//	ShowSceneOptions(node);
			//else if (node->type == HierarchyNode::NodeType::PREFAB) //Prefabs
			//{
			//}	
			//edit prefab
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_SHOW);
		}
		else
			ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 0.5f), ICON_ARROW_SHOW);
	}

	// Highlight
	//if (node->type == HierarchyNode::NodeType::SCENE)
	//{
	//	if (current_scene == node)
	//		node->color = ImVec4(0.25f, 0.25, 0.25f, 1.0f);
	//	else
	//		node->color = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	//}
	//else
	//{
		if (is_hovered || is_clicked)
			data.color[index] = colors[ImGuiCol_ButtonHovered];
		else if (data.state[index] == HN_State::SELECTED)
			data.color[index] = colors[ImGuiCol_ButtonActive];
		else
			data.color[index] = colors[ImGuiCol_WindowBg];
	//}

	// Draw Childs
	if (data.flags[index] == NodeFlags::HIDDEN && !data.childs[index].empty())
	{
		for (uint i = 0; i < data.childs[index].size(); ++i)
		{
			int child_index = FindNode(data.childs[index][i], data.name);
			if (child_index == -1)
				continue;
			DrawNode(child_index);
		}
	}
}

int HierarchyNode::FindNode(std::string name, std::vector<std::string> list) const
{
	for (uint i = 0; i < list.size(); ++i)
	{
		if (list[i] == name)
			return i;
	}
	return -1;
}

size_t HierarchyNode::CreateNode(std::string name_, NodeType type_, std::vector<std::string> childs_, int parent_index, int flags_, HN_State state_)
{
	size_t index = data.name.size();

	std::string name = name_;
	switch (type_)
	{
	case NodeType::FOLDER:		name = (name != "") ? name : "Folder"; break;
	case NodeType::GAMEOBJECT:  name = (name != "") ? name : "GameObject"; break;
	case NodeType::PREFAB:		name = (name != "") ? name : "Prefab"; break;
	}

	data.name.push_back(name);
	data.type.push_back(type_);
	data.state.push_back(state_);
	data.flags.push_back(flags_);

	data.color.push_back(ImVec4(0,0,0,0));
	data.childs.push_back(childs_);

	if (parent_index == -1)
	{
		data.parent.push_back("");
		data.indent.push_back(0);
		data.pos.push_back(data.name.size());
	}
	else
	{
		data.parent.push_back(data.name[parent_index]);
		data.pos.push_back(RecursivePos(parent_index));
		data.indent.push_back(data.indent[parent_index] + 1); //indent = parent indent + 1
		data.childs[parent_index].push_back(name); //add node to parent's child list
		SetState(HN_State::IDLE, data.name); // UnSelectAll
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
			data.pos.erase(data.pos.begin() + index);
			data.indent.erase(data.indent.begin() + index);
			data.parent.erase(data.parent.begin() + index);
			data.childs.erase(data.childs.begin() + index);
		}
	}
	nodes_list.clear();
}

void HierarchyNode::DuplicateNodes(std::vector<std::string> nodes_list, int parent_index)
{
	for (uint i = 0; i < nodes_list.size(); ++i)
	{
		int index = FindNode(nodes_list[i], data.name);
		if (index == -1)
			continue;

		// Parent
		if (parent_index == -1 && data.parent[index] != "") // if parent is null make root
			parent_index = FindNode(nodes_list[i], data.name);

		// Create Node
		std::vector<std::string> empty_childs;
		size_t new_index = CreateNode(data.name[index], data.type[index], empty_childs, parent_index);

		// Selected (unselect source node)
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
//void HierarchyNode::HandleSelection(size_t index)
//{
//	// Left Click (selection)
//	if (is_hovered) //if treenode is clicked, check whether it is a single or multi selection
//	{
//		if (ImGui::IsMouseDoubleClicked(0)) // Double-click (open folder / focus gameobject)
//		{
//			if (node->type == HierarchyNode::NodeType::FOLDER && !node->childs.empty())
//				node->is_open = !node->is_open;
//			else if (node->type == HierarchyNode::NodeType::SCENE)
//				current_scene = node;
//			else if (node->type == HierarchyNode::NodeType::GAMEOBJECT || node->type == HierarchyNode::NodeType::PREFAB)
//			{ /*focus*/
//			}
//		}
//		else if (ImGui::IsMouseClicked(0)) // Multiple Selection
//		{
//			if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
//			{
//				HierarchyNode* start_node = selected_nodes.back();
//
//				uint first = start_node->pos;
//				uint last = node->pos;
//				if (first < last)
//				{
//					for (uint i = first; i < last; ++i)
//						nodes[i]->selected = true;
//				}
//				else
//				{
//					for (uint i = first; i > last; --i)
//						nodes[i]->selected = true;
//				}
//			}
//		}
//		else if (ImGui::IsMouseReleased(0)) // Single selection
//		{
//			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !ImGui::IsMouseDragging(0))
//				UnSelectAll();
//		}
//		else if (ImGui::IsMouseClicked(1) && selected_nodes.size() <= 1 && node->type != HierarchyNode::NodeType::SCENE) // Right Click (select item to show options)
//		{
//			UnSelectAll();
//			node->selected = !node->selected; //change selection state
//		}
//	}
//
//	// Update Selected Nodes List
//	int position = FindNode(node, selected_nodes);
//	if (node->selected && position == -1)
//		selected_nodes.push_back(node);
//	else if (!node->selected && position != -1)
//		selected_nodes.erase(selected_nodes.begin() + position);
//
//	return node;
//}
//
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
//
//uint HierarchyNode::GetNameCount(size_t index)
//{
//	std::vector<HierarchyNode*> repeated_nodes;
//	for (uint i = 0; i < nodes.size(); ++i) //get nodes with repeated name and highest name count
//	{
//		if (node != nodes[i] && node->name == nodes[i]->name)
//		{
//			repeated_nodes.push_back(nodes[i]);
//			if (node->count <= nodes[i]->count)
//				node->count = nodes[i]->count;
//		}
//	}
//
//	if (!repeated_nodes.empty()) //if name is repeated
//	{
//		if (repeated_nodes.size() <= node->count) //if there are gaps between the name counts
//		{
//			node->count = 0;
//			bool found = false;
//
//			while (found == false)
//			{
//				for (uint i = 0; i < repeated_nodes.size(); ++i) //if count is repeated increase it and try again, if not repeated set count
//				{
//					if (repeated_nodes[i]->count == node->count)
//					{
//						repeated_nodes.erase(repeated_nodes.begin() + i); //if count is repeated, erase repeated count node from repeated_nodes list
//						node->count++;
//						break;
//					}
//					else if (i == repeated_nodes.size() - 1)
//					{
//						found = true;
//						break;
//					}
//
//				}
//			}
//		}
//		else //if there are no gaps
//			node->count++;
//	}
//
//	return node->count;
//}

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
		return data.pos[index] + 1;
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
//
//void HierarchyNode::SortByIndent() const
//{
//	std::priority_queue<int, std::vector<int>, IndentSort> ListOrder;
//
//	for (int pos : data.pos) //push nodes into Ordered List
//	{
//		ListOrder.push(pos);
//	}
//
//	list.clear(); //clear list
//
//	while (ListOrder.empty() == false) //push Ordered List into New List
//	{
//		list.push_back(ListOrder.top());
//		ListOrder.pop();
//	}
//}
