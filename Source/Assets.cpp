#include "Assets.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
//#include "ModuleScene.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "Configuration.h"

#include "Imgui/imgui_internal.h"
#include <windows.h>
#include <ShlObj_core.h>

#include "mmgr/mmgr.h"

ImGuiTextFilter Assets::searcher;

// ---------------------------------------------------------
Assets::Assets() : Panel("Assets", ICON_ASSETS, 4)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_NoCollapse;
	is_init = false;
	filter = ResourceType::UNKNOWN;

	// Get Files
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	nodes = App->file_system->GetAllFiles("Assets", nullptr, &ignore_ext);
	current_folder = 0;
	//current_list = nodes.childs[current_folder];
}

Assets::~Assets()
{
}

void Assets::Draw()
{
	pos_x = ImGui::GetWindowPos().x;
	pos_y = ImGui::GetWindowPos().y;

	DockSpace(); // Create Dock Space
	UpdateAssets(); // Update Files

	ChildHierarchy(); // Child Hierarchy
	ImGui::SameLine();
	ChildIcons(); // Child Icons
	
	static bool focus = false;
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) // Shortcuts
	{
		if (focus == false)
		{
			ImGui::SetWindowFocus();
			focus = true;
		}
		Shortcuts();
	}
	else
		focus = false;
}

void Assets::Shortcuts()
{
	//// Delete
	//if (App->input->Shortcut(SDL_SCANCODE_DELETE, KEY_DOWN) && !selected_nodes.empty())
	//{
	//	if (is_delete_popup)
	//		is_delete = true;
	//	else
	//	{
	//		DeleteNodes(selected_nodes);
	//		selected_nodes.clear();
	//	}
	//}

	//// Cut
	//if ((App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_X, KEY_DOWN) ||
	//	App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_X, KEY_DOWN)) && !selected_nodes.empty())
	//{
	//	if (!aux_nodes.empty())
	//	{
	//		for (std::string aux : aux_nodes)
	//			aux->cut = false;
	//	}
	//	aux_nodes = selected_nodes;
	//	is_cut = true;
	//	is_copy = false;

	//	for (AssetNode* aux : aux_nodes)
	//		aux->cut = true;
	//}

	//// Copy
	//if ((App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_C, KEY_DOWN) ||
	//	App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_C, KEY_DOWN)) && !selected_nodes.empty())
	//{
	//	aux_nodes = selected_nodes;
	//	is_copy = true;
	//	is_cut = false;
	//}

	//// Paste
	//if ((App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_V, KEY_DOWN) ||
	//	App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_V, KEY_DOWN)) && (is_copy || is_cut))
	//{
	//	if (is_cut)
	//	{
	//		for (AssetNode* aux : aux_nodes)
	//		{
	//			if (current_folder != aux && !IsChildOf(*aux, *current_folder))
	//				Cut(*aux, *current_folder);
	//			aux->cut = false;
	//		}
	//	}
	//	else if (is_copy)
	//	{
	//		for (AssetNode* aux : aux_nodes)
	//		{
	//			if (current_folder != aux && !IsChildOf(*aux, *current_folder))
	//				Copy(*aux, *current_folder);
	//		}
	//	}
	//}

	//// SelectAll
	//if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN) ||
	//	App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_A, KEY_DOWN))
	//{
	//	SelectAll();
	//}

	//// Search
	//if (App->input->Shortcut(SDL_SCANCODE_LCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN) ||
	//	App->input->Shortcut(SDL_SCANCODE_RCTRL, KEY_REPEAT, SDL_SCANCODE_F, KEY_DOWN))
	//{
	//	is_search = !is_search;
	//}
}

void Assets::Save(Config* config) const
{
}

