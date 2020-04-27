#include "Hierarchy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
//#include "ModuleScene.h"
#include "GameObject.h"

#include <queue>

#include "mmgr/mmgr.h"

// ---------------------------------------------------------
Hierarchy::Hierarchy() : Panel("Hierarchy")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

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
		if (node != nullptr && !node->childs.empty() && node->type != HierarchyNode::NodeType::SCENE)
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

void Hierarchy::DrawNode(HierarchyNode* node)
{
	// Node Parameters
	node = NodeParams(node);

	// Normal Node
	if (!node->rename)
	{
		// Indent
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15 * node->indent);
		if (node->childs.empty())
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 15);

		// Draw Node
		node->is_open = ImGui::TreeNodeEx(node->name.c_str(), node->flags);

		if (node->type == HierarchyNode::NodeType::FOLDER) //push is in NodeParams()
			ImGui::PopStyleColor();

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
			ImGui::SetWindowFocus();

		// Selection
		HandleSelection(node);

		// Update Selected Nodes List
		int pos = FindNode(node, selected_nodes);
		if (node->selected && pos == -1)
			selected_nodes.push_back(node);
		else if (!node->selected && pos != -1)
			selected_nodes.erase(selected_nodes.begin() + pos);

		// Draw Childs
		if (node->is_open)
		{
			if (!node->childs.empty())
			{
				for (HierarchyNode* child : node->childs)
					DrawNode(child);
			}
		}
	}
	else // Rename
	{
		if (selected_nodes.size() == 1) //rename if only 1 selected node
		{
			char buffer[128];
			sprintf_s(buffer, 128, "%s", node->name.c_str());
			if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				uint count = CountNode(buffer);
				if (count > 0)
					node->name = buffer + std::string(" (") + std::to_string(count) + std::string(")");
				else
					node->name = buffer;
				node->rename = false;
			}
		}
	}
}

HierarchyNode* Hierarchy::CreateNode(HierarchyNode::NodeType type, HierarchyNode* parent)
{
	HierarchyNode* node = nullptr;

	switch (type)
	{
	case HierarchyNode::NodeType::FOLDER:
		node = new NodeFolder(parent);
		break;
	case HierarchyNode::NodeType::GAMEOBJECT:
		node = new NodeGameObject(nullptr, parent); //***CHANGE NULLPTR BY EMPTY GAMEOBJECT
		break;
	case HierarchyNode::NodeType::SCENE:
		node = new NodeScene(/*new ResourceScene(),*/ parent);
		break;
	case HierarchyNode::NodeType::PREFAB:
		//node = new NodePrefab(new Prefab(), parent);
		break;
	}

	if (node == nullptr)
	{
		LOG("Error creating node", e);
		return node;
	}

	// Name Count
	uint count = CountNode(node->name.c_str());
	if (count > 0)
		node->name = node->name + std::string(" (") + std::to_string(count) + std::string(")");

	// Parent
	if (parent == nullptr && selected_nodes.empty() == false)
		node->parent = selected_nodes[0]; //parent is first selected node
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
		node->parent->flags |= ImGuiTreeNodeFlags_DefaultOpen; //make node open (display childs)
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
			node = new NodeScene(/*new ResourceScene(),*/ parent);
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

		// Selected (unselect source node)
		nodes_list[i]->selected = false;
		node->selected = true;

		// Parent
		if (parent == nullptr) //if no defined parent node, make parent root or selected node
		{
			if (nodes_list[i]->parent != nullptr) // if parent is null make root
			{
				node->parent = nodes_list[i]->parent;
				nodes_list[i]->parent->flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}
		}
		else // if defined parent node (parent is a duplicated node)
		{
			node->parent = parent;
			parent->flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		// Position and Indent
		node->indent = nodes_list[i]->indent;
		if (node->parent == nullptr)
			node->pos = nodes.size();
		else
		{
			node->pos = RecursivePos(node->parent);
			node->parent->childs.push_back(node); //add node to parent's child list
		}

		// Childs
		if (!nodes_list[i]->childs.empty()) // if node has childs, duplicate them
			DuplicateNodes(nodes_list[i]->childs, node);

		// Add node count to name
		node->name = name + std::string(" (") + std::to_string(CountNode(name.c_str())) + std::string(")");

		// Add to nodes list & Reorder by Position
		nodes.push_back(node);
		ReorderNodes(node);
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

int Hierarchy::FindNode(HierarchyNode* node, std::vector<HierarchyNode*> list)
{
	for (uint i = 0; i <list.size(); ++i)
	{
		if (list[i] == node)
			return i;
	}
	return -1;
}

HierarchyNode* Hierarchy::NodeParams(HierarchyNode* node)
{
	// Flags
	if (node->childs.empty()) // leaf
		node->flags |= ImGuiTreeNodeFlags_Leaf;
	else
		node->flags &= ~ImGuiTreeNodeFlags_Leaf;

	if (node->selected) // selected
		node->flags |= ImGuiTreeNodeFlags_Selected;
	else
		node->flags &= ~ImGuiTreeNodeFlags_Selected;

	// Type Params
	if (node->type == HierarchyNode::NodeType::FOLDER && !node->rename) //folder
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.0f, 1.0f));
	}
	else if (node->type == HierarchyNode::NodeType::SCENE) //scene
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		static ImVec4 colorf = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		const ImU32 color = ImColor(colorf);
		static float pos;
		static bool first = false;

		// Draw Scene Background
		if (node->pos == 0)
		{
			first = true;
			pos = ImGui::GetCursorPosY() + 16;
		}
		else
		{
			first = false;
			pos = ImGui::GetCursorPosY() + 18;
		}

		draw_list->AddRectFilled(ImVec2(0, pos), ImVec2(ImGui::GetWindowWidth(), pos + 15), color); //actual draw of background

		if (first)
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
	}

	return node;
}

