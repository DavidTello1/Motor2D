#include "PanelAssets.h"

#include "MessageBus.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "Imgui/imgui_internal.h"
#include <string>

#include "mmgr/mmgr.h"

ImGuiTextFilter PanelAssets::searcher;

// ---------------------------------------------------------
PanelAssets::PanelAssets(bool active) : Panel(active)
{
	icon = "";/* ICON_ASSETS; */
	name = "Assets";
	flags = ImGuiWindowFlags_NoCollapse;

	position = ImVec2(0.0f, 560.0f);
	size = ImVec2(1400, 220.0f);

	is_init = false;

	// --- Load Hierarchy Nodes
	AssetTree* root = new AssetTree("Assets", nullptr, 0, true, false);
	hierarchy_nodes.push_back(root);
	SetCurrentNode(root);

	std::vector<AssetTree*> tmp = LoadHierarchyNodes(ASSETS_FOLDER, root, 1);
	hierarchy_nodes.insert(hierarchy_nodes.end(), tmp.begin(), tmp.end());
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

		// Childs
		if (!is_hierarchy_hidden)
			DrawChildHierarchy();
		ImGui::SameLine();
		DrawChildExplorer();

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
		ImGui::DockBuilderDockWindow("ChildHierarchy", left_space);
		ImGui::DockBuilderGetNode(left_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		ImGui::DockBuilderDockWindow("ChildExplorer", right_space);
		ImGui::DockBuilderGetNode(right_space)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
	}
}

// ---------------------------------------------------------
// --- HIERARCHY CHILD ---
std::vector<AssetTree*> PanelAssets::LoadHierarchyNodes(const char* path, AssetTree* parent, int parents)
{
	if (!App->filesystem->Exists(path))
	{
		LOG("Error retrieving files: invalid path - '%s'", path, 'e');
		return std::vector<AssetTree*>();
	}

	std::vector<std::string> directories, files;
	App->filesystem->GetFolderContent(path, files, directories);

	std::vector<AssetTree*> nodes;
	for (size_t i = 0; i < directories.size(); ++i)
	{
		std::string child_path = path + std::string("/") + directories[i];
		AssetTree* node = new AssetTree(directories[i], parent, parents);
		nodes.push_back(node);

		std::vector<AssetTree*> children = LoadHierarchyNodes(child_path.c_str(), node, node->num_parents + 1); // (Recursive) Get all nodes inside child directory

		if (!children.empty())
		{
			nodes.insert(nodes.end(), children.begin(), children.end());
			node->is_leaf = false;
		}
	}

	return nodes;
}

void PanelAssets::DrawChildHierarchy()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("ChildHierarchy", NULL, child_flags))
	{
		// --- Menu Bar
		DrawHierarchyMenuBar();

		// --- Nodes
		for (size_t i = 0; i < hierarchy_nodes.size(); ++i)
		{
			DrawHierarchyNode(hierarchy_nodes[i], i);
		}
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void PanelAssets::DrawHierarchyMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		// --- Filters
		if (ImGui::Button("E", ImVec2(25.0f, 0.0f))) //*** ICON FILTER
			ImGui::OpenPopup("Filters");

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
		if (ImGui::BeginPopup("Filters"))
		{
			//ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true); // Keep popup open

			if (ImGui::MenuItem("All", NULL, filter == -1))	filter = -1;
			ImGui::Separator();
			if (ImGui::MenuItem("Folders",	  NULL,	filter == (int)ResourceType::FOLDER))		filter = (int)ResourceType::FOLDER;
			if (ImGui::MenuItem("Scenes",	  NULL,	filter == (int)ResourceType::SCENE))		filter = (int)ResourceType::SCENE;
			if (ImGui::MenuItem("Prefabs",	  NULL,	filter == (int)ResourceType::PREFAB))		filter = (int)ResourceType::PREFAB;
			if (ImGui::MenuItem("Textures",	  NULL,	filter == (int)ResourceType::TEXTURE))		filter = (int)ResourceType::TEXTURE;
			if (ImGui::MenuItem("Materials",  NULL,	filter == (int)ResourceType::MATERIAL))		filter = (int)ResourceType::MATERIAL;
			if (ImGui::MenuItem("Animations", NULL, filter == (int)ResourceType::ANIMATION))	filter = (int)ResourceType::ANIMATION;
			if (ImGui::MenuItem("Tilemaps",   NULL,	filter == (int)ResourceType::TILEMAP))		filter = (int)ResourceType::TILEMAP;
			if (ImGui::MenuItem("Audios",	  NULL,	filter == (int)ResourceType::AUDIO))		filter = (int)ResourceType::AUDIO;
			if (ImGui::MenuItem("Scripts",	  NULL,	filter == (int)ResourceType::SCRIPT))		filter = (int)ResourceType::SCRIPT;
			if (ImGui::MenuItem("Shaders",	  NULL,	filter == (int)ResourceType::SHADER))		filter = (int)ResourceType::SHADER;
			//*** ALL RESOURCE TYPES

			//ImGui::PopItemFlag();
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(2);

		// --- Search
		float search_size = ImGui::GetContentRegionAvailWidth() - 4.0f;
		static float posX = ImGui::GetCursorPosX();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 0.25f, 1.00f));
		searcher.Draw("", search_size);
		ImGui::PopStyleColor();

		if (!ImGui::IsItemActive() && !searcher.IsActive())
		{
			ImGui::SetCursorPosX(posX + 4.0f);
			ImGui::TextDisabled("Search");
		}

		ImGui::EndMenuBar();
	}
}

