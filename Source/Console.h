#pragma once
#include "Panel.h"

#include <vector>

class Console : public Panel
{
public:
	Console();
	~Console();

	void Draw() override;
	void Shortcuts() override;

public:
	static const uint default_width = 1400;
	static const uint default_height = 220;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 560;

	static ImGuiTextFilter filter;
	static ImGuiTextFilter searcher;

	bool is_debug = true;
	bool is_geometry = true;
	bool is_verbose = true;
	bool is_warning = true;

	bool is_search = false;
	bool is_auto_scroll = true;
	bool is_clear_on_play = true;
};
