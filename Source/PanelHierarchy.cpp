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
	sprintf_s(search_buffer, 128, "");
	sprintf_s(replace_buffer, 128, "");
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

	// Scene Options
	ShowSceneOptions(scene_index);

	// Draw Nodes
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(-2, 3));
	ImVec2 pos = ImGui::GetCursorPos();

	DrawSceneNode(); // Draw Scene Node

	// Get Parents
	std::vector<std::string> parents;
	for (size_t index = 0, size = nodes.data.name.size(); index < size; ++index)
	{
		// Draw Parents First
		if (nodes.data.parent[index] == "")
			parents.push_back(nodes.data.name[index]);
	}
	parents = nodes.SortByPosition(parents);

	// Draw Nodes (parents)
	for (std::string name : parents)
	{
		int index = nodes.FindNode(name, nodes.data.name);
		if (index == -1)
			continue;

		DrawNode(index);

		// Draw Reparenting Line
		if (draw_reparenting_line)
		{
			window->DrawList->AddLine(reparenting_p1, reparenting_p2, ImColor(255, 255, 255, 255));
			draw_reparenting_line = false;
		}
	}

	// Draw Connector Lines
	for (size_t index = 0, size = nodes.data.childs.size(); index < size; ++index)
	{
		if (!(nodes.data.flags[index] & NodeFlags::CLOSED) && !nodes.data.childs[index].empty())
			DrawConnectorLines(index, window->DrawList);
	}
	ImGui::PopStyleVar();

	// Unselect nodes when clicking on empty space
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
		ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !is_any_hover)
		nodes.SetState(HN_State::IDLE, nodes.data.name);

	//--- Scroll Areas ---
	Scroll(pos);

	//--- Shortcuts ---
	Shortcuts();

	if (is_search)
		SearchReplace(search_buffer, replace_buffer);

	is_any_hover = false;
	is_dragging = false;
}

