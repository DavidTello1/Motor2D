#include "Application.h"

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
	modules.push_back(resources = new ModuleResources());
	//modules.push_back(tex = new ModuleTextures());
	modules.push_back(camera = new ModuleCamera3D());
	modules.push_back(scene_base = new ModuleSceneBase());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
	//modules.push_back(audio = new ModuleAudio(true));
	//modules.push_back(ai = new ModuleAI());
	//modules.push_back(level = new ModuleLevelManager());
	//modules.push_back(programs = new ModulePrograms(true));
	//modules.push_back(renderer = new ModuleRenderer());
	modules.push_back(renderer3D = new ModuleRenderer3D());
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
	ReadConfiguration(config.GetSection("App"));

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
const char* Application::GetAppName() const
{
	return app_name.c_str();
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
const char* Application::GetOrganizationName() const
{
	return organization_name.c_str();
}

void Application::SetOrganizationName(const char * name)
{
	if (name != nullptr && name != organization_name)
	{
		organization_name = name;
		// TODO: Filesystem should adjust its writing folder
	}
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
void Application::SetFramerateLimit(uint max_framerate)
{
	if (max_framerate > 0)
		capped_ms = 1000 / max_framerate;
	else
		capped_ms = 0;
}

void Application::ReadConfiguration(const Config& config)
{
	app_name = config.GetString("Name", "Davos Game Engine");
	organization_name = config.GetString("Organization", "");
	SetFramerateLimit(config.GetInt("MaxFramerate", 0));
}

void Application::SaveConfiguration(Config& config) const
{
	config.AddString("Name", app_name.c_str());
	config.AddString("Organization", organization_name.c_str());
	config.AddInt("MaxFramerate", GetFramerateLimit());
}

string Application::GetLog()
{
	return log;
}

void Application::LoadPrefs()
{
	char* buffer = nullptr;
	file_system->Load(SETTINGS_FOLDER "config.json", &buffer);

	if (buffer != nullptr)
	{
		Config config((const char*)buffer);

		if (config.IsValid() == true)
		{
			//LOG("Loading Engine Preferences")

			ReadConfiguration(config.GetSection("App"));

			Config section;
			for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
			{
				section = config.GetSection((*it)->GetName());
				//if (section.IsValid())
				(*it)->Load(&section);
			}
		}
		//else
		//	LOG("Cannot load Engine Preferences: Invalid format")

		RELEASE_ARRAY(buffer);
	}
}

// ---------------------------------------------
void Application::SavePrefs() const
{
	Config config;

	SaveConfiguration(config.AddSection("App"));

	for (list<Module*>::const_iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->Save(&config.AddSection((*it)->GetName()));

	char *buf;
	uint size = config.Save(&buf, "Saved preferences for Davos Game Engine");
	if (App->file_system->Save(SETTINGS_FOLDER "config.json", buf, size) > 0) {}
		//LOG("Saved Engine Preferences")
	RELEASE_ARRAY(buf);
}