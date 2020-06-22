#include "Console.h"
#include "Application.h"
#include "ModuleInput.h"

#include "Imgui/imgui_internal.h"
#include <windows.h>

#include "mmgr/mmgr.h"

//Variables
char 				Console::InputBuf[256];
ImVector<char*>		Console::Items;
ImVector<char*>		Console::History;
int					Console::HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
ImGuiTextFilter		Console::Filter;
ImGuiTextFilter		Console::Searcher;
bool				Console::AutoScroll;
bool				Console::ShowVerboseLog;
bool				Console::ShowGeometryLog;
bool				Console::ShowDebugLog;
bool				Console::ShowWarningLog;
bool				Console::EnableFileName;
bool				Console::ScrollToBottom;
bool				Console::ShowSearch;
bool				Console::ClearOnPlay;

//Constructor
Console::Console() : Panel("Console", ICON_CONSOLE)
{
	LOG("Init Console");
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	flags = ImGuiWindowFlags_MenuBar;

	//Booleans
	AutoScroll = true;
	ShowDebugLog = true;
	ShowGeometryLog = true;
	ShowVerboseLog = true;
	ShowWarningLog = true;
	EnableFileName = false;
	ScrollToBottom = false;

	//Panel size & pos
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	UpdateFilters();
}

//Destructor
Console::~Console()
{
	LOG("Closing Console")
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}

//Update Console Filters
void Console::UpdateFilters()
{
	char filters_buffer[256] = " ";

	if (!ShowDebugLog)	  strcat_s(filters_buffer, 256, "-[Debug],");
	if (!ShowGeometryLog) strcat_s(filters_buffer, 256, "-[Geometry],");
	if (!ShowVerboseLog)  strcat_s(filters_buffer, 256, "-[Verbose],");
	if (!ShowWarningLog)  strcat_s(filters_buffer, 256, "-[Warning],");

	Filter.Clear();
	sprintf_s(Filter.InputBuf, 256, "%s", filters_buffer);
	Filter.Build();
}

//Draw Console Panel
void Console::Draw()
{
	static bool copy_to_clipboard = false;

	// Shortcuts
	Shortcuts();

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			copy_to_clipboard = ImGui::MenuItem("Copy");
			if (ImGui::MenuItem("Clear")) { ClearLog(); }

			ImGui::Separator();

			ImGui::MenuItem("Clear on Play", NULL, &ClearOnPlay);
			ImGui::MenuItem("Auto-scroll", NULL, &AutoScroll);
			ImGui::MenuItem("Enable file name", NULL, &EnableFileName);

			ImGui::PopItemFlag();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Filters"))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			if (ImGui::MenuItem("Debug", NULL, &ShowDebugLog)) UpdateFilters();
			if (ImGui::MenuItem("Verbose", NULL, &ShowVerboseLog)) UpdateFilters();
			if (ImGui::MenuItem("Geometry", NULL, &ShowGeometryLog)) UpdateFilters();
			if (ImGui::MenuItem("Warning", NULL, &ShowWarningLog)) UpdateFilters();

			ImGui::PopItemFlag();
			ImGui::EndMenu();
		}

		ImGui::MenuItem("Search", NULL, &ShowSearch);
		ImGui::EndMenuBar();
	}

	// Search
	if (ShowSearch)
	{
		Searcher.Draw("Search", 180);
		ImGui::Separator();
	}

	// Log Child
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
	
	// Right Click Options
	if (ImGui::BeginPopupContextWindow())
	{
		copy_to_clipboard = ImGui::Selectable("Copy", false, 0, ImVec2(60,14));
		if (ImGui::Selectable("Clear", false, 0, ImVec2(60, 14))) ClearLog();
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
	if (copy_to_clipboard)
		ImGui::LogToClipboard();

	// Actual drawing of logs
	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];
		if (!Filter.PassFilter(item) || !Searcher.PassFilter(item))
			continue;

		// Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
		if (strstr(item, "[Error]"))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); //red

		else if (strstr(item, "[Warning]"))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.0f, 1.0f)); //yellow

		else if (strstr(item, "[Geometry]"))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f)); //green

		else if (strstr(item, "[Verbose]"))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.4f, 1.0f, 1.0f)); //blue

		else if (strstr(item, "[Debug]"))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); //white

		else if (strncmp(item, "# ", 2) == 0)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));

		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	// Finish Copying to Clipboard
	if (copy_to_clipboard)
	{
		ImGui::LogFinish();
		LOG("Copied to clipboard", 'd');
		copy_to_clipboard = false;
	}

	// Scroll
	if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();

	//// Auto-focus on window apparition
	//bool reclaim_focus = false;
	//ImGui::SetItemDefaultFocus();
	//if (reclaim_focus)
	//	ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
}

void Console::Shortcuts()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
			&& App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			ShowSearch = !ShowSearch;
		}
	}
}

// Log Function
void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char final_log[4096];
	static char log_type[4096];

	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);

	// Filtering
	strcpy_s(log_type, format);
	strcat_s(log_type, "%c");
	vsprintf_s(final_log, log_type, ap);
	char char_type = final_log[strlen(final_log) - 1];

	va_end(ap);

	//Default visual studio logging
	sprintf_s(final_log, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(final_log);

	//--- Console log system
	//Adding log string distinguishment in front of the log itself
	if (char_type == 'g')		sprintf_s(log_type, 4096, "%s ", GEOMETRY_LOG_STRING);
	else if (char_type == 'd')	sprintf_s(log_type, 4096, "%s ", DEBUG_LOG_STRING);
	else if (char_type == 'w')	sprintf_s(log_type, 4096, "%s ", WARNING_LOG_STRING);
	else if (char_type == 'e')	sprintf_s(log_type, 4096, "%s ", ERROR_LOG_STRING);
	else						sprintf_s(log_type, 4096, "%s ", VERBOSE_LOG_STRING);

	if (Console::EnableFileName)
	{
		// 92 stands for '\' character
		const char* short_file = strrchr(file, 92);

		sprintf_s(final_log, 4096, "%s%s(%d) : %s", log_type, short_file + 1, line, tmp_string);
	}
	else
		sprintf_s(final_log, 4096, "%s%s", log_type, tmp_string);

	Console::AddLog(final_log);
}
