#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"
#include "ModuleFileSystem.h"
//#include "ModuleScene.h"
#include "Config.h"

#include "PanelToolbar.h"
#include "PanelConfiguration.h"
#include "PanelConsole.h"
#include "PanelHierarchy.h"
#include "PanelAssets.h"
#include "PanelResources.h"
#include "PanelScene.h"
//#include "PanelGame.h"
//#include "PanelInspector.h"

#include "Imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <windows.h>

#include "mmgr/mmgr.h"

ModuleEditor::ModuleEditor(bool start_enabled) : Module("ModuleEditor", start_enabled)
{
}

ModuleEditor::~ModuleEditor()
{
}

// Called before render is available
bool ModuleEditor::Init(Config* config)
{
	LOG("Init editor gui with imgui lib version %s", ImGui::GetVersion());

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;

	ini = config->GetString("ini", "error");
	ini_size = config->GetUInt("ini_size", 0);
	if (ini == "error" || ini_size == 0.0f)
	{
		LOG("Error loading ini file", 'e');
	}
	else
		ImGui::LoadIniSettingsFromMemory(ini.c_str(), ini_size);

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->GetContext());
	LOG("Loading ImGui");
	ImGui_ImplOpenGL3_Init();

	// Style
	ImGui::StyleColorsNew();

	// Create panels
	panels.push_back(panel_toolbar = new PanelToolbar());
	panels.push_back(panel_configuration = new PanelConfiguration());
	panels.push_back(panel_console = new PanelConsole());
	panels.push_back(panel_hierarchy = new PanelHierarchy());
	panels.push_back(panel_assets = new PanelAssets());
	panels.push_back(panel_resources = new PanelResources());
	panels.push_back(panel_scene = new PanelScene());
	//panels.push_back(panel_game = new PanelGame());
	//panels.push_back(panel_inspector = new Inspector());

	return true;
}

bool ModuleEditor::Start(Config* config)
{
	// Icon Font
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	ImFontConfig icons_config; 
	icons_config.MergeMode = true; 
	icons_config.GlyphMinAdvanceX = 14.0f;

	static const ImWchar icons_ranges[] = { ICON_MIN, ICON_MAX, 0 };
	io.Fonts->AddFontFromFileTTF(ICON_FONT, 14.0f, &icons_config, icons_ranges);

	//panel_viewport->GenerateFBO();
	return true;
}