void PanelHierarchy::Shortcuts()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		// Delete
		if (App->input->Shortcut(SDL_SCANCODE_DELETE, KEY_DOWN))
		{
			nodes.DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}

		// Duplicate
		if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_D, KEY_DOWN) ||
			App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_D, KEY_DOWN))
		{
			nodes.DuplicateNodes(selected_nodes);
		}

		// SelectAll
		if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN) ||
			App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN))
		{
			nodes.SetState(HN_State::SELECTED, nodes.data.name);
		}

		// Search
		if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN) ||
			App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN))
		{
			is_search = true;
		}
	}
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
	pos.x += ImGui::GetScrollX();
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + ImGui::GetWindowWidth(), pos.y + height));
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(ImVec4(0.25f, 0.25, 0.25f, 1.0f)));

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(pos_x);
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
	float position = bg.Max.x - 45 + ImGui::GetScrollX();
	ImVec2 limit = ImGui::GetCursorPos();
	if (position < limit.x + 5)
		position = limit.x + 5;

	ImGui::SetCursorPos(ImVec2(position, limit.y + 1));
	if (ImGui::InvisibleButton(ICON_OPTIONS, ImVec2(14, 19)))
		ImGui::OpenPopup("Scene Options");
	ImGui::OpenPopupOnItemClick("Scene Options");
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
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + ImGui::GetWindowWidth() - 9 + ImGui::GetScrollX(), pos.y + height));
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(nodes.data.color[index]));

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	if (ImGui::InvisibleButton(std::string(nodes.data.name[index] + ICON_SHOW).c_str(), ImVec2(16, height)))
		nodes.SwitchHidden(index);
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
	if (ImGui::GetScrollX() < 19)
		pos_x = pos_x + 15 - ImGui::GetScrollX();
	else
		pos_x -= 5;
	ImGui::SetCursorPos(ImVec2(pos_x + ImGui::GetScrollX(), ImGui::GetCursorPosY() - 3));
	float width = bg.Max.x - 30;
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

		if (ImGui::BeginDragDropTarget()) // Target (Reordering)
		{
			bool reorder = false;
			for (std::string selected_node : selected_nodes)
			{
				if (nodes.data.name[index] != selected_node)// error handling
				{
					is_hovered = false;
					window->DrawList->AddLine(ImVec2(pos_x, bg.Min.y + 1.5f), ImVec2(pos_x + width, bg.Min.y + 1.5f), ImColor(255, 255, 255, 255));

					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode"))
					{
						if (!reorder)
						{
							nodes.ReorderNodes(selected_nodes);
							reorder = true;
						}
						nodes.MoveNode(selected_node, nodes.data.parent[index], nodes.data.order[index], nodes.data.indent[index]);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	// Selectable
	ImGui::SetCursorPos(ImVec2(pos_x + ImGui::GetScrollX(), ImGui::GetCursorPosY() + 3));
	if (nodes.data.state[index] == HN_State::RENAME)
		width = 50;
	else
	{
		if (ImGui::GetScrollX() < 19)
			width = bg.Max.x - 30 + ImGui::GetScrollX();
		else
			width = bg.Max.x - 24;
	}
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

	// Selection
	HandleSelection(index, is_hovered, bg.Min.y, width, height);

	// Indent
	if (ImGui::GetScrollX() < 19)
		pos_x += ImGui::GetScrollX() + 2;
	else
		pos_x += 22;
	ImGui::SetCursorPosX(pos_x);
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
			if (nodes.data.flags[index] & NodeFlags::CLOSED)
				nodes.data.flags[index] &= ~NodeFlags::CLOSED;
			else
				nodes.data.flags[index] |= NodeFlags::CLOSED;
			hidden_childs = nodes.GetHiddenNodes(); //refresh hidden_childs list
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			if (nodes.data.flags[index] & NodeFlags::CLOSED)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_CLOSED);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_OPEN);
		}
		else
		{
			if (nodes.data.flags[index] & NodeFlags::CLOSED)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_CLOSED);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_OPEN);
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
	else if (nodes.data.state[index] == HN_State::SELECTED || nodes.data.state[index] == HN_State::DRAGGING)
		nodes.data.color[index] = colors[ImGuiCol_ButtonActive];
	else
		nodes.data.color[index] = colors[ImGuiCol_WindowBg];

	// Draw Childs
	if (!(nodes.data.flags[index] & NodeFlags::CLOSED) && !nodes.data.childs[index].empty())
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

