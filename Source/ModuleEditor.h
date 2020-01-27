#pragma once
#include "Module.h"

#include <vector>

class Panel;
class Configuration;
class Console;
class Hierarchy;
//class Inspector;
//class Assets;
//class Viewport;

enum Style {
	BLACK = 0,
	CLASSIC,
	DARK,
	NUM_STYLES //used for getting size
};

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void Save(Config* config) const override;
	void Load(Config* config) override;

	void Draw();

	// Panels
	Panel* GetPanel(const char* name);

	// Style
	void ChangeStyle(uint new_style);
	uint GetStyle() { return style; }

	// Utilities
	void CreateLink(const char* text, const char* url, bool bullet = false);
	void LogFPS(float fps, float ms);

private:
	void DrawMenuBar();
	void DrawAbout();

	void DrawPanels();
	void ConfirmExit();
	void Shortcuts();

	void DockSpace();

public:
	uint style = 0;

	Configuration* panel_configuration = nullptr;
	Console* panel_console = nullptr;
	Hierarchy* panel_hierarchy = nullptr;
	//Inspector* panel_inspector = nullptr;
	//Viewport* panel_viewport = nullptr;
	//Assets* panel_assets = nullptr;

	bool is_new = false;
	bool is_open = false;
	bool is_save = false;
	bool is_build = false;
	bool is_import = false;
	bool is_auto_select = false;
	bool is_about = false;

private:
	bool close = false;

	std::vector<Panel*> panels;
};
