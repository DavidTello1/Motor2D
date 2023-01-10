#include "PanelConsole.h"
#include "Application.h"
#include "ModuleInput.h"

#include <windows.h>
#include <chrono>
#include <ctime> 
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

ImGuiTextFilter PanelConsole::filter;
ImGuiTextFilter PanelConsole::searcher;

// ---------------------------------------------------------
PanelConsole::PanelConsole() : Panel("###Console", ICON_CONSOLE, 2)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

	App->ClearLog();
}

PanelConsole::~PanelConsole()
{
	App->ClearLog();
}

void PanelConsole::Draw()
{
	pos_x = ImGui::GetWindowPos().x;
	pos_y = ImGui::GetWindowPos().y;

	static bool is_copy = false;

	// Shortcuts
	Shortcuts();

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Filters"))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			ImGui::MenuItem("Debug", NULL, &is_debug);
			ImGui::MenuItem("Verbose", NULL, &is_verbose);
			ImGui::MenuItem("Geometry", NULL, &is_geometry);
			ImGui::MenuItem("Warning", NULL, &is_warning);

			ImGui::PopItemFlag();
			ImGui::EndMenu();
		}

		ImGui::MenuItem("Search", NULL, &is_search);
		ImGui::EndMenuBar();
	}

	// Search
	if (is_search)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY() - 4));
		searcher.Draw("Search", 180);
		ImGui::Separator();
	}
	
	ImGui::BeginChild("Console_Logs", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);

	// Right Click Options
	if (ImGui::BeginPopupContextWindow())
	{
		is_copy = ImGui::MenuItem("Copy", "Ctrl + C");
		if (ImGui::MenuItem("Clear")) App->ClearLog();

		ImGui::Separator();
		ImGui::MenuItem("Clear on Play", NULL, &is_clear_on_play);
		ImGui::MenuItem("Auto-scroll", NULL, &is_auto_scroll);

		ImGui::EndPopup();
	}

	//// Clear On Play
	//static bool flag = false;
	//if (ClearOnPlay)
	//{
	//	if (App->scene->state == PLAY && flag == false)
	//	{
	//		flag = true;
	//		ClearLog();
	//	}
	//	else if (App->scene->state == STOP)
	//		flag = false;
	//}

	// Tighten spacing
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	// Start Copying to Clipboard
	if (is_copy)
		ImGui::LogToClipboard();

	// Actual drawing of logs
	std::vector<Log> logs = App->GetLog();
	for (Log new_log : logs)
	{
		if (!searcher.PassFilter(new_log.message.c_str()))
			continue;

		ImVec4 color;
		if (new_log.icon == ICON_ERROR)
		{
			color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); //red
		}
		else if (new_log.icon == ICON_WARNING)
		{
			if (!is_warning)
				continue;
			color = ImVec4(1.0f, 0.75f, 0.0f, 1.0f); //yellow
		}
		else if (new_log.icon == ICON_GEOMETRY)
		{
			if (!is_geometry)
				continue;
			color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); //green
		}
		else if (new_log.icon == ICON_VERBOSE)
		{
			if (!is_verbose)
				continue;
			color = ImVec4(0.2f, 0.4f, 1.0f, 1.0f); //blue
		}
		else if (new_log.icon == ICON_DEBUG)
		{
			if (!is_debug)
				continue;
			color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //white
		}

		ImGui::TextUnformatted(new_log.time.c_str());
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(std::string(new_log.icon + std::string(" ") + new_log.message).c_str());
		ImGui::PopStyleColor();
	}
	// Finish Copying to Clipboard
	if (is_copy)
	{
		ImGui::LogFinish();
		LOG("Copied to clipboard", 'd');

		App->new_logs = 0;
		ImGui::TextUnformatted("Copied to clipboard");
		if (is_auto_scroll)
			ImGui::SetScrollHereY();

		is_copy = false;
	}

	// Scroll
	if (is_auto_scroll && App->new_logs > 0)
		ImGui::SetScrollHereY();

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void PanelConsole::Shortcuts()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
			&& App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			is_search = !is_search;
		}
	}
}

void PanelConsole::Save(Config* config) const
{
}

void PanelConsole::Load(Config* config)
{
}


// Log Function
void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static char log_type[4096];

	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);

	// Filtering
	strcpy_s(log_type, format);
	strcat_s(log_type, "%c");
	vsprintf_s(tmp_string2, log_type, ap);
	char char_type = tmp_string2[strlen(tmp_string2) - 1];

	va_end(ap);

	// Default visual studio logging
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	//--- Console log ---
	// Time
	std::time_t tmp_date2 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string time = std::ctime(&tmp_date2);
	//time = time.substr(11, 8);

	// Icon
	const char* icon;
	if		(char_type == 'g')	icon = ICON_GEOMETRY;
	else if (char_type == 'd')	icon = ICON_DEBUG;
	else if (char_type == 'w')	icon = ICON_WARNING;
	else if (char_type == 'e')	icon = ICON_ERROR;
	else						icon = ICON_VERBOSE;

	// Add Log
	if (App)
		App->AddLog(icon, time.c_str(), tmp_string);
}
