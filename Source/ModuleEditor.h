#pragma once
#include "Module.h"

class MainMenuBar;
class Panel;
class PanelConfiguration;
//class PanelToolbar;
class PanelConsole;
//class PanelResources;
//class PanelAssets;
//class PanelHierarchy;
//class PanelInspector;
//class PanelScene;
//class PanelGame;

struct OnCloseEngine;

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Init() override;
	bool Start() override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	void Draw();

	// --------------------
	const bool IsPanelConsole() const { return panel_console != nullptr; }

private:
	void DrawPanels();

	// --- MESSAGES ---
	void OnCloseEditor(OnCloseEngine* m) { quit = true; }

private:
	bool quit = false;

	MainMenuBar* menu_bar = nullptr;

	PanelConfiguration*	panel_configuration = nullptr;
	//PanelToolbar*		panel_toolbar = nullptr;
	PanelConsole*		panel_console = nullptr;
	//PanelResources*	panel_resources = nullptr;
	//PanelAssets*		panel_assets = nullptr;
	//PanelHierarchy*	panel_hierarchy = nullptr;
	//PanelInspector*	panel_inspector = nullptr;
	//PanelScene*		panel_scene = nullptr;
	//PanelGame*		panel_game = nullptr;

	//Panel* focused_panel = nullptr;
};