void PanelAssets::DrawHierarchyNode(AssetTree* node, int id)
{
	// --- Only Draw if all parents are open
	if (node->parent != nullptr)
	{
		AssetTree* parent = node->parent;
		for (int i = 0; i < node->num_parents; ++i)
		{
			if (!parent->is_open)
				return;

			parent = parent->parent;
		}
	}

	ImGui::PushID(id); // avoid problems with duplicated names

	// --- Open Indent
	const float indent = 15.0f * node->num_parents;
	if (indent > 0.0f)
		ImGui::Indent(indent);

	// --- Flags
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
	if (node == current_node)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (node->is_leaf)
		flags |= ImGuiTreeNodeFlags_Leaf;

	// --- Tree Node
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f));

	node->is_open = ImGui::TreeNodeEx(node->name.c_str(), flags);
	if (node->is_open)
		ImGui::TreePop();

	ImGui::PopStyleVar();

	// --- Selection
	if (ImGui::IsItemClicked())
	{
		SetCurrentNode(node);
	}

	// --- Drag & Drop
	//if (ImGui::BeginDragDropTarget())
	//{
	//	ImGui::EndDragDropTarget();
	//}

	// --- Close Indent
	if (indent > 0.0f)
		ImGui::Unindent(indent);

	ImGui::PopID();
}

// ---------------------------------------------------------
// --- EXPLORER CHILD  ---
void PanelAssets::UpdateNodePath()
{
	std::vector<AssetTree*> nodes;

	AssetTree* parent = current_node;
	while (parent != nullptr)
	{
		nodes.push_back(parent);
		parent = parent->parent;
	}

	std::swap(menubar_path, nodes);
}

void PanelAssets::DrawChildExplorer()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGuiWindowFlags child_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("ChildExplorer", NULL, child_flags))
	{
		// --- Menu Bar
		DrawExplorerMenuBar();

		// --- Nodes
		for (size_t i = 0; i < explorer_nodes.size(); ++i)
		{
			DrawExplorerNode(explorer_nodes[i], i);
		}

		// --- Right Click Menu
		DrawRightClick();
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void PanelAssets::DrawExplorerMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		// --- Show/Hide Hierarchy Child
		static ImGuiDir dir = ImGuiDir_Left;
		if (ImGui::ArrowButtonEx("Hide_Hierarchy", dir, ImVec2(20.0f, 20.0f)))
		{
			is_hierarchy_hidden = !is_hierarchy_hidden;
			if (is_hierarchy_hidden)
				dir = ImGuiDir_Right;
			else
				dir = ImGuiDir_Left;
		}
		ImGui::Separator();

		// --- Path
		int id = 0;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);
		for (auto i = menubar_path.rbegin(); i < menubar_path.rend(); ++i) // Reverse order
		{
			ImGui::PushID(id);

			if (ImGui::Button((*i)->name.c_str()))
			{
				SetCurrentNode(*i);
				ImGui::PopID();
				break;
			}

			if (i < menubar_path.rend() - 1)
				ImGui::Text("/");

			ImGui::PopID();
			id++;
		}

		ImGui::EndMenuBar();
	}
}

void PanelAssets::DrawRightClick()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if (ImGui::BeginPopupContextWindow("RightClick_Menu"))
	{
		if (ImGui::BeginMenu("Create")) //create
		{
			if (ImGui::MenuItem("Folder")) //folder
			{
			}
			if (ImGui::MenuItem("Script")) //script
			{
			}
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

		if (ImGui::MenuItem("Select All", NULL, false/*, !nodes.childs[current_folder].empty()*/)) //select all
		{
			//SelectAll();
		}

		if (ImGui::MenuItem("Rename", NULL, false, selected_nodes.size() == 1)) //rename
		{
		}

		if (ImGui::MenuItem("Delete", "Supr", false, !selected_nodes.empty())) //delete
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Import")) //import
		{
			//ImportAsset();
		}

		if (ImGui::MenuItem("Show in Explorer", NULL, nullptr, selected_nodes.size() == 1)) //show in explorer
		{
		}
		if (ImGui::MenuItem("Show References", NULL, nullptr, selected_nodes.size() == 1)) //references
		{
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar(2);
}

void PanelAssets::DrawExplorerNode(AssetNode* node, int id)
{

}