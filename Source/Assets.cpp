#include "Application.h"
//#include "ModuleScene.h"
//#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "Assets.h"

#include "Imgui/imgui_internal.h"
#include <windows.h>
#include <ShlObj_core.h>

#include "mmgr/mmgr.h"

ImGuiTextFilter Assets::Searcher;

// ---------------------------------------------------------
Assets::Assets() : Panel("Assets", ICON_ASSETS)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	//UpdateFilters(models);
	//UpdateFilters(materials);
	//UpdateFilters(scenes);

	UpdateAssets();
	current_folder = root;
}

Assets::~Assets()
{
	for (std::vector<AssetNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		delete* it;
	nodes.clear();
	selected_nodes.clear();
}

void Assets::Draw()
{
	//if (timer.ReadSec() > REFRESH_RATE) // Update Assets Hierarchy
	//{
	//	UpdateAssets();
	//	timer.Start();
	//}

	// --- Child Hierarchy ---
	ImGui::BeginChild("Hierarchy", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 0), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Filters")) // Filters
		{
			ImGui::EndMenu();
		}
		ImGui::MenuItem("Search", NULL, &is_search); // Search
		ImGui::EndMenuBar();
	}

	// Search Bar
	if (is_search)
	{
		Searcher.Draw("Search", 180);
		ImGui::Separator();
	}

	//Draw Hierarchy Tree
	DrawHierarchy(root);
	ImGui::EndChild();
	ImGui::SameLine();

	// --- Child Icons ---
	ImGui::BeginChild("Icons", ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar);

	// Allow selection & show options with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(1))
			ImGui::SetWindowFocus();

	// Right Click Options
	DrawRightClick();

	// Menu Bar
	if (ImGui::BeginMenuBar()) // Path with links
	{
		//if (filter_models)
		//	ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Models");

		//else if (filter_materials)
		//	ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Materials");

		//else if (filter_scenes)
		//	ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Scenes");

		//else if (selected_node.path != "")
		//	ImGui::Text(selected_node.path.c_str());

		//else // Path
		{
			if (current_folder->parent != nullptr)
			{
				std::vector<AssetNode*> parents = GetParents(current_folder);
				for (int i = parents.size() - 1; i >= 0; --i)
				{
					ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
					float pos = ImGui::GetCursorPosX();
					ImGui::Text(std::string(parents[i]->name + " >").c_str());

					float pos2 = ImGui::GetCursorPosX();
					ImGui::SetCursorPosX(pos);
					ImGui::Dummy(ImGui::GetItemRectSize());

					if (ImGui::IsItemHovered())
						color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
					if (ImGui::IsItemClicked())
						current_folder = parents[i];

					ImGui::SetCursorPosX(pos);
					ImGui::TextColored(color, std::string(parents[i]->name).c_str());
					ImGui::SetCursorPosX(pos2);
				}
			}
			ImGui::Text(current_folder->name.c_str());
		}
		ImGui::EndMenuBar();
	}

	// Draw Icons
	int columns = ((int)ImGui::GetWindowWidth() - 8) / (icon_size + 8);
	float spacing = 15.0f;
	for (uint i = 0; i < current_folder->childs.size(); ++i)
	{
		DrawNode(current_folder->childs[i]);
		if (current_folder->childs[i]->rename)
			spacing = 3.0f;
		if (columns > 0 && (i + 1) % columns != 0)
			ImGui::SameLine(0.0f, spacing);
	}

	// Unselect nodes when clicking on empty space
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(0) && !is_any_hover)
		UnSelectAll();
	is_any_hover = false;

	ImGui::EndChild();
}

void Assets::DrawHierarchy(AssetNode* node)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (node == root)
		nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	if (node->childs.empty())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (current_folder == node)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	bool open = ImGui::TreeNodeEx(node->name.c_str(), nodeFlags, node->name.c_str());

	if (ImGui::IsItemClicked())
		current_folder = node;

	if (open && !node->childs.empty())
	{
		for (uint i = 0; i < node->childs.size(); i++)
		{
			if (App->file_system->IsFolder(node->childs[i]->path.c_str()))
				DrawHierarchy(node->childs[i]);
		}
		ImGui::TreePop();
	}
}

