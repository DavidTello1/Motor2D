#include "Hierarchy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
//#include "ModuleScene.h"
#include "GameObject.h"

#include <queue>

#include "Imgui/imgui_internal.h"
#include "mmgr/mmgr.h"

// ---------------------------------------------------------
Hierarchy::Hierarchy() : Panel("Hierarchy", ICON_HIERARCHY)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;

	current_scene = CreateNode(HierarchyNode::NodeType::SCENE, nullptr, "Default Scene");
}

Hierarchy::~Hierarchy()
{
	for (std::vector<HierarchyNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		delete *it;
	nodes.clear();
	selected_nodes.clear();
}

void Hierarchy::Draw()
{
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
	for (uint i = 0; i < nodes.size(); ++i)
	{
		// Draw Parents First
		if (nodes[i]->parent == nullptr)
			DrawNode(nodes[i]);

		// Draw Connector Lines
		if (!nodes[i]->childs.empty() && nodes[i]->type != HierarchyNode::NodeType::SCENE)
			DrawConnectorLines(nodes[i], window->DrawList);
	}
	ImGui::PopStyleVar();

	//--- Empty Space ---
	ImGui::BeginChild("Empty");

	// Unselect nodes when clicking on empty space
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))
		UnSelectAll();

	// Right Click Options
	DrawRightClick();

	ImGui::EndChild();

	//--- Scroll Areas ---
	ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
	ImVec2 size = ImVec2(ImGui::GetWindowWidth() - 26, 50);

	// Top Area
	ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, pos.y + scroll.y - 8));
	ImGui::Dummy(size);
	if (ImGui::BeginDragDropTarget())
	{
		if (scroll.y >= 1.0f)
			window->Scroll.y -= 1.0f;
		ImGui::EndDragDropTarget();
	}

	//Bottom Area
	ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, ImGui::GetWindowHeight() + scroll.y - 58));
	ImGui::Dummy(size);
	if (ImGui::BeginDragDropTarget())
	{
		if (scroll.y < window->ScrollMax.y - 1.0f)
			window->Scroll.y += 1.0f;
		ImGui::EndDragDropTarget();
	}

	//--- Shortcuts ---
	Shortcuts();
}

void Hierarchy::Shortcuts()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		// Delete
		if (App->input->Shortcut(SDL_SCANCODE_DELETE, KEY_DOWN))
		{
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}

		// Duplicate
		if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_D, KEY_DOWN) ||
			App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_D, KEY_DOWN))
		{
			DuplicateNodes(selected_nodes);
		}

		// SelectAll
		if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN) ||
			App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN))
		{
			SelectAll();
		}

		// Find
		if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN) ||
			App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN))
		{
			//FindPopup();
		}
	}
}

