#include "Application.h"

#include "MessageBus.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "ModuleRenderer.h"

#include "mmgr/mmgr.h"

Application::Application()
{
	dt = 0.0f;
	frames = 0;
	last_frame_ms = -1;
	last_fps = -1;
	capped_ms = 1000 / 60;
	fps_counter = 0;

	// --- Create Message Bus ---
	message = new MessageBus();

	// --- Create Modules ---
	window = new ModuleWindow();
	input = new ModuleInput();
	filesystem = new ModuleFileSystem();
	resources = new ModuleResources();
	editor = new ModuleEditor();
	renderer = new ModuleRenderer();
}

Application::~Application()
{
	// --- Release Message Bus ---
	RELEASE(message);

	// --- Release Modules ---
	RELEASE(window);
	RELEASE(input);
	RELEASE(filesystem);
	RELEASE(resources);
	RELEASE(editor);
	RELEASE(renderer);
}

bool Application::Init()
{
	// --- Init Modules ---
	return ModulesInit();
}

bool Application::Update()
{
	// --- Prepare Update ---
	PrepareUpdate();

	// --- Modules Update ---
	if (ModulesPreUpdate() == false)  return false;
	if (ModulesUpdate() == false)	  return false;
	if (ModulesPostUpdate() == false) return false;

	// --- Finish Update ---
	FinishUpdate();

	return true;
}

bool Application::CleanUp()
{
	// --- CleanUp Modules ---
	return ModulesCleanUp();
}

void Application::AddLog(char type, const char* string)
{
	if (editor->IsPanelConsole())
		message->Publish(new OnAddLog(type, string));
}

// ---------------------------------------------
bool Application::ModulesInit()
{
	if (window->Init() == false)	 return false; // Window
	if (input->Init() == false)		 return false; // Input
	if (filesystem->Init() == false) return false; // FileSystem
	if (resources->Init() == false)	 return false; // Resources
	if (editor->Init() == false)	 return false; // Editor
	if (renderer->Init() == false)	 return false; // Renderer

	return true;
}

bool Application::ModulesPreUpdate()
{
	if (window->PreUpdate(dt) == false)		return false; // Window
	if (input->PreUpdate(dt) == false)		return false; // Input
	if (filesystem->PreUpdate(dt) == false) return false; // FileSystem
	if (resources->PreUpdate(dt) == false)	return false; // Resources
	if (editor->PreUpdate(dt) == false)		return false; // Editor
	if (renderer->PreUpdate(dt) == false)	return false; // Renderer

	return true;
}

bool Application::ModulesUpdate()
{
	if (window->Update(dt) == false)	 return false; // Window
	if (input->Update(dt) == false)		 return false; // Input
	if (filesystem->Update(dt) == false) return false; // FileSystem
	if (resources->Update(dt) == false)	 return false; // Resources
	if (editor->Update(dt) == false)	 return false; // Editor
	if (renderer->Update(dt) == false)	 return false; // Renderer

	return true;
}

bool Application::ModulesPostUpdate()
{
	if (window->PostUpdate(dt) == false)	 return false; // Window
	if (input->PostUpdate(dt) == false)		 return false; // Input
	if (filesystem->PostUpdate(dt) == false) return false; // FileSystem
	if (resources->PostUpdate(dt) == false)	 return false; // Resources
	if (editor->PostUpdate(dt) == false)	 return false; // Editor
	if (renderer->PostUpdate(dt) == false)	 return false; // Renderer

	return true;
}

bool Application::ModulesCleanUp()
{
	if (window->CleanUp() == false)		return false; // Window
	if (input->CleanUp() == false)		return false; // Input
	if (filesystem->CleanUp() == false) return false; // FileSystem
	if (resources->CleanUp() == false)	return false; // Resources
	if (editor->CleanUp() == false)		return false; // Editor
	if (renderer->CleanUp() == false)	return false; // Renderer

	return true;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

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

	// Cap fps
	if (capped_ms > 0 && (last_frame_ms < capped_ms))
		SDL_Delay(capped_ms - last_frame_ms);
}