void Assets::DrawNode(AssetNode* node)
{
	bg_color.w = 0.0f;
	border_color.w = 0.0f;

	// Dummy
	static ImGuiContext& g = *GImGui;
	float size = icon_size + g.FontSize + 11;
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::Dummy(ImVec2(size,size));

	// Handle Selection
	if (ImGui::IsItemHovered()) // Hover
	{
		is_any_hover = true;
		if (ImGui::IsMouseClicked(0) && !node->rename) // Left Click
		{
			if (!ImGui::GetIO().KeyCtrl)
				UnSelectAll();

			node->selected = !node->selected;
			//Show in Resources Panel
		}
		else if (ImGui::IsMouseClicked(1) && selected_nodes.size() <= 1) // Right Click (select item to show options)
		{
			UnSelectAll();
			node->selected = true; //change selection state
		}
		else if (ImGui::IsMouseDoubleClicked(0)) // Double Click
		{
			switch (node->type)
			{
			case AssetNode::NodeType::FOLDER:
				current_folder = node;
				break;
			case AssetNode::NodeType::SCENE:
				//Load Scene
				break;
			case AssetNode::NodeType::SCRIPT:
				//Open in Editor
				break;
			default:
				break;
			}
		}

		// Show full name
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(node->name.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();

		bg_color.w = 0.3f;
	}
	if (node->selected) // Color
	{
		bg_color.w = 0.5f;
		border_color.w = 1.0f;
	}

	// Draw Highlight
	if (!node->rename)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
	}

	// Draw Actual Node
	ImGui::SetCursorPos(ImVec2(pos.x + (size - icon_size) / 2, pos.y));
	ImGui::SetNextItemWidth((float)icon_size);

	ImGui::BeginGroup();
	ImGui::SetCursorPosY(pos.y + 5);

	ImGui::Image((ImTextureID)node->icon, ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsItemClicked() && node->rename)
		node->rename = false;

	if (!node->rename)
	{
		// Text
		std::string text = node->name;
		uint text_size = (uint)ImGui::CalcTextSize(text.c_str()).x;
		uint max_size = (uint)(icon_size - ImGui::CalcTextSize("...").x) / 7;
		if (text_size > icon_size)
			text = text.substr(0, max_size) + "...";
		else
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((icon_size - text_size) / 2));
		ImGui::Text(text.c_str());
	}
	else // Rename
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", node->name.c_str());

		ImGui::SetCursorPosX(pos.x);
		ImGui::SetNextItemWidth(size);
		if (ImGui::InputText("##RenameAsset", buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (strchr(buffer, '(') != nullptr || strchr(buffer, ')') != nullptr)
				LOG("Error renaming asset, character not valid '()'", 'e')
			else
			{
				node->rename = false;
				node->name = GetNameWithCount(buffer);
				std::string new_name = node->path.substr(0, node->path.find_last_of("/") + 1) + node->name;
				MoveFile(node->path.c_str(), new_name.c_str());
			}
		}
	}
	ImGui::EndGroup();

	// Update Selected Nodes List
	int position = FindNode(node, selected_nodes);
	if (node->selected && position == -1)
		selected_nodes.push_back(node);
	else if (!node->selected && position != -1)
		selected_nodes.erase(selected_nodes.begin() + position);
}

AssetNode* Assets::CreateNode(std::string path, AssetNode* parent, std::string name)
{
	AssetNode* node = new AssetNode();

	if (name == "")
	{
		node->name = GetFileName(path.c_str());
		if (node->name == "")
			node->name = path;
	}
	else
		node->name = name;
	node->name = GetNameWithCount(node->name);
	node->path = path + std::string("/") + node->name;
	node->type = GetType(node);

	if (node->type == AssetNode::NodeType::FOLDER)
		App->file_system->CreateFolder(node->path.c_str());

	if (parent != nullptr)
	{
		node->parent = parent;
		parent->childs.push_back(node);
	}
	else if (current_folder != nullptr)
	{
		node->parent = current_folder;
		current_folder->childs.push_back(node);
	}

	nodes.push_back(node);
	return node;
}

