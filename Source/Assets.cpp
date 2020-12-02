#include "Assets.h"
#include "Application.h"
#include "ModuleInput.h"
//#include "ModuleScene.h"
//#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "Imgui/imgui_internal.h"
#include <windows.h>
#include <ShlObj_core.h>

#include "mmgr/mmgr.h"

ImGuiTextFilter Assets::searcher;

// ---------------------------------------------------------
Assets::Assets() : Panel("Assets", ICON_ASSETS)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_NoCollapse;
	is_init = false;
	filter = AssetNode::NodeType::NONE;

	// Get Files
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	current_folder = root = GetAllFiles("Assets", nullptr, &ignore_ext);
	root->open = true;

	current_list = current_folder->childs;
}

Assets::~Assets()
{
	for (std::vector<AssetNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		delete *it;

	nodes.clear();
	selected_nodes.clear();
	aux_nodes.clear();
}

void Assets::Draw()
{
	DockSpace(); // Create Dock Space
	UpdateAssets(); // Update Files

	ChildHierarchy(); // Child Hierarchy
	ImGui::SameLine();
	ChildIcons(); // Child Icons

	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) // Shortcuts
		Shortcuts();
}

void Assets::Shortcuts()
{
	// Delete
	if (App->input->Shortcut(SDL_SCANCODE_DELETE, KEY_DOWN) && !selected_nodes.empty())
	{
		if (is_delete_popup)
			is_delete = true;
		else
		{
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
		}
	}

	// Cut
	if ((App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_X, KEY_DOWN) ||
		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_X, KEY_DOWN)) && !selected_nodes.empty())
	{
		if (!aux_nodes.empty())
		{
			for (AssetNode* aux : aux_nodes)
				aux->cut = false;
		}
		aux_nodes = selected_nodes;
		is_cut = true;
		is_copy = false;

		for (AssetNode* aux : aux_nodes)
			aux->cut = true;
	}

	// Copy
	if ((App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_C, KEY_DOWN) ||
		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_C, KEY_DOWN)) && !selected_nodes.empty())
	{
		aux_nodes = selected_nodes;
		is_copy = true;
		is_cut = false;
	}

	// Paste
	if ((App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_V, KEY_DOWN) ||
		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_V, KEY_DOWN)) && (is_copy || is_cut))
	{
		if (is_cut)
		{
			for (AssetNode* aux : aux_nodes)
			{
				if (current_folder != aux && !IsChildOf(*aux, *current_folder))
					Cut(*aux, *current_folder);
				aux->cut = false;
			}
		}
		else if (is_copy)
		{
			for (AssetNode* aux : aux_nodes)
			{
				if (current_folder != aux && !IsChildOf(*aux, *current_folder))
					Copy(*aux, *current_folder);
			}
		}
	}

	// SelectAll
	if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN) ||
		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN))
	{
		SelectAll();
	}

	// Search
	if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN) ||
		App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN))
	{
		is_search = !is_search;
	}
}

