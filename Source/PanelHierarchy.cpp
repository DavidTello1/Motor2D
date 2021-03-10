#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleResources.h"
#include "GameObject.h"

#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"
// ---------------------------------------------------------

PanelHierarchy::PanelHierarchy() : Panel("Hierarchy", ICON_HIERARCHY, 3)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;

	scene_name = "Default Scene";
}

PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	pos_x = ImGui::GetWindowPos().x;
	pos_y = ImGui::GetWindowPos().y;

	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;

	// Get Scene Name
	if (App->scene->current_scene == 0)
		scene_name = "Default Scene";
	else
	{
		scene_index = App->resources->scenes.data.GetIndexFromID(App->scene->current_scene);
		if (scene_index != -1)
			scene_name = App->resources->scenes.data.names[scene_index];
		else
			scene_name = "Default Scene";
	}

	// Allow selection & show options with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(1))
		ImGui::SetWindowFocus();

	// Right Click Options
	DrawRightClick();

	// Draw Nodes
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(-2, 3));
	ImVec2 pos = ImGui::GetCursorPos();

	DrawSceneNode(); // Draw Scene Node

	for (size_t index = 0, size = nodes.data.name.size(); index < size; ++index)
	{
		// Draw Parents First
		if (nodes.data.parent[index] == "")
			DrawNode(index);

		// Draw Connector Lines
		if (!nodes.data.childs[index].empty())
			DrawConnectorLines(index, window->DrawList);
	}
	ImGui::PopStyleVar();

	////--- Empty Space ---
	//ImGui::BeginChild("Empty");
	//if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))) // Unselect nodes when clicking on empty space
	//	UnSelectAll();
	//DrawRightClick(); // Right Click Options
	//ImGui::EndChild();

	//--- Scroll Areas ---
	Scroll(pos);

	//--- Shortcuts ---
	Shortcuts();
}

void PanelHierarchy::Shortcuts()
{
	//if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	//{
	//	// Delete
	//	if (App->input->Shortcut(SDL_SCANCODE_DELETE, KEY_DOWN))
	//	{
	//		DeleteNodes(selected_nodes);
	//		selected_nodes.clear();
	//	}

	//	// Duplicate
	//	if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_D, KEY_DOWN) ||
	//		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_D, KEY_DOWN))
	//	{
	//		DuplicateNodes(selected_nodes);
	//	}

	//	// SelectAll
	//	if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN) ||
	//		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN))
	//	{
	//		SelectAll();
	//	}

	//	// Find
	//	if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN) ||
	//		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN))
	//	{
	//		//FindPopup();
	//	}
	//}
}

void PanelHierarchy::Save(Config* config) const
{
}

void PanelHierarchy::Load(Config* config)
{
}

void PanelHierarchy::DrawSceneNode()
{
	// Main Variables
	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;
	const float height = g.FontSize + g.Style.FramePadding.y;

	// Background
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + ImGui::GetWindowWidth() + ImGui::GetScrollX(), pos.y + height));
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(ImVec4(0.25f, 0.25, 0.25f, 1.0f)));

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	if (ImGui::InvisibleButton(std::string(scene_name + ICON_SHOW).c_str(), ImVec2(15, height)))
		is_scene_hidden = !is_scene_hidden;
	ImGui::SameLine();

	ImGui::SetCursorPosX(pos_x);
	if (ImGui::IsItemHovered())
		is_scene_hidden ? ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_HIDE) : ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_SHOW);
	else
		is_scene_hidden ? ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), ICON_HIDE) : ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.0f), ICON_SHOW);
	ImGui::SameLine();

	// Node Icon
	ImGui::SetCursorPosX(pos_x + 19);
	ImGui::Text(ICON_SCENE_OBJ);
	ImGui::SameLine(0.0f, 3.0f);

	// Name
	if (is_scene_saved == false)
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), scene_name.c_str());
	else
		ImGui::Text(scene_name.c_str());
	ImGui::SameLine();

	// Options
	ImVec2 limit = ImGui::GetCursorPos();
	float position = ImGui::GetWindowWidth() - 22;
	if (position < limit.x + 5)
		position = limit.x + 5;

	ImGui::SetCursorPos(ImVec2(position, limit.y + 1));
	ImGui::InvisibleButton(ICON_OPTIONS, ImVec2(14, 19));
	ShowSceneOptions(scene_index); //*** NOT APPLY STYLE
	ImGui::SameLine();

	ImGui::SetCursorPosX(position);
	if (ImGui::IsItemHovered())
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_OPTIONS);
	else
		ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 0.5f), ICON_OPTIONS);
}