void Assets::DeleteNodes(std::vector<AssetNode*> nodes_list)
{
	while (!nodes_list.empty())
	{
		AssetNode* node = nodes_list.front();
		int pos;

		// Delete node from parent's child list
		if (node->parent != nullptr)
		{
			pos = FindNode(node, node->parent->childs);
			if (pos != -1)
				node->parent->childs.erase(node->parent->childs.begin() + pos);
		}

		// Delete childs
		if (!node->childs.empty())
			DeleteNodes(node->childs);

		// Delete node data
		App->file_system->Remove(node->path.c_str());
		pos = FindNode(node, nodes);
		if (pos != -1)
		{
			delete nodes[pos];
			nodes.erase(nodes.begin() + pos);
		}
		nodes_list.erase(nodes_list.begin());
	}
	nodes_list.clear();
}

void Assets::SelectAll()
{
	for (std::vector<AssetNode*>::iterator it = current_folder->childs.begin(); it != current_folder->childs.end(); ++it)
		(*it)->selected = true;
}

void Assets::UnSelectAll()
{
	for (std::vector<AssetNode*>::iterator it = selected_nodes.begin(); it != selected_nodes.end(); ++it)
	{
		(*it)->selected = false;
		(*it)->rename = false;
	}
	selected_nodes.clear();
}

bool Assets::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Assets"))
	{
		if (ImGui::BeginMenu("Create")) //create
		{
			if (ImGui::MenuItem("Folder")) //folder
				CreateNode(current_folder->path, nullptr, "Folder");

			if (ImGui::MenuItem("Script")) //script
				CreateNode(current_folder->path, nullptr, "Script.scr");

			ImGui::EndMenu();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty())) //cut
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, !selected_nodes.empty())) //copy
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, is_copy || is_cut)) //paste
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, current_folder != nullptr && !current_folder->childs.empty())) //select all
			SelectAll();

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
			selected_nodes.front()->rename = true;

		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
		{
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}
		ImGui::Separator();

		if (ImGui::BeginMenu("View")) //view
		{
			if (ImGui::MenuItem("Very Big Icons", NULL, icon_size == VERY_BIG_SIZE && !is_list_view)) //very big
			{
				icon_size = VERY_BIG_SIZE;
				is_list_view = false;
			}
			if (ImGui::MenuItem("Big Icons", NULL, icon_size == BIG_SIZE && !is_list_view)) //big
			{
				icon_size = BIG_SIZE;
				is_list_view = false;
			}
			if (ImGui::MenuItem("Medium Icons", NULL, icon_size == MEDIUM_SIZE && !is_list_view)) //medium
			{
				icon_size = MEDIUM_SIZE;
				is_list_view = false;
			}
			if (ImGui::MenuItem("Small Icons", NULL, icon_size == SMALL_SIZE && !is_list_view)) //small
			{
				icon_size = SMALL_SIZE;
				is_list_view = false;
			}
			ImGui::Separator();

			ImGui::MenuItem("List", NULL, &is_list_view); //list view
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Order")) //order
		{
			if (ImGui::MenuItem("Type", NULL, order == 0)) //type
			{
			}
			if (ImGui::MenuItem("Name", NULL, order == 1)) //name
			{
			}
			if (ImGui::MenuItem("Date", NULL, order == 2)) //date
			{
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Ascending", NULL, is_ascending_order)) //ascending order
				is_ascending_order = !is_ascending_order;

			if (ImGui::MenuItem("Descending", NULL, !is_ascending_order)) //descending order
				is_ascending_order = !is_ascending_order;
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Show in Explorer", NULL, nullptr, selected_nodes.size() == 1)) //show in explorer
		{
			TCHAR  buffer[4096] = TEXT("");
			GetFullPathName(selected_nodes[0]->path.c_str(), 4096, buffer, NULL);

			HRESULT hr = CoInitializeEx(0, NULL);
			ITEMIDLIST* pidl = ILCreateFromPath(std::string(buffer).substr(0, std::string(buffer).find_last_of("/")).c_str());
			if (pidl) 
			{
				SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
				ILFree(pidl);
			}
		}
		if (ImGui::MenuItem("Show References", NULL, nullptr, selected_nodes.size() == 1)) //references
		{
		}
		ImGui::EndPopup();
		return true;
	}
	return false;
}

void Assets::UpdateAssets()
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	root = GetAllFiles("Assets", nullptr, &ignore_ext);
}