HierarchyNode* Hierarchy::HandleSelection(HierarchyNode* node)
{
	// Left Click (selection)
	if (ImGui::IsItemClicked(0)) //if treenode is clicked, check whether it is a single or multi selection
	{
		if (ImGui::IsMouseDoubleClicked(0) && selected_nodes.size() == 1) // Double-click (rename)
			node->rename = true;
		else
		{
			if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
				UnSelectAll();

			node->selected = !node->selected; //change selection state
		}
	}

	// Right Click (select item to show options)
	if (ImGui::IsItemClicked(1) && selected_nodes.size() <= 1)
	{
		UnSelectAll();
		node->selected = !node->selected; //change selection state
	}

	return node;
}

uint Hierarchy::CountNode(const char* name)
{
	uint count = 0;
	for (uint i = 0; i < nodes.size(); ++i)
	{
		std::string node_name = nodes[i]->name.substr(0, nodes[i]->name.find_first_of("(") - 1);

		if (strcmp(name, node_name.c_str()) == 0)
			count++;
	}
	return count;
}

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
	{
		uint num_childs = node->childs.size() - 1;
		HierarchyNode* last_child = node->childs[num_childs];
		return RecursivePos(last_child);
	}
}

void Hierarchy::DrawConnectorLines(HierarchyNode* node, ImDrawList* draw_list)
{
	// Check if any parent is closed
	bool draw = true;
	HierarchyNode* tmp_node = node;
	while (tmp_node != nullptr)
	{
		if (!tmp_node->is_open)
		{
			draw = false;
			break;
		}
		tmp_node = tmp_node->parent;
	}

	// Actual draw
	if (draw)
	{
		uint last_child_pos = (uint)node->childs[node->childs.size() - 1]->pos; //***ADAPT POS TO SEE IF ANY CHILD IS CLOSED

		static ImVec4 colorf = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
		const ImU32 color = ImColor(colorf);
		ImVec2 initial_pos = ImVec2(3 + 15 * (node->indent + 1), 60 + 17 * node->pos);
		ImVec2 final_pos = ImVec2(initial_pos.x, 53 + 17 * last_child_pos);

		draw_list->AddLine(initial_pos, final_pos, color); // vertical line
		draw_list->AddLine(final_pos, ImVec2(final_pos.x + 7, final_pos.y), color); // horizontal line
	}
}

bool Hierarchy::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Hierarchy"))
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

		ImGui::EndPopup();
		return true;
	}
	return false;
}

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

void Hierarchy::Shortcuts()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) // Delete
		{
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}

		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) || // Duplicate
			(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN))
		{
			DuplicateNodes(selected_nodes);
		}

		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) || // SelectAll
			(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN))
		{
			SelectAll();
		}
	}
}