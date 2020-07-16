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

	current_scene = CreateNode(HierarchyNode::NodeType::SCENE, nullptr, "Default Scene");

	flags = ImGuiWindowFlags_HorizontalScrollbar;
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
	// Allow selection & show options with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(1))
		ImGui::SetWindowFocus();

	// Right Click Options
	DrawRightClick();

	// Draw Nodes
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	for (HierarchyNode* node : nodes)
	{
		// Draw Parents First
		if (node->parent == nullptr)
			DrawNode(node);

		// Draw Connector Lines
		if (!node->childs.empty() && node->type != HierarchyNode::NodeType::SCENE)
			DrawConnectorLines(node, draw_list);
	}

	//--- Empty Space ---
	ImGui::BeginChild("Empty");

	// Unselect nodes when clicking on empty space
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))
		UnSelectAll();

	// Right Click Options
	DrawRightClick();

	ImGui::EndChild();

	//--- Shortcuts ---
	Shortcuts();
}

void Hierarchy::Shortcuts()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		// Delete
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}

		// Duplicate
		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) ||
			(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN))
		{
			DuplicateNodes(selected_nodes);
		}

		// SelectAll
		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) ||
			(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN))
		{
			SelectAll();
		}

		// Find
		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) ||
			(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN))
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
	ImU32 id = window->GetID(node->name.c_str());
	static ImGuiStyle* style = &ImGui::GetStyle();
	static ImVec4* colors = style->Colors;
	const ImU32 color = ImColor(node->color);
	const float height = g.FontSize + g.Style.FramePadding.y;
	bool is_hovered, is_clicked;

	// Background
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + ImGui::GetWindowWidth() + ImGui::GetScrollX(), pos.y + height));
	window->DrawList->AddRectFilled(bg.Min, bg.Max, color);

	// Selectable
	float limit_x = 0;
	if (node->type == HierarchyNode::NodeType::SCENE || node->type == HierarchyNode::NodeType::PREFAB)
		limit_x = 20;

	float width = bg.Max.x - limit_x;
	if (node->rename)
		width = pos.x + 50;

	if (ImGui::ButtonBehavior(ImRect(pos.x + 28, bg.Min.y, width, bg.Max.y), id, &is_hovered, &is_clicked, ImGuiButtonFlags_PressedOnRelease))
			node->selected = !node->selected;

	//if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
	//	ImGui::SetWindowFocus();

	// Selection
	HandleSelection(node, is_hovered);

	// Shown Icon
	float pos_x = ImGui::GetCursorPosX();
	if (ImGui::InvisibleButton(std::string(node->name + ICON_SHOW).c_str(), ImVec2(15, height)))
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

	// Small Space
	ImGui::SetCursorPosX(pos_x + 15);
	width = 5;
	if (node->childs.empty())
		width = 15;
	
	if (ImGui::InvisibleButton(std::string(node->name + "ss").c_str(), ImVec2(width, height)))
		node->selected = !node->selected;
	ImGui::SetItemAllowOverlap();

	if (ImGui::IsItemHovered())
		is_hovered = true;
	ImGui::SameLine();

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
		if (ImGui::InvisibleButton(std::string(node->name + ICON_ARROW_OPEN).c_str(), ImVec2(15, height)))
			node->is_open = !node->is_open;

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
				ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), ICON_ARROW_OPEN);
			else
				ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), ICON_ARROW_CLOSED);
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
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), node->name.c_str());
			else
				ImGui::Text(node->name.c_str());
		}
		else
			ImGui::Text(node->name.c_str());
	}
	else
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", node->name.c_str());

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			node->name = CreateName(buffer);
			node->rename = false;
		}
	}

	// Scene Options & Edit Prefab Buttons
	if (node->type == HierarchyNode::NodeType::SCENE)
	{
		ImGui::SameLine();
		ImVec2 limit = ImGui::GetCursorPos();
		pos_x = ImGui::GetWindowWidth() - 23;
		if (pos_x < limit.x)
			pos_x = limit.x;

		ImGui::SetCursorPos(ImVec2(pos_x, limit.y + 1));
		ImGui::InvisibleButton(std::string(node->name + ICON_OPTIONS).c_str(), ImVec2(15, height));

		ShowSceneOptions(node);

		ImGui::SameLine();
		ImGui::SetCursorPosX(pos_x);
		if (ImGui::IsItemHovered())
		{
			is_hovered = true;
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_OPTIONS);
		}
		else
			ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 0.5f), ICON_OPTIONS);
	}
	else if (node->type == HierarchyNode::NodeType::PREFAB)
	{
		ImGui::SameLine();
		ImVec2 limit = ImGui::GetCursorPos();
		pos_x = ImGui::GetWindowWidth() - 23;
		if (pos_x < limit.x)
			pos_x = limit.x;

		ImGui::SetCursorPos(ImVec2(pos_x, limit.y + 1));
		if (ImGui::InvisibleButton(std::string(node->name + ICON_ARROW_SHOW).c_str(), ImVec2(15, height)))
		{}	//edit prefab

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
	if (node->type == HierarchyNode::NodeType::SCENE)
	{
		if (current_scene == node)
			node->color = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
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
		for (HierarchyNode* child : node->childs)
			DrawNode(child);
	}
}

