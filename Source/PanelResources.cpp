#include "PanelResources.h"
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
		case ResourceType::PREFAB:		DrawOptionsPrefab(index);	 break;
		case ResourceType::TEXTURE:		DrawOptionsTexture(index);	 break;
		case ResourceType::MATERIAL:	DrawOptionsMaterial(index);  break;
		case ResourceType::ANIMATION:	DrawOptionsAnimation(index); break;
		case ResourceType::TILEMAP:		DrawOptionsTilemap(index);	 break;
		case ResourceType::AUDIO:		DrawOptionsAudio(index);	 break;
		case ResourceType::SCRIPT:		DrawOptionsScript(index);	 break;
		case ResourceType::SHADER:		DrawOptionsShader(index);	 break;
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
		case ResourceType::PREFAB:		DrawPreviewPrefab(index);	 break;
		case ResourceType::TEXTURE:		DrawPreviewTexture(index);	 break;
		case ResourceType::MATERIAL:	DrawPreviewMaterial(index);	 break;
		case ResourceType::ANIMATION:	DrawPreviewAnimation(index); break;
		case ResourceType::TILEMAP:		DrawPreviewTilemap(index);	 break;
		case ResourceType::AUDIO:		DrawPreviewAudio(index);	 break;
		case ResourceType::SCRIPT:		DrawPreviewScript(index);	 break;
		case ResourceType::SHADER:		DrawPreviewShader(index);	 break;
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

// --- IMPORTING OPTIONS ---
void PanelResources::DrawOptionsPrefab(size_t index)
{
}

void PanelResources::DrawOptionsTexture(size_t index)
{
}

void PanelResources::DrawOptionsMaterial(size_t index)
{
}

void PanelResources::DrawOptionsAnimation(size_t index)
{
}

void PanelResources::DrawOptionsTilemap(size_t index)
{
}

void PanelResources::DrawOptionsAudio(size_t index)
{
}

void PanelResources::DrawOptionsScript(size_t index)
{
}

void PanelResources::DrawOptionsShader(size_t index)
{
}

// --- PREVIEW ---
void PanelResources::DrawPreviewPrefab(size_t index) 
{
}

void PanelResources::DrawPreviewTexture(size_t index) 
{
}

void PanelResources::DrawPreviewMaterial(size_t index) 
{
}

void PanelResources::DrawPreviewAnimation(size_t index) 
{
}

void PanelResources::DrawPreviewTilemap(size_t index) 
{
}

void PanelResources::DrawPreviewAudio(size_t index) 
{
}

void PanelResources::DrawPreviewScript(size_t index) 
{
}

void PanelResources::DrawPreviewShader(size_t index) 
{
}
