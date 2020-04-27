#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleRenderer.h"
//#include "ModuleScene.h"
#include "Config.h"

#include "Configuration.h"
#include "Console.h"
#include "Hierarchy.h"
//#include "Inspector.h"
//#include "Assets.h"
//#include "Viewport.h"

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

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->context);
	LOG("Loading ImGui");
	ImGui_ImplOpenGL3_Init();

	// Set style
	Load(config);

	// Create panels
	panels.push_back(panel_configuration = new Configuration());
	panels.push_back(panel_console = new Console());
	panels.push_back(panel_hierarchy = new Hierarchy());
	//panels.push_back(panel_inspector = new Inspector());
	//panels.push_back(panel_assets = new Assets());
	//panels.push_back(panel_viewport = new Viewport());

	return true;
}

bool ModuleEditor::Start(Config* config)
{
	//panel_viewport->GenerateFBO();
	return true;
}

bool ModuleEditor::PreUpdate(float dt)
{
	//// Start the frame
	//panel_viewport->PreUpdate();
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

	if (close)
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
	config->AddUInt("Style", style);
}

void ModuleEditor::Load(Config* config)
{
	style = config->GetUInt("Style", Style::DARK);
	ChangeStyle(style);
}

// Drawing of the FULL gui (first gets drawn the Menus, then panels)
void ModuleEditor::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();

	// Draw functions
	DockSpace();
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
			ImGui::MenuItem("Configuration", NULL, &GetPanel("Configuration")->active);
			ImGui::Separator();
			ImGui::MenuItem("Console", NULL, &GetPanel("Console")->active);
			ImGui::MenuItem("Hierarchy", NULL, &GetPanel("Hierarchy")->active);
			//ImGui::MenuItem("Inspector", NULL, &GetPanel("Inspector")->active);
			//ImGui::MenuItem("Assets", NULL, &GetPanel("Assets")->active);

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
	for (std::vector<Panel*>::const_iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->IsActive())
		{
			ImGui::SetNextWindowPos(ImVec2((float)(*it)->pos_x, (float)(*it)->pos_y), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2((float)(*it)->width, (float)(*it)->height), ImGuiCond_FirstUseEver);

			// Configuration
			if ((*it)->GetName() == "Configuration")
			{
				if (ImGui::Begin((*it)->GetName(), NULL, (*it)->flags))
				{
					(*it)->Draw();
					ImGui::End();
				}
			}
			else // Other Panels
			{
				// Draw Panel
				if (ImGui::Begin((*it)->GetName(), &(*it)->active, (*it)->flags))
				{
					(*it)->Draw();
					ImGui::End();
				}
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
		close = true;

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
			close = true;
		}
		ImGui::SameLine();

		ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		pos = ImGui::GetCursorPosX();
		if (ImGui::Button("Close", ImVec2(size.x / 3, 22)))
		{
			ImGui::CloseCurrentPopup();

			LOG("Exiting Application");
			close = true;
		}
		ImGui::SameLine();

		ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		if (ImGui::Button("Cancel", ImVec2(size.x / 3, 22)))
		{
			ImGui::CloseCurrentPopup();
			close = false;
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

Panel* ModuleEditor::GetPanel(const char* name)
{
	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->GetName() == name)
			return (*it);
	}
	return nullptr;
}

void ModuleEditor::ChangeStyle(uint new_style)
{
	style = new_style;

	if (style == Style::BLACK)
		ImGui::StyleColorsNew();

	else if (style == Style::CLASSIC)
		ImGui::StyleColorsClassic();

	else if (style == Style::DARK)
		ImGui::StyleColorsDark();
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

void ModuleEditor::DockSpace()
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", 0, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();

	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

	ImGui::End();
}
