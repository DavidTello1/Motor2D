#include "PanelAssets.h"

#include "Application.h"
#include "ModuleResources.h"

#include <string>

#include "mmgr/mmgr.h"

PanelAssets::PanelAssets(bool active) : Panel(active)
{
	icon = "";/* ICON_ASSETS; */
	name = "Assets";
	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

	position = ImVec2(0.0f, 560.0f);
	size = ImVec2(1400, 220.0f);

	// --- Load Hierarchy Nodes
	LoadHierarchyNodes();
}

PanelAssets::~PanelAssets()
{
	// --- Clear Hierarchy Nodes
	for (size_t i = 0; i < hierarchy_nodes.size(); ++i)
	{
		RELEASE(hierarchy_nodes[i]);
	}

	// --- Clear Explorer Nodes

}

void PanelAssets::Draw()
{
	// --- Begin Panel ---
	static const std::string name = icon + std::string(" ") + this->name;
	if (ImGui::Begin(name.c_str(), &active, flags))
	{
		if (hierarchy_nodes.empty())
			LoadHierarchyNodes();

		// --- Hierarchy
		DrawChildHierarchy();

		// --- Explorer
		DrawChildExplorer();

		// --- Shortcuts
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			//App->editor->focused_panel = this; //*** MESSAGE
			//Shortcuts();
		}
	}
	ImGui::End();
}

void PanelAssets::Shortcuts()
{
}

void PanelAssets::Save(Config* config) const
{
}

void PanelAssets::Load(Config* config)
{
}

// ---------------------------------------------------------
void PanelAssets::DrawChildHierarchy()
{
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("Hierarchy", ImVec2(windowSize.x * 0.25f, windowSize.y), true);

	for (size_t i = 0; i < hierarchy_nodes.size(); ++i)
	{
		DrawHierarchyNode(hierarchy_nodes[i]);
	}

	ImGui::EndChild();
}

void PanelAssets::LoadHierarchyNodes()
{
	std::vector<ResourceHandle> handles;
	App->resources->GetResourceHandles(handles);
	for (ResourceHandle handle : handles)
	{
		if (handle.type != (int)ResourceType::FOLDER)
			continue;

		Resource* resource = App->resources->GetResource(handle.id);
		//if (resource == nullptr)
		//	continue;

		AssetNode* node = new AssetNode(resource);
		hierarchy_nodes.push_back(node);
	}
}

void PanelAssets::DrawHierarchyNode(AssetNode* node)
{
	if (ImGui::TreeNode(node->resource->name))
	{
		ImGui::TreePop();
	}
}

// ---------------------------------------------------------
void PanelAssets::DrawChildExplorer()
{
}
