#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
//#include "ModuleSceneBase.h"
#include "ModuleRenderer.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "Config.h"

#include <windows.h>
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include "mmgr/mmgr.h"

using namespace std;

Application::Application()
{
	new_logs = 0;
	dt = 0.0f;
	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 1000 / 60;
	fps_counter = 0;
	random = new math::LCG();

	logs.reserve(MAX_LOG_SIZE + 1);

	modules.push_back(file_system = new ModuleFileSystem(ASSETS_FOLDER));
	modules.push_back(window = new ModuleWindow());
	modules.push_back(input = new ModuleInput());
	modules.push_back(resources = new ModuleResources());
	//modules.push_back(tex = new ModuleTextures());
	//modules.push_back(camera = new ModuleCamera3D());
	//modules.push_back(scene_base = new ModuleSceneBase());
	modules.push_back(scene = new ModuleScene());
	//modules.push_back(audio = new ModuleAudio(true));
	//modules.push_back(ai = new ModuleAI());
	//modules.push_back(level = new ModuleLevelManager());
	//modules.push_back(programs = new ModulePrograms(true));
	modules.push_back(renderer = new ModuleRenderer());
	modules.push_back(editor = new ModuleEditor());
}

// ---------------------------------------------
Application::~Application()
{
	for (Module* mod : modules)
		RELEASE(mod);
	
	RELEASE(random);
}

