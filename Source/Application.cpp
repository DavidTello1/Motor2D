#include "Application.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
//#include "ModuleScene.h"
//#include "ModuleSceneBase.h"
#include "ModuleRenderer.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
//#include "ModuleResources.h"
#include "Config.h"

#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include "mmgr/mmgr.h"

using namespace std;

Application::Application()
{
	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 1000 / 60;
	fps_counter = 0;
	random = new math::LCG();

	modules.push_back(file_system = new ModuleFileSystem(ASSETS_FOLDER));
	modules.push_back(window = new ModuleWindow());
	//modules.push_back(resources = new ModuleResources());
	//modules.push_back(tex = new ModuleTextures());
	//modules.push_back(camera = new ModuleCamera3D());
	//modules.push_back(scene_base = new ModuleSceneBase());
	//modules.push_back(scene = new ModuleScene());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
	//modules.push_back(audio = new ModuleAudio(true));
	//modules.push_back(ai = new ModuleAI());
	//modules.push_back(level = new ModuleLevelManager());
	//modules.push_back(programs = new ModulePrograms(true));
	modules.push_back(renderer = new ModuleRenderer());
	//modules.push_back(renderer3D = new ModuleRenderer3D());
}

// ---------------------------------------------
Application::~Application()
{
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		RELEASE(*it);
	
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
	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		ret = (*it)->Init(&(config.GetSection((*it)->GetName())));
	}

	// Another round, just before starting the Updates. Only called for "active" modules
	// we send the configuration again in case a module needs it
	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if ((*it)->IsActive() == true)
			ret = (*it)->Start(&(config.GetSection((*it)->GetName())));
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

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->PreUpdate(dt);

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->Update(dt);

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->PostUpdate(dt);

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

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		if ((*it)->IsActive() == true)
			ret = (*it)->CleanUp();

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
			for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
			{
				section = config.GetSection((*it)->GetName());
				//if (section.IsValid())
				(*it)->Load(&section);
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

	for (list<Module*>::const_iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->Save(&config.AddSection((*it)->GetName()));

	char *buf;
	uint size = config.Save(&buf, "Saved engine configuration");
	if (App->file_system->Save(SETTINGS_FOLDER "config.json", buf, size) > 0) {}
	RELEASE_ARRAY(buf);
}