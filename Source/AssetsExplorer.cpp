#include "AssetsExplorer.h"

#include "Application.h"
#include "MessageBus.h"

#include "AssetNode.h"

#include "Resource.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

void AssetsExplorer::Draw(AssetNode* current_node, bool is_forward, bool is_backward)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGuiWindowFlags child_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("ChildExplorer", NULL, child_flags))
	{
		// --- Click on Background
		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))
		{
			UnSelectAll(current_node);
		}

		// --- Menu Bar
		MenuBar(current_node, is_forward, is_backward);

		// --- Nodes
		for (size_t i = 0; i < current_node->childs.size(); ++i)
		{
			DrawNode(current_node->childs[i], i); //***
			HandleSelection(current_node->childs[i]);

			ImGui::SameLine(0.0f, 5.0f);
		}

		// --- Right Click Menu
		DrawRightClick();
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void AssetsExplorer::MenuBar(AssetNode* current_node, bool is_forward, bool is_backward)
{
	if (ImGui::BeginMenuBar())
	{
		// --- Show/Hide Hierarchy Child
		static ImGuiDir dir = ImGuiDir_Left;
		if (ImGui::ArrowButtonEx("Hide_Hierarchy", dir, ImVec2(25.0f, 20.0f)))
		{
			is_hierarchy_hidden = !is_hierarchy_hidden;
			dir = (is_hierarchy_hidden) ? ImGuiDir_Right : ImGuiDir_Left;

			App->message->Publish(new OnHidePanelAssetsHierarchy(is_hierarchy_hidden));
		}
		ImGui::SameLine(0.0f, 1.0f);
		ImGui::Separator();
		ImGui::SameLine(0.0f, 1.0f);

		// --- Back Button
		ImGuiButtonFlags backward_flags = ImGuiButtonFlags_None;
		if (!is_backward)
		{
			backward_flags = ImGuiButtonFlags_Disabled;
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
		}
		if (ImGui::ArrowButtonEx("Back", ImGuiDir_Left, ImVec2(20.0f, 20.0f), backward_flags))
		{
			App->message->Publish(new OnPanelAssetsHistoryBackward());
		}
		if (!is_backward)
			ImGui::PopStyleColor();

		// --- Forward Button
		ImGuiButtonFlags forward_flags = ImGuiButtonFlags_None;
		if (!is_forward)
		{
			forward_flags = ImGuiButtonFlags_Disabled;
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
		}
		if (ImGui::ArrowButtonEx("Forward", ImGuiDir_Right, ImVec2(20.0f, 20.0f), forward_flags))
		{
			App->message->Publish(new OnPanelAssetsHistoryForward());
		}
		if (!is_forward)
			ImGui::PopStyleColor();

		ImGui::Separator();
		ImGui::SameLine(0.0f, 5.0f);

		// --- Path
		int id = 0;
		for (auto i = menubar_path.rbegin(); i < menubar_path.rend(); ++i) // Reverse order
		{
			ImGui::PushID(id);

			if (ImGui::Button((*i)->name.c_str()))
			{
				App->message->Publish(new OnChangedPanelAssetsCurrentNode(*i));
				ImGui::PopID();
				break;
			}

			if (i < menubar_path.rend() - 1 && ImGui::Button(">"))
			{
				ImGui::OpenPopup("Child_Folders");
			}

			bool current_node_changed = false;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
			if (ImGui::BeginPopup("Child_Folders"))
			{
				for (AssetNode* child : (*i)->childs)
				{
					auto j = i + 1;
					if (ImGui::MenuItem(child->name.c_str(), NULL, child == *j))
					{
						if (child != current_node)
							App->message->Publish(new OnChangedPanelAssetsCurrentNode(child));

						current_node_changed = true;
						break;
					}
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleVar(2);

			ImGui::PopID();

			if (current_node_changed)
				break;

			id++;
		}

		ImGui::EndMenuBar();
	}
}

void AssetsExplorer::DrawNode(AssetNode* node, int id) //***
{
	static const ImVec2 bg_size = ImVec2(90, 90);
	static const ImVec2 padding = ImVec2(5, 5);
	static const ImVec2 image_size = ImVec2(64, 64);
	static const int max_textSize = bg_size.x - (padding.x * 2.0f);

	// --- Begin Group
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

	ImGui::PushID(id);
	//ImGui::PushID(node->id);
	ImGui::BeginGroup();

	// Background
	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::Selectable("###background", node->is_selected, NULL, bg_size);

	// Image
	ImVec2 imagePos = ImVec2(pos.x + (bg_size.x - image_size.x) / 2.0f, pos.y + padding.y);
	ImGui::SetCursorPos(imagePos);
	ImGui::Image(0, image_size);

	// Text
	std::string label = node->name;
	ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

	if (textSize.x > max_textSize)
	{
		int dots_size = ImGui::CalcTextSize("...").x;
		size_t endchar = (max_textSize - dots_size) / 7.0f;

		label = label.substr(0, endchar) + "...";
		textSize.x = (float)max_textSize;
	}
	ImVec2 textPos = ImVec2(pos.x + (bg_size.x - textSize.x) / 2.0f, pos.y + bg_size.y - padding.y - textSize.y);
	ImGui::SetCursorPos(textPos);
	ImGui::Text(label.c_str());

	// --- End Group
	ImGui::EndGroup();
	ImGui::PopID();
	ImGui::PopStyleVar();
}

void AssetsExplorer::HandleSelection(AssetNode* node)
{
	// --- Handle Selection
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		if (ImGui::IsMouseDoubleClicked(0)) // Double L-Click
		{
			UnSelectAll(node->parent);
			node->state = NodeState::SELECTED;
			OpenNode(node);
		}
		else if (ImGui::IsItemClicked(1)) // Right Click
		{
			if (selected_nodes.size() <= 1)
				UnSelectAll(node->parent);

			node->state = NodeState::SELECTED;
		}
		else if (ImGui::IsItemClicked(0)) // Left Click
		{
			switch (node->state)
			{
			case NodeState::DEFAULT:
				node->state = NodeState::SELECTED;
				break;

			case NodeState::SELECTED:
				node->state = NodeState::RENAME;
				break;

			case NodeState::CUT:
				break;

			case NodeState::RENAME:
				node->state = NodeState::SELECTED;
				break;
			}
		}
	}
}

void AssetsExplorer::OpenNode(AssetNode* node)
{
	switch ((ResourceType)node->type)
	{
	case ResourceType::FOLDER:		App->message->Publish(new OnChangedPanelAssetsCurrentNode(node));
	case ResourceType::TEXTURE:		break;
	case ResourceType::AUDIO:		break;
	case ResourceType::TEXTFILE:	break;
	case ResourceType::SCRIPT:		break;
	case ResourceType::SHADER:		break;
	case ResourceType::SCENE:		break;
	case ResourceType::PREFAB:		break;
	case ResourceType::MATERIAL:	break;
	case ResourceType::ANIMATION:	break;
	case ResourceType::TILEMAP:		break;

	case ResourceType::COUNT:
		break;
	default:
		break;
	}
}

void AssetsExplorer::SelectAll(AssetNode* current_node)
{
	for (size_t i = 0; i < current_node->childs.size(); ++i)
	{
		current_node->childs[i]->is_selected = true;
	}
}

void AssetsExplorer::UnSelectAll(AssetNode* current_node)
{
	for (size_t i = 0; i < current_node->childs.size(); ++i)
	{
		current_node->childs[i]->is_selected = false;
	}
}

void AssetsExplorer::DrawRightClick()
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

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, false/*!selected_nodes.empty()*/)) //cut
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, false/*!selected_nodes.empty()*/)) //copy
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false/*is_copy || is_cut*/)) //paste
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Select All", NULL, false, false/*!current_node->childs.empty()*/)) //select all
		{
			//SelectAll(current_node);
		}

		if (ImGui::MenuItem("Rename", NULL, false, false/*selected_nodes.size() == 1*/)) //rename
		{
		}

		if (ImGui::MenuItem("Delete", "Supr", false, false/*!selected_nodes.empty()*/)) //delete
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Import")) //import
		{
			//ImportAsset();
		}

		if (ImGui::MenuItem("Show in Explorer", NULL, nullptr, false/*selected_nodes.size() == 1*/)) //show in explorer
		{
		}
		if (ImGui::MenuItem("Show References", NULL, nullptr, false/*selected_nodes.size() == 1*/)) //references
		{
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar(2);
}

void AssetsExplorer::UpdateMenuBarPath(AssetNode* current_node)
{
	std::vector<AssetNode*> nodes;

	AssetNode* parent = current_node;

	for (uint i = 0; i <= current_node->num_parents; ++i)
	{
		nodes.push_back(parent);
		parent = parent->parent;
	}

	std::swap(menubar_path, nodes);
}
