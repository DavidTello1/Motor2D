#include "PanelAssets.h"
#include "Application.h"
#include "ModuleInput.h"
//#include "ModuleScene.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "ModuleEditor.h"
#include "PanelConfiguration.h"

#include "Imgui/imgui_internal.h"
#include <windows.h>
#include <ShlObj_core.h>

#include "mmgr/mmgr.h"

ImGuiTextFilter PanelAssets::searcher;

// ---------------------------------------------------------
PanelAssets::PanelAssets() : Panel("Assets", ICON_ASSETS, 4)
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
	nodes = App->resources->GetAllFiles("Assets", nullptr, &ignore_ext);
	current_list = nodes.name;
	current_folder = 0;
}

PanelAssets::~PanelAssets()
{
}

void PanelAssets::Draw()
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

void PanelAssets::Shortcuts()
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
			for (std::string aux : aux_nodes)
				SetState(aux, AN_State::IDLE);
		}
		aux_nodes = selected_nodes;
		is_cut = true;
		is_copy = false;

		for (std::string aux : aux_nodes)
			SetState(aux, AN_State::CUT);
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
			for (std::string aux : aux_nodes)
			{
				int pos = FindNode(aux.c_str(), nodes.name);
				if (pos != 1)
				{
					if (current_folder != pos && !IsChildOf(pos, nodes.name[current_folder].c_str()))
						Cut(pos, current_folder);
					nodes.state[pos] = AN_State::IDLE;
				}
			}
		}
		else if (is_copy)
		{
			for (std::string aux : aux_nodes)
			{
				int pos = FindNode(aux.c_str(), nodes.name);
				if (pos != 1)
				{
					if (current_folder != pos && !IsChildOf(pos, nodes.name[current_folder].c_str()))
						Copy(pos, current_folder);
				}
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

void PanelAssets::Save(Config* config) const
{
}

void PanelAssets::Load(Config* config)
{
}

void PanelAssets::ChildHierarchy()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::Begin("Assets_Hierarchy", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	ImGui::PopStyleVar();

	// --- MENU BAR ---
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Import")) // Import
		{
			// Open Explorer to Select File
			// ImportFromExplorer(selected_file)
		}
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
			if (ImGui::MenuItem("Shaders", NULL, filter == ResourceType::SHADER))		filter = ResourceType::SHADER;
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

	//---  DRAW HIERARCHY TREE ---
	ImGui::BeginChild("HierarchyTree");

	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(pos.x + 5, pos.y + 3));
	DrawHierarchy(0); //root

	// Scroll
	Scroll(pos);

	is_any_hover = false;
	is_arrow_hover = false;

	ImGui::EndChild();
	ImGui::End();
}

void PanelAssets::ChildIcons()
{
	ImGui::Begin("Assets_Icons", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	// Allow selection & show options with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(1))
		ImGui::SetWindowFocus();

	// Right Click Options
	DrawRightClick();

	// Delete Popup
	DeletePopup();

	// --- MENU BAR ---
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
		case ResourceType::SHADER:		ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Showing All Shaders"); break;

		case ResourceType::UNKNOWN: // Path with links
			if (nodes.parent[current_folder] != "")
				DrawPath();
			ImGui::Text(nodes.name[current_folder].c_str());
			break;
		}
		ImGui::EndMenuBar();
	}

	// --- DRAW ICONS ---
	int columns = (int)(ImGui::GetContentRegionAvailWidth() / (node_size + 4)); // Get Number of Columns

	// Get Current List to Draw (filters)
	if (filter == ResourceType::UNKNOWN)
		current_list = nodes.childs[current_folder];
	else
		current_list = nodes.name;

	// Draw
	if (is_list_view) // List View
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		DrawList(current_list);
	}
	else // Icon View
		DrawIcons(current_list, columns);

	// Unselect nodes when clicking on empty space
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
		ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && !is_any_hover)
		UnSelectAll();

	ImGui::End();
}