bool ModuleEditor::PreUpdate(float dt)
{
	//// Start the frame
	//panel_viewport->PreUpdate();

	if (ini_change)
	{
		ImGui::LoadIniSettingsFromMemory(ini.c_str(), ini_size);
		ini_change = false;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();

	return true;
}

bool ModuleEditor::Update(float dt)
{
	return true;
}

bool ModuleEditor::PostUpdate(float dt)
{
	//// End the frame
	//panel_viewport->PostUpdate();

	if (is_close)
		return false;

	return true;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Freeing editor gui");

	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		RELEASE(*it);
	}
	panels.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::Save(Config* config) const
{
	size_t ini_data_size = 0;
	const char* ini_data = ImGui::SaveIniSettingsToMemory(&ini_data_size);
	config->AddString("ini", ini_data);
	config->AddUInt("ini_size", ini_data_size);

	for (Panel* panel : panels)
	{
		config->AddBool(std::string("active" + std::string(panel->GetName())).c_str(), panel->active);
		panel->Save(&config->AddSection(panel->GetName()));
	}
}

void ModuleEditor::Load(Config* config)
{
	ini = config->GetString("ini", "error");
	ini_size = config->GetUInt("ini_size", 0);
	if (ini == "error" || ini_size == 0.0f)
		LOG("Error loading ini file", 'e')
	else
	{
		ini_change = true;
		App->file_system->Save("imgui.ini", ini.data(), ini_size);
	}

	for (Panel* panel : panels)
	{
		panel->active = config->GetBool(std::string("active" + std::string(panel->GetName())).c_str(), false);
		panel->Load(&config->GetSection(panel->GetName()));
	}
}

// Drawing of the FULL gui (first gets drawn the Menus, then panels)
void ModuleEditor::Draw()
{
	// DockSpace
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + TOOLBAR_SIZE + MENUBAR_SIZE));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - TOOLBAR_SIZE - MENUBAR_SIZE));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DockSpace", NULL, window_flags);
	ImGuiID main_dockspace = ImGui::GetID("MyDockspace");
	float menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();

	ImGui::DockSpace(main_dockspace);
	ImGui::End();
	ImGui::PopStyleVar(3);

	// Draw functions
	DrawMenuBar();
	DrawAbout();
	DrawPanels();

	// Shortcuts
	Shortcuts();

	// Are you sure you want to Quit
	if (App->input->quit == true)
	{
		if (is_about)
			is_about = false;

		ConfirmExit();
	}

	// Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		// File
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N", false))
			{
			}
			if (ImGui::MenuItem("Open", "Ctrl+O", false))
			{
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				//uint id = 0;
				//std::string path = App->scene->scene_name;
				//path = ASSETS_FOLDER + path;
				//std::string written_file;
				
				//ResourceScene* scene = (ResourceScene*)App->resources->CreateInitResource(Resource::Type::scene, id, path.c_str(), written_file);
				//scene->SaveOwnFormat(written_file);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Build", "Ctrl+S"))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "ESC"))
				App->input->quit = true;

			ImGui::EndMenu();
		}

		// Edit
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false))
			{
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false))
			{
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Import", "Ctrl+I", false, false))
			{
			}
			if (ImGui::MenuItem("Export", "Ctrl+E", false, false))
			{
			}
			ImGui::Separator();


			if (ImGui::MenuItem("Cut", "Ctrl+X", false, false))
			{
			}
			if (ImGui::MenuItem("Copy", "Ctrl+C", false, false))
			{
			}
			if (ImGui::MenuItem("Paste", "Ctrl+V", false, false))
			{
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, false))
			{
			}
			if (ImGui::MenuItem("Delete", "Supr", false, false))
			{
			}
			ImGui::EndMenu();
		}

		// GameObjects
		if (ImGui::BeginMenu("GameObjects"))
		{

			ImGui::EndMenu();
		}

		// Tools
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Map Editor", NULL, false, false))
			{
			}
			if (ImGui::MenuItem("Audio Editor", NULL, false, false))
			{
			}
			if (ImGui::MenuItem("Animations", NULL, false, false))
			{
			}
			if (ImGui::MenuItem("Particles", NULL, false, false))
			{
			}
			if (ImGui::MenuItem("Lights", NULL, false, false))
			{
			}
			if (ImGui::MenuItem("Shaders", NULL, false, false))
			{
			}
			ImGui::EndMenu();
		}

		// Windows
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Configuration", NULL, &GetPanel(1)->active);
			ImGui::Separator();
			ImGui::MenuItem("Console", NULL, &GetPanel(2)->active);
			ImGui::MenuItem("Hierarchy", NULL, &GetPanel(3)->active);
			ImGui::MenuItem("Assets", NULL, &GetPanel(4)->active);
			ImGui::MenuItem("Resources", NULL, &GetPanel(5)->active);
			ImGui::MenuItem("Scene", NULL, &GetPanel(6)->active);
			//ImGui::MenuItem("Game", NULL, &GetPanel(7)->active);
			//ImGui::MenuItem("Inspector", NULL, &GetPanel(8)->active);

			ImGui::EndMenu();
		}

		// Help
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Github"))
				ShellExecuteA(NULL, "open", "https://github.com/DavidTello1/Motor2D", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Documentation"))
				ShellExecuteA(NULL, "open", "https://github.com/DavidTello1/Motor2D/wiki", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Latest Version"))
				ShellExecuteA(NULL, "open", "https://github.com/DavidTello1/Motor2D/releases", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Report a bug"))
				ShellExecuteA(NULL, "open", "https://github.com/DavidTello1/Motor2D/issues", NULL, NULL, SW_SHOWNORMAL);

			ImGui::Separator();
			if (ImGui::MenuItem("About"))
				is_about = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ModuleEditor::DrawPanels()
{
	for (uint i = 0; i <panels.size(); ++i)
	{
		if (panels[i]->active)
		{
			ImGui::SetNextWindowPos(ImVec2((float)panels[i]->pos_x, (float)panels[i]->pos_y), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2((float)panels[i]->width, (float)panels[i]->height), ImGuiCond_FirstUseEver);

			std::string name = std::string(panels[i]->GetIcon()) + std::string(" ") + std::string(panels[i]->GetName());

			// Configuration
			if (panels[i]->GetName() == "Configuration")
			{
				ImGui::SetNextWindowPos(ImVec2(panel_configuration->default_pos_x, panel_configuration->default_pos_y), ImGuiCond_Appearing);
				ImGui::Begin(name.c_str(), NULL, panels[i]->flags);
				panels[i]->Draw();
				if (ImGui::IsWindowAppearing())
				{
					panel_configuration->GetLayouts(); //update layouts when opened
				}
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
					focused_panel = panels[i];

				ImGui::End();
			}
			else if (panels[i]->GetName() == "Toolbar")
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + MENUBAR_SIZE));
				ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, TOOLBAR_SIZE));
				ImGui::SetNextWindowViewport(viewport->ID);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 5.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::Begin(name.c_str(), NULL, panels[i]->flags);
				ImGui::PopStyleVar(3);
				panels[i]->Draw();
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
					focused_panel = panels[i];

				ImGui::End();
			}
 			else if (panels[i]->GetName() == "Assets" || panels[i]->GetName() == "Resources" || panels[i]->GetName() == "Scene")
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				ImGui::Begin(name.c_str(), &panels[i]->active, panels[i]->flags);
				ImGui::PopStyleVar();
				panels[i]->Draw();
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
					focused_panel = panels[i];

				ImGui::End();
			}
			else if (i == 2) // Console
			{
				static char console_name[128];
				if (App->new_logs > 99)
					sprintf_s(console_name, "Console (99+)###Console");
				else if (App->new_logs > 0)
					sprintf_s(console_name, "Console (%d)###Console", App->new_logs);
				else
					sprintf_s(console_name, "Console###Console");

				ImGui::Begin(std::string(panels[i]->GetIcon() + std::string(" ") + console_name).c_str(), &panels[i]->active, panels[i]->flags);
				panels[i]->Draw();
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
				{
					focused_panel = panels[i];
					App->new_logs = 0;
				}
				ImGui::End();
			}
			else // Other Panels
			{
				ImGui::Begin(name.c_str(), &panels[i]->active, panels[i]->flags);
				panels[i]->Draw();
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
					focused_panel = panels[i];
				ImGui::End();
			}

			//if ((*it)->GetName() == "Inspector" && (App->scene->is_creating || App->scene->is_selecting)) //show inspector when a gameobject is created/selected
			//{
			//	ImGui::SetWindowFocus();
			//	p = ImGui::GetWindowPos();
			//	(*it)->pos_x = p.x;
			//	(*it)->pos_y = p.y;
			
			//	App->scene->is_creating = false;
			//	App->scene->is_selecting = false;
			//	focused_panel = *it;
			//}

		}
	}
}