void Assets::ChildHierarchy()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::Begin("Assets_Hierarchy", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	ImGui::PopStyleVar();

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Filters")) // Filters
		{
			if (ImGui::MenuItem("None", NULL, filter == AssetNode::NodeType::NONE))
				filter = AssetNode::NodeType::NONE;
			ImGui::Separator();

			if (ImGui::MenuItem("Folders", NULL, filter == AssetNode::NodeType::FOLDER))
				filter = AssetNode::NodeType::FOLDER;
			if (ImGui::MenuItem("Scenes", NULL, filter == AssetNode::NodeType::SCENE))
				filter = AssetNode::NodeType::SCENE;
			if (ImGui::MenuItem("Prefabs", NULL, filter == AssetNode::NodeType::PREFAB))
				filter = AssetNode::NodeType::PREFAB;
			if (ImGui::MenuItem("Textures", NULL, filter == AssetNode::NodeType::TEXTURE))
				filter = AssetNode::NodeType::TEXTURE;
			if (ImGui::MenuItem("Materials", NULL, filter == AssetNode::NodeType::MATERIAL))
				filter = AssetNode::NodeType::MATERIAL;
			if (ImGui::MenuItem("Animations", NULL, filter == AssetNode::NodeType::ANIMATION))
				filter = AssetNode::NodeType::ANIMATION;
			if (ImGui::MenuItem("Tilemaps", NULL, filter == AssetNode::NodeType::TILEMAP))
				filter = AssetNode::NodeType::TILEMAP;
			if (ImGui::MenuItem("Audios", NULL, filter == AssetNode::NodeType::AUDIO))
				filter = AssetNode::NodeType::AUDIO;
			if (ImGui::MenuItem("Scripts", NULL, filter == AssetNode::NodeType::SCRIPT))
				filter = AssetNode::NodeType::SCRIPT;

			ImGui::EndMenu();
		}
		ImGui::MenuItem("Search", NULL, &is_search); // Search
		ImGui::EndMenuBar();
	}

	// Search Bar
	if (is_search)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		searcher.Draw("Search", 180);
		ImGui::Separator();
	}

	//Draw Hierarchy Tree
	ImGui::BeginChild("HierarchyTree");

	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(pos.x + 5, pos.y + 3));
	DrawHierarchy(*root);

	// Scroll
	ImVec2 scroll = ImGui::GetCurrentWindow()->Scroll;
	ImVec2 size = ImVec2(ImGui::GetWindowContentRegionMax().x, 25);

	if (ImGui::GetCursorPosY() < scroll.y)
		ImGui::SetScrollHereY();

	ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, pos.y + scroll.y)); // Top Area
	ImGui::Dummy(size);
	if (!is_arrow_hover && ImGui::BeginDragDropTarget())
	{
		if (scroll.y >= 1.0f)
			ImGui::GetCurrentWindow()->Scroll.y -= 1.0f;
		ImGui::EndDragDropTarget();
	}

	ImGui::SetCursorPos(ImVec2(pos.x + scroll.x - 4, ImGui::GetWindowHeight() + scroll.y - 25.0f)); //Bottom Area
	ImGui::Dummy(size);
	if (!is_arrow_hover && ImGui::BeginDragDropTarget())
	{
		if (scroll.y < ImGui::GetCurrentWindow()->ScrollMax.y)
			ImGui::GetCurrentWindow()->Scroll.y += 1.0f;
		ImGui::EndDragDropTarget();
	}

	if (ImGui::GetScrollY() > ImGui::GetScrollMaxY())
		ImGui::SetScrollX(ImGui::GetScrollMaxY());
	is_arrow_hover = false;

	ImGui::EndChild();
	ImGui::End();
}

