#include "PanelConfiguration.h"

#include "Application.h"
#include "MessageBus.h"

#include <string>
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

PanelConfiguration::PanelConfiguration(bool active) : Panel(active), fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{
	icon = ""; /* ICON_CONFIGURATION; */
	name = "Configuration";
	flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

	position = ImVec2(360.0f, 100.0f);
	size = ImVec2(650.0, 500.0f);

	// --- Messages ---
	App->message->Subscribe(this, &PanelConfiguration::OpenPanel);
}

PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	// --- Set Position & Size ---
	ImGui::SetNextWindowPos(position, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);

	// --- Begin Panel ---
	static const std::string name = icon + std::string(" ") + this->name;
	ImGui::Begin(name.c_str(), NULL, flags);

	// --- Index
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 8.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 15.0f));
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	windowSize.y -= 30;
	ImGui::BeginChild("Index", ImVec2(windowSize.x * 0.25f, windowSize.y), true);

	ImGui::Indent(8.0f);
	if (ImGui::Selectable("Application", curr_index == Index::APPLICATION))	curr_index = Index::APPLICATION;
	else if (ImGui::Selectable("Input", curr_index == Index::INPUT_DRAW))	curr_index = Index::INPUT_DRAW;
	ImGui::Unindent();

	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::PopStyleVar(2);

	// --- Content
	ImGui::BeginChild("Content", ImVec2(0, windowSize.y), true);
	switch (curr_index)
	{
	case Index::APPLICATION: DrawApplication(); break;
	case Index::INPUT_DRAW:  DrawInput();		break;
	default: break;
	}
	ImGui::EndChild();

	// --- Buttons
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 15.0f));
	static ImVec2 button_size = ImVec2(windowSize.x * 0.25f, 22);
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 2.0f * button_size.x - 2, ImGui::GetCursorPosY() + 2));
	if (ImGui::Button("Accept", button_size))
	{
		LOG("Saved Configuration", 'i');
		Save(config);
		active = false;
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();

	if (ImGui::Button("Cancel", button_size))
	{
		Load(config);
		active = false;
		ImGui::CloseCurrentPopup();
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

//---------------------------------
void PanelConfiguration::Save(Config* config) const
{
}

void PanelConfiguration::Load(Config* config)
{
}

void PanelConfiguration::AddFPS(float fps, float ms)
{
	static uint count = 0;
	if (count == FPS_LOG_SIZE)
	{
		for (uint i = 0; i < FPS_LOG_SIZE - 1; ++i)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
	}
	else
		++count;

	fps_log[count - 1] = fps;
	ms_log[count - 1] = ms;
}

//---------------------------------
// --- DRAWS ---
void PanelConfiguration::DrawApplication()
{
}

void PanelConfiguration::DrawInput()
{
}

//---------------------------------
// --- MESSAGES ---