void Hierarchy::DrawNode(HierarchyNode* node)
{
	// Main Variables
	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;
	static ImGuiStyle* style = &ImGui::GetStyle();
	static ImVec4* colors = style->Colors;
	const float height = g.FontSize + g.Style.FramePadding.y;
	bool is_hovered = false, is_clicked = false;

	// Real Node Name
	std::string name = node->name;
	if (node->count != 0)
		name = name + std::string(" (") + std::to_string(node->count) + std::string(")");

	// Background
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + ImGui::GetWindowWidth() + ImGui::GetScrollX(), pos.y + height));
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(node->color));

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	if (ImGui::InvisibleButton(std::string(name + ICON_SHOW).c_str(), ImVec2(15, height)))
		node->is_shown = !node->is_shown;
	ImGui::SameLine();
	ImGui::SetCursorPosX(pos_x);
	if (ImGui::IsItemHovered())
	{
		is_hovered = true;
		if (node->is_shown)
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_SHOW);
		else
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_HIDE);
	}
	else
	{
		if (node->is_shown)
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.0f), ICON_SHOW);
		else
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), ICON_HIDE);
	}
	ImGui::SameLine();

	// Drag&Drop selectable (top of node)
	ImGui::SetCursorPos(ImVec2(pos_x + 15, ImGui::GetCursorPosY() - 3));
	float width = bg.Max.x - 53;
	if (ImGui::InvisibleButton(std::string(name + "dnd").c_str(), ImVec2(width, 3)))
	{
		node->selected = !node->selected;
		is_clicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		is_hovered = true;
		if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
			ImGui::SetWindowFocus();
	}
	ImGui::SetItemAllowOverlap();
	ImGui::SameLine();

	if (ImGui::BeginDragDropTarget()) // Reordering
	{
		for (uint i = 0; i < selected_nodes.size(); ++i)
		{
			if (((node->type == HierarchyNode::NodeType::SCENE && selected_nodes[i]->type == HierarchyNode::NodeType::SCENE) ||
				(node->type != HierarchyNode::NodeType::SCENE && selected_nodes[i]->type != HierarchyNode::NodeType::SCENE)) &&
				(node != selected_nodes[i]))// error handling
			{
				is_hovered = false;
				window->DrawList->AddLine(ImVec2(pos_x + 15, bg.Min.y + 1.5f), ImVec2(pos_x + 15 + width, bg.Min.y + 1.5f), ImColor(255, 255, 255, 255));

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode"))
					MoveNode(selected_nodes[i], node->parent, node, node->indent);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Selectable
	ImGui::SetCursorPos(ImVec2(pos_x + 15, ImGui::GetCursorPosY() + 3));
	if (node->rename)
		width = 34;
	if (ImGui::InvisibleButton(name.c_str(), ImVec2(width, height)))
	{
		node->selected = !node->selected;
		is_clicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		is_hovered = true;
		if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
			ImGui::SetWindowFocus();
	}
	ImGui::SetItemAllowOverlap();
	ImGui::SameLine();

	//Drag and Drop
	if (!selected_nodes.empty() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) // Source
	{
		ImGui::SetDragDropPayload("HierarchyNode", &name, sizeof(std::string));

		if (selected_nodes.size() == 1)
		{
			std::string node_name = selected_nodes[0]->name;
			if (selected_nodes[0]->count != 0)
				node_name = node_name + std::string(" (") + std::to_string(selected_nodes[0]->count) + std::string(")");
			ImGui::Text(node_name.c_str());
		}
		else
			ImGui::Text(std::to_string(selected_nodes.size()).c_str());

		ImGui::EndDragDropSource();
	}
	ImGui::SetItemAllowOverlap();

	if (ImGui::BeginDragDropTarget()) // Reparenting
	{
		for (uint i = 0; i < selected_nodes.size(); ++i)
		{
			if (selected_nodes[i]->type != HierarchyNode::NodeType::SCENE && IsChildOf(selected_nodes[i], node) == false && node != selected_nodes[i]) // error handling
			{
				HierarchyNode* last_child = GetLastChild(node);
				float w = pos_x + 15;
				float h = bg.Min.y - 1.5f + (height + 3) * float(last_child->pos - node->pos + 1);
				window->DrawList->AddLine(ImVec2(w, h), ImVec2(w + width, h), ImColor(255, 255, 255, 255));

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode"))
					MoveNode(selected_nodes[i], node, nullptr, -1);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Selection
	HandleSelection(node, is_hovered);

	// Indent
	ImGui::SetCursorPosX(pos_x + 17);
	if (node->indent > 0)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15 * (node->indent - 1));
	if (node->childs.empty())
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);
	else
	{
		// Arrow Icon
		pos_x = ImGui::GetCursorPosX();
		if (ImGui::InvisibleButton(std::string(name + ICON_ARROW_OPEN).c_str(), ImVec2(15, height)))
		{
			node->is_open = !node->is_open;
			hidden_childs = GetHiddenNodes(); //refresh hidden_childs list
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			if (node->is_open)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_ARROW_CLOSED);
		}
		else
		{
			if (node->is_open)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ICON_ARROW_CLOSED);
		}
		ImGui::SameLine(0.0f, 2.0f);
	}

	// Node Icon
	if (node->type == HierarchyNode::NodeType::FOLDER) //folder
		ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), node->icon.c_str());
	else if (node->type == HierarchyNode::NodeType::PREFAB) //prefab
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 1.0f, 1.0f), node->icon.c_str());
	else
		ImGui::Text(node->icon.c_str());
	ImGui::SameLine(0.0f, 2.0f);

	// Name
	if (!node->rename)
	{
		if (node->type == HierarchyNode::NodeType::SCENE)
		{
			NodeScene* node_scene = (NodeScene*)node;
			if (node_scene->is_saved == false)
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), name.c_str());
			else
				ImGui::Text(name.c_str());
		}
		else
			ImGui::Text(name.c_str());
	}
	else // Rename
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", node->name.c_str());

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (strchr(buffer, '(') != nullptr || strchr(buffer, ')') != nullptr)
				LOG("Error renaming node, character not valid '()'", 'e')
			else
			{
				node->name = buffer;
				node->count = 0;
				node->count = GetNameCount(node);
				node->rename = false;
			}
		}
		
		// Space after input text
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8);
		width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 8;
		if (width == 0.0f) //error handling (invisible button size cannot be 0)
			width = 0.1f;
		if (ImGui::InvisibleButton(std::string(node->name + std::to_string(node->count)).c_str(), ImVec2(width, height)))
		{
			node->rename = false;
			is_clicked = true;
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
		{
			is_hovered = true;
			if (ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
				ImGui::SetWindowFocus();
		}
	}

	// Scene Options & Edit Prefab Buttons
	if (node->type == HierarchyNode::NodeType::SCENE || node->type == HierarchyNode::NodeType::PREFAB)
	{
		std::string icon = ICON_OPTIONS;
		if (node->type == HierarchyNode::NodeType::PREFAB)
			icon = ICON_ARROW_SHOW;

		ImGui::SameLine();
		ImVec2 limit = ImGui::GetCursorPos();
		pos_x = ImGui::GetWindowWidth() - 37 + ImGui::GetScrollX();
		if (pos_x < limit.x)
			pos_x = limit.x;

		ImGui::SetCursorPos(ImVec2(pos_x, limit.y + 1));
		ImGui::InvisibleButton(std::string(name + icon).c_str(), ImVec2(15, height));
		{
			if (node->type == HierarchyNode::NodeType::SCENE) // Scenes
				ShowSceneOptions(node);
			else if (node->type == HierarchyNode::NodeType::PREFAB) //Prefabs
			{
			}	//edit prefab
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), icon.c_str());
		}
		else
			ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 0.5f), icon.c_str());
	}

	// Highlight
	if (node->type == HierarchyNode::NodeType::SCENE)
	{
		if (current_scene == node)
			node->color = ImVec4(0.25f, 0.25, 0.25f, 1.0f);
		else
			node->color = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	}
	else
	{
		if (is_hovered || is_clicked)
			node->color = colors[ImGuiCol_ButtonHovered];
		else if (node->selected)
			node->color = colors[ImGuiCol_ButtonActive];
		else
			node->color = colors[ImGuiCol_WindowBg];
	}

	// Draw Childs
	if (node->is_open && !node->childs.empty())
	{
		for (uint i = 0; i < node->childs.size(); ++i)
			DrawNode(node->childs[i]);
	}
}

