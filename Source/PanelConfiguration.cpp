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
	if (ImGui::Begin(name.c_str(), NULL, flags))
	{
		static ImVec2 windowSize = ImGui::GetContentRegionAvail();

		// --- Index
		DrawChildIndex(windowSize);

		ImGui::SameLine();

		// --- Content
		DrawChildContent(windowSize);

		// --- Buttons
		DrawButtons(windowSize);
	}
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
void PanelConfiguration::DrawChildIndex(ImVec2 windowSize)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 8.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 15.0f));
	
	if (ImGui::BeginChild("Index", ImVec2(windowSize.x * 0.25f, windowSize.y - 30.0f), true))
	{
		ImGui::Indent(8.0f);

		if		(ImGui::Selectable("Application", current_index == Index::APPLICATION))		current_index = Index::APPLICATION;
		else if (ImGui::Selectable("Input", current_index == Index::INPUT))					current_index = Index::INPUT;
		else if (ImGui::Selectable("File System", current_index == Index::FILESYSTEM))		current_index = Index::FILESYSTEM;

		ImGui::Unindent();
	}
	ImGui::EndChild();

	ImGui::PopStyleVar(2);
}

void PanelConfiguration::DrawChildContent(ImVec2 windowSize)
{
	if (ImGui::BeginChild("Content", ImVec2(0, windowSize.y - 30.0f), true))
	{
		switch (current_index)
		{
		case Index::APPLICATION: DrawApplication(); break;
		case Index::INPUT:		 DrawInput();		break;
		case Index::FILESYSTEM:	 DrawFilesystem();	break;

		default:
			break;
		}
	}
	ImGui::EndChild();
}

void PanelConfiguration::DrawButtons(ImVec2 windowSize)
{
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
}

void PanelConfiguration::DrawApplication()
{
}

void PanelConfiguration::DrawInput()
{
}

void PanelConfiguration::DrawFilesystem()
{
}

//---------------------------------
// --- MESSAGES ---