void PanelHierarchy::DrawNode(size_t index)
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
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(nodes.data.color[index]));

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	if (ImGui::InvisibleButton(std::string(nodes.data.name[index] + ICON_SHOW).c_str(), ImVec2(16, height)))
	{
		if (nodes.data.flags[index] & NodeFlags::HIDDEN)
			nodes.data.flags[index] &= ~NodeFlags::HIDDEN;
		else
			nodes.data.flags[index] |= NodeFlags::HIDDEN;
	}
	ImGui::SameLine();
	ImGui::SetCursorPosX(pos_x);
	if (ImGui::IsItemHovered())
	{
		is_hovered = true;
		nodes.data.flags[index] & NodeFlags::HIDDEN ? ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_HIDE) : ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_SHOW);
	}
	else
		nodes.data.flags[index] & NodeFlags::HIDDEN ? ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), ICON_HIDE) : ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.0f), ICON_SHOW);
	ImGui::SameLine();

	// Drag&Drop selectable (top of node)
	ImGui::SetCursorPos(ImVec2(pos_x + 15, ImGui::GetCursorPosY() - 3));
	float width = bg.Max.x - 53;
	if (width <= 0.0f)
		width = 0.01f;
	if (nodes.data.state[index] != HN_State::RENAME)
	{
		if (ImGui::InvisibleButton(std::string(nodes.data.name[index] + "dnd").c_str(), ImVec2(width, 3)))
			is_clicked = true;
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
		{
			is_hovered = true;
			if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
				ImGui::SetWindowFocus();
		}
		ImGui::SetItemAllowOverlap();
		ImGui::SameLine();

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
	}

	// Selectable
	ImGui::SetCursorPos(ImVec2(pos_x + 15, ImGui::GetCursorPosY() + 3));
	if (nodes.data.state[index] == HN_State::RENAME)
		width = 50;
	if (ImGui::InvisibleButton(nodes.data.name[index].c_str(), ImVec2(width, height)))
	{
		is_clicked = true;
		if (nodes.data.state[index] == HN_State::RENAME)
			nodes.data.state[index] = HN_State::SELECTED;
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

	// Selection
	HandleSelection(index, is_hovered);

	// Indent
	ImGui::SetCursorPosX(pos_x + 17);
	if (nodes.data.indent[index] > 0)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15 * nodes.data.indent[index]);
	if (nodes.data.childs[index].empty())
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);
	else
	{
		// Arrow Icon
		pos_x = ImGui::GetCursorPosX() - 1;
		if (ImGui::InvisibleButton(std::string(nodes.data.name[index] + ICON_ARROW_OPEN).c_str(), ImVec2(16, height)))
		{
			if (nodes.data.flags[index] & NodeFlags::OPEN)
				nodes.data.flags[index] &= ~NodeFlags::OPEN;
			else
				nodes.data.flags[index] |= NodeFlags::OPEN;
			//hidden_childs = GetHiddenNodes(); //refresh hidden_childs list
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			if (nodes.data.flags[index] & NodeFlags::OPEN)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_CLOSED);
		}
		else
		{
			if (nodes.data.flags[index] & NodeFlags::OPEN)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_CLOSED);
		}
		ImGui::SameLine(0.0f, 2.0f);
	}

	// Node Icon
	switch (nodes.data.type[index])
	{
	case NodeType::FOLDER:		ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), ICON_FOLDER); break;
	case NodeType::PREFAB:		ImGui::TextColored(ImVec4(0.2f, 1.0f, 1.0f, 1.0f), ICON_PREFAB); break;
	case NodeType::GAMEOBJECT:	ImGui::Text(ICON_GAMEOBJECT); break;
	}
	ImGui::SameLine(0.0f, 2.0f);

	// Name
	if (nodes.data.state[index] != HN_State::RENAME)
		ImGui::Text(nodes.data.name[index].c_str());
	else // Rename
		RenameNode(index);

	// Edit Prefab Button
	if (nodes.data.type[index] == NodeType::PREFAB)
	{
		ImGui::SameLine();
		ImVec2 limit = ImGui::GetCursorPos();
		pos_x = ImGui::GetWindowWidth() - 37 + ImGui::GetScrollX();
		if (pos_x < limit.x)
			pos_x = limit.x;

		ImGui::SetCursorPos(ImVec2(pos_x, limit.y + 1));
		if (ImGui::InvisibleButton(std::string(nodes.data.name[index] + ICON_ARROW_SHOW).c_str(), ImVec2(15, height)))
		{
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
	if (is_hovered || is_clicked)
		nodes.data.color[index] = colors[ImGuiCol_ButtonHovered];
	else if (nodes.data.state[index] == HN_State::SELECTED || nodes.data.state[index] == HN_State::RENAME)
		nodes.data.color[index] = colors[ImGuiCol_ButtonActive];
	else
		nodes.data.color[index] = colors[ImGuiCol_WindowBg];

	// Draw Childs
	if ((nodes.data.flags[index] & NodeFlags::OPEN) && !nodes.data.childs[index].empty())
	{
		for (uint i = 0; i < nodes.data.childs[index].size(); ++i)
		{
			int child_index = nodes.FindNode(nodes.data.childs[index][i], nodes.data.name);
			if (child_index == -1)
				continue;
			DrawNode(child_index);
		}
	}
}

void PanelHierarchy::HandleSelection(size_t index, bool is_hovered)
{
	// Left Click (selection)
	if (is_hovered) //if treenode is clicked, check whether it is a single or multi selection
	{
		if (ImGui::IsMouseDoubleClicked(0)) // Double-click (open folder / focus gameobject)
		{
			if (nodes.data.type[index] == NodeType::FOLDER && !nodes.data.childs[index].empty())
			{
				if (nodes.data.flags[index] & NodeFlags::OPEN)
					nodes.data.flags[index] &= ~NodeFlags::OPEN;
				else
					nodes.data.flags[index] |= NodeFlags::OPEN;
			}
			else if (nodes.data.type[index] == NodeType::GAMEOBJECT || nodes.data.type[index] == NodeType::PREFAB)
			{
				/*focus*/
			}
		}
		else if (ImGui::IsMouseClicked(0)) // Multiple Selection
		{
			if (ImGui::GetIO().KeyCtrl) // Multiple Selection (Ctrl)
			{
				if (nodes.data.state[index] == HN_State::SELECTED)
					nodes.data.state[index] = HN_State::IDLE;
				else
					nodes.data.state[index] = HN_State::SELECTED;
			}
			else if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
			{
				int pos = nodes.FindNode(selected_nodes.back().c_str(), nodes.data.name);
				if (pos != -1)
				{
					if (index < (size_t)pos)
					{
						for (size_t i = index; i < (size_t)pos; ++i)
							nodes.data.state[i] = HN_State::SELECTED;
					}
					else
					{
						for (size_t i = index; i > (size_t)pos; --i)
							nodes.data.state[i] = HN_State::SELECTED;
					}
				}
			}
		}
		else if (ImGui::IsMouseReleased(0)) // Single selection
		{
			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && nodes.data.state[index] != HN_State::DRAGGING && nodes.data.state[index] != HN_State::RENAME)
			{
				bool selected = (nodes.data.state[index] == HN_State::SELECTED);
				nodes.SetState(HN_State::IDLE, nodes.data.name);

				if (selected && selected_nodes.size() <= 1)
					nodes.data.state[index] = HN_State::IDLE;
				else
					nodes.data.state[index] = HN_State::SELECTED;
			}
		}
		else if (ImGui::IsMouseClicked(1)) // Right Click (select item to show options)
		{
			if (selected_nodes.size() <= 1 || nodes.data.state[index] != HN_State::SELECTED)
				nodes.SetState(HN_State::IDLE, nodes.data.name);
			nodes.data.state[index] = HN_State::SELECTED;
		}
	}

	// Update Selected Nodes List
	int position = nodes.FindNode(nodes.data.name[index], selected_nodes);
	if (nodes.data.state[index] == HN_State::SELECTED && position == -1)
		selected_nodes.push_back(nodes.data.name[index]);
	else if (nodes.data.state[index] != HN_State::SELECTED && position != -1)
		selected_nodes.erase(selected_nodes.begin() + position);
}