HierarchyNode* Hierarchy::CreateNode(HierarchyNode::NodeType type, HierarchyNode* parent, std::string name)
{
	HierarchyNode* node = nullptr;

	switch (type)
	{
	case HierarchyNode::NodeType::FOLDER:
		if (name == "")
			name = "Folder";
		node = new NodeFolder(parent, name);
		break;
	case HierarchyNode::NodeType::GAMEOBJECT:
		if (name == "")
			name = "GameObject";
		node = new NodeGameObject(nullptr, parent); //***CHANGE NULLPTR BY EMPTY GAMEOBJECT
		break;
	case HierarchyNode::NodeType::SCENE:
		if (name == "")
			name = "Scene";
		node = new NodeScene(/*new ResourceScene()*/);
		break;
	case HierarchyNode::NodeType::PREFAB:
		if (name == "")
			name = "Prefab";
		node = new NodePrefab(/*new Prefab(), */parent);
		break;
	}

	if (node == nullptr)
	{
		LOG("Error creating node", e);
		return node;
	}

	// Name Count
	node->count = GetNameCount(node);

	// Parent
	if (parent == nullptr && type != HierarchyNode::NodeType::SCENE)
	{
		if (selected_nodes.empty() == true)
			node->parent = current_scene; //parent is current scene
		else
			node->parent = selected_nodes[0]; //parent is first selected node
	}
	else if (parent != nullptr)
		node->parent = parent;

	// Position and Indent
	if (node->parent == nullptr)
	{
		node->indent = 0;
		node->pos = nodes.size();
	}
	else
	{
		node->pos = RecursivePos(node->parent);
		node->indent = node->parent->indent + 1; //indent = parent indent + 1
		node->parent->childs.push_back(node); //add node to parent's child list
		UnSelectAll();
	}

	// Selected
	node->selected = true;

	// Add to Nodes List & Reorder by Position
	nodes.push_back(node);
	ReorderNodes(node);

	return node;
}