AssetNode* Assets::GetNode(std::string name)
{
	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i]->name == name)
			return nodes[i];
	}
	return nullptr;
}

int Assets::FindNode(AssetNode* node, std::vector<AssetNode*> list)
{
	for (uint i = 0; i < list.size(); ++i)
	{
		if (list[i] == node)
			return i;
	}
	return -1;
}

std::string Assets::GetNameWithCount(std::string name)
{
	if (nodes.empty())
		return name;

	bool found = false;
	uint count = 0;
	std::string new_name = name;

	while (found == false)
	{
		for (uint i = 0; i < nodes.size(); ++i)
		{
			if (new_name == nodes[i]->name)
			{
				count++;
				new_name = name + (" (") + std::to_string(count) + (")");
				break;
			}
			else if (i == nodes.size() - 1)
			{
				found = true;
				break;
			}
		}
	}
	return new_name;
}

AssetNode::NodeType Assets::GetType(AssetNode* node)
{
	std::string extension = GetExtension(node->path.c_str());

	if (extension == "png" || extension == "jpg" || extension == "tga") // texture
		return AssetNode::NodeType::TEXTURE;

	else if (extension == "pfb") // prefab
		return AssetNode::NodeType::PREFAB;

	else if (extension == "dvs") // scene
		return AssetNode::NodeType::SCENE;

	else if (extension == "scr") //script
		return AssetNode::NodeType::SCRIPT;

	else if (extension == "") // folder
		return AssetNode::NodeType::FOLDER;

	else
		return AssetNode::NodeType::NONE;
}

std::vector<AssetNode*> Assets::GetParents(AssetNode* node)
{
	std::vector<AssetNode*> parents;
	AssetNode* aux_node = node;
	while (aux_node->parent != nullptr)
	{
		parents.push_back(aux_node->parent);
		aux_node = aux_node->parent;
	}
	return parents;
}

AssetNode* Assets::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext)
{
	AssetNode* node = new AssetNode();

	if (App->file_system->Exists(directory)) //check if directory exists
	{
		node->path = directory;
		node->name = GetFileName(directory);
		if (node->name == "")
			node->name = directory;
		node->type = GetType(node);

		if (App->file_system->IsFolder(directory))
		{
			std::vector<std::string> file_list, dir_list;
			App->file_system->GetFolderContent(directory, file_list, dir_list);

			//Adding all child directories
			for (uint i = 0; i < dir_list.size(); i++)
			{
				std::string str = directory + std::string("/") + dir_list[i];
				AssetNode* child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);
				node->childs.push_back(child);
				child->parent = node;
			}

			//Adding all child files
			for (uint i = 0; i < file_list.size(); i++)
			{
				bool filter = true, discard = false;
				if (filter_ext != nullptr)
					filter = CheckExtension(file_list[i].c_str(), *filter_ext); //check if file_ext == filter_ext
				else if (ignore_ext != nullptr)
					discard = CheckExtension(file_list[i].c_str(), *ignore_ext); //check if file_ext == ignore_ext

				if (filter == true && discard == false)
				{
					std::string str = directory + std::string("/") + file_list[i];
					AssetNode* child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);
					node->childs.push_back(child);
				}
			}
		}
	}

	if (node != nullptr)
		nodes.push_back(node);

	return node;
}