void Assets::ChildIcons()
{
	ImGui::Begin("Assets_Icons", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	// Allow selection & show options with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(1))
		ImGui::SetWindowFocus();

	// Right Click Options
	DrawRightClick();

	// Delete Popup
	if (is_delete && is_delete_popup)
		ImGui::OpenPopup("Confirm Delete");

	if (ImGui::BeginPopupModal("Confirm Delete", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text(ICON_WARNING);
		ImGui::SameLine();
		ImGui::Text("Are you sure you want to delete?");
		ImGui::Text("This action cannot be undone");
		ImGui::NewLine();

		static bool check = false;
		if (ImGui::Checkbox("Do not show this message again", &check))
			is_delete_popup = !check;
		ImGui::NewLine();

		float pos = ImGui::GetCursorPosX();
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (ImGui::Button("Delete", ImVec2(size.x / 2, 22)))
		{
			is_delete = false;
			DeleteNodes(selected_nodes);
			selected_nodes.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();

		ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		pos = ImGui::GetCursorPosX();
		if (ImGui::Button("Cancel", ImVec2(size.x / 2, 22)))
		{
			is_delete = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		switch (filter)
		{
		case AssetNode::NodeType::FOLDER:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Folders");
			break;
		case AssetNode::NodeType::SCENE:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Scenes");
			break;
		case AssetNode::NodeType::PREFAB:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Prefabs");
			break;
		case AssetNode::NodeType::TEXTURE:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Textures");
			break;
		case AssetNode::NodeType::MATERIAL:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Materials");
			break;
		case AssetNode::NodeType::ANIMATION:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Animations");
			break;
		case AssetNode::NodeType::TILEMAP:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All TileMaps");
			break;
		case AssetNode::NodeType::AUDIO:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Audios");
			break;
		case AssetNode::NodeType::SCRIPT:
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Scripts");
			break;
		case AssetNode::NodeType::NONE: // Path with links
			if (current_folder->parent != nullptr)
				DrawPath();
			ImGui::Text(current_folder->name.c_str());
			break;
		}
		ImGui::EndMenuBar();
	}

	// Draw Icons
	int columns = (int)(ImGui::GetContentRegionAvailWidth() / (node_size + 4));
	if (filter == AssetNode::NodeType::NONE)
		current_list = current_folder->childs;
	else
		current_list = nodes;

	for (uint i = 0, size = current_list.size(); i < size; ++i)
	{
		AssetNode* current_node = current_list[i];
		if ((filter != AssetNode::NodeType::NONE && filter != current_node->type) || !searcher.PassFilter(current_node->name.c_str()))
			continue;

		if (is_list_view)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			DrawNodeList(*current_node);
		}
		else
		{
			float pos = ImGui::GetCursorPosX();
			DrawNodeIcon(*current_node);

			if (columns > 0 && (i + 1) % columns != 0)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(pos + node_size + 5);
			}
		}
	}

	// Unselect nodes when clicking on empty space
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && 
		ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
		UnSelectAll();

	ImGui::End();
}

void Assets::DrawHierarchy(AssetNode& node)
{
	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;
	static ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 size = ImVec2(ImGui::GetWindowContentRegionMax().x, 14);
	ImVec4 color = colors[ImGuiCol_WindowBg];
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + size.x + ImGui::GetScrollX(), pos.y + size.y + 2));
	
	// Selectable
	ImGui::Dummy(size);
	if (ImGui::IsItemHovered())
	{
		color = colors[ImGuiCol_ButtonHovered];
		if (ImGui::IsItemClicked())
		{
			UnSelectAll();
			current_folder = &node;
			filter = AssetNode::NodeType::NONE;
		}
	}
	if (current_folder == &node)
		color = colors[ImGuiCol_ButtonActive];

	// Drag & Drop
	if (ImGui::BeginDragDropTarget() && node.type == AssetNode::NodeType::FOLDER)
	{
		color = colors[ImGuiCol_ButtonHovered];
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
		{
			for (AssetNode* selected_node : selected_nodes)
			{
				if (selected_node != &node && !IsChildOf(*selected_node, node))
					Cut(*selected_node, node);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Highlight
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(color));

	// Indent
	ImGui::SameLine();
	ImGui::SetCursorPosX(pos.x + 14 * GetNumParents(node));

	// Arrow
	pos = ImGui::GetCursorPos();
	ImGui::SetCursorPosY(pos.y - 1);
	if (!node.childs.empty())
	{
		if (node.open)
			ImGui::Text(ICON_ARROW_OPEN);
		else
			ImGui::Text(ICON_ARROW_CLOSED);

		if (ImGui::IsItemClicked())
			node.open = !node.open;

		if (!node.open && ImGui::BeginDragDropTarget() && node.type == AssetNode::NodeType::FOLDER)
		{
			node.open = true;
			is_arrow_hover = true;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos.x + 16);
	}

	// Name
	ImGui::Text(node.name.c_str());

	// Childs
	if (node.open && !node.childs.empty())
	{
		for (AssetNode* child : node.childs)
		{
			if (App->file_system->IsFolder(child->path.c_str()))
				DrawHierarchy(*child);
		}
	}
}

void Assets::DrawNodeIcon(AssetNode& node)
{
	bg_color.w = 0.0f;
	border_color.w = 0.0f;
	node_size = icon_size + 25.0f;
	ImVec2 pos = ImGui::GetCursorPos();

	// Dummy
	ImGui::Dummy(ImVec2(node_size,node_size));

	//Drag and Drop
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceAllowNullID)) // Source
	{
		ImGui::SetDragDropPayload("AssetNode", &node.name, sizeof(std::string));

		if (!selected_nodes.empty()) // Popup text
		{
			if (selected_nodes.size() == 1)
				ImGui::Text(selected_nodes[0]->name.c_str());
			else
				ImGui::Text(std::to_string(selected_nodes.size()).c_str());
		}
		else
			ImGui::Text(node.name.c_str());

		if (!node.selected && !selected_nodes.empty() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) // Selection
			UnSelectAll();
		node.selected = true;

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget() && node.type == AssetNode::NodeType::FOLDER) // Target
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
		{
			for (AssetNode* selected_node : selected_nodes)
				Cut(*selected_node, node);
		}
		bg_color.w = 0.3f;
		ImGui::EndDragDropTarget();
	}

	// Handle Selection
	HandleSelection(node);

	// Draw Highlight
	if (!node.rename)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
	}

	// Draw Actual Node
	ImGui::SetCursorPos(ImVec2(pos.x + (node_size - icon_size) / 2, pos.y));

	ImGui::BeginGroup();
	ImGui::SetCursorPosY(pos.y + 5);

	if (node.cut) // Transparent Image if node is cut
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

	ImGui::Image((ImTextureID)node.icon, ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0)); // Image

	if (node.cut)
		ImGui::PopStyleVar();

	if (ImGui::IsItemClicked() && node.rename)
		node.rename = false;

	if (!node.rename)
	{
		// Text
		std::string text = node.name;
		uint text_size = (uint)ImGui::CalcTextSize(text.c_str()).x;
		uint max_size = (uint)(node_size - 7 - ImGui::CalcTextSize("...").x) / 7;
		ImGui::SetCursorPosX(pos.x + 7);
		
		if (text_size > node_size - 14)
			text = text.substr(0, max_size) + "...";
		else
			ImGui::SetCursorPosX(pos.x + (node_size - text_size) / 2);
		ImGui::Text(text.c_str());

		// Show full name
		if (ImGui::IsItemHovered() && !ImGui::IsMouseDragging())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(node.name.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	else // Rename
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", node.name.c_str());

		ImGui::SetCursorPosX(pos.x);
		ImGui::SetNextItemWidth(node_size);
		if (ImGui::InputText("##RenameAsset", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (strchr(buffer, '(') != nullptr || strchr(buffer, ')') != nullptr)
				LOG("Error renaming asset, character not valid '()'", 'e')
			else
			{
				node.rename = false;
				node.name = GetNameWithCount(buffer);
				std::string new_name = node.path.substr(0, node.path.find_last_of("/") + 1) + node.name;
				MoveFile(node.path.c_str(), new_name.c_str());
				node.path = new_name;
			
				for (AssetNode* child : node.childs)
					UpdatePath(*child, node.path);
			}
		}
		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::EndGroup();

	// Update Selected Nodes List
	int position = FindNode(node, selected_nodes);
	if (node.selected && position == -1)
		selected_nodes.push_back(&node);
	else if (!node.selected && position != -1)
		selected_nodes.erase(selected_nodes.begin() + position);
}

void Assets::DrawNodeList(AssetNode& node)
{
	ImVec4 text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	bg_color.w = 0.0f;
	border_color.w = 0.0f;
	ImVec2 pos = ImGui::GetCursorPos();
	ImVec2 size = ImVec2(ImGui::GetContentRegionAvailWidth(), 20);
	bool tooltip = true;

	// Dummy
	ImGui::Dummy(size);

	//Drag and Drop
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceAllowNullID)) // Source
	{
		ImGui::SetDragDropPayload("AssetNode", &node.name, sizeof(std::string));

		if (!selected_nodes.empty()) // Popup text
		{
			if (selected_nodes.size() == 1)
				ImGui::Text(selected_nodes[0]->name.c_str());
			else
				ImGui::Text(std::to_string(selected_nodes.size()).c_str());
		}
		else
			ImGui::Text(node.name.c_str());

		if (!node.selected && !selected_nodes.empty() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) // Selection
			UnSelectAll();
		node.selected = true;

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget() && node.type == AssetNode::NodeType::FOLDER) // Target
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
		{
			for (AssetNode* selected_node : selected_nodes)
				Cut(*selected_node, node);
		}
		bg_color.w = 0.3f;
		ImGui::EndDragDropTarget();
	}

	// Handle Selection
	HandleSelection(node);

	// Draw Highlight
	if (!node.rename)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
	}

	// Draw Actual Node
	ImGui::SetCursorPos(ImVec2(pos.x + (node_size - icon_size) / 2, pos.y));

	ImGui::BeginGroup();
	ImGui::SetCursorPosY(pos.y + 4);

	if (node.cut)
		text_color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

	if (!node.rename)
	{
		// Text
		ImVec4 icon_color = GetIconColor(node.type);
		std::string icon = GetIconList(node.type);
		std::string text = node.name;
		uint text_size = (uint)ImGui::CalcTextSize(std::string(icon + " " + text).c_str()).x;
		int max_size = (int)(size.x - 7 - ImGui::CalcTextSize("...").x) / 7;
		ImGui::SetCursorPosX(pos.x + 7);

		if (max_size >= 3 && text_size > size.x - 14)
			text = text.substr(0, max_size) + "...";
		else if (max_size < 3)
			text = "...";
		else
			tooltip = false;

		ImGui::TextColored(icon_color, icon.c_str());
		ImGui::SameLine();
		ImGui::TextColored(text_color, text.c_str());
	}
	else // Rename
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", node.name.c_str());

		ImGui::SetCursorPosX(pos.x);
		if (ImGui::InputText("##RenameAsset", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (strchr(buffer, '(') != nullptr || strchr(buffer, ')') != nullptr)
				LOG("Error renaming asset, character not valid '()'", 'e')
			else
			{
				node.rename = false;
				node.name = GetNameWithCount(buffer);
				std::string new_name = node.path.substr(0, node.path.find_last_of("/") + 1) + node.name;
				MoveFile(node.path.c_str(), new_name.c_str());
				node.path = new_name;

				for (AssetNode* child : node.childs)
					UpdatePath(*child, node.path);
			}
		}
		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::EndGroup();

	// Show full name
	if (tooltip && ImGui::IsItemHovered() && !ImGui::IsMouseDragging())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(node.name.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Update Selected Nodes List
	int position = FindNode(node, selected_nodes);
	if (node.selected && position == -1)
		selected_nodes.push_back(&node);
	else if (!node.selected && position != -1)
		selected_nodes.erase(selected_nodes.begin() + position);
}

// --- MAIN HELPERS ---
AssetNode* Assets::HandleSelection(AssetNode& node)
{
	if (ImGui::IsItemHovered()) // Hover
	{
		if (ImGui::IsMouseClicked(0) && !node.rename) // Left Click
		{
			//Show in Resources Panel

			if (rename_node != nullptr) // Quit Rename
			{
				rename_node->rename = false;
				rename_node = nullptr;
			}

			if (ImGui::GetIO().KeyCtrl) // Multiple Selection (Ctrl)
			{
				if (selected_nodes.size() > 1)
					node.selected = !node.selected;
				else
					node.selected = true;
			}
			else if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
			{
				int pos1 = FindNode(node, current_list);
				int pos2 = FindNode(*selected_nodes.back(), current_list);

				if (pos1 < pos2)
				{
					for (int i = pos1; i < pos2; ++i)
						current_list[i]->selected = true;
				}
				else
				{
					for (int i = pos1; i >= pos2; --i)
						current_list[i]->selected = true;
				}
			}
		}
		if (ImGui::IsMouseReleased(0)) // Single Selection (Left Release)
		{
			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !ImGui::IsMouseDragging() && !node.rename)
			{
				UnSelectAll();
				node.selected = true;
			}
		}
		else if (ImGui::IsMouseClicked(1)) // Right Click
		{
			if (selected_nodes.size() <= 1 || !node.selected)
				UnSelectAll();
			node.selected = true;
		}

		if (ImGui::IsMouseDoubleClicked(0)) // Double Click
		{
			switch (node.type)
			{
			case AssetNode::NodeType::FOLDER:
				current_folder = &node;
				filter = AssetNode::NodeType::NONE;
				UnSelectAll();
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

		bg_color.w = 0.3f;
	}

	if (node.selected) // Color
	{
		bg_color.w = 0.5f;
		border_color.w = 1.0f;
	}

	return &node;
}

bool Assets::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Assets"))
	{
		if (ImGui::BeginMenu("Create")) //create
		{
			if (ImGui::MenuItem("Folder")) //folder
			{
				AssetNode* new_node = CreateNode("Folder", current_folder);
				UnSelectAll();
				new_node->selected = true;
			}
			if (ImGui::MenuItem("Script")) //script
			{
				AssetNode* new_node = CreateNode("Script.scr", current_folder);
				UnSelectAll();
				new_node->selected = true;
			}

			ImGui::EndMenu();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty())) //cut
		{
			if (!aux_nodes.empty())
			{
				for (AssetNode* aux : aux_nodes)
					aux->cut = false;
			}
			aux_nodes = selected_nodes;
			is_cut = true;
			is_copy = false;

			for (AssetNode* aux : aux_nodes)
				aux->cut = true;
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, !selected_nodes.empty())) //copy
		{
			aux_nodes = selected_nodes;
			is_copy = true;
			is_cut = false;
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, is_copy || is_cut)) //paste
		{
			if (is_cut)
			{
				for (AssetNode* aux : aux_nodes)
				{
					if (current_folder != aux && !IsChildOf(*aux, *current_folder))
						Cut(*aux, *current_folder);
					aux->cut = false;
				}
			}
			else if (is_copy)
			{
				for (AssetNode* aux : aux_nodes)
				{
					if (current_folder != aux && !IsChildOf(*aux, *current_folder))
						Copy(*aux, *current_folder);
				}
			}
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, current_folder != nullptr && !current_folder->childs.empty())) //select all
			SelectAll();

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
		{
			selected_nodes.front()->rename = true;
			rename_node = selected_nodes.front();
		}

		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
		{
			if (is_delete_popup)
				is_delete = true;
			else
			{
				DeleteNodes(selected_nodes);
				selected_nodes.clear();
			}
		}
		ImGui::Separator();

		if (ImGui::BeginMenu("View")) //view
		{
			ImGui::MenuItem("List", NULL, &is_list_view); //list view
			ImGui::Separator();

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
	HWND wd = FindWindow(NULL, App->GetAppName());
	if (wd != GetForegroundWindow() && is_engine_focus)
	{
		is_engine_focus = false;
	}
	else if (wd == GetForegroundWindow() && !is_engine_focus)
	{
		is_engine_focus = true;
		std::string path = current_folder->path;

		std::vector<std::string>open_nodes;
		for (uint i = 0, size = nodes.size(); i < size; ++i)
		{
			if (nodes[i]->open)
				open_nodes.push_back(nodes[i]->path);
			delete nodes[i];
		}
		nodes.clear();

		std::vector<std::string> ignore_ext;
		ignore_ext.push_back("meta");
		root = GetAllFiles("Assets", nullptr, &ignore_ext);

		root->open = true;
		for (std::string node_path : open_nodes)
		{
			AssetNode* node = GetNode(node_path);
			if (node)
				node->open = true;
		}

		if (current_folder != nullptr)
			current_folder = GetNode(path);
		if (current_folder == nullptr)
			current_folder = root;
	}
}