void Hierarchy::DeleteNodes(std::vector<HierarchyNode*> nodes_list, bool reorder)
{
	while(!nodes_list.empty())
	{
		if (reorder) //reorder list by indent (only once)
		{
			nodes_list = SortByIndent(nodes_list);
			reorder = false;
		}

		HierarchyNode* node = nodes_list.front();

		// Delete node from parent's child list
		if (node->parent != nullptr)
		{
			int pos = FindNode(node, node->parent->childs);
			if (pos != -1)
				node->parent->childs.erase(node->parent->childs.begin() + pos);
		}

		// Delete childs
		if (!node->childs.empty())
			DeleteNodes(node->childs, false);

		// Update nodes pos
		ReorderNodes(node, true);
		
		// Delete node data
		int pos = FindNode(node, nodes);
		if (pos != -1)
		{
			delete nodes[pos];
			nodes.erase(nodes.begin() + pos);
		}
		nodes_list.erase(nodes_list.begin());		
	}
	nodes_list.clear();
}

void Hierarchy::DuplicateNodes(std::vector<HierarchyNode*> nodes_list, HierarchyNode* parent)
{
	for (uint i = 0; i < nodes_list.size(); ++i)
	{
		// Parent
		HierarchyNode* final_parent = parent;
		if (parent == nullptr)
		{
			if (nodes_list[i]->parent != nullptr) // if parent is null make root
				final_parent = nodes_list[i]->parent;
		}

		// Create Node
		HierarchyNode* node = CreateNode(nodes_list[i]->type, final_parent, nodes_list[i]->name);

		// Selected (unselect source node)
		nodes_list[i]->selected = false;

		// Childs
		if (!nodes_list[i]->childs.empty()) // if node has childs, duplicate them
			DuplicateNodes(nodes_list[i]->childs, node);
	}
}

void Hierarchy::SelectAll()
{
	for (std::vector<HierarchyNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		(*it)->selected = true;
}

void Hierarchy::UnSelectAll()
{
	for (std::vector<HierarchyNode*>::iterator it = selected_nodes.begin(); it != selected_nodes.end(); ++it)
	{
		(*it)->selected = false;
		(*it)->rename = false;
	}
	selected_nodes.clear();
}

// --- MAIN HELPERS ---
HierarchyNode* Hierarchy::HandleSelection(HierarchyNode* node, bool is_hovered)
{
	// Left Click (selection)
	if (is_hovered) //if treenode is clicked, check whether it is a single or multi selection
	{
		if (ImGui::IsMouseDoubleClicked(0)) // Double-click (open folder / focus gameobject)
		{
			if (node->type == HierarchyNode::NodeType::FOLDER && !node->childs.empty())
				node->is_open = !node->is_open;
			else if (node->type == HierarchyNode::NodeType::SCENE)
				current_scene = node;
			else if (node->type == HierarchyNode::NodeType::GAMEOBJECT || node->type == HierarchyNode::NodeType::PREFAB)
			{ /*focus*/ }
		}
		else if (ImGui::IsMouseClicked(0)) // Multiple Selection
		{			
			if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
			{
				HierarchyNode* start_node = selected_nodes.back();

				uint first = start_node->pos;
				uint last = node->pos;
				if (first < last)
				{
					for (uint i = first; i < last; ++i)
						nodes[i]->selected = true;
				}
				else
				{
					for (uint i = first; i > last; --i)
						nodes[i]->selected = true;
				}
			}
		}
		else if (ImGui::IsMouseReleased(0)) // Single selection
		{
			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !ImGui::IsMouseDragging())
				UnSelectAll();
		}
		else if (ImGui::IsMouseClicked(1) && selected_nodes.size() <= 1 && node->type != HierarchyNode::NodeType::SCENE) // Right Click (select item to show options)
		{
			UnSelectAll();
			node->selected = !node->selected; //change selection state
		}
	}

	// Update Selected Nodes List
	int position = FindNode(node, selected_nodes);
	if (node->selected && position == -1)
		selected_nodes.push_back(node);
	else if (!node->selected && position != -1)
		selected_nodes.erase(selected_nodes.begin() + position);

	return node;
}

