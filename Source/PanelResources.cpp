#include "PanelResources.h"
#include "Resource.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "PanelAssets.h"

#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

// ---------------------------------------------------------
PanelResources::PanelResources() : Panel("Resources", ICON_RESOURCES, 5)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_NoCollapse;

	is_init = false;
}

PanelResources::~PanelResources()
{
}

void PanelResources::Draw()
{
	// DockSpace
	DockSpace();

	// Resource Variables
	int index = -1;
	ResourceType type = ResourceType::UNKNOWN;
	if (App->editor->panel_assets->selected_nodes.size() == 1)
	{
		int pos = App->editor->panel_assets->FindNode(App->editor->panel_assets->selected_nodes[0].c_str(), App->editor->panel_assets->nodes.name);
		if (pos != -1)
		{
			const char* path = App->editor->panel_assets->nodes.path[pos].c_str();
			type = App->editor->panel_assets->nodes.type[pos];
			index = App->resources->GetResourceIndex(type, path);
		}
	}

	// Importing Options
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::Begin("Resources_Options", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	ImGui::PopStyleVar();

	if (ImGui::BeginMenuBar()) // menu bar
	{
		ImGui::Text("Importing Options");
		ImGui::EndMenuBar();
	}
	if (index != -1) // draw by type
	{
		switch (type)
		{
		case ResourceType::FOLDER:		break;
		case ResourceType::SCENE:		break;
		case ResourceType::PREFAB:		break;
		case ResourceType::TEXTURE:		/*DrawOptionsTexture(index);*/ break;
		case ResourceType::MATERIAL:	break;
		case ResourceType::ANIMATION:	break;
		case ResourceType::TILEMAP:		break;
		case ResourceType::AUDIO:		break;
		case ResourceType::SCRIPT:		break;
		case ResourceType::SHADER:		break;
		default:						break;
		}
	}
	ImGui::End();

	// Preview
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::Begin("Resources_Preview", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	ImGui::PopStyleVar();

	if (ImGui::BeginMenuBar()) //menu bar
	{
		ImGui::Text("Preview");
		ImGui::EndMenuBar();
	}
	if (index != -1) // draw by type
	{
		switch (type)
		{
		case ResourceType::FOLDER:		break;
		case ResourceType::SCENE:		break;
		case ResourceType::PREFAB:		break;
		case ResourceType::TEXTURE:		/*DrawPreviewTexture(index);*/ break;
		case ResourceType::MATERIAL:	break;
		case ResourceType::ANIMATION:	break;
		case ResourceType::TILEMAP:		break;
		case ResourceType::AUDIO:		break;
		case ResourceType::SCRIPT:		break;
		case ResourceType::SHADER:		break;
		default:						break;
		}
	}
	ImGui::End();
}

void PanelResources::Shortcuts()
{
}

void PanelResources::Save(Config* config) const
{
}

void PanelResources::Load(Config* config)
{
}

//void PanelResources::DrawResourceData(ResourceType type, size_t index)
//{
//	ResourceData data;
//	switch (type)
//	{
//	case ResourceType::FOLDER:		return;
//	case ResourceType::SCENE:		break;
//	case ResourceType::PREFAB:		break;
//	case ResourceType::TEXTURE:		data = App->resources->textures.data; break;
//	case ResourceType::MATERIAL:	break;
//	case ResourceType::ANIMATION:	break;
//	case ResourceType::TILEMAP:		break;
//	case ResourceType::AUDIO:		break;
//	case ResourceType::SCRIPT:		break;
//	case ResourceType::SHADER:		break;
//	default:						return;
//	}
//
//	ImGui::Text("Name: %s", App->file_system->GetFileName(data.files_assets[index].c_str()).c_str());
//	ImGui::Text("UID: %l", data.ids[index]);
//	ImGui::Text("Assets File: %s", data.files_assets[index].c_str());
//	ImGui::Text("Library File: %s", data.files_library[index].c_str());
//	ImGui::Text("Size: ");
//	ImGui::Text("Times Loaded: %d", data.times_loaded[index]);
//	ImGui::Separator();
//}

void PanelResources::DockSpace()
{
	ImGuiID dock_space = ImGui::GetID("Resources");
	if (is_init == false)
	{
		is_init = true;

		ImGui::DockSpace(dock_space);
		ImGuiDockNode* dock_node = ImGui::DockBuilderGetNode(dock_space);
		if (!dock_node->IsSplitNode())
		{
			ImGuiID top_space, bottom_space;
			ImGui::DockBuilderSplitNode(dock_space, ImGuiDir_Up, 0.8f, &top_space, &bottom_space);
			ImGui::DockBuilderDockWindow("Resources_Options", top_space);
			ImGui::DockBuilderGetNode(top_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
			ImGui::DockBuilderDockWindow("Resources_Preview", bottom_space);
			ImGui::DockBuilderGetNode(bottom_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}
	}
	else
		ImGui::DockSpace(dock_space, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit);
}