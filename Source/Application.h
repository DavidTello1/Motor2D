#pragma once

#include "Globals.h"
#include "Timer.h"

class MessageBus;

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleFileSystem;
class ModuleEditor;
class ModuleRenderer;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Update();
	bool CleanUp();

	// --- Framerate ---
	const int GetFPS() const { return last_fps; }
	const uint GetFramerateLimit() const { return (capped_ms > 0) ? (uint)((1.0f / (float)capped_ms) * 1000.0f) : 0; }
	void SetFramerateLimit(uint max_framerate) { capped_ms = (max_framerate > 0) ? 1000 / max_framerate : 0; }

	// --- Logs ---
	void AddLog(char type, const char* message);

private:
	bool ModulesInit();
	bool ModulesPreUpdate();
	bool ModulesUpdate();
	bool ModulesPostUpdate();
	bool ModulesCleanUp();

	void PrepareUpdate();
	void FinishUpdate();

// ---------------------------------------------
public:
	MessageBus* message;

	ModuleWindow* window;
	ModuleInput* input;
	ModuleFileSystem* filesystem;
	ModuleEditor* editor;
	ModuleRenderer* renderer;

private:
	Timer ms_timer;
	Timer fps_timer;

	float dt;
	uint32 frames;
	int fps_counter;
	int last_fps;
	int last_frame_ms;
	int capped_ms;
};

extern Application* App;