void Assets::DrawPath()
{
	std::vector<AssetNode*> parents = GetParents(*current_folder);
	std::string path, aux_path;
	int count = 0, position = 0;

	// path
	for (int i = parents.size() - 1; i >= 0; --i)
		path += parents[i]->name + " > ";
	path += current_folder->name;
	aux_path = path;

	// check size and get count of parents not shown
	float size = ImGui::CalcTextSize(path.c_str()).x + 44;
	float max_size = ImGui::GetWindowWidth() + ImGui::CalcTextSize("<< ").x;
	if (size > max_size)
	{
		int offset = int((size - max_size) / 7);
		path = path.substr(offset);
		position = path.find_first_of(">");

		int found = 0;
		if (position != -1)
		{
			while (found - offset != position)
			{
				found = aux_path.find(">", found + 1);
				count++;
			}
		}
		else
			count = parents.size();

		// '<<' Button
		ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		float pos = ImGui::GetCursorPosX();
		ImGui::Text("<<");

		bool hovered = false;
		if (ImGui::IsItemHovered())
			color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);

		if (ImGui::IsItemClicked())
			ImGui::OpenPopup("<<");

		if (ImGui::BeginPopup("<<")) //popup
		{
			for (int i = parents.size() - 1, size = parents.size() - count; i >= size; --i)
			{
				if (ImGui::MenuItem(parents[i]->name.c_str()))
					current_folder = parents[i];
			}
			ImGui::EndPopup();
		}

		float pos2 = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(pos);
		ImGui::TextColored(color, "<<");
		ImGui::SetCursorPosX(pos2);
	}

	// Buttons
	if (position != -1)
	{
		static uint selected = 0;
		for (int i = parents.size() - count - 1; i >= 0; --i)
		{
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImVec4 color2 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			// Parent Button
			float pos = ImGui::GetCursorPosX();
			ImGui::Text(parents[i]->name.c_str());

			if (ImGui::IsItemHovered())
				color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
			if (ImGui::IsItemClicked())
				current_folder = parents[i];

			float pos2 = ImGui::GetCursorPosX();
			ImGui::SetCursorPosX(pos);
			ImGui::TextColored(color, parents[i]->name.c_str());
			ImGui::SetCursorPosX(pos2);

			// '>' Button
			pos = ImGui::GetCursorPosX();
			ImGui::Text(">");

			if (ImGui::IsItemHovered())
				color2 = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);

			if (ImGui::IsItemClicked())
			{
				ImGui::OpenPopup(">");
				selected = i;
			}

			if (i == selected && ImGui::BeginPopup(">")) //popup
			{
				for (AssetNode* child : parents[i]->childs)
				{
					if (child->type == AssetNode::NodeType::FOLDER &&
						ImGui::MenuItem(child->name.c_str(), NULL, child == current_folder || FindNode(*child, parents) != -1))
						current_folder = child;
				}
				ImGui::EndPopup();
			}

			pos2 = ImGui::GetCursorPosX();
			ImGui::SetCursorPosX(pos);
			ImGui::TextColored(color2, ">");
			ImGui::SetCursorPosX(pos2);
		}
	}
}

