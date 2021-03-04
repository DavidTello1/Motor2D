#include "PanelResources.h"
#include "Resource.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "PanelAssets.h"

#include "mmgr/mmgr.h"

// ---------------------------------------------------------
PanelResources::PanelResources() : Panel("Resources", ICON_RESOURCES, 5)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
}

PanelResources::~PanelResources()
{
}

void PanelResources::Draw()
{
	if (App->editor->panel_assets->selected_nodes.size() == 1)
	{
		int index = App->editor->panel_assets->FindNode(App->editor->panel_assets->selected_nodes[0].c_str(), App->editor->panel_assets->nodes.name);
		if (index != -1)
		{
			const char* path = App->editor->panel_assets->nodes.path[index].c_str();
			ResourceType type = App->editor->panel_assets->nodes.type[index];

			DrawResourceData(type, App->resources->GetResourceIndex(type, path));
			DrawImportOptions(type, App->resources->GetResourceIndex(type, path));
			DrawPreview(type, App->resources->GetResourceIndex(type, path));
		}
	}
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

void PanelResources::DrawResourceData(ResourceType type, size_t index)
{
	ResourceData data;
	switch (type)
	{
	case ResourceType::FOLDER:		return;
	case ResourceType::SCENE:		break;
	case ResourceType::PREFAB:		break;
	case ResourceType::TEXTURE:		data = App->resources->textures.data; break;
	case ResourceType::MATERIAL:	break;
	case ResourceType::ANIMATION:	break;
	case ResourceType::TILEMAP:		break;
	case ResourceType::AUDIO:		break;
	case ResourceType::SCRIPT:		break;
	case ResourceType::SHADER:		break;
	default:						return;
	}

	ImGui::Text("Name: %s", App->file_system->GetFileName(data.files_assets[index].c_str()).c_str());
	ImGui::Text("UID: %l", data.ids[index]);
	ImGui::Text("Assets File: %s", data.files_assets[index].c_str());
	ImGui::Text("Library File: %s", data.files_library[index].c_str());
	ImGui::Text("Size: ");
	ImGui::Text("Times Loaded: %d", data.times_loaded[index]);
	ImGui::Separator();
}

void PanelResources::DrawImportOptions(ResourceType type, size_t index)
{

}

void PanelResources::DrawPreview(ResourceType type, size_t index)
{

}