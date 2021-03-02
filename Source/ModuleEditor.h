#pragma once
#include "Module.h"

#include <vector>

#define TOOLBAR_SIZE 42
#define MENUBAR_SIZE 19

class Panel;
class Toolbar;
class Configuration;
class Console;
class Hierarchy;
class Assets;
class Resources;
//class Inspector;
//class Viewport;

struct ImGuiWindowClass;

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
	Panel* GetPanel(uint ID);

	// Utilities
	void CreateLink(const char* text, const char* url, bool bullet = false);
	void LogFPS(float fps, float ms);

private:
	void DrawMenuBar();
	void DrawAbout();

	void DrawPanels();
	void ConfirmExit();
	void Shortcuts();

public:
	Panel* focused_panel = nullptr;
	Toolbar* panel_toolbar = nullptr;
	Configuration* panel_configuration = nullptr;
	Console* panel_console = nullptr;
	Hierarchy* panel_hierarchy = nullptr;
	Assets* panel_assets = nullptr;
	Resources* panel_resources = nullptr;
	//Inspector* panel_inspector = nullptr;
	//Viewport* panel_scene = nullptr;
	//Game* panel_game = nullptr;

	bool is_auto_select = false;
	bool is_about = false;
	bool ini_change = false;

	std::string ini;
	uint ini_size = 0;

private:
	std::vector<Panel*> panels;

	bool close = false;
};