void PanelAssets::DrawHierarchy(size_t index)
{
	static ImGuiContext& g = *GImGui;
	static ImGuiWindow* window = g.CurrentWindow;
	static ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 size = ImVec2(ImGui::GetWindowContentRegionMax().x, 14);
	ImVec4 color = colors[ImGuiCol_WindowBg];
	ImVec2 pos = window->DC.CursorPos;
	ImRect bg(ImVec2(pos.x - 10, pos.y - g.Style.FramePadding.y), ImVec2(pos.x + size.x + ImGui::GetScrollX(), pos.y + size.y));
	
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

	// Drag & Drop
	if (ImGui::BeginDragDropTarget() && nodes.type[index] == ResourceType::FOLDER) // Target
	{
		color = colors[ImGuiCol_ButtonHovered];
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
		{
			for (std::string selected_node : selected_nodes)
			{
				int sel_index = FindNode(selected_node.c_str(), nodes.name);
				if (sel_index != -1)
				{
					if (sel_index != index && !IsChildOf(sel_index, nodes.name[index].c_str()))
						Cut(sel_index, index);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Highlight
	window->DrawList->AddRectFilled(bg.Min, bg.Max, ImColor(color));

	// Indent
	ImGui::SameLine();
	uint num_parents = GetNumParents(index);
	float indent = pos.x + 15 * num_parents;
	if (num_parents == 1)
		indent += 2;
	if (nodes.childs[index].empty())
		indent += 16;
	ImGui::SetCursorPosX(indent);

	// Arrow
	pos = ImGui::GetCursorPos();
	ImGui::SetCursorPosY(pos.y - 1);
	if (!nodes.childs[index].empty())
	{
		if (nodes.open[index])
			ImGui::Text(ICON_ARROW_OPEN);
		else
			ImGui::Text(ICON_ARROW_CLOSED);

		if (ImGui::IsItemClicked())
			nodes.open[index] = !nodes.open[index];

		if (!nodes.open[index] && ImGui::BeginDragDropTarget() && nodes.type[index] == ResourceType::FOLDER)
		{
			nodes.open[index] = true;
			is_arrow_hover = true;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos.x + 16);
	}

	// Name
	ImGui::Text(nodes.name[index].c_str());

	// Childs
	if (nodes.open[index] && !nodes.childs[index].empty())
	{
		for (std::string child : nodes.childs[index])
		{
			int pos = FindNode(child.c_str(), nodes.name);
			if (pos != -1 && App->file_system->IsFolder(nodes.path[pos].c_str()))
				DrawHierarchy(pos);
		}
	}
}

void PanelAssets::DrawIcons(std::vector<std::string> current_list, uint columns)
{
	size_t aux_current_folder = current_folder;
	for (size_t i = 0, list_size = current_list.size(); i < list_size; ++i)
	{
		if (current_folder != aux_current_folder) // Current folder has changed
			break;

		// Filter
		int index = FindNode(current_list[i].c_str(), nodes.name);
		if (index != -1)
		{
			if (index == 0 || (filter != ResourceType::UNKNOWN && filter != nodes.type[index]) || !searcher.PassFilter(nodes.name[index].c_str()))
				continue;

			bg_color.w = 0.0f;
			border_color.w = 0.0f;
			node_size = icon_size + 25.0f;
			ImVec2 pos = ImGui::GetCursorPos();
			float column_pos = pos.x;

			// Dummy
			ImGui::Dummy(ImVec2(node_size, node_size));

			// Handle Selection
			HandleSelection(index);

			// Draw Highlight
			if (nodes.state[index] != AN_State::RENAME)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
			}

			// Draw Actual Node
			ImGui::SetCursorPos(ImVec2(pos.x + (node_size - icon_size) / 2, pos.y));
			ImGui::BeginGroup();
			ImGui::SetCursorPosY(pos.y + 5);

			// Transparent Image if node is cut
			if (nodes.state[index] == AN_State::CUT)
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

			// Image
			ImGui::Image((ImTextureID)GetNodeImage(nodes.type[index]), ImVec2((float)icon_size, (float)icon_size), ImVec2(0, 1), ImVec2(1, 0));

			if (nodes.state[index] == AN_State::CUT)
				ImGui::PopStyleVar();

			// Stop Rename if Clicked
			if (ImGui::IsItemClicked() && nodes.state[index] == AN_State::RENAME)
				nodes.state[index] = AN_State::SELECTED;

			// Text
			if (nodes.state[index] != AN_State::RENAME)
			{
				std::string text = nodes.name[index];
				uint text_size = (uint)ImGui::CalcTextSize(text.c_str()).x;
				uint max_size = (uint)(node_size - 7 - ImGui::CalcTextSize("...").x) / 7;
				ImGui::SetCursorPosX(pos.x + 7);

				// Get Final Text Displayed
				if (text_size > node_size - 14)
					text = text.substr(0, max_size) + "...";
				else
					ImGui::SetCursorPosX(pos.x + (node_size - text_size) / 2);
				ImGui::Text(text.c_str());

				// Show full name (Tooltip)
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
				ImGui::SetCursorPosX(pos.x);
				Rename(index);
			}

			ImGui::EndGroup();

			// Update Selected Nodes List
			int position = FindNode(nodes.name[index].c_str(), selected_nodes);
			if ((nodes.state[index] == AN_State::SELECTED || nodes.state[index] == AN_State::DRAGGING) && position == -1)
				selected_nodes.push_back(nodes.name[index]);
			else if (nodes.state[index] != AN_State::SELECTED && nodes.state[index] != AN_State::DRAGGING && position != -1)
				selected_nodes.erase(selected_nodes.begin() + position);

			// Columns
			if (columns > 0 && (i + 1) % columns != 0)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(column_pos + node_size + 5);
			}
		}
	}
}

void PanelAssets::DrawList(std::vector<std::string> current_list)
{
	size_t aux_current_folder = current_folder;
	for (size_t i = 0, list_size = current_list.size(); i < list_size; ++i)
	{
		if (current_folder != aux_current_folder) // Current folder has changed
			break;

		// Filter
		int index = FindNode(current_list[i].c_str(), nodes.name);
		if (index != -1)
		{
			if (index == 0 || (filter != ResourceType::UNKNOWN && filter != nodes.type[index]) || !searcher.PassFilter(nodes.name[index].c_str()))
				continue;

			ImVec4 text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			bg_color.w = 0.0f;
			border_color.w = 0.0f;
			ImVec2 pos = ImGui::GetCursorPos();
			ImVec2 size = ImVec2(ImGui::GetContentRegionAvailWidth(), 20);
			bool tooltip = true;

			// Dummy
			ImGui::Dummy(size);

			// Handle Selection
			HandleSelection(index);

			// Draw Highlight
			if (nodes.state[index] != AN_State::RENAME)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(bg_color), 3.0f);
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(border_color), 3.0f);
			}

			// Draw Actual Node
			ImGui::SetCursorPos(ImVec2(pos.x + (node_size - icon_size) / 2, pos.y));

			ImGui::BeginGroup();
			ImGui::SetCursorPosY(pos.y + 4);

			// Transparent Text if node is cut
			if (nodes.state[index] == AN_State::CUT)
				text_color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

			// Icon
			ImVec4 icon_color = GetIconColor(nodes.type[index]);
			std::string icon = GetIconList(nodes.type[index]);
			ImGui::TextColored(icon_color, icon.c_str());
			ImGui::SameLine();

			if (nodes.state[index] != AN_State::RENAME)
			{
				// Text
				std::string text = nodes.name[index];
				uint text_size = (uint)ImGui::CalcTextSize(text.c_str()).x;
				int max_size = (int)(size.x - 7 - ImGui::CalcTextSize("...").x) / 7;
				//ImGui::SetCursorPosX(pos.x + 7);

				if (max_size >= 3 && text_size > size.x - 14)
					text = text.substr(0, max_size) + "...";
				else if (max_size < 3)
					text = "...";
				else
					tooltip = false;

				ImGui::TextColored(text_color, text.c_str());
			}
			else // Rename
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
				Rename(index);
			}

			ImGui::EndGroup();

			// Show full name
			if (tooltip && ImGui::IsItemHovered() && !ImGui::IsMouseDragging(0))
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(nodes.name[index].c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			// Update Selected Nodes List
			int position = FindNode(nodes.name[index].c_str(), selected_nodes);
			if ((nodes.state[index] == AN_State::SELECTED || nodes.state[index] == AN_State::DRAGGING) && position == -1)
				selected_nodes.push_back(nodes.name[index]);
			else if (nodes.state[index] != AN_State::SELECTED && nodes.state[index] != AN_State::DRAGGING && position != -1)
				selected_nodes.erase(selected_nodes.begin() + position);
		}
	}
}