HierarchyNode* Hierarchy::CreateNode(HierarchyNode::NodeType type, HierarchyNode* parent, std::string name)
{
	HierarchyNode* node = nullptr;

	switch (type)
	{
	case HierarchyNode::NodeType::FOLDER:
		if (name != "")
			node = new NodeFolder(parent, name);
		else
			node = new NodeFolder(parent);
		break;
	case HierarchyNode::NodeType::GAMEOBJECT:
		if (name != "")
			node = new NodeGameObject(nullptr, parent, name);
		else
			node = new NodeGameObject(nullptr, parent); //***CHANGE NULLPTR BY EMPTY GAMEOBJECT
		break;
	case HierarchyNode::NodeType::SCENE:
		if (name != "")
			node = new NodeScene(/*new ResourceScene(),*/ name);
		else
			node = new NodeScene(/*new ResourceScene()*/);
		break;
	case HierarchyNode::NodeType::PREFAB:
		if (name != "")
			node = new NodePrefab(/*new Prefab(), */parent, name);
		else
			node = new NodePrefab(/*new Prefab(), */parent);
		break;
	}

	if (node == nullptr)
	{
		LOG("Error creating node", e);
		return node;
	}

	// Name Count
	node->name = CreateName(node->name.c_str());

	// Parent
	if (parent == nullptr && type != HierarchyNode::NodeType::SCENE)
	{
		if (selected_nodes.empty() == false)
			node->parent = selected_nodes[0]; //parent is first selected node
		else
			node->parent = current_scene; //parent is current scene
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
		if (reorder) //reorder list (only once)
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
		delete nodes[pos];
		nodes.erase(nodes.begin() + pos);
		nodes_list.erase(nodes_list.begin());		
	}
	nodes_list.clear();
}

