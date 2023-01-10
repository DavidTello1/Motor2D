#pragma once
#include "Globals.h"
#include "Timer.h"

//#include "PCG/pcg_basic.h"
#include <string>
#include <vector>

#define MAX_LOG_SIZE 1000

struct Log {
	std::string icon, time, message;
};

class Module;
class ModuleWindow;
class ModuleInput;
//class ModuleSceneBase;
class ModuleScene;
class ModuleRenderer;
class ModuleEditor;
class ModuleFileSystem;
class ModuleResources;

class Config;

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

	bool LoadPrefs(const char* name);
	bool SavePrefs(const char* name) const;

	void AddLog(const char* icon, const char* time, const char* message);
	void ClearLog();
	std::vector<Log> GetLog() { return logs; }
	int GetFPS() { return last_fps; }

	UID GenerateUID();

private:
	void PrepareUpdate();
	void FinishUpdate();

public:
	ModuleWindow* window;
	ModuleInput* input;
	//ModuleSceneBase* scene_base;
	ModuleScene* scene;
	ModuleRenderer* renderer;
	//ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleFileSystem* file_system;
	ModuleResources* resources;

	//pcg32_random_t rng1, rng2; 
	uint new_logs;

private:
	Timer	ms_timer;
	Timer	fps_timer;
	Uint32	frames;
	float	dt;
	int		fps_counter;
	int		last_frame_ms;
	int		last_fps;
	int		capped_ms;

	std::vector<Module*> modules;
	std::vector<Log> logs;

	std::string app_name;
	std::string organization_name;
};

extern Application* App;
