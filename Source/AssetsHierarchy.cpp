#include "AssetsHierarchy.h"

#include "Application.h"
#include "MessageBus.h"

#include "AssetNode.h"
#include "Resource.h" //***RESOURCE_TYPE

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

ImGuiTextFilter AssetsHierarchy::searcher;

void AssetsHierarchy::Draw(std::vector<AssetNode*> hierarchy_nodes)
{
	if (this->is_active == false)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("ChildHierarchy", NULL, child_flags))
	{
		// --- Menu Bar
		MenuBar();

		// --- Nodes
		for (size_t i = 0; i < hierarchy_nodes.size(); ++i)
		{
			AssetNode* node = hierarchy_nodes[i];
			if (node->is_hidden)
				continue;

			DrawNode(node, i); //***
			HandleSelection(node);
		}
	}
	ImGui::End();

	ImGui::PopStyleVar();

}

void AssetsHierarchy::MenuBar()
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
			if (ImGui::MenuItem("Folders", NULL, filter == (int)ResourceType::FOLDER))		filter = (int)ResourceType::FOLDER;
			if (ImGui::MenuItem("Scenes", NULL, filter == (int)ResourceType::SCENE))		filter = (int)ResourceType::SCENE;
			if (ImGui::MenuItem("Prefabs", NULL, filter == (int)ResourceType::PREFAB))		filter = (int)ResourceType::PREFAB;
			if (ImGui::MenuItem("Textures", NULL, filter == (int)ResourceType::TEXTURE))		filter = (int)ResourceType::TEXTURE;
			if (ImGui::MenuItem("Materials", NULL, filter == (int)ResourceType::MATERIAL))		filter = (int)ResourceType::MATERIAL;
			if (ImGui::MenuItem("Animations", NULL, filter == (int)ResourceType::ANIMATION))	filter = (int)ResourceType::ANIMATION;
			if (ImGui::MenuItem("Tilemaps", NULL, filter == (int)ResourceType::TILEMAP))		filter = (int)ResourceType::TILEMAP;
			if (ImGui::MenuItem("Audios", NULL, filter == (int)ResourceType::AUDIO))		filter = (int)ResourceType::AUDIO;
			if (ImGui::MenuItem("Scripts", NULL, filter == (int)ResourceType::SCRIPT))		filter = (int)ResourceType::SCRIPT;
			if (ImGui::MenuItem("Shaders", NULL, filter == (int)ResourceType::SHADER))		filter = (int)ResourceType::SHADER;
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

void AssetsHierarchy::DrawNode(AssetNode* node, int id) //***
{
	ImGui::PushID(id); //***
	//ImGui::PushID(node->id); // avoid problems with duplicated names

	// --- Open Indent
	const float indent = 15.0f * node->num_parents;
	if (indent > 0.0f)
		ImGui::Indent(indent);

	// --- Flags
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
	if (node->is_selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (node->childs.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	// --- Tree Node
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f));

	if (ImGui::TreeNodeEx(node->name.c_str(), flags))
		ImGui::TreePop();

	ImGui::PopStyleVar();

	// --- Close Indent
	if (indent > 0.0f)
		ImGui::Unindent(indent);

	ImGui::PopID();
}

void AssetsHierarchy::HandleSelection(AssetNode* node)
{
	// --- Clicked Arrow (Open/Close)
	if (ImGui::IsItemToggledOpen())
	{
		node->is_open = !node->is_open;
		UpdateNodesVisibility(node, !node->is_open);
		App->message->Publish(new OnChangedPanelAssetsCurrentNode(node));
	}

	// --- Selection
	else if (ImGui::IsItemClicked())
	{
		App->message->Publish(new OnChangedPanelAssetsCurrentNode(node));
	}

	// --- Drag & Drop
	//if (ImGui::BeginDragDropTarget())
	//{
	//	ImGui::EndDragDropTarget();
	//}
}

void AssetsHierarchy::UpdateNodesVisibility(AssetNode* node, bool hidden)
{
	for (size_t i = 0; i < node->childs.size(); ++i)
	{
		AssetNode* child = node->childs[i];
		child->is_hidden = hidden;

		if (child->is_open)
			UpdateNodesVisibility(child, hidden);
	}
}
