#include "Console.h"
#include "Application.h"
#include "ModuleInput.h"

#include "Imgui/imgui_internal.h"
#include <windows.h>

#include "mmgr/mmgr.h"

char Console::input_buffer[256];
ImVector<char*> Console::items;
ImVector<char*> Console::history;
int Console::history_pos;    // -1: new line, 0..History.Size-1 browsing history.
ImGuiTextFilter Console::filter;
ImGuiTextFilter Console::searcher;

// ---------------------------------------------------------
Console::Console() : Panel("Console", ICON_CONSOLE)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

	ClearLog();
	memset(input_buffer, 0, sizeof(input_buffer));
	history_pos = -1;

	UpdateFilters();
}

Console::~Console()
{
	ClearLog();
	for (int i = 0; i < history.Size; i++)
		free(history[i]);
}

void Console::Draw()
{
	static bool is_copy = false;

	// Shortcuts
	Shortcuts();

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Filters"))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			if (ImGui::MenuItem("Debug", NULL, &is_debug)) UpdateFilters();
			if (ImGui::MenuItem("Verbose", NULL, &is_verbose)) UpdateFilters();
			if (ImGui::MenuItem("Geometry", NULL, &is_geometry)) UpdateFilters();
			if (ImGui::MenuItem("Warning", NULL, &is_warning)) UpdateFilters();

			ImGui::PopItemFlag();
			ImGui::EndMenu();
		}

		ImGui::MenuItem("Search", NULL, &is_search);
		ImGui::EndMenuBar();
	}

	// Search
	if (is_search)
	{
		searcher.Draw("Search", 180);
		ImGui::Separator();
	}

	// Log Child
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve)); // Leave room for 1 separator + 1 InputText
	
	// Right Click Options
	if (ImGui::BeginPopupContextWindow())
	{
		is_copy = ImGui::MenuItem("Copy", "Ctrl + C");
		if (ImGui::MenuItem("Clear")) ClearLog();

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
	for (int i = 0; i < items.Size; i++)
	{
		const char* item = items[i];
		if (!filter.PassFilter(item) || !searcher.PassFilter(item))
			continue;

		// Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
		if (strstr(item, ICON_ERROR))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); //red

		else if (strstr(item, ICON_WARNING))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.0f, 1.0f)); //yellow

		else if (strstr(item, ICON_GEOMETRY))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f)); //green

		else if (strstr(item, ICON_VERBOSE))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.4f, 1.0f, 1.0f)); //blue

		else if (strstr(item, ICON_DEBUG))
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); //white

		else if (strncmp(item, "# ", 2) == 0)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));

		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	// Finish Copying to Clipboard
	if (is_copy)
	{
		ImGui::LogFinish();
		LOG("Copied to clipboard", 'd');
		is_copy = false;
	}

	// Scroll
	if (is_scroll_to_bottom || (is_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	is_scroll_to_bottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void Console::Shortcuts()
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

void Console::UpdateFilters()
{
	char filters_buffer[256] = " ";

	if (!is_debug)	  strcat_s(filters_buffer, 256, ICON_DEBUG);
	if (!is_geometry) strcat_s(filters_buffer, 256, ICON_GEOMETRY);
	if (!is_verbose)  strcat_s(filters_buffer, 256, ICON_VERBOSE);
	if (!is_warning)  strcat_s(filters_buffer, 256, ICON_WARNING);

	filter.Clear();
	sprintf_s(filter.InputBuf, 256, "%s", filters_buffer);
	filter.Build();
}

// --- LOGS ---
char* Console::Strdup(const char* str)
{
	size_t len = strlen(str) + 1;
	void* buf = malloc(len);
	IM_ASSERT(buf);
	return (char*)memcpy(buf, (const void*)str, len);
}

void Console::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	items.push_back(Strdup(buf));
}

void Console::ClearLog()
{
	for (int i = 0; i < items.Size; ++i)
		free(items[i]);
	items.clear();
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
	if (char_type == 'g')		sprintf_s(log_type, 4096, "%s ", ICON_GEOMETRY);
	else if (char_type == 'd')	sprintf_s(log_type, 4096, "%s ", ICON_DEBUG);
	else if (char_type == 'w')	sprintf_s(log_type, 4096, "%s ", ICON_WARNING);
	else if (char_type == 'e')	sprintf_s(log_type, 4096, "%s ", ICON_ERROR);
	else						sprintf_s(log_type, 4096, "%s ", ICON_VERBOSE);

	sprintf_s(final_log, 4096, "%s%s", log_type, tmp_string);

	Console::AddLog(final_log);
}