void PanelHierarchy::RenameNode(size_t index)
{
	char buffer[128];
	sprintf_s(buffer, 128, "%s", nodes.data.name[index].c_str());

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
	if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	{
		nodes.data.state[index] = HN_State::SELECTED;
		if (buffer != nodes.data.name[index])
		{
			nodes.data.name[index] = nodes.GetNameCount(buffer);

			// update childs
			if (!nodes.data.childs[index].empty())
			{
				for (std::string child : nodes.data.childs[index])
				{
					int child_index = nodes.FindNode(child, nodes.data.name);
					if (child_index != -1)
						nodes.data.parent[child_index] = nodes.data.name[index];
				}
			}
		}
	}
	if (ImGui::IsItemClicked() || is_rename_flag)
	{
		is_rename_flag = false;
		ImGui::SetKeyboardFocusHere(-1);
	}
}

void PanelHierarchy::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Hierarchy"))
	{
		CreateMenu();

		if (ImGui::MenuItem("Create Prefab", NULL, false, !selected_nodes.empty())) //create prefab
		{
			//CreatePrefab(name, object, folder);
		} 
		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, !selected_nodes.empty())) //duplicate
			nodes.DuplicateNodes(selected_nodes);

		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty())) //cut
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, !selected_nodes.empty())) //copy
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) //paste
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, !nodes.data.name.empty())) //select all
			nodes.SetState(HN_State::SELECTED, nodes.data.name);

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
		{
			is_rename_flag = true;
			int pos = nodes.FindNode(selected_nodes[0], nodes.data.name);
			if (pos != -1)
				nodes.data.state[pos] = HN_State::RENAME;
		}

		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
		{
			nodes.DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Search", "Ctrl+F")) //search
		{

		}

		ImGui::EndPopup();
	}
}

