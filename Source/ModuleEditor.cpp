#include "Application.h"
#include "Config.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleRenderer.h"
//#include "ModuleScene.h"

//#include "Console.h"
//#include "Hierarchy.h"
//#include "Inspector.h"
//#include "Configuration.h"
//#include "Assets.h"
//#include "Viewport.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <windows.h>
#include <string.h>
#include <algorithm>
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
	LOG("Init editor gui with imgui lib version %s", ImGui::GetVersion(), 'd');

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->context);
	LOG("Loading ImGui", 'd');
	ImGui_ImplOpenGL3_Init();

	// Set style
	Load(config);
	if (style == Style::NEW)
		ImGui::StyleColorsNew();

	else if (style == Style::DARK)
		ImGui::StyleColorsDark();

	else if (style == Style::CLASSIC)
		ImGui::StyleColorsClassic();

	else if (style == Style::LIGHT)
		ImGui::StyleColorsLight();

	//// Create panels
	//panels.push_back(tab_hierarchy = new Hierarchy());
	//panels.push_back(tab_inspector = new Inspector());
	//panels.push_back(tab_configuration = new Configuration());
	//panels.push_back(tab_console = new Console());
	//panels.push_back(tab_assets = new Assets());
	////panels.push_back(tab_viewport = new Viewport());

	return true;
}

bool ModuleEditor::Start(Config* config)
{
	//tab_viewport->GenerateFBO();
	return true;
}

bool ModuleEditor::PreUpdate(float dt)
{
	//// Start the frame
	//tab_viewport->PreUpdate();
	return true;
}

bool ModuleEditor::Update(float dt)
{
	return true;
}

bool ModuleEditor::PostUpdate(float dt)
{
	//// End the frame
	//tab_viewport->PostUpdate();

	if (close)
		return false;

	return true;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Freeing editor gui", 'd');

	//for (vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	//{
	//	RELEASE(*it);
	//}
	//panels.clear();

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
	style = config->GetUInt("Style", Style::NEW);
}

// Drawing of the FULL gui (first gets drawn the Menus, then panels)
void ModuleEditor::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();

	// Draw functions
	DockSpace();
	DrawMenu();
	DrawDemo();
	DrawAbout();
	DrawPanels();

	// Menu Functionalities
	if (is_new)
	{
		//...
		is_new = false;
	}
	if (is_open)
	{
		//...
		is_open = false;
	}
	if (is_save) //save
	{
		//uint id = 0;
		//std::string path = App->scene->scene_name;
		//path = ASSETS_FOLDER + path;
		//std::string written_file;

		//ResourceScene* scene = (ResourceScene*)App->resources->CreateInitResource(Resource::Type::scene, id, path.c_str(), written_file);
		//scene->SaveOwnFormat(written_file);
		is_save = false;
	}
	if (is_import)
	{
		//...
		is_import = false;
	}

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