void ModuleEditor::DrawAbout()
{
	if (is_about) //about
	{
		ImGui::OpenPopup("About");
		if (ImGui::BeginPopupModal("About", &is_about, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			//CreateLink("Davos Game Engine", "https://github.com/ponspack9/GameEngine");
			//ImGui::Text("Davos is a game engine developed by two students of CITM:");
			//CreateLink("Oscar Pons", "https://github.com/ponspack9");
			//ImGui::SameLine();
			//ImGui::Text("&");
			//ImGui::SameLine();
			//CreateLink("David Tello", "https://github.com/DavidTello1");
			//ImGui::NewLine();

			ImGui::Text("3rd Party Libraries used:");
			CreateLink("SDL", "http://www.libsdl.org/index.php", true);
			CreateLink("OpenGL", "https://opengl.org/", true);
			CreateLink("Glew", "http://glew.sourceforge.net/", true);
			CreateLink("ImGui", "https://github.com/ocornut/imgui", true);
			CreateLink("DevIL", "http://openil.sourceforge.net/", true);
			ImGui::NewLine();

			//ImGui::Text("License:");
			//ImGui::Text("MIT License");
			//ImGui::Text("Copyright 2019. Oscar Pons and David Tello");
			//ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files(the 'Software'), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions :");
			//ImGui::Text("The above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.");
			//ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.");
			//ImGui::NewLine();

			ImGui::EndPopup();
		}
	}
}

void ModuleEditor::ConfirmExit()
{
	static ImVec2 size = ImVec2(0, 0);
	static float pos = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		is_close = true;

	ImGui::OpenPopup("Quit");
	if (ImGui::BeginPopupModal("Quit", &App->input->quit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		size = ImGui::GetContentRegionAvail();

		ImGui::Text("Are you sure you want to Quit?");
		ImGui::NewLine();

		pos = ImGui::GetCursorPosX();
		if (ImGui::Button("Save", ImVec2(size.x / 3, 22)))
		{
			// TODO: Save
			ImGui::CloseCurrentPopup();

			LOG("Saving Application and Exiting",);
			is_close = true;
		}
		ImGui::SameLine();

		ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		pos = ImGui::GetCursorPosX();
		if (ImGui::Button("Close", ImVec2(size.x / 3, 22)))
		{
			ImGui::CloseCurrentPopup();

			LOG("Exiting Application");
			is_close = true;
		}
		ImGui::SameLine();

		ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		if (ImGui::Button("Cancel", ImVec2(size.x / 3, 22)))
		{
			ImGui::CloseCurrentPopup();
			is_close = false;
			App->input->quit = false;
		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::Shortcuts()
{
	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) || //New
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN))
	{
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) || //Open
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN))
	{
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) || //Save
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN))
	{
	}

	if ((App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP)) //Quit
	{
		App->input->quit = true;
	}
}

Panel* ModuleEditor::GetPanel(uint ID)
{
	for (Panel* panel : panels)
	{
		if (panel->GetID() == ID)
			return panel;
	}
	return nullptr;
}

void ModuleEditor::CreateLink(const char* text, const char* url, bool bullet)
{
	ImVec2 size = ImGui::CalcTextSize(text);
	ImVec2 pos = ImGui::GetCursorPos();
	ImVec4 color;

	if (bullet)
	{
		ImGui::BulletText(text);
		pos.x += 21;
	}
	else
		ImGui::Text(text);
	ImGui::SameLine();

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
	else
		color = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);

	ImGui::SetCursorPos(pos);
	ImGui::TextColored(color, text);
	ImGui::SameLine();

	ImGui::SetCursorPos(pos);

	if (ImGui::InvisibleButton(text, ImVec2(size)))
	{
		ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
	}
}

void ModuleEditor::LogFPS(float fps, float ms)
{
	if (panel_configuration != nullptr)
		panel_configuration->AddFPS(fps, ms);
}
