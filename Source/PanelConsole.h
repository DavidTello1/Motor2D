#pragma once
#include "Panel.h"

#include "Globals.h"

#include <vector>
#include <string>

struct OnAddLog;
struct OnPlayEngine;

class PanelConsole : public Panel
{
	struct Log {
		char type;
		std::string message;
	};

public:
	PanelConsole(bool active = true);
	~PanelConsole();

	void Draw() override;
	void Shortcuts() override;

private:
	void Save(Config* config) const override;
	void Load(Config* config) override;

	void ClearLog();

	// ----------------------
	void DrawMenuBar();
	void DrawChildLogs();
	void DrawLogs();

	void RightClick();

	// ----------------------
	void OnLog(OnAddLog* m);
	void OnPlay(OnPlayEngine* m);

private:
	static constexpr int MAX_LOG_SIZE = 1000;

	std::vector<Log> logs;
	uint new_logs = 0;

	static ImGuiTextFilter filter;
	static ImGuiTextFilter searcher;

	bool is_copy = false;

	bool is_debug = true;
	bool is_geometry = true;
	bool is_verbose = true;
	bool is_warning = true;

	bool is_auto_scroll = true;
	bool is_clear_on_play = true;
	//bool is_collapse = false;
};