void Assets::Load(Config* config)
{
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
			if (ImGui::MenuItem("None", NULL, filter == ResourceType::UNKNOWN))			filter = ResourceType::UNKNOWN;
			ImGui::Separator();
			if (ImGui::MenuItem("Folders", NULL, filter == ResourceType::FOLDER))		filter = ResourceType::FOLDER;
			if (ImGui::MenuItem("Scenes", NULL, filter == ResourceType::SCENE))			filter = ResourceType::SCENE;
			if (ImGui::MenuItem("Prefabs", NULL, filter == ResourceType::PREFAB))		filter = ResourceType::PREFAB;
			if (ImGui::MenuItem("Textures", NULL, filter == ResourceType::TEXTURE))		filter = ResourceType::TEXTURE;
			if (ImGui::MenuItem("Materials", NULL, filter == ResourceType::MATERIAL))	filter = ResourceType::MATERIAL;
			if (ImGui::MenuItem("Animations", NULL, filter == ResourceType::ANIMATION)) filter = ResourceType::ANIMATION;
			if (ImGui::MenuItem("Tilemaps", NULL, filter == ResourceType::TILEMAP))		filter = ResourceType::TILEMAP;
			if (ImGui::MenuItem("Audios", NULL, filter == ResourceType::AUDIO))			filter = ResourceType::AUDIO;
			if (ImGui::MenuItem("Scripts", NULL, filter == ResourceType::SCRIPT))		filter = ResourceType::SCRIPT;
			ImGui::EndMenu();
		}
		ImGui::MenuItem("Search", NULL, &is_search); // Search
		ImGui::EndMenuBar();
	}

	// Search Bar
	if (is_search)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 7, ImGui::GetCursorPosY() + 2));
		searcher.Draw("Search", 180);
		ImGui::Separator();
	}

	//Draw Hierarchy Tree
	ImGui::BeginChild("HierarchyTree");

	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(pos.x + 5, pos.y + 3));
	DrawHierarchy(0); //root

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
			//DeleteNodes(selected_nodes);
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
		case ResourceType::FOLDER:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Folders"); break;
		case ResourceType::SCENE:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Scenes"); break;
		case ResourceType::PREFAB:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Prefabs"); break;
		case ResourceType::TEXTURE:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Textures"); break;
		case ResourceType::MATERIAL:	ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Materials"); break;
		case ResourceType::ANIMATION:	ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Animations"); break;
		case ResourceType::TILEMAP:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All TileMaps"); break;
		case ResourceType::AUDIO:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Audios"); break;
		case ResourceType::SCRIPT:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Scripts"); break;

		case ResourceType::UNKNOWN: // Path with links
			//if (nodes.parent[current_folder] != "")
			//	DrawPath();
			ImGui::Text(nodes.name[current_folder].c_str());
			break;
		}
		ImGui::EndMenuBar();
	}

	// Draw Icons
	int columns = (int)(ImGui::GetContentRegionAvailWidth() / (node_size + 4));
	//if (filter == ResourceType::UNKNOWN)
	//	current_list = nodes.childs[current_folder];
	//else
		//current_list = nodes;

	for (uint i = 0, size = nodes.childs[current_folder].size(); i < size; ++i)
	{
		int index = FindNode(nodes.childs[current_folder][i].c_str(), nodes.name);
		if (index != -1)
		{
			if ((filter != ResourceType::UNKNOWN && filter != nodes.type[index]) || !searcher.PassFilter(nodes.name[index].c_str()))
				continue;
		}
		if (is_list_view)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			DrawNodeList(index);
		}
		else
		{
			float pos = ImGui::GetCursorPosX();
			DrawNodeIcon(index);

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

void Assets::DrawHierarchy(size_t index)
{
	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;
	static ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 size = ImVec2(ImGui::GetWindowContentRegionMax().x, 14);
	ImVec4 color = colors[ImGuiCol_WindowBg];
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + size.x + ImGui::GetScrollX(), pos.y + size.y + 2));
	
	// Selectable
	pos = ImGui::GetCursorPos();
	ImGui::Dummy(size);
	if (ImGui::IsItemHovered())
	{
		color = colors[ImGuiCol_ButtonHovered];
		if (ImGui::IsItemClicked())
		{
			UnSelectAll();
			current_folder = index;
			filter = ResourceType::UNKNOWN;
		}
	}
	if (current_folder == index)
		color = colors[ImGuiCol_ButtonActive];

	//// Drag & Drop
	//if (ImGui::BeginDragDropTarget() && nodes.type[index] == ResourceType::FOLDER)
	//{
	//	color = colors[ImGuiCol_ButtonHovered];
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
	//	{
	//		for (std::string selected_node : selected_nodes)
	//		{
	//			if (selected_node != &node && !IsChildOf(*selected_node, node))
	//				Cut(*selected_node, node);
	//		}
	//	}
	//	ImGui::EndDragDropTarget();
	//}

	// Highlight
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(color));

	//// Indent
	//ImGui::SameLine();
	//ImGui::SetCursorPosX(pos.x + 14 * GetNumParents(node));

	//// Arrow
	//pos = ImGui::GetCursorPos();
	//ImGui::SetCursorPosY(pos.y - 1);
	//if (!nodes.childs[index].empty())
	//{
	//	if (nodes.open[index])
	//		ImGui::Text(ICON_ARROW_OPEN);
	//	else
	//		ImGui::Text(ICON_ARROW_CLOSED);

	//	if (ImGui::IsItemClicked())
	//		nodes.open[index] = !nodes.open[index];

	//	if (!nodes.open[index] && ImGui::BeginDragDropTarget() && nodes.type[index] == ResourceType::FOLDER)
	//	{
	//		nodes.open[index] = true;
	//		is_arrow_hover = true;
	//	}
	//	ImGui::SameLine();
	//	ImGui::SetCursorPosX(pos.x + 16);
	//}

	//// Name
	//ImGui::Text(nodes.name[index].c_str());

	//// Childs
	//if (nodes.open[index] && !nodes.childs[index].empty())
	//{
	//	for (std::string child : nodes.childs[index])
	//	{
	//		size_t pos = GetNode(child);
	//		if (App->file_system->IsFolder(nodes.path[pos].c_str()))
	//			DrawHierarchy(pos);
	//	}
	//}
}