// ---------------------------------------------
bool Application::Init()
{
	bool ret = true;
	char* buffer = nullptr;
	bool config_exists = true;

	// Load Config
	if (file_system->Load(SETTINGS_FOLDER "Default.json", &buffer) == 0)
		config_exists = false;

	Config config((const char*)buffer);
	ReadConfig(config.GetSection("App"));

	// We init everything, even if not enabled
	for (Module* mod : modules)
		ret = mod->Init(&(config.GetSection(mod->GetName())));

	// Another round, just before starting the Updates. Only called for "active" modules
	// we send the configuration again in case a module needs it
	for (Module* mod : modules)
	{
		if (mod->IsActive() == true)
			ret = mod->Start(&(config.GetSection(mod->GetName())));
	}

	if (!config_exists)
	{
		SavePrefs("Default");

		if (!App->file_system->Exists("imgui.ini"))
		{
			// Save ini file
			std::string ini = "[Window][DockSpace]\nPos=0,61\nSize=1108,587\nCollapsed=0\n\n[Window][Console]\nPos=0,560\nSize=1400,220\nCollapsed=0\nDockId=0x00000003,0\n\n[Window][Hierarchy]\nPos=0,19\nSize=258,539\nCollapsed=0\nDockId=0x00000004,0\n\n[Window][Debug##Default]\nPos=60,60\nSize=400,400\nCollapsed=0\n\n[Window][Dear ImGui Metrics]\nPos=60,60\nSize=338,239\nCollapsed=0\n\n[Window][Dear ImGui Demo]\nPos=650,20\nSize=550,680\nCollapsed=0\n\n[Window][Configuration]\nPos=360,100\nSize=650,500\nCollapsed=0\n\n[Window][ï„œConsole]\nPos=0,560\nSize=1400,220\nCollapsed=0\n\n[Window][î €Configuration]\nPos=360,100\nSize=650,500\nCollapsed=0\n\n[Window][ïƒ‰Hierarchy]\nPos=0,20\nSize=265,540\nCollapsed=0\n\n[Window][ï„œ Console]\nPos=0,560\nSize=1400,220\nCollapsed=0\n\n[Window][ïƒ‰ Hierarchy]\nPos=0,20\nSize=265,540\nCollapsed=0\n\n[Window][î € Configuration]\nPos=360,100\nSize=650,500\nCollapsed=0\n\n[Window][ï€ Console]\nPos=0,566\nSize=1400,214\nCollapsed=0\nDockId=0x578CAA54,0\n\n[Window][ï€€ Configuration]\nPos=67,90\nSize=650,500\nCollapsed=0\n\n[Window][ï€‚ Hierarchy]\nPos=0,61\nSize=224,393\nCollapsed=0\nDockId=0x00000006,0\n\n[Window][ï€… Assets]\nPos=0,456\nSize=1108,192\nCollapsed=0\nDockId=0x00000002,1\n\n[Window][Assets_Hierarchy]\nPos=0,475\nSize=248,173\nCollapsed=0\nDockId=0x00000009\n\n[Window][Assets_Icons]\nPos=250,475\nSize=858,173\nCollapsed=0\nDockId=0x0000000A\n\n[Window][###Console]\nPos=0,456\nSize=1108,192\nCollapsed=0\nDockId=0x00000002,0\n\n[Window][ï‚ Toolbar]\nPos=57,156\nSize=1152,79\nCollapsed=0\n\n[Window][Toolbar]\nSize=1152,62\nCollapsed=0\nDockId=0x0000000B\n\n[Docking][Data]\nDockSpace     ID=0x006DCF07 Window=0x58EF55FC Pos=0,475 Size=1108,173 Split=X\n  DockNode    ID=0x00000009 Parent=0x006DCF07 SizeRef=248,179 NoTabBar=1 Selected=0x684F8E27\n  DockNode    ID=0x0000000A Parent=0x006DCF07 SizeRef=1134,179 CentralNode=1 NoTabBar=1 Selected=0xB99294EB\nDockSpace     ID=0x578CAA54 Pos=0,0 Size=1152,648 Split=Y\n  DockNode    ID=0x00000001 Parent=0x578CAA54 SizeRef=1400,539 Split=X\n    DockNode  ID=0x00000004 Parent=0x00000001 SizeRef=258,539 Selected=0x788BAA0D\n    DockNode  ID=0x00000005 Parent=0x00000001 SizeRef=1140,539 CentralNode=1\n  DockNode    ID=0x00000003 Parent=0x578CAA54 SizeRef=1400,220 Selected=0xF9BEF62A\nDockSpace     ID=0xC82E6A9B Window=0x9A404470 Pos=0,61 Size=1108,587 Split=Y\n  DockNode    ID=0x00000008 Parent=0xC82E6A9B SizeRef=1152,389 Split=X\n    DockNode  ID=0x00000006 Parent=0x00000008 SizeRef=224,581 Selected=0x18083233\n    DockNode  ID=0x00000007 Parent=0x00000008 SizeRef=926,581 CentralNode=1\n  DockNode    ID=0x00000002 Parent=0xC82E6A9B SizeRef=1152,190 Selected=0x58EF55FC\nDockSpace     ID=0xEC501347 Pos=0,0 Size=1152,648 Split=Y\n  DockNode    ID=0x0000000B Parent=0xEC501347 SizeRef=1152,62 NoCloseButton=1\n  DockNode    ID=0x0000000C Parent=0xEC501347 SizeRef=1152,565 CentralNode=1\n\n";
			uint ini_size = 2750;
			App->file_system->Save("imgui.ini", ini.data(), ini_size);

			// Update config file
			file_system->Load(SETTINGS_FOLDER "Default.json", &buffer);
			Config config((const char*)buffer);
			config.GetSection(editor->GetName()).AddString("ini", ini.c_str());
			config.GetSection(editor->GetName()).AddUInt("ini_size", ini_size);

			// Save updated config file
			char* buf;
			uint size = config.Save(&buf, "Saved engine configuration");
			std::string filename = SETTINGS_FOLDER + std::string("Default.json");
			ret = App->file_system->Save(filename.c_str(), buf, size) > 0;
			RELEASE_ARRAY(buf);

			//// Load ini settings
			//LoadLayoutDefault(); //default ini configuration (set from code)
		}
	}

	RELEASE_ARRAY(buffer);
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
bool Application::Update()
{
	bool ret = true;
	PrepareUpdate();

	for (Module* mod : modules)
		if (mod->IsActive() == true)
			ret = mod->PreUpdate(dt);

	for (Module* mod : modules)
		if (mod->IsActive() == true)
			ret = mod->Update(dt);

	for (Module* mod : modules)
		if (mod->IsActive() == true)
			ret = mod->PostUpdate(dt);

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	// Recap on framecount and fps
	++frames;
	++fps_counter;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = fps_counter;
		fps_counter = 0;
		fps_timer.Start();
	}

	last_frame_ms = ms_timer.Read();

	// cap fps
	if (capped_ms > 0 && (last_frame_ms < capped_ms))
		SDL_Delay(capped_ms - last_frame_ms);

	// notify the editor
	editor->LogFPS((float)last_fps, (float)last_frame_ms);
}

