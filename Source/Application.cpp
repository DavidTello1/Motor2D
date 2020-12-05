#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
//#include "ModuleSceneBase.h"
#include "ModuleRenderer.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
//#include "ModuleResources.h"
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
	//modules.push_back(resources = new ModuleResources());
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

	file_system->Load(SETTINGS_FOLDER "config.json", &buffer);

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
	app_name = config.GetString("Name", "Davos Game Engine");
	organization_name = config.GetString("Organization", "");
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
void Application::LoadPrefs(bool default)
{
	char* buffer = nullptr;
	if (default)
		file_system->Load(SETTINGS_FOLDER "default_config.json", &buffer);
	else
		file_system->Load(SETTINGS_FOLDER "config.json", &buffer);

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
		}

		RELEASE_ARRAY(buffer);
	}
}

// ---------------------------------------------
void Application::SavePrefs() const
{
	Config config;

	SaveConfig(config.AddSection("App"));

	for (Module* mod : modules)
		mod->Save(&config.AddSection(mod->GetName()));

	char *buf;
	uint size = config.Save(&buf, "Saved engine configuration");
	if (App->file_system->Save(SETTINGS_FOLDER "config.json", buf, size) > 0) {}
	RELEASE_ARRAY(buf);
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