void Assets::DrawNodeIcon(size_t index)
{
	bg_color.w = 0.0f;
	border_color.w = 0.0f;
	node_size = icon_size + 25.0f;
	ImVec2 pos = ImGui::GetCursorPos();

	// Dummy
	ImGui::Dummy(ImVec2(node_size,node_size));

	////Drag and Drop
	//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceAllowNullID)) // Source
	//{
	//	ImGui::SetDragDropPayload("AssetNode", &node.name, sizeof(std::string));

	//	if (!selected_nodes.empty()) // Popup text
	//	{
	//		if (selected_nodes.size() == 1)
	//			ImGui::Text(selected_nodes[0]->name.c_str());
	//		else
	//			ImGui::Text(std::to_string(selected_nodes.size()).c_str());
	//	}
	//	else
	//		ImGui::Text(node.name.c_str());

	//	if (!node.selected && !selected_nodes.empty() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) // Selection
	//		UnSelectAll();
	//	node.selected = true;

	//	ImGui::EndDragDropSource();
	//}
	//if (ImGui::BeginDragDropTarget() && node.type == ResourceType::FOLDER) // Target
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
	//	{
	//		for (AssetNode* selected_node : selected_nodes)
	//			Cut(*selected_node, node);
	//	}
	//	bg_color.w = 0.3f;
	//	ImGui::EndDragDropTarget();
	//}

	// Handle Selection
	HandleSelection(index);

	// Draw Highlight
	if (nodes.state[index] != State::RENAME)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
	}

	// Draw Actual Node
	ImGui::SetCursorPos(ImVec2(pos.x + (node_size - icon_size) / 2, pos.y));

	ImGui::BeginGroup();
	ImGui::SetCursorPosY(pos.y + 5);

	if (nodes.state[index] == State::CUT) // Transparent Image if node is cut
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

	ImGui::Image((ImTextureID)0, ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0)); // Image

	if (nodes.state[index] == State::CUT)
		ImGui::PopStyleVar();

	if (ImGui::IsItemClicked() && nodes.state[index] == State::RENAME)
		nodes.state[index] = State::SELECTED;

	if (nodes.state[index] != State::RENAME)
	{
		// Text
		std::string text = nodes.name[index];
		uint text_size = (uint)ImGui::CalcTextSize(text.c_str()).x;
		uint max_size = (uint)(node_size - 7 - ImGui::CalcTextSize("...").x) / 7;
		ImGui::SetCursorPosX(pos.x + 7);
		
		if (text_size > node_size - 14)
			text = text.substr(0, max_size) + "...";
		else
			ImGui::SetCursorPosX(pos.x + (node_size - text_size) / 2);
		ImGui::Text(text.c_str());

		// Show full name
		if (ImGui::IsItemHovered() && !ImGui::IsMouseDragging(0))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(nodes.name[index].c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	else // Rename
	{
		char buffer[128];
		sprintf_s(buffer, 128, "%s", nodes.name[index].c_str());

		ImGui::SetCursorPosX(pos.x);
		ImGui::SetNextItemWidth(node_size);
		if (ImGui::InputText("##RenameAsset", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			nodes.state[index] = State::SELECTED;
			if (buffer != nodes.name[index])
			{
				nodes.name[index] = GetNameWithCount(buffer);
				std::string new_path = nodes.path[index];
				new_path = new_path.substr(0, new_path.find_last_of("/") + 1) + nodes.name[index];
				MoveFile(nodes.path[index].c_str(), new_path.c_str());
				nodes.path[index] = new_path.c_str();
			
				for (std::string child : nodes.childs[index])
					UpdatePath(GetNode(child), nodes.path[index]);
			}
		}
		if (ImGui::IsItemClicked() || is_rename_flag)
		{
			is_rename_flag = false;
			ImGui::SetKeyboardFocusHere(-1);
		}
	}
	ImGui::EndGroup();

	// Update Selected Nodes List
	int position = FindNode(nodes.name[index].c_str(), selected_nodes);
	if (nodes.state[index] == State::SELECTED && position == -1)
		selected_nodes.push_back(nodes.name[index]);
	else if (nodes.state[index] != State::SELECTED && position != -1)
		selected_nodes.erase(selected_nodes.begin() + position);
}

void Assets::DrawNodeList(size_t index)
{
	//ImVec4 text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	//bg_color.w = 0.0f;
	//border_color.w = 0.0f;
	//ImVec2 pos = ImGui::GetCursorPos();
	//ImVec2 size = ImVec2(ImGui::GetContentRegionAvailWidth(), 20);
	//bool tooltip = true;

	//// Dummy
	//ImGui::Dummy(size);

	////Drag and Drop
	//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceAllowNullID)) // Source
	//{
	//	ImGui::SetDragDropPayload("AssetNode", &node.name, sizeof(std::string));

	//	if (!selected_nodes.empty()) // Popup text
	//	{
	//		if (selected_nodes.size() == 1)
	//			ImGui::Text(selected_nodes[0]->name.c_str());
	//		else
	//			ImGui::Text(std::to_string(selected_nodes.size()).c_str());
	//	}
	//	else
	//		ImGui::Text(node.name.c_str());

	//	if (!node.selected && !selected_nodes.empty() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) // Selection
	//		UnSelectAll();
	//	node.selected = true;

	//	ImGui::EndDragDropSource();
	//}
	//if (ImGui::BeginDragDropTarget() && node.type == ResourceType::FOLDER) // Target
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
	//	{
	//		for (AssetNode* selected_node : selected_nodes)
	//			Cut(*selected_node, node);
	//	}
	//	bg_color.w = 0.3f;
	//	ImGui::EndDragDropTarget();
	//}

	//// Handle Selection
	//HandleSelection(node);

	//// Draw Highlight
	//if (!node.rename)
	//{
	//	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
	//	ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
	//}

	//// Draw Actual Node
	//ImGui::SetCursorPos(ImVec2(pos.x + (node_size - icon_size) / 2, pos.y));

	//ImGui::BeginGroup();
	//ImGui::SetCursorPosY(pos.y + 4);

	//if (node.cut)
	//	text_color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

	//if (!node.rename)
	//{
	//	// Text
	//	ImVec4 icon_color = GetIconColor(node.type);
	//	std::string icon = GetIconList(node.type);
	//	std::string text = node.name;
	//	uint text_size = (uint)ImGui::CalcTextSize(std::string(icon + " " + text).c_str()).x;
	//	int max_size = (int)(size.x - 7 - ImGui::CalcTextSize("...").x) / 7;
	//	ImGui::SetCursorPosX(pos.x + 7);

	//	if (max_size >= 3 && text_size > size.x - 14)
	//		text = text.substr(0, max_size) + "...";
	//	else if (max_size < 3)
	//		text = "...";
	//	else
	//		tooltip = false;

	//	ImGui::TextColored(icon_color, icon.c_str());
	//	ImGui::SameLine();
	//	ImGui::TextColored(text_color, text.c_str());
	//}
	//else // Rename
	//{
	//	char buffer[128];
	//	sprintf_s(buffer, 128, "%s", node.name.c_str());

	//	ImGui::SetCursorPosX(pos.x);
	//	if (ImGui::InputText("##RenameAsset", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	//	{
	//		node.rename = false;
	//		if (buffer != node.name)
	//		{
	//			node.name = GetNameWithCount(buffer);
	//			std::string new_name = node.path.substr(0, node.path.find_last_of("/") + 1) + node.name;
	//			MoveFile(node.path.c_str(), new_name.c_str());
	//			node.path = new_name;

	//			for (AssetNode* child : node.childs)
	//				UpdatePath(*child, node.path);
	//		}
	//	}
	//	if (ImGui::IsItemClicked() || is_rename_flag)
	//	{
	//		is_rename_flag = false;
	//		ImGui::SetKeyboardFocusHere(-1);
	//	}
	//}
	//ImGui::EndGroup();

	//// Show full name
	//if (tooltip && ImGui::IsItemHovered() && !ImGui::IsMouseDragging(0))
	//{
	//	ImGui::BeginTooltip();
	//	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	//	ImGui::TextUnformatted(node.name.c_str());
	//	ImGui::PopTextWrapPos();
	//	ImGui::EndTooltip();
	//}

	//// Update Selected Nodes List
	//int position = FindNode(node, selected_nodes);
	//if (node.selected && position == -1)
	//	selected_nodes.push_back(&node);
	//else if (!node.selected && position != -1)
	//	selected_nodes.erase(selected_nodes.begin() + position);
}

// --- MAIN HELPERS ---
void Assets::HandleSelection(size_t index)
{
	if (ImGui::IsItemHovered()) // Hover
	{
		if (ImGui::IsMouseClicked(0) && nodes.state[index] != State::RENAME) // Left Click
		{
			//Show in Resources Panel

			if (ImGui::GetIO().KeyCtrl) // Multiple Selection (Ctrl)
			{
				if (selected_nodes.size() > 1)
				{
					if (nodes.state[index] == State::SELECTED)
						nodes.state[index] = State::IDLE;
					else
						nodes.state[index] = State::SELECTED;
				}
				else
					nodes.state[index] = State::SELECTED;
			}
			else if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
			{
				size_t pos1 = GetNode(nodes.path[index]);
				size_t pos2 = FindNode(selected_nodes.back().c_str(), nodes.name);

				if (pos1 < pos2)
				{
					for (size_t i = pos1; i < pos2; ++i)
						nodes.state[i] = State::SELECTED;
				}
				else
				{
					for (size_t i = pos1; i >= pos2; --i)
						nodes.state[i] = State::SELECTED;
				}
			}
		}
		if (ImGui::IsMouseReleased(0)) // Single Selection (Left Release)
		{
			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !ImGui::IsMouseDragging(0) && nodes.state[index] != State::RENAME)
			{
				UnSelectAll();
				nodes.state[index] = State::SELECTED;
			}
		}
		else if (ImGui::IsMouseClicked(1)) // Right Click
		{
			if (selected_nodes.size() <= 1 || nodes.state[index] != State::SELECTED)
				UnSelectAll();
			nodes.state[index] = State::SELECTED;
		}

		if (ImGui::IsMouseDoubleClicked(0)) // Double Click
		{
			switch (nodes.type[index])
			{
			case ResourceType::FOLDER:
				current_folder = index;
				filter = ResourceType::UNKNOWN;
				UnSelectAll();
				break;
			case ResourceType::SCENE:
				//Load Scene
				break;
			case ResourceType::SCRIPT:
				//Open in Editor
				break;
			default:
				break;
			}
		}

		bg_color.w = 0.3f;
	}

	if (nodes.state[index] == State::SELECTED) // Color
	{
		bg_color.w = 0.5f;
		border_color.w = 1.0f;
	}
}

bool Assets::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Assets"))
	{
		if (ImGui::BeginMenu("Create")) //create
		{
			if (ImGui::MenuItem("Folder")) //folder
			{
				size_t index = CreateNode("Folder", current_folder);
				UnSelectAll();
				nodes.state[index] = State::SELECTED;
			}
			if (ImGui::MenuItem("Script")) //script
			{
				size_t index = CreateNode("Script.scr", current_folder);
				UnSelectAll();
				nodes.state[index] = State::SELECTED;
			}

			ImGui::EndMenu();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty())) //cut
		{
			//if (!aux_nodes.empty())
			//{
			//	for (AssetNode* aux : aux_nodes)
			//		aux->cut = false;
			//}
			//aux_nodes = selected_nodes;
			//is_cut = true;
			//is_copy = false;

			//for (AssetNode* aux : aux_nodes)
			//	aux->cut = true;
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, !selected_nodes.empty())) //copy
		{
			aux_nodes = selected_nodes;
			is_copy = true;
			is_cut = false;
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, is_copy || is_cut)) //paste
		{
			//if (is_cut)
			//{
			//	for (AssetNode* aux : aux_nodes)
			//	{
			//		if (current_folder != aux && !IsChildOf(*aux, *current_folder))
			//			Cut(*aux, *current_folder);
			//		aux->cut = false;
			//	}
			//}
			//else if (is_copy)
			//{
			//	for (AssetNode* aux : aux_nodes)
			//	{
			//		if (current_folder != aux && !IsChildOf(*aux, *current_folder))
			//			Copy(*aux, *current_folder);
			//	}
			//}
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, !nodes.childs[current_folder].empty())) //select all
			SelectAll();

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
		{
			is_rename_flag = true;

			size_t index = GetNode(selected_nodes.front());
			nodes.state[index] = State::RENAME;
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
			size_t index = GetNode(selected_nodes[0]);
			GetFullPathName(nodes.path[index].c_str(), 4096, buffer, NULL);

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

void Assets::UpdateAssets() //***put the check inside ModuleInput.cpp as a SDL_Event and then call UpdateAssets()
{
	HWND wd = FindWindow(NULL, App->GetAppName());
	if (wd != GetForegroundWindow() && is_engine_focus)
	{
		is_engine_focus = false;
	}
	else if (wd == GetForegroundWindow() && !is_engine_focus)
	{
		is_engine_focus = true;
		std::string path = nodes.path[current_folder];

		std::vector<std::string> open_nodes;
		for (size_t i = 0, size = nodes.name.size(); i < size; ++i)
		{
			if (nodes.open[i])
				open_nodes.push_back(nodes.path[i]);
		}
		nodes.Clear();

		std::vector<std::string> ignore_ext;
		ignore_ext.push_back("meta");
		nodes = App->file_system->GetAllFiles("Assets", nullptr, &ignore_ext);

		nodes.open[0] = true;
		for (std::string node_path : open_nodes)
		{
			int index = GetNode(node_path);
			if (index != -1)
				nodes.open[index] = true;
		}

		if (current_folder != 0)
			current_folder = GetNode(path);

		// Update Layouts (Configuration Panel)
		App->editor->panel_configuration->GetLayouts();
	}
}

//void Assets::DrawPath()
//{
//	std::vector<AssetNode*> parents = GetParents(*current_folder);
//	std::string path, aux_path;
//	int count = 0, position = 0;
//
//	// path
//	for (int i = parents.size() - 1; i >= 0; --i)
//		path += parents[i]->name + " > ";
//	path += current_folder->name;
//	aux_path = path;
//
//	// check size and get count of parents not shown
//	float size = ImGui::CalcTextSize(path.c_str()).x + 44;
//	float max_size = ImGui::GetWindowWidth() + ImGui::CalcTextSize("<< ").x;
//	if (size > max_size)
//	{
//		int offset = int((size - max_size) / 7);
//		path = path.substr(offset);
//		position = path.find_first_of(">");
//
//		int found = 0;
//		if (position != -1)
//		{
//			while (found - offset != position)
//			{
//				found = aux_path.find(">", found + 1);
//				count++;
//			}
//		}
//		else
//			count = parents.size();
//
//		// '<<' Button
//		ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
//		float pos = ImGui::GetCursorPosX();
//		ImGui::Text("<<");
//
//		bool hovered = false;
//		if (ImGui::IsItemHovered())
//			color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
//
//		if (ImGui::IsItemClicked())
//			ImGui::OpenPopup("<<");
//
//		if (ImGui::BeginPopup("<<")) //popup
//		{
//			for (int i = parents.size() - 1, size = parents.size() - count; i >= size; --i)
//			{
//				if (ImGui::MenuItem(parents[i]->name.c_str()))
//					current_folder = parents[i];
//			}
//			ImGui::EndPopup();
//		}
//
//		float pos2 = ImGui::GetCursorPosX();
//		ImGui::SetCursorPosX(pos);
//		ImGui::TextColored(color, "<<");
//		ImGui::SetCursorPosX(pos2);
//	}
//
//	// Buttons
//	if (position != -1)
//	{
//		static uint selected = 0;
//		for (int i = parents.size() - count - 1; i >= 0; --i)
//		{
//			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
//			ImVec4 color2 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
//
//			// Parent Button
//			float pos = ImGui::GetCursorPosX();
//			ImGui::Text(parents[i]->name.c_str());
//
//			if (ImGui::IsItemHovered())
//				color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
//			if (ImGui::IsItemClicked())
//				current_folder = parents[i];
//
//			float pos2 = ImGui::GetCursorPosX();
//			ImGui::SetCursorPosX(pos);
//			ImGui::TextColored(color, parents[i]->name.c_str());
//			ImGui::SetCursorPosX(pos2);
//
//			// '>' Button
//			pos = ImGui::GetCursorPosX();
//			ImGui::Text(">");
//
//			if (ImGui::IsItemHovered())
//				color2 = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
//
//			if (ImGui::IsItemClicked())
//			{
//				ImGui::OpenPopup(">");
//				selected = i;
//			}
//
//			if (i == selected && ImGui::BeginPopup(">")) //popup
//			{
//				for (AssetNode* child : parents[i]->childs)
//				{
//					if (child->type == ResourceType::FOLDER &&
//						ImGui::MenuItem(child->name.c_str(), NULL, child == current_folder || FindNode(*child, parents) != -1))
//						current_folder = child;
//				}
//				ImGui::EndPopup();
//			}
//
//			pos2 = ImGui::GetCursorPosX();
//			ImGui::SetCursorPosX(pos);
//			ImGui::TextColored(color2, ">");
//			ImGui::SetCursorPosX(pos2);
//		}
//	}
//}

// --- NODES ---
size_t Assets::CreateNode(std::string name_, size_t parent_index)
{
	// Get Node Variables
	std::string name;
	if (name_ == "")
	{
		name = App->file_system->GetFileName(nodes.path[parent_index].c_str());
		if (name == "")
			name = nodes.path[parent_index];
	}
	else
		name = name_;
	name = GetNameWithCount(name);
	std::string path = nodes.path[parent_index] + std::string("/") + name;
	ResourceType type = App->file_system->GetType(path.c_str());
	std::vector<std::string> empty_childs;

	// Add Node
	size_t index = nodes.Add(path.c_str(), name, type, empty_childs, nodes.name[parent_index]);

	// Create Real Folder
	if (nodes.type[index] == ResourceType::FOLDER)
		App->file_system->CreateFolder(nodes.path[index].c_str());

	// Add node to parent's child list
	nodes.childs[parent_index].push_back(nodes.name[index]);

	return index;
}

void Assets::DeleteNodes(std::vector<std::string> nodes_list)
{
	while (!nodes_list.empty())
	{
		size_t index = GetNode(nodes_list.front());

		// Delete node from parent's child list
		const char* parent_name = nodes.parent[index].c_str();
		if (parent_name != "")
		{
			int parent_index = FindNode(parent_name, nodes.name);
			if (parent_index != -1)
			{
				int child_index = FindNode(nodes.name[index].c_str(), nodes.childs[parent_index]);
				if (child_index != -1)
					nodes.childs[parent_index].erase(nodes.childs[parent_index].begin() + child_index);
			}
		}

		// Delete childs
		if (!nodes.childs[index].empty())
			DeleteNodes(nodes.childs[index]);

		// Delete node data
		App->file_system->Remove(nodes.path[index].c_str());
		nodes.Remove(index);
		nodes_list.erase(nodes_list.begin());
	}
	nodes_list.clear();
}

size_t Assets::GetNode(const std::string path) const
{
	for (size_t index = 0, size = nodes.path.size(); index < size; ++index)
	{
		if (nodes.path[index] == path)
			return index;
	}
	return 0;
}

//std::vector<AssetNode*> Assets::GetParents(AssetNode& node) const
//{
//	std::vector<AssetNode*> parents;
//	AssetNode* aux_node = &node;
//	while (aux_node->parent != nullptr)
//	{
//		parents.push_back(aux_node->parent);
//		aux_node = aux_node->parent;
//	}
//	return parents;
//}

//uint Assets::GetNumParents(AssetNode& node) const
//{
//	uint count = 0;
//	AssetNode* aux_node = &node;
//	while (aux_node->parent != nullptr)
//	{
//		count++;
//		aux_node = aux_node->parent;
//	}
//	return count;
//}

std::string Assets::GetNameWithCount(const std::string name) const
{
	if (nodes.name.empty())
		return name;

	bool found = false;
	uint count = 0;
	std::string new_name = name;

	while (found == false)
	{
		for (size_t i = 0, size = nodes.name.size(); i < size; ++i)
		{
			if (new_name == nodes.name[i])
			{
				count++;
				new_name = name + (" (") + std::to_string(count) + (")");
				break;
			}
			else if (i == nodes.name.size() - 1)
			{
				found = true;
				break;
			}
		}
	}
	return new_name;
}

std::string Assets::GetIconList(const ResourceType type) const
{
	switch (type)
	{
	case ResourceType::FOLDER:		return ICON_FOLDER;
	case ResourceType::SCENE:		return ICON_SCENE;
	case ResourceType::PREFAB:		return ICON_PREFAB;
	case ResourceType::TEXTURE:		return ICON_TEXTURE;
	case ResourceType::MATERIAL:	return ICON_SHADER;
	case ResourceType::ANIMATION:	return ICON_ANIMATION;
	case ResourceType::TILEMAP:		return ICON_TILEMAP;
	case ResourceType::AUDIO:		return ICON_AUDIO;
	case ResourceType::SCRIPT:		return ICON_SCRIPT;
	case ResourceType::SHADER:		return ICON_SHADER;
	case ResourceType::UNKNOWN:		return "";
	default:						return "";
	}
}

ImVec4 Assets::GetIconColor(const ResourceType type) const
{
	switch (type)
	{
	case ResourceType::FOLDER:		return ImVec4(0.9f, 0.6f, 0.0f, 1.0f); //orange
	case ResourceType::SCENE:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //white
	case ResourceType::PREFAB:		return ImVec4(0.2f, 1.0f, 1.0f, 1.0f); //blue
	case ResourceType::TEXTURE:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::MATERIAL:	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::ANIMATION:	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::TILEMAP:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::AUDIO:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::SCRIPT:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::SHADER:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	case ResourceType::UNKNOWN:		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	default:						return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //
	}
}

//bool Assets::IsChildOf(const AssetNode& node, AssetNode& child) const
//{
//	for (AssetNode* aux : node.childs)
//	{
//		if (aux == &child || (!aux->childs.empty() && IsChildOf(*aux, child)))
//			return true;
//	}
//	return false;
//}

int Assets::FindNode(const char* name, std::vector<std::string> list) const
{
	for (uint i = 0; i < list.size(); ++i)
	{
		if (list[i] == name)
			return i;
	}
	return -1;
}

void Assets::UpdatePath(size_t index, const std::string path)
{
	nodes.path[index] = std::string(path + ("/") + nodes.name[index]).c_str();

	if (!nodes.childs[index].empty())
	{
		for (std::string child : nodes.childs[index])
			UpdatePath(GetNode(child), nodes.path[index]);
	}
}

void Assets::SelectAll()
{
	for (size_t index = 0, size = nodes.name.size(); index < size; ++index)
		nodes.state[index] = State::SELECTED;
}

void Assets::UnSelectAll()
{
	for (size_t index = 0, size = nodes.name.size(); index < size; ++index)
		nodes.state[index] = State::IDLE;

	selected_nodes.clear();
}

//void Assets::Cut(AssetNode& node, AssetNode& parent) const
//{
//	// Move in Explorer
//	MoveFile(node.path.c_str(), std::string(parent.path + ("/") + node.name).c_str());
//
//	// Update Parent
//	node.parent->childs.erase(node.parent->childs.begin() + FindNode(node, node.parent->childs)); //erase from parent's child list
//	node.parent = &parent; //set new parent
//	node.parent->childs.push_back(&node); //add node to new parent's child list
//
//	// Update Childs
//	UpdatePath(node, parent.path);
//}
//
//void Assets::Copy(AssetNode& node, AssetNode& parent)
//{
//	// Create new Node
//	std::string new_name = node.name + " - copy";
//	AssetNode* new_node = CreateNode(new_name, &parent);
//
//	// If has childs, copy them too
//	if (!node.childs.empty())
//	{
//		std::vector<AssetNode*> childs = node.childs;
//		for (AssetNode* child : childs)
//			Copy(*child, *new_node);
//	}
//
//	//// Copy in Explorer
//	//std::string full_path = App->file_system->GetBasePath() + node.path;
//	//std::string full_newpath = App->file_system->GetBasePath() + new_node->path;
//
//	//App->file_system->Copy(full_path.c_str(), full_newpath.c_str());
//}

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