std::string Assets::GetFileName(const char* path) const
{
	const char* file_name = strrchr(path, 128);
	if (file_name == nullptr)
		file_name = (strrchr(path, '/') != nullptr) ? strrchr(path, '/') : "";

	if (file_name != "")
		file_name++;

	return file_name;
}

std::string Assets::GetExtension(const char* path) const
{
	char buffer[32] = "";
	const char* last_dot = strrchr(path, '.');
	if (last_dot != nullptr)
		strcpy_s(buffer, last_dot + 1);

	std::string extension(buffer);
	return extension;
}

bool Assets::CheckExtension(const char* path, std::vector<std::string> extensions) const
{
	std::string ext = "";
	ext = GetExtension(path);

	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
}

//void Assets::ImportAsset(const PathNode& node)
//{
//	UID id = App->resources->GetIDFromMeta(std::string(node.path + (".meta")).c_str());
//	Resource* resource = App->resources->GetResource(id);
//
//	if (resource && resource->GetType() == Resource::material) // ***ERROR IMPORTING WHEN FILE EXISTS IN LIBRARY_FOLDER
//	{
//		GameObject* obj = App->scene->GetSelectedGameObject();
//		if (obj != nullptr)
//		{
//			ComponentMaterial* mat = (ComponentMaterial*)obj->GetComponent(Component::Type::Material);
//			if (mat == nullptr)
//				mat = (ComponentMaterial*)obj->AddComponent(Component::Type::Material);
//
//			resource->LoadToMemory();
//			mat->SetMaterial((ResourceMaterial*)resource);
//		}
//	}
//	else if (resource && resource->GetType() == Resource::model)
//	{
//		resource->LoadToMemory();
//		ResourceModel* res = (ResourceModel*)resource;
//		res->CreateGameObjects(App->file_system->GetFileName(node.path.c_str()).c_str()); //create gameobjects from model
//		App->scene->RedoQuatree();
//	}
//}

//void Assets::UpdateFilters(PathNode& node)
//{
//	std::vector<std::string> filter_ext;
//
//	if (node.children == models.children)
//	{
//		filter_ext.push_back("fbx");
//		filter_ext.push_back("obj");
//	}
//	else if (node.children == materials.children)
//	{
//		filter_ext.push_back("png");
//		filter_ext.push_back("dds");
//		filter_ext.push_back("jpg");
//		filter_ext.push_back("tga");
//	}
//	else if (node.children == scenes.children)
//	{
//		filter_ext.push_back("dvs");
//	}
//
//	node = App->file_system->GetAllFiles("Assets", &filter_ext);
//	FilterFolders(node, node);
//
//	uint size = node.children.size();
//	uint cont = 0;
//	for (uint i = 0; i < size; ++i)
//	{
//		uint j = i - cont;
//		if (node.children[j].file == false)
//		{
//			node.children.erase(node.children.begin() + j);
//			cont++;
//		}
//	}
//
//	current_node = node;
//}

//void Assets::FilterFolders(PathNode& node, PathNode& parent)
//{
//	for (uint i = 0; i < node.children.size(); ++i)
//	{
//		if (node.children[i].file == false) // if folder filter again
//			FilterFolders(node.children[i], node);
//
//		else if (parent.children != node.children)//if file
//			parent.children.push_back(node.children[i]); //add children to node
//	}
//}

//Resource* Assets::GetSelectedResource()
//{
//	if (selected_node.path != "" && selected_node.file == true)
//	{
//		std::string metaFile = selected_node.path + (".meta");
//		uint64 resourceID = App->resources->GetIDFromMeta(metaFile.c_str());
//		return App->resources->GetResource(resourceID);
//	}
//	return nullptr;
//}