#pragma once

#include "Module.h"
#include "Panel.h"
#include <vector>

#define FILE_MAX 250

class Console;
//class Configuration;
//class Hierarchy;
//class Inspector;
//class Assets;
//class Viewport;

enum Style {
	NEW = 0,
	CLASSIC,
	DARK,
	LIGHT
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

	void CreateLink(const char* text, const char* url, bool bullet = false);
	void LogFPS(float fps, float ms);

	// Panels
	Panel* GetPanel(const char* name);
	int GetPanelWidth(Panel* panel) const { return panel->width; }
	int GetPanelHeight(Panel* panel) const { return panel->height; }
	int GetPanelPosX(Panel* panel) const { return panel->pos_x; }
	int GetPanelPosY(Panel* panel) const { return panel->pos_y; }
	bool GetPanelActive(Panel* panel) const { return panel->active; }
	Panel* GetPanelFocused() { return focused_panel; }

private:
	void DrawMenu();
	void DrawDemo();
	void DrawAbout();
	void DrawPanels();
	void ConfirmExit();
	void Shortcuts();

	void DockSpace();

public:
	Panel* focused_panel = nullptr;

	//Configuration* tab_configuration = nullptr;
	//Hierarchy* tab_hierarchy = nullptr;
	Console* tab_console = nullptr;
	//Inspector* tab_inspector = nullptr;
	//Viewport* tab_viewport = nullptr;
	//Assets* tab_assets = nullptr;

	//Bools
	bool is_new = false;
	bool is_open = false;
	bool is_save = false;
	bool is_import = false;

	bool is_auto_select = false;

	bool is_show_demo = false;
	bool is_about = false;

private:
	bool close = false;
	uint style = 0;

	std::vector<Panel*> panels;
};
