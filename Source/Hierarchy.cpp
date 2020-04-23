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
	for (HierarchyNode* node : nodes)
	{
		if (node->parent == nullptr)
			DrawNode(node);
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

	// Rename
	if (node->rename)
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", node->name.c_str());
		if (ImGui::InputText("##RenameNode", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			node->name = buffer;
			node->rename = false;
		}
	}
	else
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15 * node->indent); //set indent

		// Draw Node
		bool is_open = ImGui::TreeNodeEx(node->name.c_str(), node->flags);

		if (node->is_folder && !node->rename) //push is in NodeParams()
			ImGui::PopStyleColor();

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
			ImGui::SetWindowFocus();

		// Left Click (selection)
		if (ImGui::IsItemClicked(0)) //if treenode is clicked, check whether it is a single or multi selection
		{
			if (ImGui::IsMouseDoubleClicked(0)) // Double-click (rename)
				node->rename = true;
			else
			{
				if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
					UnSelectAll();

				node->selected = !node->selected; //change selection state
			}
		}

		// Right Click (select item to show options)
		if (ImGui::IsItemClicked(1))
		{
			UnSelectAll();
			node->selected = !node->selected; //change selection state
		}

		// Add/Remove from selected_nodes list
		int pos = FindNode(node, selected_nodes);
		if (node->selected && pos == -1)
			selected_nodes.push_back(node);
		else if (!node->selected && pos != -1)
			selected_nodes.erase(selected_nodes.begin() + pos);

		// Draw Childs
		if (is_open)
		{
			if (!node->childs.empty())
			{
				for (HierarchyNode* child : node->childs)
					DrawNode(child);
			}
		}
	}
}

HierarchyNode* Hierarchy::CreateNode(const char* name, bool is_folder, HierarchyNode* parent, bool selected, GameObject* object/*, ResourceScene* scene*/)
{
	HierarchyNode* node = new HierarchyNode();

	// Name
	node->name = name;
	uint count = CountNode(name);
	if (count > 0)
		node->name = name + std::string(" (") + std::to_string(count) + std::string(")");

	// Parent
	if (parent == nullptr && selected_nodes.empty() == false)
		node->parent = selected_nodes[0]; //parent is first selected node
	else if (parent != nullptr)
		node->parent = parent;

	// Position & Indent
	if (node->parent == nullptr)
	{
		node->pos = (int)nodes.size();
		node->indent = 0;
	}
	else
	{
		if (node->parent->childs.empty()) //if parent has no childs (pos = parent pos + 1)
			node->pos = node->parent->pos + 1;
		else
			node->pos = node->parent->childs.back()->pos + 1; //if parent has childs (pos = last child pos + 1)

		node->indent = node->parent->indent + 1; //indent = parent indent + 1
		node->parent->childs.push_back(node); //add node to parent's child list
		node->parent->flags |= ImGuiTreeNodeFlags_DefaultOpen; //make node open (display childs)
		UnSelectAll();
	}
	for (uint i = node->pos; i < nodes.size(); ++i) // update Nodes with new positions
		nodes[i]->pos++;

	// Selected
	node->selected = selected;

	// Node Type (folder, object or scene)
	if (is_folder == true)
		node->is_folder = is_folder;
	else
	{
		if (object != nullptr)
			node->object = object;
		//else if (scene != nullptr)
		//{
		//	node->scene = scene;
		//	node->flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		//}
	}

	// Add to Nodes List & Reorder by Position
	nodes.push_back(node);
	nodes = SortByPosition(nodes);

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

		// Delete node data
		int pos = FindNode(node, nodes);
		delete nodes[pos];
		nodes.erase(nodes.begin() + pos);
		nodes_list.erase(nodes_list.begin());

		// Update nodes pos
		
	}
	nodes_list.clear();
}

void Hierarchy::DuplicateNodes(std::vector<HierarchyNode*> nodes_list, HierarchyNode* parent)
{
	uint size = nodes_list.size();
	for (uint i = 0; i < size; ++i)
	{
		HierarchyNode* node = new HierarchyNode();

		std::string name = nodes_list[i]->name.substr(0, nodes_list[i]->name.find_first_of("(") - 1);
		node->name =  name + std::string(" (") + std::to_string(CountNode(name.c_str())) + std::string(")");
		node->indent = nodes_list[i]->indent;
		node->is_folder = nodes_list[i]->is_folder;

		nodes_list[i]->selected = false;
		node->selected = true;

		if (nodes_list[i]->object != nullptr) // if object != nullptr, ResourceScene is not read
			node->object = nodes_list[i]->object;

		//else if (scene != nullptr)
		//{
		//	node->scene = nodes_list[i]->scene;
		//	node->flags = nodes_list[i]->flags;
		//}

		if (parent == nullptr) //if no defined parent node, make parent root or selected node
		{
			if (nodes_list[i]->parent != nullptr) // if parent is null make root
			{
				node->parent = nodes_list[i]->parent;
				nodes_list[i]->parent->childs.push_back(node);
				nodes_list[i]->parent->flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}
		}
		else // if defined parent node (parent is a duplicated node)
		{
			node->parent = parent;
			parent->childs.push_back(node);
			parent->flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		if (!nodes_list[i]->childs.empty()) // if node has childs, duplicate them
			DuplicateNodes(nodes_list[i]->childs, node);

		nodes.push_back(node);
		//OrderHierarchy(); //order hierarchy nodes
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

	// Folder Params
	if (node->is_folder && !node->rename)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.0f, 1.0f));

	//// Scene Params
	//if (node->scene != nullptr)
	//{
	//	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//	static ImVec4 colorf = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	//	const ImU32 color = ImColor(colorf);
	//	static float pos = ImGui::GetCursorPosY();
	//	static bool first = false;

	//	// Draw Scene Background
	//	if (node->pos == 0)
	//	{
	//		first = true;
	//		pos += 16;
	//	}
	//	else
	//	{
	//		first = false;
	//		pos += 18;
	//	}

	//	draw_list->AddRectFilled(ImVec2(0, pos), ImVec2(ImGui::GetWindowWidth(), pos + 15), color); //actual draw of background

	//	if (first)
	//		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
	//}

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

bool Hierarchy::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Hierarchy"))
	{
		if (ImGui::BeginMenu("Create")) //create
		{
			if (ImGui::MenuItem("Folder"))
				CreateNode("Folder", true);

			if (ImGui::MenuItem("Scene"))
				CreateNode("Scene");

			if (ImGui::MenuItem("GameObject"))
				CreateNode("GameObject");

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
	}
}