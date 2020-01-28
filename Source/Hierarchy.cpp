#include "Application.h"
#include "Hierarchy.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"

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
	// Right Click Options
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
				CreateNode("Folder", true);

			if (ImGui::MenuItem("Scene"))
				CreateNode("Scene");

			if (ImGui::MenuItem("GameObject"))
				CreateNode("GameObject");

			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, !selected_nodes.empty()))	{}
			//DuplicateNodes(selected_nodes);

		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty()))
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, !selected_nodes.empty()))
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1))
		{
		}
		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty()))
			DeleteNodes(selected_nodes);

		ImGui::EndPopup();
	}

	// Draw Nodes
	for (HierarchyNode* node : nodes)
		DrawNode(node);
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
	{
		if (node->childs.empty()) // if leaf, hide arrow
			node->flags &= ~ImGuiTreeNodeFlags_Bullet;
		else
			node->flags |= ImGuiTreeNodeFlags_Bullet;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.0f, 1.0f));
	}

	//// Scene Params
	//if (node->scene != nullptr && !node->is_folder)
	//{
	//	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//	static ImVec4 colorf = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	//	const ImU32 color = ImColor(colorf);
	//	static float pos = ImGui::GetCursorPosY();
	//	static bool first = false;

	//	// Draw Scene Background
	//	if (pos < 10)
	//	{
	//		first = true;
	//		pos += 16;
	//	}
	//	else
	//	{
	//		first = false;
	//		pos += 18;
	//	}

	//	draw_list->AddRectFilled(ImVec2(0, pos_y), ImVec2(ImGui::GetWindowWidth(), pos_y + 15), color); //actual draw of background

	//	if (first)
	//		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
	//}

	// Draw Node
	bool is_open = ImGui::TreeNodeEx(node->name.c_str(), node->flags);

	if (node->is_folder)
		ImGui::PopStyleColor();

	if (ImGui::IsItemClicked()) // if treenode is clicked, check whether it is a single or multi selection
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
			
			// Add/Remove from selected_nodes list
			int pos = FindNode(node, selected_nodes);
			if (node->selected && pos == -1)
				selected_nodes.push_back(node);
			else if (!node->selected && pos > -1)
				selected_nodes.erase(selected_nodes.begin() + pos);
		}
	}

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
	node->parent = parent;
	node->is_folder = is_folder;
	node->selected = selected;

	if (is_folder) // if folder = true, GameObject and ResourceScene are not read
	{
		node->flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet;
	}
	else if (object != nullptr) // if object != nullptr, ResourceScene is not read
	{
		node->object = object;
		node->flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
	}
	//else if (scene != nullptr)
	//{
	//	node->scene = scene;
	//	node->flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	//}

	if (node->parent != nullptr)
	{
		node->parent = parent;
		node->parent->childs.push_back(node);
	}

	if (node != nullptr)
	{
		nodes.push_back(node);

		if (node->selected)
			selected_nodes.push_back(node);
	}

	return node;
}

void Hierarchy::DeleteNodes(std::vector<HierarchyNode*> nodes_list)
{
	for (int i = 0; i < nodes_list.size(); ++i)
	{
		int pos = FindNode(nodes_list[i], nodes);
		delete nodes[pos];

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
	for (int i = 0; i <list.size(); ++i)
	{
		if (list[i] == node)
			return i;
	}
	return -1;
}