void Hierarchy::MoveNode(HierarchyNode* node, HierarchyNode* parent, HierarchyNode* pos, int indent)
{
	// If new parent is child of node (error handling)
	if (IsChildOf(node, parent) || node == parent)
		return;

	// Erase node and childs from nodes list
	std::vector<HierarchyNode*> childs_list = GetAllChilds(node);
	int position = FindNode(node, nodes);
	if (position != -1)
	{
		nodes.erase(nodes.begin() + position); //erase from nodes list (data is mantained in *node)
		ReorderNodes(node, true);
	}
	if (!childs_list.empty())
	{
		for (uint i = 0; i < childs_list.size(); ++i) //erase childs from nodes list
		{
			position = FindNode(childs_list[i], nodes);
			if (position != -1)
			{
				childs_list[i]->parent->childs.erase(childs_list[i]->parent->childs.begin() + FindNode(childs_list[i], childs_list[i]->parent->childs));
				nodes.erase(nodes.begin() + position);
				ReorderNodes(childs_list[i], true);
			}
		}
	}

	// If node has parent, delete node from parent's child list
	if (node->parent != nullptr)
		node->parent->childs.erase(node->parent->childs.begin() + FindNode(node, node->parent->childs));

	// Set indent
	if (indent == -1)
	{
		node->indent = parent->indent + 1;
		if (parent->type == HierarchyNode::NodeType::SCENE)
			node->indent = 1;
	}
	else
		node->indent = indent;

	// Set pos
	if (pos == nullptr)
	{
		if (parent->childs.empty())
			node->pos = parent->pos + 1;
		else
			node->pos = GetLastChild(parent)->pos + 1;
	}
	else
		node->pos = pos->pos;

	// Add to nodes list and Reorder all nodes
	nodes.push_back(node);
	ReorderNodes(node);

	// Set parent and add to child list
	if (parent != nullptr)
	{
		node->parent = parent;
		parent->childs.push_back(node);
		parent->childs = SortByPosition(parent->childs); //order childs (needed for reordering correctly)
	}

	// Move Childs
	if (!childs_list.empty())
	{
		for (uint i = 0; i < childs_list.size(); ++i)
		{
			UpdateNode(childs_list[i]); //update node's pos and indent to new parent's pos
			childs_list[i]->parent->childs.push_back(childs_list[i]); //add node back to parent's child list
			nodes.push_back(childs_list[i]); //add node to nodes_list
			ReorderNodes(childs_list[i]); //reorder nodes
		}
	}
}

bool Hierarchy::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Hierarchy"))
	{
		CreateMenu();

		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, !selected_nodes.empty())) //duplicate
			DuplicateNodes(selected_nodes);

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

		if (ImGui::MenuItem("Select All", NULL, false, !nodes.empty())) //select all
			SelectAll();

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
			selected_nodes.front()->rename = true;

		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
		{
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Search", "Ctrl+F")) //search
		{

		}

		ImGui::EndPopup();
		return true;
	}
	return false;
}

bool Hierarchy::ShowSceneOptions(HierarchyNode* node)
{
	if (ImGui::BeginPopupContextItem(0,0))
	{
		NodeScene* scene_node = (NodeScene*)node;

		if (ImGui::MenuItem("Set Active Scene"))
			current_scene = node;
		ImGui::Separator();

		if (ImGui::MenuItem("Save Scene", NULL, false, !scene_node->is_saved))
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

		if (ImGui::MenuItem("Discard Changes", NULL, false, !scene_node->is_saved))
		{}
		ImGui::Separator();

		if (ImGui::MenuItem("Show in Assets"))
		{}

		CreateMenu();

		ImGui::EndPopup();
		return true;
	}
	return false;
}