// --- NODES ---
AssetNode* Assets::CreateNode(std::string name, AssetNode* parent)
{
	AssetNode* node = new AssetNode();

	if (name == "")
	{
		node->name = GetFileName(parent->path.c_str());
		if (node->name == "")
			node->name = parent->path;
	}
	else
		node->name = name;
	node->name = GetNameWithCount(node->name);
	node->path = parent->path + std::string("/") + node->name;
	node->type = GetType(*node);

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
			pos = FindNode(*node, node->parent->childs);
			if (pos != -1)
				node->parent->childs.erase(node->parent->childs.begin() + pos);
		}

		// Delete childs
		if (!node->childs.empty())
			DeleteNodes(node->childs);

		// Delete node data
		App->file_system->Remove(node->path.c_str());
		pos = FindNode(*node, nodes);
		if (pos != -1)
		{
			delete nodes[pos];
			nodes.erase(nodes.begin() + pos);
		}
		nodes_list.erase(nodes_list.begin());
	}
	nodes_list.clear();
}

AssetNode* Assets::GetNode(const std::string path) const
{
	for (AssetNode* node : nodes)
	{
		if (node->path == path)
			return node;
	}
	return nullptr;
}

std::vector<AssetNode*> Assets::GetParents(AssetNode& node) const
{
	std::vector<AssetNode*> parents;
	AssetNode* aux_node = &node;
	while (aux_node->parent != nullptr)
	{
		parents.push_back(aux_node->parent);
		aux_node = aux_node->parent;
	}
	return parents;
}

