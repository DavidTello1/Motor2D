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

	last_id = 0;

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

	// Empty Space
	ImGui::BeginChild("Empty");

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))) //unselect nodes when clicking on empty space
		UnSelectAll();

	DrawRightClick(); //draw right-click options

	ImGui::EndChild();

	// Shortcuts
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) // Delete
		{
			DeleteNodes(selected_nodes);
		}

		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) || // Duplicate
			(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN))
		{
			DuplicateNodes(selected_nodes);
		}
	}
}

void Hierarchy::DrawNode(HierarchyNode* node)
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
	if (node->is_folder)
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

	// Draw Node
	bool is_open = ImGui::TreeNodeEx(node->name.c_str(), node->flags);

	if (node->is_folder)
		ImGui::PopStyleColor();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(0)) //allow selecting when right-click options is shown
		ImGui::SetWindowFocus();

	// Left Click (selection)
	if (ImGui::IsItemClicked(0)) // if treenode is clicked, check whether it is a single or multi selection
	{
		if (ImGui::IsMouseDoubleClicked(0)) // Double Click
		{
			node->rename = true;
		}
		else
		{
			if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
			{
				if (selected_nodes.size() > 1) // if selecting node inside a multi-selection (clear all except node)
					UnSelectAll();
				else
					UnSelectAll(node);
			}

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

	else if (!node->selected && pos > -1)
		selected_nodes.erase(selected_nodes.begin() + pos);

	// Open Node
	if (is_open)
	{
		if (!node->childs.empty())
		{
			for (HierarchyNode* child : node->childs)
				DrawNode(child);
		}
		ImGui::TreePop();
	}
}

HierarchyNode* Hierarchy::CreateNode(const char* name, bool is_folder, HierarchyNode* parent, bool selected, GameObject* object/*, ResourceScene* scene*/)
{
	HierarchyNode* node = new HierarchyNode();

	last_id++;
	node->name = name + std::string("##") + std::to_string(last_id);
	node->is_folder = is_folder;
	node->selected = selected;

	if (object != nullptr) // if object != nullptr, ResourceScene is not read
		node->object = object;

	//else if (scene != nullptr)
	//{
	//	node->scene = scene;
	//	node->flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	//}

	if (parent == nullptr) // if parent is null, check for first selected node
	{
		if (!selected_nodes.empty())
		{
			node->parent = selected_nodes[0];
			selected_nodes[0]->childs.push_back(node);
			selected_nodes[0]->flags |= ImGuiTreeNodeFlags_DefaultOpen;
			UnSelectAll(node);
		}
	}
	else
	{
		node->parent = parent;
		parent->childs.push_back(node);
		parent->flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	nodes.push_back(node);
	//OrderHierarchy(); //order hierarchy nodes

	return node;
}

void Hierarchy::DeleteNodes(std::vector<HierarchyNode*> nodes_list)
{
	for (uint i = 0; i < nodes_list.size(); ++i)
	{
		int pos = 0;

		// Delete node from parent's child list
		if (nodes_list[i]->parent != nullptr)
		{
			pos = FindNode(nodes_list[i], nodes_list[i]->parent->childs);
			nodes_list[i]->parent->childs.erase(nodes_list[i]->parent->childs.begin() + pos);
		}

		// Delete childs
		if (!nodes_list[i]->childs.empty())
		{
			for (uint j = 0; j < nodes_list[i]->childs.size(); ++j) //if childs are inside deletion list, erase them from list first and then delete
			{
				pos = FindNode(nodes_list[i]->childs[j], nodes_list);
				if (pos != -1)
					nodes_list.erase(nodes_list.begin() + pos);
			}

			DeleteNodes(nodes_list[i]->childs);
		}

		// Delete node data
		pos = FindNode(nodes_list[i], nodes);
		delete nodes[pos];

		// Delete node from Nodes List
		nodes.erase(nodes.begin() + pos);
	}
	selected_nodes.clear();
	//nodes_list.clear();
}

void Hierarchy::DuplicateNodes(std::vector<HierarchyNode*> nodes_list, HierarchyNode* parent)
{
	uint size = nodes_list.size();
	for (uint i = 0; i < size; ++i)
	{
		HierarchyNode* node = new HierarchyNode();

		last_id++;
		node->name = nodes_list[i]->name.substr(0, nodes_list[i]->name.find_first_of("##")) + std::string("##") + std::to_string(last_id);
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

void Hierarchy::UnSelectAll(HierarchyNode* exception)
{
	for (std::vector<HierarchyNode*>::iterator it = selected_nodes.begin(); it != selected_nodes.end(); ++it)
	{
		if ((*it)->selected == true)
		{
			if (exception != nullptr && *it == exception)
				continue;
			else
				(*it)->selected = false;
		}
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

void Hierarchy::OrderHierarchy()
{
	std::priority_queue<HierarchyNode*, std::vector<HierarchyNode*>, Sort> ListOrder;

	for (HierarchyNode* node : nodes) //push nodes into Ordered List
		ListOrder.push(node);

	nodes.clear(); //clear Nodes List

	while (ListOrder.empty() == false) //push Ordered List into Nodes List
	{
		nodes.push_back(ListOrder.top());
		ListOrder.pop();
	}
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

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
		{
		}
		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
			DeleteNodes(selected_nodes);

		ImGui::EndPopup();
		return true;
	}
	return false;
}