void Hierarchy::DrawConnectorLines(HierarchyNode* node, ImDrawList* draw_list)
{
	// Error handling
	if (node == nullptr)
	{
		LOG("node was NULL", 'e');
		return;
	}

	// If any parent is closed do not draw
	bool draw = true;
	HierarchyNode* tmp_node = node;
	while (tmp_node != nullptr)
	{
		if (!tmp_node->is_open)
			return;

		tmp_node = tmp_node->parent;
	}

	// Get number of hidden childs that actually affect the node
	uint num_hidden = hidden_childs.size();
	uint num_hidden2 = 0;
	for (uint i = 0; i < hidden_childs.size(); ++i)
	{
		// If any of the hidden_childs' pos > node pos and is not child (or child of childs) of node, substract them from count
		if (hidden_childs[i]->pos > node->pos && !IsChildOf(node, hidden_childs[i]))
			num_hidden--;

		// If any of the hidden_childs are childs of last child, substract them from count
		if (IsChildOf(node->childs[node->childs.size() - 1], hidden_childs[i]))
			num_hidden--;

		// Get num_hidden2 for initial_pos of parent
		if (hidden_childs[i]->pos < node->pos)
			num_hidden2++;
	}

	// Positions
	uint last_child_pos = (uint)node->childs[node->childs.size() - 1]->pos - num_hidden;  //get last_child_pos updated to hidden childs
	uint parent_pos = (uint)node->pos - num_hidden2;

	// Real Positions
	ImVec2 initial_pos = ImVec2(ImGui::GetWindowPos().x + 1 + 15 * float(node->indent + 1), ImGui::GetWindowPos().y + 38 + 19 * (float)parent_pos); //initial pos
	ImVec2 final_pos = ImVec2(initial_pos.x, ImGui::GetWindowPos().y + 34 + 19 * (float)last_child_pos); //final pos

	// Connector Lines
	draw_list->AddLine(ImVec2(initial_pos.x - ImGui::GetScrollX(), initial_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // vertical line
	draw_list->AddLine(ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x + 7 - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // horizontal line
}

int Hierarchy::FindNode(HierarchyNode* node, std::vector<HierarchyNode*> list)
{
	for (uint i = 0; i <list.size(); ++i)
	{
		if (list[i] == node)
			return i;
	}
	return -1;
}

// --- NODE CREATION ---
void Hierarchy::CreateMenu()
{
	if (ImGui::BeginMenu("Create")) //create
	{
		if (ImGui::MenuItem("Folder"))
			CreateNode(HierarchyNode::NodeType::FOLDER);

		if (ImGui::MenuItem("Scene"))
			CreateNode(HierarchyNode::NodeType::SCENE);

		if (ImGui::MenuItem("GameObject"))
			CreateNode(HierarchyNode::NodeType::GAMEOBJECT);

		//if (ImGui::MenuItem("Prefab"))
		//	CreateNode(HierarchyNode::NodeType::PREFAB);

		ImGui::EndMenu();
	}
}

uint Hierarchy::GetNameCount(HierarchyNode* node)
{
	std::vector<HierarchyNode*> repeated_nodes;
	for (uint i = 0; i < nodes.size(); ++i) //get nodes with repeated name and highest name count
	{
		if (node != nodes[i] && node->name == nodes[i]->name)
		{
			repeated_nodes.push_back(nodes[i]);
			if (node->count <= nodes[i]->count)
				node->count = nodes[i]->count;
		}
	}

	if (!repeated_nodes.empty()) //if name is repeated
	{
		if (repeated_nodes.size() <= node->count) //if there are gaps between the name counts
		{
			node->count = 0;
			bool found = false;

			while (found == false)
			{
				for (uint i = 0; i < repeated_nodes.size(); ++i) //if count is repeated increase it and try again, if not repeated set count
				{
					if (repeated_nodes[i]->count == node->count)
					{
						repeated_nodes.erase(repeated_nodes.begin() + i); //if count is repeated, erase repeated count node from repeated_nodes list
						node->count++;
						break;
					}
					else if (i == repeated_nodes.size() - 1)
					{
						found = true;
						break;
					}

				}
			}
		}
		else //if there are no gaps
			node->count++;
	}

	return node->count;
}

// --- NODE POS ---
void Hierarchy::ReorderNodes(HierarchyNode* node, bool is_delete)
{
	for (uint i = node->pos; i < nodes.size(); ++i)
	{
		if (nodes[i] == node)
			continue;
		else
		{
			if (is_delete)
				nodes[i]->pos--;
			else
				nodes[i]->pos++;
		}
	}
	nodes = SortByPosition(nodes);
}

uint Hierarchy::RecursivePos(HierarchyNode* node)
{
	if (node->childs.empty())
		return node->pos + 1;
	else
		return RecursivePos(node->childs[node->childs.size() - 1]);
}

void Hierarchy::UpdateNode(HierarchyNode* node)
{
	if (node->parent->childs.empty())
		node->pos = node->parent->pos + 1;
	else
		node->pos = GetLastChild(node->parent)->pos + 1;
	node->indent = node->parent->indent + 1;
}

// --- CONNECTOR LINES ---
std::vector<HierarchyNode*> Hierarchy::GetHiddenNodes()
{
	std::vector<HierarchyNode*> hidden;
	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i]->type == HierarchyNode::NodeType::SCENE)
		{
			if (!nodes[i]->is_open) //if closed add all childs to list
			{
				std::vector<HierarchyNode*> tmp_list = GetAllChilds(nodes[i]);
				hidden.insert(hidden.end(), tmp_list.begin(), tmp_list.end());
			}
			else // add closed childs' childs to list
			{
				std::vector<HierarchyNode*> tmp_list = GetClosedChilds(nodes[i]);
				hidden.insert(hidden.end(), tmp_list.begin(), tmp_list.end()); //add tmp_list to hidden_childs
			}
		}
	}
	return hidden;
}