void PanelAssets::ImportAsset()
{
}

// --- MAIN HELPERS ---
void PanelAssets::HandleSelection(size_t index)
{
	//Drag and Drop
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceAllowNullID)) // Source
	{
		ImGui::SetDragDropPayload("AssetNode", &index, sizeof(std::size_t));

		if (!selected_nodes.empty()) // Popup text
		{
			if (selected_nodes.size() == 1)
				ImGui::Text(selected_nodes[0].c_str());
			else
				ImGui::Text(std::to_string(selected_nodes.size()).c_str());
		}

		// Selection
		if (nodes.state[index] != AN_State::DRAGGING && nodes.state[index] != AN_State::SELECTED && !selected_nodes.empty() && 
			!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) 
			UnSelectAll();
		nodes.state[index] = AN_State::DRAGGING;

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget() && nodes.type[index] == ResourceType::FOLDER) // Target
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetNode"))
		{
			for (std::string selected_node : selected_nodes)
			{
				int pos = FindNode(selected_node.c_str(), nodes.name);
				if (pos != -1)
					Cut(pos, index);
			}
		}
		bg_color.w = 0.3f;
		ImGui::EndDragDropTarget();
	}

	// Selection
	if (ImGui::IsItemHovered()) // Hover
	{
		is_any_hover = true;
		if (ImGui::IsMouseClicked(0) && nodes.state[index] != AN_State::RENAME) // Left Click
		{
			//***Show in Resources Panel

			if (ImGui::GetIO().KeyCtrl) // Multiple Selection (Ctrl)
			{
				if (nodes.state[index] == AN_State::SELECTED)
					nodes.state[index] = AN_State::IDLE;
				else
					nodes.state[index] = AN_State::SELECTED;
			}
			else if (ImGui::GetIO().KeyShift && !selected_nodes.empty()) // Multiple Selection (Shift)
			{
				size_t pos = FindNode(selected_nodes.back().c_str(), nodes.name);
				if (index < pos)
				{
					for (size_t i = index; i < pos; ++i)
						nodes.state[i] = AN_State::SELECTED;
				}
				else
				{
					for (size_t i = index; i >= pos; --i)
						nodes.state[i] = AN_State::SELECTED;
				}
			}
		}
		if (ImGui::IsMouseReleased(0)) // Single Selection (Left Release)
		{
			if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift && nodes.state[index] != AN_State::DRAGGING && nodes.state[index] != AN_State::RENAME)
			{
				UnSelectAll();
				nodes.state[index] = AN_State::SELECTED;
			}
		}
		else if (ImGui::IsMouseClicked(1)) // Right Click
		{
			if (selected_nodes.size() <= 1 || nodes.state[index] != AN_State::SELECTED)
				UnSelectAll();
			nodes.state[index] = AN_State::SELECTED;
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

	if (nodes.state[index] == AN_State::SELECTED || nodes.state[index] == AN_State::DRAGGING) // Color
	{
		bg_color.w = 0.5f;
		border_color.w = 1.0f;
	}
}

