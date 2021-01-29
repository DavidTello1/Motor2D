#pragma once
#include "Module.h"

#include <vector>

class Panel;
class Configuration;
class Console;
class Hierarchy;
class Assets;
//class Inspector;
//class Viewport;

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

	void DockSpace();

public:
	Panel* focused_panel = nullptr;
	Configuration* panel_configuration = nullptr;
	Console* panel_console = nullptr;
	Hierarchy* panel_hierarchy = nullptr;
	Assets* panel_assets = nullptr;
	//Inspector* panel_inspector = nullptr;
	//Viewport* panel_viewport = nullptr;
	//Game* panel_game = nullptr;

	bool is_auto_select = false;
	bool is_about = false;

private:
	std::vector<Panel*> panels;

	bool close = false;
};
