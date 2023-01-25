#include "PanelConsole.h"

#include "Application.h"
#include "MessageBus.h"

#include <string>
#include <chrono>
#include <ctime>
#include "Imgui/imgui_internal.h"

#include "mmgr/mmgr.h"

ImGuiTextFilter PanelConsole::filter;
ImGuiTextFilter PanelConsole::searcher;

// ---------------------------------------------------------
PanelConsole::PanelConsole(bool active) : Panel(active)
{
	icon = "";/* ICON_CONSOLE; */
	name = "Console";
	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

	position = ImVec2(0.0f, 560.0f);
	size = ImVec2(1400, 220.0f);

	ClearLog();

	// --- Messages ---
	App->message->Subscribe(this, &PanelConsole::OnLog);
}

PanelConsole::~PanelConsole()
{
	ClearLog();
}

void PanelConsole::Draw()
{
	// --- Set Name
	static char console_name[128];
	if (new_logs > 99)
		sprintf_s(console_name, "Console (99+)###Console");
	else if (new_logs > 0)
		sprintf_s(console_name, "Console (%d)###Console", new_logs);
	else
		sprintf_s(console_name, "Console###Console");

	// --- Begin Panel
	std::string name = icon + std::string(" ") + console_name;
	if (ImGui::Begin(name.c_str(), &active, flags))
	{
		// --- Menu Bar
		DrawMenuBar();

		// --- Logs
		DrawChildLogs();

		// --- Shortcuts
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			//App->editor->focused_panel = this; //*** MESSAGE
			//Shortcuts();
			new_logs = 0;
		}
	}
	ImGui::End();
}

void PanelConsole::DrawMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		// --- Filters
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

		// --- Options
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			ImGui::MenuItem("Auto Scroll", NULL, &is_auto_scroll);
			ImGui::MenuItem("Clear on Play", NULL, &is_clear_on_play);
			//ImGui::MenuItem("Collapse", NULL, &is_collapse);

			ImGui::PopItemFlag();
			ImGui::EndMenu();
		}

		// --- Search
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.25f, 0.25f, 1.00f));
		searcher.Draw("", 180);
		ImGui::PopStyleColor();

		if (!ImGui::IsItemActive() && !searcher.IsActive())
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 184);
			ImGui::TextDisabled("Search");
		}

		ImGui::EndMenuBar();
	}
}

void PanelConsole::DrawChildLogs()
{
	ImGui::BeginChild("ConsoleLogs", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	// --- Right Click
	RightClick();

	// Start Copy
	if (is_copy)
		ImGui::LogToClipboard();

	// --- Draw Logs
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	DrawLogs();

	// Finish Copy
	if (is_copy)
	{
		ImGui::LogFinish();
		LOG("Copied to clipboard", 'd');

		new_logs = 0;
		ImGui::TextUnformatted("Copied to clipboard");
		if (is_auto_scroll)
			ImGui::SetScrollHereY();

		is_copy = false;
	}

	// Scroll
	if (is_auto_scroll && new_logs > 0)
		ImGui::SetScrollHereY();

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void PanelConsole::DrawLogs()
{
	for (Log log : logs)
	{
		if (!searcher.PassFilter(log.message.c_str()))
			continue;

		// Time
		//std::time_t tmp_date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		//std::string time = std::ctime(&tmp_date);
		//time = time.substr(11, 8) + " - ";

		//ImGui::TextUnformatted(time.c_str());
		ImGui::TextUnformatted(" - ");
		ImGui::SameLine(0.0f, 0.0f);

		// Text Color
		ImVec4 color;
		if (log.type == 'g')
		{
			if (!is_geometry)
				continue;
			color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); //green
		}
		else if (log.type == 'd')
		{
			if (!is_debug)
				continue;
			color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //white
		}
		else if (log.type == 'w')
		{
			if (!is_warning)
				continue;
			color = ImVec4(1.0f, 0.75f, 0.0f, 1.0f); //yellow
		}
		else if (log.type == 'e')
		{
			color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); //red
		}
		else
		{
			if (!is_verbose)
				continue;
			color = ImVec4(0.2f, 0.4f, 1.0f, 1.0f); //blue
		}

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(log.message.c_str());
		ImGui::PopStyleColor();
	}
}

void PanelConsole::RightClick()
{
	if (ImGui::BeginPopupContextWindow())
	{
		is_copy = ImGui::MenuItem("Copy");

		if (ImGui::MenuItem("Clear")) 
			ClearLog();

		ImGui::EndPopup();
	}
}

void PanelConsole::Shortcuts()
{
}

void PanelConsole::Save(Config* config) const
{
}

void PanelConsole::Load(Config* config)
{
}

void PanelConsole::ClearLog()
{
	logs.erase(logs.begin(), logs.end());
	logs.clear();

	new_logs = 0;
}

// ---------------------------------------------
void PanelConsole::OnLog(OnAddLog* m)
{
	if (logs.size() >= MAX_LOG_SIZE)
		logs.erase(logs.begin());

	logs.push_back({ m->type, m->message });

	new_logs++;
}

void PanelConsole::OnPlay(OnPlayEngine* m)
{
	if (is_clear_on_play == false)
		return;

	ClearLog();
}
