#pragma once
#include "Panel.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

class Console : public Panel
{
public:
	Console();
	~Console();

	void Draw() override;
	void Shortcuts() override;

	void UpdateFilters();

	static void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	static void ClearLog();

	static char* Strdup(const char* str); //portable helper

public:
	static const uint default_width = 1400;
	static const uint default_height = 220;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 560;

	static char input_buffer[256];
	static ImVector<char*> items;
	static ImVector<char*> history;

	static int history_pos;    // -1: new line, 0..History.Size-1 browsing history.
	static ImGuiTextFilter filter;
	static ImGuiTextFilter searcher;

	bool is_debug = true;
	bool is_geometry = true;
	bool is_verbose = true;
	bool is_warning = true;

	bool is_search = false;
	bool is_auto_scroll = true;
	bool is_scroll_to_bottom = false;
	bool is_clear_on_play = true;
};
