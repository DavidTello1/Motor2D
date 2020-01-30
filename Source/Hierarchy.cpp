#include "Application.h"
#include "Hierarchy.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
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

	last_pos = 0;

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
	if (ImGui::IsMouseClicked(1)) //allow selection & show options with right click
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

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(0)) //unselect nodes when clicking on empty space
		UnSelectAll();

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(1)) //unselect nodes when clicking on empty space
	{
		UnSelectAll();
	}
		DrawRightClick(); //draw right-click options

	ImGui::EndChild();
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

	node->name = name + std::string("##") + std::to_string(int(nodes.size()));
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

	if (node != nullptr)
	{
		node->pos = last_pos;
		last_pos++;
		nodes.push_back(node);

		if (node->selected)
			selected_nodes.push_back(node);

		//OrderHierarchy(); //order hierarchy nodes
	}

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
			DeleteNodes(nodes_list[i]->childs);

		// Delete node data
		pos = FindNode(nodes_list[i], nodes);
		delete nodes[pos];

		// Delete node from Nodes & Selected_Nodes lists
		nodes.erase(nodes.begin() + pos);
		selected_nodes.clear();
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

void Hierarchy::DrawRightClick()
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
		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, !selected_nodes.empty())) {} //duplicate
		//DuplicateNodes(selected_nodes);

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
	}
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