bool PanelAssets::DrawRightClick()
{
	if (ImGui::BeginPopupContextWindow("Assets"))
	{
		if (ImGui::BeginMenu("Create")) //create
		{
			if (ImGui::MenuItem("Folder")) //folder
			{
				size_t index = CreateNode("Folder", current_folder);
				UnSelectAll();
				nodes.state[index] = AN_State::SELECTED;
			}
			if (ImGui::MenuItem("Script")) //script
			{
				size_t index = CreateNode("Script.scr", current_folder);
				UnSelectAll();
				nodes.state[index] = AN_State::SELECTED;
			}

			ImGui::EndMenu();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, !selected_nodes.empty())) //cut
		{
			if (!aux_nodes.empty())
			{
				for (std::string aux : aux_nodes)
					SetState(aux, AN_State::IDLE);
			}
			aux_nodes = selected_nodes;
			is_cut = true;
			is_copy = false;

			for (std::string aux : aux_nodes)
				SetState(aux, AN_State::CUT);
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
				for (std::string aux : aux_nodes)
				{
					int pos = FindNode(aux.c_str(), nodes.name);
					if (pos != -1)
					{
						if (current_folder != pos && !IsChildOf(pos, nodes.name[current_folder].c_str()))
							Cut(pos, current_folder);
						nodes.state[pos] = AN_State::IDLE;
					}
				}
			}
			else if (is_copy)
			{
				for (std::string aux : aux_nodes)
				{
					int pos = FindNode(aux.c_str(), nodes.name);
					if (pos != -1)
					{
						if (current_folder != pos && !IsChildOf(pos, nodes.name[current_folder].c_str()))
							Copy(pos, current_folder);
					}
				}
			}
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, !nodes.childs[current_folder].empty())) //select all
			SelectAll();

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
		{
			is_rename_flag = true;
			SetState(selected_nodes[0], AN_State::RENAME);
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
		ImGui::Separator();

		if (ImGui::MenuItem("Import")) //import
			ImportAsset();

		if (ImGui::MenuItem("Show in Explorer", NULL, nullptr, selected_nodes.size() == 1)) //show in explorer
		{
			TCHAR  buffer[4096] = TEXT("");
			int index = FindNode(selected_nodes[0].c_str(), nodes.name);
			if (index != -1)
			{
				GetFullPathName(nodes.path[index].c_str(), 4096, buffer, NULL);

				HRESULT hr = CoInitializeEx(0, NULL);
				ITEMIDLIST* pidl = ILCreateFromPath(std::string(buffer).substr(0, std::string(buffer).find_last_of("/")).c_str());
				if (pidl)
				{
					SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
					ILFree(pidl);
				}
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

void PanelAssets::UpdateAssets() //***put the check inside ModuleInput.cpp as a SDL_Event and then call UpdateAssets()
{
	HWND wd = FindWindow(NULL, App->GetAppName());
	if (wd != GetForegroundWindow() && is_engine_focus)
	{
		is_engine_focus = false;
	}
	else if (wd == GetForegroundWindow() && !is_engine_focus)
	{
		is_engine_focus = true;
		std::string node_name = nodes.name[current_folder];

		std::vector<std::string> open_nodes;
		for (size_t i = 0, size = nodes.name.size(); i < size; ++i)
		{
			if (nodes.open[i])
				open_nodes.push_back(nodes.name[i]);
		}
		nodes.Clear();

		std::vector<std::string> ignore_ext;
		ignore_ext.push_back("meta");
		nodes = App->resources->GetAllFiles("Assets", nullptr, &ignore_ext);

		nodes.open[0] = true;
		for (std::string name : open_nodes)
		{
			int index = FindNode(name.c_str(), nodes.name);
			if (index != -1)
				nodes.open[index] = true;
		}

		if (current_folder != 0)
		{
			int index = FindNode(node_name.c_str(), nodes.name);
			if (index != -1)
				current_folder = index;
		}

		// Update Layouts (Configuration Panel)
		App->editor->panel_configuration->GetLayouts();
	}
}

void PanelAssets::DrawPath()
{
	// Main Variables
	std::vector<std::string> parents = GetParents(current_folder);
	std::string path, aux_path;
	int count = 0, position = 0;

	// Path
	for (int i = parents.size() - 1; i >= 0; --i)
		path += parents[i] + " > ";
	path += nodes.name[current_folder];
	aux_path = path;

	// '<<' Button
	float size = ImGui::CalcTextSize(path.c_str()).x + 44;
	float max_size = ImGui::GetWindowWidth() + ImGui::CalcTextSize("<< ").x;
	if (size > max_size) // Check size and get count of parents not shown
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

		// '<<' Button Text
		ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		float pos = ImGui::GetCursorPosX();
		ImGui::Text("<<");

		bool hovered = false;
		if (ImGui::IsItemHovered())
			color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);

		if (ImGui::IsItemClicked())
			ImGui::OpenPopup("<<");

		if (ImGui::BeginPopup("<<")) // Popup
		{
			for (int i = parents.size() - 1, size = parents.size() - count; i >= size; --i)
			{
				if (ImGui::MenuItem(parents[i].c_str()))
				{
					int index = FindNode(parents[i].c_str(), nodes.name);
					if (index != -1)
						current_folder = index;
				}
			}
			ImGui::EndPopup();
		}

		float pos2 = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(pos);
		ImGui::TextColored(color, "<<");
		ImGui::SetCursorPosX(pos2);
	}

	// Path Buttons
	if (position != -1)
	{
		static uint selected = 0;
		for (int i = parents.size() - count - 1; i >= 0; --i)
		{
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImVec4 color2 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			// Parent Button
			float pos = ImGui::GetCursorPosX();
			ImGui::Text(parents[i].c_str());

			if (ImGui::IsItemHovered())
				color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
			if (ImGui::IsItemClicked())
			{
				int index = FindNode(parents[i].c_str(), nodes.name);
				if (index != -1)
					current_folder = index;
			}

			float pos2 = ImGui::GetCursorPosX();
			ImGui::SetCursorPosX(pos);
			ImGui::TextColored(color, parents[i].c_str());
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
				int index = FindNode(parents[i].c_str(), nodes.name);
				if (index != -1)
				{
					for (std::string child : nodes.childs[index])
					{
						int child_index = FindNode(child.c_str(), nodes.name);
						if (child_index != -1 && nodes.type[child_index] == ResourceType::FOLDER &&
							ImGui::MenuItem(child.c_str(), NULL, child_index == current_folder || FindNode(child.c_str(), parents) != -1))
							current_folder = child_index;
					}
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
size_t PanelAssets::CreateNode(std::string name_, size_t parent_index)
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
	ResourceType type = App->resources->GetType(path.c_str());
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

void PanelAssets::DeleteNodes(std::vector<std::string> nodes_list)
{
	int index = 0;
	for (size_t i = 0, size = nodes_list.size(); i < size; ++i)
	{
		index = FindNode(nodes_list[i].c_str(), nodes.name);
		if (index != -1)
		{
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

			App->file_system->Remove(nodes.path[index].c_str());
			nodes.Remove(index);
		}
	}
	nodes_list.clear();
}

std::vector<std::string> PanelAssets::GetParents(size_t index) const
{
	std::vector<std::string> parents;
	size_t aux_index = index;
	while (aux_index != -1 && nodes.parent[aux_index] != "")
	{
		parents.push_back(nodes.parent[aux_index]);
		aux_index = FindNode(nodes.parent[aux_index].c_str(), nodes.name);			
	}
	return parents;
}

uint PanelAssets::GetNumParents(size_t index) const
{
	uint count = 0;
	size_t aux_index = index;
	while (aux_index != -1 && !nodes.parent.empty() && nodes.parent[aux_index] != "")
	{
		count++;
		aux_index = FindNode(nodes.parent[aux_index].c_str(), nodes.name);
	}
	return count;
}

std::string PanelAssets::GetNameWithCount(const std::string name) const
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

std::string PanelAssets::GetIconList(const ResourceType type) const
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

ImVec4 PanelAssets::GetIconColor(const ResourceType type) const
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

uint32_t PanelAssets::GetNodeImage(const ResourceType type) const
{
	switch (type)
	{
	case ResourceType::FOLDER:		return App->resources->textures.texture.buffer[1];
	case ResourceType::SCENE:		return App->resources->textures.texture.buffer[2];
	case ResourceType::PREFAB:		return App->resources->textures.texture.buffer[3];
	case ResourceType::TEXTURE:		return App->resources->textures.texture.buffer[4];
	case ResourceType::MATERIAL:	return App->resources->textures.texture.buffer[5];
	case ResourceType::ANIMATION:	return App->resources->textures.texture.buffer[6];
	case ResourceType::TILEMAP:		return App->resources->textures.texture.buffer[7];
	case ResourceType::AUDIO:		return App->resources->textures.texture.buffer[8];
	case ResourceType::SCRIPT:		return App->resources->textures.texture.buffer[9];
	case ResourceType::SHADER:		return App->resources->textures.texture.buffer[10];
	case ResourceType::UNKNOWN:		return App->resources->textures.texture.buffer[11];
	default:						return 0;
	}
}

bool PanelAssets::IsChildOf(const size_t node, const char* child_name) const
{
	for (std::string aux : nodes.childs[node])
	{
		int pos = FindNode(aux.c_str(), nodes.name);
		if (pos != -1)
		{
			if (aux == child_name || (!nodes.childs[pos].empty() && IsChildOf(pos, child_name)))
				return true;
		}
	}
	return false;
}

int PanelAssets::FindNode(const char* name, std::vector<std::string> list) const
{
	for (size_t i = 0, size = list.size(); i < size; ++i)
	{
		if (list[i] == name)
			return i;
	}
	return -1;
}

void PanelAssets::UpdatePath(size_t index, std::string path, std::string parent)
{
	nodes.path[index] = std::string(path + ("/") + nodes.name[index]).c_str();
	nodes.parent[index] = parent;

	if (!nodes.childs[index].empty())
	{
		for (std::string child : nodes.childs[index])
		{
			int pos = FindNode(child.c_str(), nodes.name);
			if (pos != -1)
				UpdatePath(pos, nodes.path[index], nodes.name[index]);
		}
	}
}

void PanelAssets::SetState(std::string name, AN_State state)
{
	int pos = FindNode(name.c_str(), nodes.name);
	if (pos != -1)
		nodes.state[pos] = state;
}

void PanelAssets::SelectAll()
{
	for (size_t index = 0, size = nodes.name.size(); index < size; ++index)
		nodes.state[index] = AN_State::SELECTED;
}

void PanelAssets::UnSelectAll()
{
	for (size_t index = 0, size = nodes.name.size(); index < size; ++index)
	{
		if (nodes.state[index] != AN_State::CUT)
			nodes.state[index] = AN_State::IDLE;
	}
	selected_nodes.clear();
}

void PanelAssets::Cut(size_t node, size_t parent)
{
	// Move in Explorer
	MoveFile(nodes.path[node].c_str(), std::string(nodes.path[parent] + ("/") + nodes.name[node]).c_str());

	// Update Parent
	int parent_index = FindNode(nodes.parent[node].c_str(), nodes.name);
	if (parent_index != -1)
	{
		int child_index = FindNode(nodes.name[node].c_str(), nodes.childs[parent_index]);
		if (child_index != -1)
			nodes.childs[parent_index].erase(nodes.childs[parent_index].begin() + child_index); //erase from parent's child list
	}
	nodes.parent[node] = nodes.name[parent]; //set new parent
	nodes.childs[parent].push_back(nodes.name[node]); //add node to new parent's child list

	// Update Childs
	UpdatePath(node, nodes.path[parent], nodes.name[parent]);
}

void PanelAssets::Copy(size_t node, size_t parent)
{
	// Create new Node
	std::string new_name = nodes.name[node] + " - copy";
	size_t new_node = CreateNode(new_name, parent);

	// If has childs, copy them too
	if (!nodes.childs[node].empty())
	{
		std::vector<std::string> childs = nodes.childs[node];
		for (std::string child : childs)
		{
			int child_pos = FindNode(child.c_str(), nodes.name);
			if (child_pos != -1)
				Copy(child_pos, new_node);
		}
	}

	//// Copy in Explorer
	//std::string full_path = App->file_system->GetBasePath() + node.path;
	//std::string full_newpath = App->file_system->GetBasePath() + new_node->path;

	//App->file_system->Copy(full_path.c_str(), full_newpath.c_str());
}

// --- OTHERS ---
void PanelAssets::DockSpace()
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

void PanelAssets::Scroll(ImVec2 pos)
{
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
}

void PanelAssets::DeletePopup()
{
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
}

void PanelAssets::Rename(size_t index)
{
	char buffer[128];
	sprintf_s(buffer, 128, "%s", nodes.name[index].c_str());

	ImGui::SetNextItemWidth(node_size);
	if (ImGui::InputText("##RenameAsset", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	{
		nodes.state[index] = AN_State::SELECTED;
		if (buffer != nodes.name[index])
		{
			std::string old_name = nodes.name[index];
			nodes.name[index] = GetNameWithCount(buffer);
			std::string new_path = nodes.path[index];
			new_path = new_path.substr(0, new_path.find_last_of("/") + 1) + nodes.name[index];
			MoveFile(nodes.path[index].c_str(), new_path.c_str());
			nodes.path[index] = new_path.c_str();

			for (std::string child : nodes.childs[index])
			{
				int pos = FindNode(child.c_str(), nodes.name);
				if (pos != -1)
					UpdatePath(pos, nodes.path[index], nodes.name[index]);
			}

			// Update Parent's Child List
			int parent_index = FindNode(nodes.parent[index].c_str(), nodes.name);
			if (parent_index != -1)
			{
				int child_index = FindNode(old_name.c_str(), nodes.childs[parent_index]);
				if (child_index != -1)
					nodes.childs[parent_index][child_index] = nodes.name[index];
			}
		}
	}
	if (ImGui::IsItemClicked() || is_rename_flag)
	{
		is_rename_flag = false;
		ImGui::SetKeyboardFocusHere(-1);
	}
}

//void PanelAssets::ImportAsset(const PathNode& node)
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

//Resource* PanelAssets::GetSelectedResource()
//{
//	if (selected_node.path != "" && selected_node.file == true)
//	{
//		std::string metaFile = selected_node.path + (".meta");
//		uint64 resourceID = App->resources->GetIDFromMeta(metaFile.c_str());
//		return App->resources->GetResource(resourceID);
//	}
//	return nullptr;
//}