void ModuleEditor::DrawMenu()
{
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File")) //file
		{
			if (ImGui::MenuItem("New", "Ctrl+N", false, false))
				is_new = true;

			if (ImGui::MenuItem("Open", "Ctrl+O", false, false))
				is_open = true;

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				is_save = true;

			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "ESC"))
				App->input->quit = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) //edit
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false))
			{
			}

			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false))
			{
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Import", NULL, false, false))
			{
			}

			if (ImGui::MenuItem("Export", NULL, false, false))
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
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) //view
		{
			//if (ImGui::MenuItem("Show Plane", NULL, &is_plane))
			//	is_show_plane = !is_show_plane;

			//if (ImGui::MenuItem("Show Axis", NULL, &is_axis))
			//	is_show_axis = !is_show_axis;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options")) //options
		{
			if (ImGui::BeginMenu("Style"))
			{
				if (ImGui::MenuItem("New", NULL, style == Style::NEW))
				{
					ImGui::StyleColorsNew();
					style = Style::NEW;
				}

				if (ImGui::MenuItem("Classic", NULL, style == Style::CLASSIC))
				{
					ImGui::StyleColorsClassic();
					style = Style::CLASSIC;
				}

				if (ImGui::MenuItem("Dark", NULL, style == Style::DARK))
				{
					ImGui::StyleColorsDark();
					style = Style::DARK;
				}

				if (ImGui::MenuItem("Light", NULL, style == Style::LIGHT))
				{
					ImGui::StyleColorsLight();
					style = Style::LIGHT;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) //windows
		{
			//ImGui::MenuItem("Hierarchy", NULL, &GetPanel("Hierarchy")->active);
			//ImGui::MenuItem("Configuration", NULL, &GetPanel("Configuration")->active);
			//ImGui::MenuItem("Inspector", NULL, &GetPanel("Inspector")->active);
			//ImGui::MenuItem("Console", NULL, &GetPanel("Console")->active);
			//ImGui::MenuItem("Assets", NULL, &GetPanel("Assets")->active);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) //help
		{
			ImGui::MenuItem("ImGui Demo", NULL, &is_show_demo);
			ImGui::Separator();

			if (ImGui::MenuItem("Github"))
				ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Documentation"))
				ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/wiki", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Latest Version"))
				ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/releases", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Report a bug"))
				ShellExecuteA(NULL, "open", "https://github.com/ponspack9/GameEngine/issues", NULL, NULL, SW_SHOWNORMAL);

			ImGui::Separator();
			if (ImGui::MenuItem("About"))
				is_about = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ModuleEditor::DrawDemo()
{
	if (is_show_demo) //show demo
	{
		ImGui::ShowDemoWindow(&is_show_demo);
		ImGui::ShowMetricsWindow();
	}
}

void ModuleEditor::DrawAbout()
{
	if (is_about) //about
	{
		ImGui::OpenPopup("About");
		if (ImGui::BeginPopupModal("About", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
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

			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
				is_about = false;
			}
			ImGui::EndPopup();
		}
	}
}

void ModuleEditor::DrawPanels()
{
	//for (vector<Panel*>::const_iterator it = panels.begin(); it != panels.end(); ++it)
	//{
	//	if ((*it)->IsActive())
	//	{
	//		ImGui::SetNextWindowPos(ImVec2((float)(*it)->pos_x, (float)(*it)->pos_y), ImGuiCond_FirstUseEver);
	//		ImGui::SetNextWindowSize(ImVec2((float)(*it)->width, (float)(*it)->height), ImGuiCond_FirstUseEver);
	//		ImVec2 p;

	//		if ((*it)->has_menubar) //panel has a menu bar
	//		{
	//			if (ImGui::Begin((*it)->GetName(), &(*it)->active, ImGuiWindowFlags_MenuBar))
	//			{
	//				(*it)->Draw();
	//				p = ImGui::GetWindowPos();
	//				(*it)->pos_x = p.x;
	//				(*it)->pos_y = p.y;

	//				if (ImGui::IsWindowHovered())
	//					focused_panel = *it;
	//			}
	//		}
	//		else
	//		{
	//			if (ImGui::Begin((*it)->GetName(), &(*it)->active))
	//			{
	//				(*it)->Draw();
	//				p = ImGui::GetWindowPos();
	//				(*it)->pos_x = p.x;
	//				(*it)->pos_y = p.y;

	//				if (ImGui::IsWindowHovered())
	//					focused_panel = *it;
	//			}
	//		}

	//		if ((*it)->GetName() == "Inspector" && (App->scene->is_creating || App->scene->is_selecting)) //show inspector when a gameobject is created/selected
	//		{
	//			ImGui::SetWindowFocus();
	//			p = ImGui::GetWindowPos();
	//			(*it)->pos_x = p.x;
	//			(*it)->pos_y = p.y;

	//			App->scene->is_creating = false;
	//			App->scene->is_selecting = false;
	//			focused_panel = *it;
	//		}
	//		ImGui::End();
	//	}
	//}
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

		//pos = ImGui::GetCursorPosX();
		//if (ImGui::Button("Save", ImVec2(size.x / 3, 20)))
		//{
		//	//Save
		//	ImGui::CloseCurrentPopup();

		//	LOG("SAVING APPLICATION AND EXITING", 'd');
		//	close = true;
		//}
		//ImGui::SameLine();

		//ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		pos = ImGui::GetCursorPosX();
		if (ImGui::Button("Close", ImVec2(size.x / 2, 20)))
		{
			ImGui::CloseCurrentPopup();

			LOG("EXITING APPLICATION", 'd');
			close = true;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
		if (ImGui::Button("Cancel", ImVec2(size.x / 2, 20)))
		{
			ImGui::CloseCurrentPopup();
			LOG("CANCEL EXIT", 'd');
			close = false;
			App->input->quit = false;
		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::Shortcuts()
{
	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) ||
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN))
	{
		is_new = true;
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) ||
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN))
	{
		is_open = true;
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) ||
		(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN))
	{
		is_save = true;
	}

	if ((App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP))
	{
		App->input->quit = true;
	}
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

//Panel* ModuleEditor::GetPanel(const char* name)
//{
//	for (vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
//	{
//		if ((*it)->GetName() == name)
//			return (*it);
//	}
//	return nullptr;
//}

void ModuleEditor::LogFPS(float fps, float ms)
{
	//if (tab_configuration != nullptr)
	//	tab_configuration->AddFPS(fps, ms);
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