void PanelHierarchy::HandleSelection(size_t index, bool is_hovered, float bg_Min_y, float width, float height)
{
	//Drag and Drop
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) // Source
	{
		ImGui::SetDragDropPayload("HierarchyNode", &index, sizeof(size_t));
	
		if (!selected_nodes.empty()) // Popup text
		{
			if (selected_nodes.size() == 1)
				ImGui::Text(selected_nodes[0].c_str());
			else
				ImGui::Text("%d", selected_nodes.size());

			// Selection
			if (nodes.data.state[index] != HN_State::DRAGGING && nodes.data.state[index] != HN_State::SELECTED && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift)
				nodes.SetState(HN_State::IDLE, nodes.data.name);
		}
		nodes.data.state[index] = HN_State::DRAGGING;
		ImGui::EndDragDropSource();
	}
	ImGui::SetItemAllowOverlap();

	if (ImGui::BeginDragDropTarget()) // Target (Reparenting)
	{
		is_dragging = true;
		bool reorder = false;
		for (std::string selected_node : selected_nodes)
		{
			int selected_index = nodes.FindNode(selected_node, nodes.data.name);
			if (selected_index != -1 && index != selected_index && !nodes.IsChildOf(index, selected_index)) // error handling
			{
				ReparentingLine(index, bg_Min_y - 1.5f, width, height + 3); // draw reparenting line

				// Reparent Node
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode"))
				{
					if (!reorder)
					{
						nodes.ReorderNodes(selected_nodes); // reorder nodes with exceptions
						reorder = true;
						nodes.data.flags[index] &= ~NodeFlags::CLOSED;
						hidden_childs = nodes.GetHiddenNodes();
					}
					nodes.MoveNode(selected_node, nodes.data.name[index]); // reparent node
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Left Click (selection)
	if (is_hovered) //if treenode is clicked, check whether it is a single or multi selection
	{
		is_any_hover = true;
		if (ImGui::IsMouseDoubleClicked(0)) // Double-click (open folder / focus gameobject)
		{
			if (nodes.data.type[index] == NodeType::FOLDER && !nodes.data.childs[index].empty())
			{
				if (nodes.data.flags[index] & NodeFlags::CLOSED)
					nodes.data.flags[index] &= ~NodeFlags::CLOSED;
				else
					nodes.data.flags[index] |= NodeFlags::CLOSED;

				hidden_childs = nodes.GetHiddenNodes();
			}
			else if (nodes.data.type[index] == NodeType::GAMEOBJECT || nodes.data.type[index] == NodeType::PREFAB)
			{
				/*focus*/
			}
		}
		else if (ImGui::IsMouseClicked(0)) // Multiple Selection
		{
			//if (rename_node != -1)
			//{
			//	nodes.data.state[rename_node] = HN_State::IDLE;
			//	rename_node = -1;
			//}

			if (ImGui::GetIO().KeyCtrl) // Multiple Selection (Ctrl)
			{
				if (nodes.data.state[index] == HN_State::SELECTED)
					nodes.data.state[index] = HN_State::IDLE;
				else
					nodes.data.state[index] = HN_State::SELECTED;
			}
			else if (ImGui::GetIO().KeyShift) // Multiple Selection (Shift)
			{
				int pos = 0;
				if (!selected_nodes.empty())
				{
					pos = nodes.FindNode(selected_nodes.back().c_str(), nodes.data.name);
					if (pos == -1)
						pos = 0;
				}
				nodes.SelectNodesInRangeByPos(index, pos);
			}
		}
		else if (ImGui::IsMouseReleased(0)) // Single selection
		{
			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !is_dragging &&
				nodes.data.state[index] != HN_State::DRAGGING && nodes.data.state[index] != HN_State::RENAME)
			{
				nodes.SetState(HN_State::IDLE, nodes.data.name); //unselect all
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
	if ((nodes.data.state[index] == HN_State::SELECTED || nodes.data.state[index] == HN_State::DRAGGING) && position == -1)
		selected_nodes.push_back(nodes.data.name[index]);
	else if (nodes.data.state[index] != HN_State::SELECTED && nodes.data.state[index] != HN_State::DRAGGING && position != -1)
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
			std::string old_name = nodes.data.name[index];
			nodes.data.name[index] = nodes.GetNameCount(buffer);

			// Update Childs
			for (std::string child : nodes.data.childs[index])
			{
				int child_index = nodes.FindNode(child, nodes.data.name);
				if (child_index != -1)
					nodes.data.parent[child_index] = nodes.data.name[index];
			}

			// Update Parent's Child List
			int parent_index = nodes.FindNode(nodes.data.parent[index].c_str(), nodes.data.name);
			if (parent_index != -1)
			{
				int child_index = nodes.FindNode(old_name.c_str(), nodes.data.childs[parent_index]);
				if (child_index != -1)
					nodes.data.childs[parent_index][child_index] = nodes.data.name[index];
			}
		}
	}
	if (ImGui::IsItemClicked() || is_rename_flag)
	{
		is_rename_flag = false;
		ImGui::SetKeyboardFocusHere(-1);
	}
	if (ImGui::IsItemClicked(1))
		nodes.data.state[index] = HN_State::SELECTED;
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
			{
				nodes.data.state[pos] = HN_State::RENAME;
				rename_node = pos;
			}
		}

		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
		{
			nodes.DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Search", "Ctrl+F")) //search
			is_search = true;

		ImGui::EndPopup();
	}
}

void PanelHierarchy::ShowSceneOptions(size_t index)
{
	if (ImGui::BeginPopup("Scene Options"))
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

void PanelHierarchy::SearchReplace(char* search_buffer, char* replace_buffer)
{
	ImGui::SetNextWindowSize(ImVec2(250, 238));
	ImGui::Begin("Search and Replace", &is_search, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	static bool show_search = true;
	const char* items[] = { "Current Scene", "Current Layer", "Selected Objects", "Type: Layer", "Type: Folder", "Type: GameObject", "Type: Prefab" };
	static int current_item = 0;
	static bool full_words = false;
	static bool match_case = false;

	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x, ImGui::GetCursorPosY() - 5));
	if (show_search) // --- SEARCH ---
	{
		// Menu Buttons
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		if (ImGui::Button("Search", ImVec2(ImGui::GetWindowWidth() / 2, 0))) show_search = true;
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::Button("Replace", ImVec2(ImGui::GetWindowWidth() / 2, 0))) show_search = false;
		ImGui::Separator();

		// Actual Draw
		ImGui::InputText("Search##1", search_buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		// Combo
		ImGui::SetNextItemWidth(162);
		ImGui::Combo("##ResourceCombo", &current_item, items, IM_ARRAYSIZE(items));

		// Checkbox
		ImGui::Checkbox("Only full words", &full_words);
		ImGui::Checkbox("Match Lower and Upper case", &match_case);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 33);
		ImGui::Separator();

		// Buttons
		ImGui::Button("Previous", ImVec2(ImGui::GetContentRegionAvailWidth() / 2, 0));
		ImGui::SameLine(0.0f, 1.0f);
		ImGui::Button("Next", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 1);
		ImGui::Button("Search All", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
	}
	else // --- REPLACE ---
	{
		// Menu Buttons
		if (ImGui::Button("Search", ImVec2(ImGui::GetWindowWidth() / 2, 0))) show_search = true;
		ImGui::SameLine(0.0f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		if (ImGui::Button("Replace", ImVec2(ImGui::GetWindowWidth() / 2, 0))) show_search = false;
		ImGui::PopStyleColor();
		ImGui::Separator();

		// Actual Draw
		ImGui::InputText("Search##2", search_buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::InputText("Replace##2", replace_buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		// Combo
		ImGui::SetNextItemWidth(162);
		ImGui::Combo("##ResourceCombo", &current_item, items, IM_ARRAYSIZE(items));

		// Checkbox
		ImGui::Checkbox("Only full words", &full_words);
		ImGui::Checkbox("Match Lower and Upper case", &match_case);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
		ImGui::Separator();

		// Buttons
		ImGui::Button("Previous##2", ImVec2(ImGui::GetContentRegionAvailWidth() / 2, 0));
		ImGui::SameLine(0.0f, 1.0f);
		ImGui::Button("Next##2", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));

		ImGui::Button("Replace##3", ImVec2(ImGui::GetContentRegionAvailWidth() / 2, 0));
		ImGui::SameLine(0.0f, 1.0f);
		ImGui::Button("Replace All", ImVec2(ImGui::GetContentRegionAvailWidth(), 0));

	}
	ImGui::End();
}

void PanelHierarchy::ReparentingLine(size_t index, float offset, float width, float height)
{
	uint num_hidden = 0;
	if (nodes.data.flags[index] & NodeFlags::CLOSED)
		num_hidden = nodes.GetAllChilds(index).size();
	else
		num_hidden = nodes.GetClosedChilds(index).size();

	float h = offset + height * float(nodes.data.order[nodes.GetLastChild(index)] - nodes.data.order[index] + 1 - num_hidden);
	float w = pos_x + 15;
	reparenting_p1 = ImVec2(w, h);
	reparenting_p2 = ImVec2(w + width, h);
	draw_reparenting_line = true;
}

void PanelHierarchy::DrawConnectorLines(size_t index, ImDrawList* draw_list)
{
	// If any parent is closed do not draw
	int parent_index = index;
	while (parent_index != -1)
	{
		if (nodes.data.flags[parent_index] & NodeFlags::CLOSED)
			return;
		
		parent_index = nodes.FindNode(nodes.data.parent[parent_index], nodes.data.name);
	}

	// Get number of hidden childs that actually affect the node
	uint num_hidden = hidden_childs.size();
	uint num_hidden2 = 0;
	for (size_t i = 0, size = hidden_childs.size(); i < size; ++i)
	{
		int child_index = nodes.FindNode(hidden_childs[i], nodes.data.name);
		if (child_index == -1)
		{
			num_hidden--;
			continue;
		}

		// If any of the hidden_childs' order > node order and is not child (or child of childs) of node, substract them from count
		if (nodes.data.order[child_index] > nodes.data.order[index] && !nodes.IsChildOf(child_index, index))
			num_hidden--;

		// If any of the hidden_childs are childs of last child, substract them from count
		int last_child_index = nodes.FindNode(nodes.data.childs[index].back(), nodes.data.name);
		if (last_child_index != -1 && nodes.IsChildOf(child_index, last_child_index))
			num_hidden--;

		// Get num_hidden2 for initial_pos of parent
		if (nodes.data.order[child_index] < nodes.data.order[index])
			num_hidden2++;
	}

	// Positions
	int last_child_index = nodes.FindNode(nodes.data.childs[index].back(), nodes.data.name);
	uint last_child_pos = (uint)nodes.data.order[last_child_index] - num_hidden;  //get last_child_pos updated to hidden childs
	uint parent_pos = (uint)nodes.data.order[index] - num_hidden2;

	// Real Positions
	ImVec2 initial_pos = ImVec2(ImGui::GetWindowPos().x + 16 + 15 * float(nodes.data.indent[index] + 1), ImGui::GetWindowPos().y + 41 + 19 * (float)parent_pos); //initial pos
	ImVec2 final_pos = ImVec2(initial_pos.x, ImGui::GetWindowPos().y + 38 + 19 * (float)last_child_pos); //final pos

	// Connector Lines
	draw_list->AddLine(ImVec2(initial_pos.x - ImGui::GetScrollX(), initial_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // vertical line
	draw_list->AddLine(ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x + 7 - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // horizontal line
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

			int parent_index = nodes.FindNode(parent, nodes.data.name);
			if (parent_index != -1)
			{
				nodes.data.flags[parent_index] &= ~NodeFlags::CLOSED;
				hidden_childs = nodes.GetHiddenNodes();
			}
			nodes.SetState(HN_State::IDLE, nodes.data.name); //unselect all

			rename_node = nodes.CreateNode(NodeType::FOLDER, empty_childs, "", parent, 0, HN_State::RENAME);
			is_rename_flag = true;
		}

		if (ImGui::MenuItem("GameObject"))
		{
			std::vector<std::string> empty_childs;
			std::string parent = "";
			if (!selected_nodes.empty())
				parent = selected_nodes[0];

			nodes.SetState(HN_State::IDLE, nodes.data.name); //unselect all

			rename_node = nodes.CreateNode(NodeType::GAMEOBJECT, empty_childs, "", parent, 0, HN_State::RENAME);
			is_rename_flag = true;
		}
		ImGui::EndMenu();
	}
}

void PanelHierarchy::Scroll(ImVec2 pos)
{
	ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
	ImVec2 size = ImVec2(ImGui::GetWindowWidth() - 26, 50);

	if (ImGui::GetCursorPosY() < scroll.y)
		ImGui::SetScrollHereY();

	// Top Area
	ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, pos.y + scroll.y - 8));
	ImGui::Dummy(size);
	if (ImGui::BeginDragDropTarget())
	{
		if (scroll.y >= 1.0f)
			ImGui::GetCurrentWindow()->Scroll.y -= 1.0f;
		ImGui::EndDragDropTarget();
	}

	//Bottom Area
	ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, ImGui::GetContentRegionAvail().y + scroll.y - 60));
	ImGui::Dummy(size);
	if (ImGui::BeginDragDropTarget())
	{
		if (scroll.y < ImGui::GetCurrentWindow()->ScrollMax.y - 1.0f)
			ImGui::GetCurrentWindow()->Scroll.y += 1.0f;
		ImGui::EndDragDropTarget();
	}

	if (ImGui::GetScrollY() > ImGui::GetScrollMaxY())
		ImGui::SetScrollX(ImGui::GetScrollMaxY());
}