uint Assets::GetNumParents(AssetNode& node) const
{
	uint count = 0;
	AssetNode* aux_node = &node;
	while (aux_node->parent != nullptr)
	{
		count++;
		aux_node = aux_node->parent;
	}
	return count;
}

AssetNode::NodeType Assets::GetType(const AssetNode& node) const
{
	std::string extension = GetExtension((&node)->path.c_str());

	if (extension == "png" || extension == "jpg" || extension == "tga")
		return AssetNode::NodeType::TEXTURE;

	else if (extension == "pfb")
		return AssetNode::NodeType::PREFAB;

	else if (extension == "dvs")
		return AssetNode::NodeType::SCENE;

	else if (extension == "scr")
		return AssetNode::NodeType::SCRIPT;

	else if (extension == "")
		return AssetNode::NodeType::FOLDER;

	else
		return AssetNode::NodeType::NONE;
}

std::string Assets::GetNameWithCount(const std::string name) const
{
	if (nodes.empty())
		return name;

	bool found = false;
	uint count = 0;
	std::string new_name = name;

	while (found == false)
	{
		for (uint i = 0, size = nodes.size(); i < size; ++i)
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

std::string Assets::GetIconList(const AssetNode::NodeType type) const
{
	switch (type)
	{
	case AssetNode::NodeType::FOLDER:
		return ICON_FOLDER;
	case AssetNode::NodeType::SCENE:
		return ICON_SCENE;
	case AssetNode::NodeType::PREFAB:
		return ICON_PREFAB;
	case AssetNode::NodeType::TEXTURE:
		return ICON_TEXTURE;
	case AssetNode::NodeType::MATERIAL:
		return ICON_SHADER;
	case AssetNode::NodeType::ANIMATION:
		return ICON_ANIMATION;
	case AssetNode::NodeType::TILEMAP:
		return ICON_TILEMAP;
	case AssetNode::NodeType::AUDIO:
		return ICON_AUDIO;
	case AssetNode::NodeType::SCRIPT:
		return ICON_SCRIPT;
	case AssetNode::NodeType::NONE:
		return "";
	default:
		return "";
	}
}

ImVec4 Assets::GetIconColor(const AssetNode::NodeType type) const
{
	switch (type)
	{
	case AssetNode::NodeType::FOLDER:
		return ImVec4(0.9f, 0.6f, 0.0f, 1.0f); //orange
	case AssetNode::NodeType::SCENE:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //white
	case AssetNode::NodeType::PREFAB:
		return ImVec4(0.2f, 1.0f, 1.0f, 1.0f); //blue
	case AssetNode::NodeType::TEXTURE:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case AssetNode::NodeType::MATERIAL:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case AssetNode::NodeType::ANIMATION:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case AssetNode::NodeType::TILEMAP:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case AssetNode::NodeType::AUDIO:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case AssetNode::NodeType::SCRIPT:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case AssetNode::NodeType::NONE:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	default:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	}
}

bool Assets::IsChildOf(const AssetNode& node, AssetNode& child) const
{
	for (AssetNode* aux : node.childs)
	{
		if (aux == &child || (!aux->childs.empty() && IsChildOf(*aux, child)))
			return true;
	}
	return false;
}

int Assets::FindNode(const AssetNode& node, const std::vector<AssetNode*> list) const
{
	for (uint i = 0; i < list.size(); ++i)
	{
		if (list[i] == &node)
			return i;
	}
	return -1;
}

void Assets::UpdatePath(AssetNode& node, const std::string path) const
{
	node.path = path + ("/") + node.name;

	if (!node.childs.empty())
	{
		for (AssetNode* child : node.childs)
			UpdatePath(*child, node.path);
	}
}

void Assets::SelectAll() const
{
	for (AssetNode* node : current_list)
		node->selected = true;
}

void Assets::UnSelectAll()
{
	for (AssetNode* node : current_list)
	{
		node->selected = false;
		node->rename = false;
	}
	selected_nodes.clear();
}

void Assets::Cut(AssetNode& node, AssetNode& parent) const
{
	// Move in Explorer
	MoveFile(node.path.c_str(), std::string(parent.path + ("/") + node.name).c_str());

	// Update Parent
	node.parent->childs.erase(node.parent->childs.begin() + FindNode(node, node.parent->childs)); //erase from parent's child list
	node.parent = &parent; //set new parent
	node.parent->childs.push_back(&node); //add node to new parent's child list

	// Update Childs
	UpdatePath(node, parent.path);
}

void Assets::Copy(AssetNode& node, AssetNode& parent)
{
	// Create new Node
	AssetNode* new_node = CreateNode(node.name, &parent);

	// If has childs, copy them too
	if (!node.childs.empty())
	{
		std::vector<AssetNode*> childs = node.childs;
		for (AssetNode* child : childs)
			Copy(*child, *new_node);
	}

	// Copy in Explorer
	App->file_system->Copy(node.path.c_str(), new_node->path.c_str());
}

// --- FILES ---
AssetNode* Assets::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext)
{
	AssetNode* node = new AssetNode();

	if (App->file_system->Exists(directory)) //check if directory exists
	{
		node->path = directory;
		node->name = GetFileName(directory);
		if (node->name == "")
			node->name = directory;
		node->type = GetType(*node);

		std::vector<std::string> file_list, dir_list;
		App->file_system->GetFolderContent(directory, file_list, dir_list);

		//Adding all child directories
		for (std::string dir : dir_list)
		{
			std::string str = directory + std::string("/") + dir;
			AssetNode* child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);
			node->childs.push_back(child);
			child->parent = node;
		}

		//Adding all child files
		for (std::string file : file_list)
		{
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
				filter = CheckExtension(file.c_str(), *filter_ext); //check if file_ext == filter_ext
			else if (ignore_ext != nullptr)
				discard = CheckExtension(file.c_str(), *ignore_ext); //check if file_ext == ignore_ext

			if (filter == true && discard == false)
			{
				std::string str = directory + std::string("/") + file;
				AssetNode* child = GetAllFiles(str.c_str(), filter_ext, ignore_ext);
				node->childs.push_back(child);
			}
		}
	}
	if (node->type != AssetNode::NodeType::NONE)
		nodes.push_back(node);
	else
		LOG("Error retrieving files", 'e');

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
	std::string ext = GetExtension(path);
	for (std::string extension : extensions)
	{
		if (extension == ext)
			return true;
	}
	return false;
}

// --- OTHERS ---
void Assets::DockSpace()
{
	ImGuiID dock_space = ImGui::GetID("Assets");
	if (is_init == false)
	{
		is_init = true;

		ImGui::DockSpace(dock_space);
		ImGuiDockNode* dock_node = ImGui::DockBuilderGetNode(dock_space);
		if (!dock_node->IsSplitNode())
		{
			ImGuiID left_space, right_space;
			ImGui::DockBuilderSplitNode(dock_space, ImGuiDir_Left, 0.18f, &left_space, &right_space);
			ImGui::DockBuilderDockWindow("Assets_Hierarchy", left_space);
			ImGui::DockBuilderGetNode(left_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
			ImGui::DockBuilderDockWindow("Assets_Icons", right_space);
			ImGui::DockBuilderGetNode(right_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}
	}
	else
		ImGui::DockSpace(dock_space, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit);
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