// ---------------------------------------------
bool Application::CleanUp()
{
	bool ret = true;

	for (uint i = 0; i < modules.size(); i++)
		if (modules[i]->IsActive() == true)
			ret = modules[i]->CleanUp(); 

	return ret;
}

// ---------------------------------------------
uint Application::GetFramerateLimit() const
{
	if (capped_ms > 0)
		return (uint)((1.0f / (float)capped_ms) * 1000.0f);
	else
		return 0;
}

// ---------------------------------------------
void Application::SetAppName(const char * name)
{
	if (name != nullptr && name != app_name)
	{
		app_name = name;
		window->SetTitle(name);
		// TODO: Filesystem should adjust its writing folder
	}
}

// ---------------------------------------------
void Application::SetOrganizationName(const char * name)
{
	if (name != nullptr && name != organization_name)
	{
		organization_name = name;
		// TODO: Filesystem should adjust its writing folder
	}
}

// ---------------------------------------------
void Application::SetFramerateLimit(uint max_framerate)
{
	if (max_framerate > 0)
		capped_ms = 1000 / max_framerate;
	else
		capped_ms = 0;
}

// ---------------------------------------------
void Application::ReadConfig(const Config& config)
{
	SetAppName(config.GetString("Name", "Davos Game Engine"));
	SetOrganizationName(config.GetString("Organization", ""));
	SetFramerateLimit(config.GetInt("MaxFramerate", 0));
}

// ---------------------------------------------
void Application::SaveConfig(Config& config) const
{
	config.AddString("Name", app_name.c_str());
	config.AddString("Organization", organization_name.c_str());
	config.AddInt("MaxFramerate", GetFramerateLimit());
}

// ---------------------------------------------
bool Application::LoadPrefs(const char* name)
{
	bool ret = false;
	char* buffer = nullptr;
	std::string filename = SETTINGS_FOLDER + std::string(name) + ".json";
	file_system->Load(filename.c_str(), &buffer);

	if (buffer != nullptr)
	{
		Config config((const char*)buffer);

		if (config.IsValid() == true)
		{
			ReadConfig(config.GetSection("App"));

			Config section;
			for (Module* mod : modules)
			{
				section = config.GetSection(mod->GetName());
				//if (section.IsValid())
				mod->Load(&section);
			}
			ret = true;
		}
		RELEASE_ARRAY(buffer);
	}
	return ret;
}

// ---------------------------------------------
bool Application::SavePrefs(const char* name) const
{
	bool ret = false;
	Config config;
	SaveConfig(config.AddSection("App"));

	for (Module* mod : modules)
		mod->Save(&config.AddSection(mod->GetName()));

	char *buf;
	uint size = config.Save(&buf, "Saved engine configuration");
	std::string filename = SETTINGS_FOLDER + std::string(name) + ".json";
	if (App->file_system->Save(filename.c_str(), buf, size) > 0)
		ret = true;

	RELEASE_ARRAY(buf);
	return ret;
}

// ---------------------------------------------
void Application::AddLog(const char* icon, const char* time, const char* message)
{
	if (logs.size() > MAX_LOG_SIZE)
		logs.erase(logs.begin());

	logs.push_back({ icon, time, message });

	new_logs++;
}

void Application::ClearLog()
{
	logs.erase(logs.begin(), logs.end());
	logs.clear();

	new_logs = 0;
}