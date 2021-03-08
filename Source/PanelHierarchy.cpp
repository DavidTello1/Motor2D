#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
//#include "ModuleScene.h"
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

	// Allow selection & show options with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(1))
		ImGui::SetWindowFocus();

	// Right Click Options
	DrawRightClick();

	// Draw Nodes
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(-2, 3));
	ImVec2 pos = ImGui::GetCursorPos();

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

void PanelHierarchy::DrawRightClick()
{
	//if (ImGui::BeginPopupContextWindow("Hierarchy"))
	//{
	//	CreateMenu();

	//	if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, !selected_nodes.empty())) //duplicate
	//		DuplicateNodes(selected_nodes);

	//	ImGui::Separator();

	//	if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty())) //cut
	//	{
	//	}
	//	if (ImGui::MenuItem("Copy", "Ctrl+C", false, !selected_nodes.empty())) //copy
	//	{
	//	}
	//	if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) //paste
	//	{
	//	}
	//	ImGui::Separator();

	//	if (ImGui::MenuItem("Select All", NULL, false, !nodes.empty())) //select all
	//		SelectAll();

	//	if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
	//		selected_nodes.front()->rename = true;

	//	if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
	//	{
	//		DeleteNodes(selected_nodes);
	//		selected_nodes.clear();
	//	}
	//	ImGui::Separator();

	//	if (ImGui::MenuItem("Search", "Ctrl+F")) //search
	//	{

	//	}

	//	ImGui::EndPopup();
	//}
}

void PanelHierarchy::ShowSceneOptions(size_t index)
{
	if (ImGui::BeginPopupContextItem(0,0))
	{
		//NodeScene* scene_node = (NodeScene*)node;

		//if (ImGui::MenuItem("Set Active Scene"))
		//	current_scene = node;
		//ImGui::Separator();

		if (ImGui::MenuItem("Save Scene", NULL, false/*, !scene_node->is_saved*/))
		{}

		if (ImGui::MenuItem("Save Scene As"))
		{}

		if (ImGui::MenuItem("Save All"))
		{}
		ImGui::Separator();

		if (ImGui::MenuItem("Unload Scene"))
		{}

		if (ImGui::MenuItem("Remove Scene"))
		{
			//if scene nodes is the only scene, delete and create default scene
			//if scene node is current_scene -> change to first scene in nodes
			std::vector<HierarchyNode*>list;
			//push back scene node and all its childs and childs of childs
			//DeleteNodes(list);
		}

		if (ImGui::MenuItem("Discard Changes", NULL, false/*, !scene_node->is_saved*/))
		{}
		ImGui::Separator();

		if (ImGui::MenuItem("Show in Assets"))
		{}

		CreateMenu();

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
	//if (ImGui::BeginMenu("Create")) //create
	//{
	//	if (ImGui::MenuItem("Folder"))
	//		CreateNode(HierarchyNode::NodeType::FOLDER);

	//	if (ImGui::MenuItem("Scene"))
	//		CreateNode(HierarchyNode::NodeType::SCENE);

	//	if (ImGui::MenuItem("GameObject"))
	//		CreateNode(HierarchyNode::NodeType::GAMEOBJECT);

	//	//if (ImGui::MenuItem("Prefab"))
	//	//	CreateNode(HierarchyNode::NodeType::PREFAB);

	//	ImGui::EndMenu();
	//}
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