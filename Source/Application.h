#pragma once
#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/include/MathGeoLibFwd.h"
#include <list>

class Module;
class ModuleWindow;
class ModuleInput;
//class ModuleSceneBase;
class ModuleScene;
class ModuleRenderer;
class ModuleEditor;
class ModuleFileSystem;
//class ModuleResources;

class Config;
class LCG;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Update();
	bool CleanUp();

	const char* GetAppName() const { return app_name.c_str(); }
	const char* GetOrganizationName() const { return organization_name.c_str(); }
	uint GetFramerateLimit() const;

	void SetAppName(const char* name);
	void SetOrganizationName(const char* name);
	void SetFramerateLimit(uint max_framerate);

	void ReadConfig(const Config& config);
	void SaveConfig(Config& config) const;

	void LoadPrefs(bool default = false);
	void SavePrefs() const;

	std::string GetLog() { return log; }
	math::LCG& Random() { return *random; }

private:
	void PrepareUpdate();
	void FinishUpdate();

public:
	math::LCG* random = nullptr;

	ModuleWindow* window;
	ModuleInput* input;
	//ModuleSceneBase* scene_base;
	ModuleScene* scene;
	ModuleRenderer* renderer;
	//ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleFileSystem* file_system;
	//ModuleResources* resources;

private:
	Timer	ms_timer;
	Timer	fps_timer;
	Uint32	frames;
	float	dt;
	int		fps_counter;
	int		last_frame_ms;
	int		last_fps;
	int		capped_ms;

	std::list<Module*> modules;
	std::string log;
	std::string app_name;
	std::string organization_name;
};

extern Application* App;
