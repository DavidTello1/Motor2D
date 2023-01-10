#include "ModuleInput.h"

#include "Application.h"
#include "MessageBus.h"

#include "SDL\include\SDL.h"
#include "imGui/imgui_impl_sdl.h"

#include "mmgr/mmgr.h"

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
	quit = false;
}

ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

bool ModuleInput::Init()
{
	LOG("Init SDL input event system", 'i');

	SDL_Init(0);
	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}

	return true;
}

bool ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();
	mouse_motion_x = mouse_motion_y = 0;

	// --- Check keyboard & mouse input
	GetKeyboardInput();
	GetMouseInput();

	// --- Mouse events
	MouseEvents();

	return true;
}

bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem", 'i');

	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	return true;
}

// ---------------------------------------------
void ModuleInput::GetKeyboardInput()
{
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
			keyboard[i] = (keyboard[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keyboard[i] = (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}
}

void ModuleInput::GetMouseInput()
{
	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
			mouse_buttons[i] = (mouse_buttons[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			mouse_buttons[i] = (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}
}

void ModuleInput::MouseEvents()
{
	SDL_Event e;
	static const char* dir;
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[e.button.button - 1] = KEY_DOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[e.button.button - 1] = KEY_UP;
			break;

		case SDL_MOUSEMOTION:
			mouse_motion_x = e.motion.xrel;
			mouse_motion_y = e.motion.yrel;
			mouse_x = e.motion.x;
			mouse_y = e.motion.y;
			break;

		case SDL_MOUSEWHEEL:
			mouse_wheel = e.wheel.y;
			break;

		case SDL_DROPFILE:
			//dir = e.drop.file;
			//App->resources->ImportFromExplorer(dir);
			//SDL_free(e.drop.file);
			break;

		case SDL_QUIT:
			App->message->Publish(new OnCloseEngine());
			break;

		case SDL_WINDOWEVENT:
			//static bool flag = false;
			//if (e.window.event == SDL_WINDOWEVENT_MAXIMIZED && App->window->IsMaximized() == false)
			//{
			//	App->window->SetMaximized(true);
			//	flag = true;
			//}
			//else if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			//{
			//	if (SDL_GetWindowFlags(App->window->GetWindow()) & SDL_WINDOW_MAXIMIZED)
			//	{
			//		if (flag)
			//			flag = false;
			//		else
			//		{
			//			if (App->window->IsMaximized() == true)
			//				App->window->SetMaximized(false);
			//		}
			//	}
			//	else
			//	{
			//		App->window->UpdateSize(e.window.data1, e.window.data2);
			//		if (App->window->IsMaximized() == true)	
			//			App->window->SetMaximized(false);
			//	}
			//}
			break;
		}
	}
}
