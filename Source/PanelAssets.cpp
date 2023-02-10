#include "PanelAssets.h"

#include "MessageBus.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "AssetsExplorer.h"
#include "AssetsHierarchy.h"

#include "Resource.h"

#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

// ---------------------------------------------------------
PanelAssets::PanelAssets(bool active) : Panel(active)
{
	icon = "";/* ICON_ASSETS; */
	name = "Assets";
	flags = ImGuiWindowFlags_NoCollapse;

	position = ImVec2(0.0f, 560.0f);
	size = ImVec2(1400, 220.0f);

	is_init = false;

	// --- Create Hierarchy & Explorer
	hierarchy = new AssetsHierarchy("ChildHierarchy");
	explorer = new AssetsExplorer("ChildExplorer");

	// --- Create Root
	root = new AssetNode(0, "Assets");
	root->is_hidden = false;
	hierarchy_nodes.push_back(root);
	SetCurrentNode(root);

	// --- Load Hierarchy Nodes
	LoadAssetsNodes(ASSETS_FOLDER, root, 1);

	// --- Messages ---
	App->message->Subscribe(this, &PanelAssets::OnCurrentNodeChanged);
}

PanelAssets::~PanelAssets()
{
	// --- Release Hierarchy & Explorer
	RELEASE(hierarchy);
	RELEASE(explorer);

	// --- Release Nodes
	RELEASE(root);
	for (size_t i = 0; i < nodes_list.size(); ++i)
	{
		RELEASE(nodes_list[i]);
	}
	nodes_list.clear();
	hierarchy_nodes.clear();
	selected_nodes.clear();
}

void PanelAssets::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	// --- Begin Panel ---
	static const std::string name = icon + std::string(" ") + this->name;
	if (ImGui::Begin(name.c_str(), &active, flags))
	{
		// DockSpace
		ImGuiID dock_space = ImGui::GetID(this->name);
		if (is_init)
			ImGui::DockSpace(dock_space, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoSplit);
		else
			InitDockspace();

		// Child Hierarchy
		hierarchy->Draw(hierarchy_nodes);

		ImGui::SameLine();

		// Child Explorer
		explorer->Draw(current_node);

		// --- Shortcuts
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			//App->editor->focused_panel = this; //*** MESSAGE
			//Shortcuts();
		}
	}
	ImGui::End();

	ImGui::PopStyleVar();
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
void PanelAssets::InitDockspace()
{
	ImGuiID dock_space = ImGui::GetID(this->name);
	ImGui::DockSpace(dock_space);
	ImGuiDockNode* dock_node = ImGui::DockBuilderGetNode(dock_space);
	if (!dock_node->IsSplitNode())
	{
		ImGuiID left_space, right_space;
		ImGui::DockBuilderSplitNode(dock_space, ImGuiDir_Left, 0.18f, &left_space, &right_space);
		ImGui::DockBuilderDockWindow(hierarchy->name, left_space);
		ImGui::DockBuilderGetNode(left_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		ImGui::DockBuilderDockWindow(explorer->name, right_space);
		ImGui::DockBuilderGetNode(right_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
	}
}

// ---------------------------------------------------------
void PanelAssets::LoadAssetsNodes(const char* path, AssetNode* parent, int parents)
{
	if (!App->filesystem->Exists(path))
	{
		LOG("Error retrieving files: invalid path - '%s'", path, 'e');
		return;
	}

	std::vector<std::string> directories, files;
	App->filesystem->GetFolderContent(path, files, directories);

	for (size_t i = 0; i < directories.size(); ++i)
	{
		std::string child_path = path + std::string("/") + directories[i];

		UID id = 0; //*** App->resources->GetResourceID(child_path);
		int type = (int)ResourceType::FOLDER;
		int preview_index = -1; //*** GetPreviewIndex(type, id);

		AssetNode* node = new AssetNode(id, directories[i], parent, type, parents, preview_index);
		nodes_list.push_back(node);

		parent->childs.push_back(node); // Add to parent's child list
		hierarchy_nodes.push_back(node); // Add to hierarchy_nodes list

		// (Recursive) Get all nodes inside child directory
		LoadAssetsNodes(child_path.c_str(), node, node->num_parents + 1);
	}

	for (size_t i = 0; i < files.size(); ++i)
	{
		std::string child_path = path + std::string("/") + files[i];

		UID id = 0; //*** App->resources->GetResourceID(child_path);
		int type = -1; //*** App->resources->GetResourceType(files[i]);
		int preview_index = -1; //*** GetPreviewIndex(type, id);

		AssetNode* node = new AssetNode(id, files[i], parent, type, parents, preview_index);
		nodes_list.push_back(node);

		parent->childs.push_back(node); // Add to parent's child list
	}
}

void PanelAssets::SetCurrentNode(AssetNode* node)
{
	current_node = node;

	if (explorer != nullptr)
		explorer->UpdateMenuBarPath(node);
}

void PanelAssets::OnCurrentNodeChanged(OnChangedPanelAssetsCurrentNode* m)
{
	SetCurrentNode(m->node);
}