std::vector<HierarchyNode*> Hierarchy::GetAllChilds(HierarchyNode* node)
{
	std::vector<HierarchyNode*> num_childs;
	for (uint i = 0; i < node->childs.size(); ++i)
	{
		num_childs.push_back(node->childs[i]);

		if (!node->childs[i]->childs.empty()) //if child has childs add them to list
		{
			std::vector<HierarchyNode*> tmp_list = GetAllChilds(node->childs[i]); //get all childs
			num_childs.insert(num_childs.end(), tmp_list.begin(), tmp_list.end()); //add childs list to hidden_childs
		}
	}
	return num_childs;
}

std::vector<HierarchyNode*> Hierarchy::GetClosedChilds(HierarchyNode* node)
{
	std::vector<HierarchyNode*> hidden_childs;
	for (uint i = 0; i < node->childs.size(); ++i)
	{
		if (!node->childs[i]->is_open) //if node is closed add childs to list
		{
			std::vector<HierarchyNode*> tmp_list = GetAllChilds(node->childs[i]); //get all childs
			hidden_childs.insert(hidden_childs.end(), tmp_list.begin(), tmp_list.end()); //add childs list to hidden_childs
		}
		else
		{
			std::vector<HierarchyNode*> tmp_list = GetClosedChilds(node->childs[i]); //if node is open check if any child is closed
			hidden_childs.insert(hidden_childs.end(), tmp_list.begin(), tmp_list.end()); //add tmp_list to hidden_childs
		}
	}
	return hidden_childs;
}

bool Hierarchy::IsChildOf(HierarchyNode* parent, HierarchyNode* node)
{
	if (parent != nullptr && node != nullptr && node->parent != nullptr)
	{
		if (node->parent == parent)
			return true;
		else
			return IsChildOf(parent, node->parent);
	}
	return false;
}

HierarchyNode* Hierarchy::GetLastChild(HierarchyNode* node)
{
	if (node->childs.empty())
		return node;
	else
		return GetLastChild(node->childs.back());
}

// --- SORTERS ---
std::vector<HierarchyNode*> Hierarchy::SortByPosition(std::vector<HierarchyNode*> list)
{
	std::priority_queue<HierarchyNode*, std::vector<HierarchyNode*>, PositionSort> ListOrder;

	for (HierarchyNode* node : list) //push nodes into Ordered List
		ListOrder.push(node);

	list.clear(); //clear list

	while (ListOrder.empty() == false) //push Ordered List into New List
	{
		list.push_back(ListOrder.top());
		ListOrder.pop();
	}
	return list;
}

std::vector<HierarchyNode*> Hierarchy::SortByIndent(std::vector<HierarchyNode*> list)
{
	std::priority_queue<HierarchyNode*, std::vector<HierarchyNode*>, IndentSort> ListOrder;

	for (HierarchyNode* node : list) //push nodes into Ordered List
		ListOrder.push(node);

	list.clear(); //clear list

	while (ListOrder.empty() == false) //push Ordered List into New List
	{
		list.push_back(ListOrder.top());
		ListOrder.pop();
	}
	return list;
}