void Hierarchy::DuplicateNodes(std::vector<HierarchyNode*> nodes_list, HierarchyNode* parent)
{
	uint size = nodes_list.size();
	for (uint i = 0; i < size; ++i)
	{
		HierarchyNode* node = nullptr;

		switch (nodes_list[i]->type)
		{
		case HierarchyNode::NodeType::FOLDER:
			node = new NodeFolder(parent);
			break;
		case HierarchyNode::NodeType::GAMEOBJECT:
			node = new NodeGameObject(nullptr, parent); //***CHANGE NULLPTR BY EMPTY GAMEOBJECT
			break;
		case HierarchyNode::NodeType::SCENE:
			node = new NodeScene(/*new ResourceScene()*/);
			break;
		case HierarchyNode::NodeType::PREFAB:
			//node = new NodePrefab(new Prefab(), parent);
			break;
		}

		if (node == nullptr)
		{
			LOG("Error creating node", e);
			return;
		}

		// Name
		std::string name = nodes_list[i]->name.substr(0, nodes_list[i]->name.find_first_of(" ("));
		node->name = CreateName(name.c_str());

		// Selected (unselect source node)
		nodes_list[i]->selected = false;
		node->selected = true;

		// Parent
		if (parent == nullptr)
		{
			if (nodes_list[i]->parent != nullptr) // if parent is null make root
			{
				node->parent = nodes_list[i]->parent;
			}
		}
		else // if defined parent node (parent is a duplicated node)
		{
			node->parent = parent;
		}

		// Position and Indent
		node->indent = nodes_list[i]->indent;
		if (node->parent == nullptr)
			node->pos = nodes.size();
		else
		{
			node->pos = RecursivePos(node->parent, false);
			node->parent->childs.push_back(node); //add node to parent's child list
		}

		// Add to nodes list & Reorder by Position
		nodes.push_back(node);
		ReorderNodes(node);

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
		else if (ImGui::IsMouseClicked(0))
		{
			if (!ImGui::GetIO().KeyCtrl) // Single selection
			{
				if (node->selected && selected_nodes.size() == 1) // Rename
					node->rename = true;
				else
					UnSelectAll();
			}
			else if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
			{

			}
		}

		// Right Click (select item to show options)
		if (ImGui::IsMouseClicked(1) && selected_nodes.size() <= 1 && node->type != HierarchyNode::NodeType::SCENE)
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
	if (node == nullptr)
		return;

	// If any parent is closed do not draw
	bool draw = true;
	HierarchyNode* tmp_node = node;
	while (tmp_node != nullptr)
	{
		if (!tmp_node->is_open)
			return;

		tmp_node = tmp_node->parent;
	}

	// Get all hidden nodes
	std::vector<HierarchyNode*> hidden_childs = GetHiddenNodes(); //*** ONLY CALL GetHiddenNodes() WHEN A NODE IS OPENED/CLOSED

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

	// Color
	static ImVec4 colorf = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	const ImU32 color = ImColor(colorf);

	// Positions
	uint last_child_pos = (uint)node->childs[node->childs.size() - 1]->pos - num_hidden;  //get last child pos updated to hidden childs
	uint parent_pos = (uint)node->pos - num_hidden2;

	// Real Positions
	ImVec2 initial_pos = ImVec2(ImGui::GetWindowPos().x + 1 + 15 * float(node->indent + 1), ImGui::GetWindowPos().y + 38 + 20 * (float)parent_pos); //initial pos
	ImVec2 final_pos = ImVec2(initial_pos.x, ImGui::GetWindowPos().y + 34 + 20 * (float)last_child_pos); //final pos

	// Connector Lines
	draw_list->AddLine(ImVec2(initial_pos.x - ImGui::GetScrollX(), initial_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), color); // vertical line
	draw_list->AddLine(ImVec2(final_pos.x - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), ImVec2(final_pos.x + 7 - ImGui::GetScrollX(), final_pos.y - ImGui::GetScrollY()), color); // horizontal line
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

std::string Hierarchy::CreateName(const char* base_name)
{
	if (nodes.empty())
		return std::string(base_name);

	bool found = false;
	uint count = 0;
	std::string name = base_name;

	while (found == false)
	{
		for (uint i = 0; i < nodes.size(); ++i)
		{
			if (name == nodes[i]->name)
			{
				count++;
				name = base_name + std::string(" (") + std::to_string(count) + std::string(")");
				break;
			}
			else if (i == nodes.size() - 1)
			{
				found = true;
				break;
			}
		}
	}

	return name;
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

uint Hierarchy::RecursivePos(HierarchyNode* node, bool is_duplicate)
{
	if (node->childs.empty())
	{
		if (is_duplicate)
			return node->pos;
		else
			return node->pos + 1;
	}
	else
		return RecursivePos(node->childs[node->childs.size() - 1]);
}

// --- CONNECTOR LINES ---
std::vector<HierarchyNode*> Hierarchy::GetHiddenNodes()
{
	std::vector<HierarchyNode*> hidden;
	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i]->parent == nullptr) //** CHANGE TO IF PARENT->TYPE == SCENE
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
	if (node->parent != nullptr)
	{
		if (node->parent == parent)
			return true;
		else
			return IsChildOf(parent, node->parent);
	}
	return false;
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
