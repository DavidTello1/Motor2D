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
		int scene_index = App->resources->scenes.data.GetIndexFromID(App->scene->current_scene);
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

	HierarchyNodeData data = nodes.data;
	for (size_t index = 0, size = data.name.size(); index < size; ++index)
	{
		// Draw Parents First
		if (data.parent[index] == "")
			nodes.DrawNode(index);

		// Draw Connector Lines
		if (!data.childs[index].empty())
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
	ShowSceneOptions(App->scene->current_scene); //*** NOT APPLY STYLE
	ImGui::SameLine();

	ImGui::SetCursorPosX(position);
	if (ImGui::IsItemHovered())
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_OPTIONS);
	else
		ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 0.5f), ICON_OPTIONS);
}

void PanelHierarchy::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Hierarchy"))
	{
		CreateMenu();

		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, !nodes.selected_nodes.empty())) //duplicate
			nodes.DuplicateNodes(nodes.selected_nodes);

		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !nodes.selected_nodes.empty())) //cut
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, !nodes.selected_nodes.empty())) //copy
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) //paste
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, !nodes.data.name.empty())) //select all
			nodes.SetState(HN_State::SELECTED, nodes.data.name);

		if (ImGui::MenuItem("Rename", NULL, false, nodes.selected_nodes.size() == 1)) //rename
		{
			int pos = nodes.FindNode(nodes.selected_nodes[0], nodes.data.name);
			if (pos != -1)
				nodes.data.state[pos] = HN_State::RENAME;
		}

		if (ImGui::MenuItem("Delete", "Supr", false, !nodes.selected_nodes.empty())) //delete
		{
			nodes.DeleteNodes(nodes.selected_nodes);
			nodes.selected_nodes.clear();
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
			if (!nodes.selected_nodes.empty())
				parent = nodes.selected_nodes[0];

			nodes.CreateNode("", NodeType::FOLDER, empty_childs, parent);
		}

		if (ImGui::MenuItem("GameObject"))
		{
			std::vector<std::string> empty_childs;
			std::string parent = "";
			if (!nodes.selected_nodes.empty())
				parent = nodes.selected_nodes[0];

			nodes.CreateNode("", NodeType::GAMEOBJECT, empty_childs, parent);
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