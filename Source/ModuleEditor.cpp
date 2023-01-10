#include "ModuleEditor.h"

#include "Utils.h"
#include "MessageBus.h"

#include "MainMenuBar.h"
#include "PanelConfiguration.h"
#include "PanelConsole.h"

// *** MESSAGES
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer.h"
// ---

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <windows.h>

#include "mmgr/mmgr.h"

ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	LOG("Init editor gui with imgui lib version %s", ImGui::GetVersion(), 'i');

	// --- Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking

	LOG("Loading ImGui", 'i');
	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->GetContext());
	ImGui_ImplOpenGL3_Init();

	// --- Style
	ImGui::StyleColorsNew();

	// --- Main MenuBar
	menu_bar = new MainMenuBar();

	// --- Panels Creation
	//panel_toolbar = new PanelToolbar();
	panel_configuration = new PanelConfiguration(false);
	panel_console = new PanelConsole();
	//panel_hierarchy = new PanelHierarchy();
	//panel_assets = new PanelAssets();
	//panel_resources = new PanelResources();
	//panel_scene = new PanelScene();
	//panel_game = new PanelGame();
	//panel_inspector = new Inspector();

	// --- Messages ---
	App->message->Subscribe(this, &ModuleEditor::OnCloseEditor);

	return true;
}

bool ModuleEditor::Start()
{
	//// --- Icon Font
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//
	//ImFontConfig icons_config;
	//icons_config.MergeMode = true;
	//icons_config.GlyphMinAdvanceX = 14.0f;
	//
	//static const ImWchar icons_ranges[] = { ICON_MIN, ICON_MAX, 0 };
	//io.Fonts->AddFontFromFileTTF(ICON_FONT, 14.0f, &icons_config, icons_ranges);

	return true;
}

bool ModuleEditor::PreUpdate(float dt)
{
	// --- New Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow()); //***
	ImGui::NewFrame();

	return true;
}

bool ModuleEditor::PostUpdate(float dt)
{
	if (quit == true)
		return false;

	return true;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Freeing editor gui", 'i');

	// --- Release Main MenuBar
	RELEASE(menu_bar);

	// --- Release Panels
	//RELEASE(panel_toolbar);
	RELEASE(panel_configuration);
	RELEASE(panel_console);
	//RELEASE(panel_hierarchy);
	//RELEASE(panel_assets);
	//RELEASE(panel_resources);
	//RELEASE(panel_scene);
	//RELEASE(panel_game);
	//RELEASE(panel_inspector);

	// --- Shutdown Imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

// ---------------------------------------------
void ModuleEditor::Draw()
{
	// --- Dockspace ---
	Utils::Dockspace(18.0f);

	// --- Menu Bar ---
	menu_bar->Draw();

	// --- Panels ---
	DrawPanels();

	//// --- Shortcuts ---
	//Shortcuts();

	// --- Render GUI ---
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void ModuleEditor::DrawPanels()
{
	// --- Configuration ---
	if (panel_configuration->IsActive()) 
		panel_configuration->Draw();

	// --- Toolbar ---

	// --- Console ---
	if (panel_console->IsActive())
		panel_console->Draw();

	// --- Resources ---
	// --- Assets ---
	// --- Hierarchy ---
	// --- Inspector ---
	// --- Scene ---
	// --- Game ---
}