void PanelHierarchy::ShowSceneOptions(size_t index)
{
	if (ImGui::BeginPopupContextItem(0,0))
	{
		if (ImGui::MenuItem("Save Scene", NULL, false, !is_scene_saved))
		{}

		if (ImGui::MenuItem("Save Scene As"))
		{}
		ImGui::Separator();

		if (ImGui::MenuItem("Remove Scene"))
		{}

		if (ImGui::MenuItem("Discard Changes", NULL, false, !is_scene_saved))
		{}
		ImGui::Separator();

		if (ImGui::MenuItem("Show in Assets"))
		{}
		ImGui::EndPopup();
	}
}

void PanelHierarchy::DrawConnectorLines(size_t index, ImDrawList* draw_list)
{
	//// Error handling
	//if (node == nullptr)
	//{
	//	LOG("node was NULL", 'e');
	//	return;
	//}

	//// If any parent is closed do not draw
	//bool draw = true;
	//HierarchyNode* tmp_node = node;
	//while (tmp_node != nullptr)
	//{
	//	if (!tmp_node->is_open)
	//		return;

	//	tmp_node = tmp_node->parent;
	//}

	//// Get number of hidden childs that actually affect the node
	//uint num_hidden = hidden_childs.size();
	//uint num_hidden2 = 0;
	//for (uint i = 0; i < hidden_childs.size(); ++i)
	//{
	//	// If any of the hidden_childs' pos > node pos and is not child (or child of childs) of node, substract them from count
	//	if (hidden_childs[i]->pos > node->pos && !IsChildOf(node, hidden_childs[i]))
	//		num_hidden--;

	//	// If any of the hidden_childs are childs of last child, substract them from count
	//	if (IsChildOf(node->childs[node->childs.size() - 1], hidden_childs[i]))
	//		num_hidden--;

	//	// Get num_hidden2 for initial_pos of parent
	//	if (hidden_childs[i]->pos < node->pos)
	//		num_hidden2++;
	//}

	//// Positions
	//uint last_child_pos = (uint)node->childs[node->childs.size() - 1]->pos - num_hidden;  //get last_child_pos updated to hidden childs
	//uint parent_pos = (uint)node->pos - num_hidden2;

	//// Real Positions
	//ImVec2 initial_pos = ImVec2(ImGui::GetWindowPos().x + 1 + 15 * float(node->indent + 1), ImGui::GetWindowPos().y + 38 + 19 * (float)parent_pos); //initial pos
	//ImVec2 final_pos = ImVec2(initial_pos.x, ImGui::GetWindowPos().y + 34 + 19 * (float)last_child_pos); //final pos

	//// Connector Lines
	//draw_list->AddLine(ImVec2(initial_pos.x - ImGui::GetScrollX(), initial_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // vertical line
	//draw_list->AddLine(ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x + 7 - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // horizontal line
}

