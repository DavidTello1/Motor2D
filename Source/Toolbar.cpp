#include "Toolbar.h"

#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

// ---------------------------------------------------------
Toolbar::Toolbar() : Panel("Toolbar", ICON_TEXTURE, 0)
{
	flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
}

Toolbar::~Toolbar()
{
}

void Toolbar::Draw()
{
	pos_x = ImGui::GetWindowPos().x;
	pos_y = ImGui::GetWindowPos().y;

	ImVec2 pos = ImGui::GetCursorPos();
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - BUTTON_SIZE * 2.5f + 2);
	// Play / Stop
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();

	// Pause
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}
	ImGui::SameLine();

	// Step
	if (ImGui::ImageButton(0, ImVec2(BUTTON_SIZE, BUTTON_SIZE)))
	{
	}

	// Tabs
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 450, pos.y + 18));
	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_FittingPolicyMask_))
	{
		if (ImGui::BeginTabItem("Scene", NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animation"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Particle"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Audio"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Lighting"))
		{
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shaders"))
		{
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}