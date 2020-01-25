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

	has_menubar = true;
}

Hierarchy::~Hierarchy()
{}

void Hierarchy::Draw()
{
	// Right Click Options
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
			}
			if (ImGui::MenuItem("Scene"))
			{
			}
			if (ImGui::MenuItem("GameObject"))
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, true))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, true))
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, true))
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, true))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Rename", NULL, false, selected_node != nullptr))
		{
		}
		if (ImGui::MenuItem("Delete", "Supr", false, selected_node != nullptr))
		{
		}
		ImGui::EndPopup();
	}

	// Scenes and GameObjects
	ImGui::Text(App->scene->GetName());
	ImGui::Separator();

	//// Drawing scene gameobjects as nodes
	//for (GameObject* obj : ModuleScene::root_object->GetChilds())
	//	DrawNode(obj);

	//ImGui::Separator();
}

void Hierarchy::DrawNode(GameObject* obj)
{
	//// Flags
	//flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

	//if (obj->HasChilds()) // leaf
	//	flags &= ~ImGuiTreeNodeFlags_Leaf;
	//else
	//	flags |= ImGuiTreeNodeFlags_Leaf;

	//if (obj->is_selected) // selected
	//	flags |= ImGuiTreeNodeFlags_Selected;
	//else
	//	flags &= ~ImGuiTreeNodeFlags_Selected;

	//// Draw Node
	//bool is_open = ImGui::TreeNodeEx(obj->GetName(), flags);

	//if (ImGui::IsItemClicked()) // if treenode is clicked, check whether it is a single or multi selection
	//{
	//	if (ImGui::IsMouseDoubleClicked(0)) // Double Click
	//	{
	//		obj->is_rename = true;
	//	}
	//	else
	//	{
	//		if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
	//		{
	//			App->scene->UnSelectAll(obj);
	//		}

	//	ToggleSelection(obj); // Always need to toggle the state of selection of the node, getting its current state
	//}
	//}

	//// If node is open, draw childs if it has
	//if (is_open && obj->HasChilds())
	//{
	//	for (GameObject* child : obj->GetChilds())
	//		DrawNode(child);
	//	ImGui::TreePop();
	//}
}

void Hierarchy::Select(GameObject* object) 
{ 
	object->is_selected = true; 
}

void Hierarchy::UnSelect(GameObject* object) 
{ 
	object->is_selected = false; 
}

bool Hierarchy::ToggleSelection(GameObject* obj) // Toggles the state of the node, returns current state after toggled
{
	if (obj->is_selected)
		UnSelect(obj);
	else
		Select(obj);

	return obj->is_selected;
}
