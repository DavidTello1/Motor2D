#include "Utils.h"

#include "Imgui/imgui.h"
#include <windows.h>

void Utils::Dockspace(float paddingTop)
{
	// --- Viewport ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + paddingTop));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - paddingTop));
	ImGui::SetNextWindowViewport(viewport->ID);

	// --- Window Flags ---
	ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// --- Style Vars ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	// --- Dockspace ---
	ImGui::Begin("DockSpace", NULL, window_flags);
	ImGui::DockSpace(ImGui::GetID("MyDockspace"));
	ImGui::End();
	ImGui::PopStyleVar(3);
}

void Utils::Link(const char* url)
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL); // Open Link
}

void Utils:: Link(const char* text, const char* url)
{
	const ImVec2 pos = ImGui::GetCursorPos();
	ImVec4 color = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);

	// --- Hover ---
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	}

	// --- Text ---
	ImGui::TextColored(color, text);
	ImGui::SameLine();

	// --- Invisible Button ---
	ImGui::SetCursorPos(pos);
	if (ImGui::InvisibleButton(text, ImGui::CalcTextSize(text)))
	{
		Link(url);
	}
}