// --- NODE CREATION ---
void PanelHierarchy::CreateMenu()
{
	if (ImGui::BeginMenu("Create")) //create
	{
		if (ImGui::MenuItem("Folder"))
		{
			std::vector<std::string> empty_childs;
			std::string parent = "";
			if (!selected_nodes.empty())
				parent = selected_nodes[0];

			nodes.CreateNode(NodeType::FOLDER, empty_childs, "", parent, 0, HN_State::SELECTED);

			if (!selected_nodes.empty())
			{
				int index = nodes.FindNode(selected_nodes[0], nodes.data.name);
				if (index != -1)
					nodes.data.state[index] = HN_State::IDLE;
			}
		}

		if (ImGui::MenuItem("GameObject"))
		{
			std::vector<std::string> empty_childs;
			std::string parent = "";
			if (!selected_nodes.empty())
				parent = selected_nodes[0];

			nodes.CreateNode(NodeType::GAMEOBJECT, empty_childs, "", parent, 0, HN_State::SELECTED);

			if (!selected_nodes.empty())
			{
				int index = nodes.FindNode(selected_nodes[0], nodes.data.name);
				if (index != -1)
					nodes.data.state[index] = HN_State::IDLE;
			}
		}
		ImGui::EndMenu();
	}
}

void PanelHierarchy::Scroll(ImVec2 pos)
{
	//ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
	//ImVec2 size = ImVec2(ImGui::GetWindowWidth() - 26, 50);

	//// Top Area
	//ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, pos.y + scroll.y - 8));
	//ImGui::Dummy(size);
	//if (ImGui::BeginDragDropTarget())
	//{
	//	if (scroll.y >= 1.0f)
	//		window->Scroll.y -= 1.0f;
	//	ImGui::EndDragDropTarget();
	//}

	////Bottom Area
	//ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, ImGui::GetWindowHeight() + scroll.y - 58));
	//ImGui::Dummy(size);
	//if (ImGui::BeginDragDropTarget())
	//{
	//	if (scroll.y < window->ScrollMax.y - 1.0f)
	//		window->Scroll.y += 1.0f;
	//	ImGui::EndDragDropTarget();
	//}
}