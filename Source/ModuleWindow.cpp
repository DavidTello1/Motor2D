#include "ModuleWindow.h"

#include "mmgr/mmgr.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface", 'i');

	// --- Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}

	// --- Use OpenGL 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// --- Create window
	SDL_DisplayMode DM;
	SDL_GetDesktopDisplayMode(0, &DM);
	screen_width = int(DM.w * 0.75f);
	screen_height = int(DM.h * 0.75f);
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, flags);
	if (window == NULL)
	{
		LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}
	
	// --- Get Window Surface
	screen_surface = SDL_GetWindowSurface(window);

	return true;
}

bool ModuleWindow::Start()
{
	// --- Refresh Aspect Ratios	
	SDL_SetWindowSize(window, screen_width, screen_height);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems", 'i');

	// --- Destroy Window
	if (window != nullptr)
		SDL_DestroyWindow(window);

	// --- Quit SDL Subsystems
	SDL_Quit();

	return true;
}

uint ModuleWindow::GetRefreshRate() const
{
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError(), 'e');
		return 0;
	}
	return (uint)dm.refresh_rate;
}

void ModuleWindow::GetMaxMinSize(uint& min_width, uint& min_height, uint& max_width, uint& max_height) const
{
	min_width = MIN_SCREEN_WIDTH;
	min_height = MIN_SCREEN_HEIGHT;
	max_width = MAX_SCREEN_WIDTH;
	max_height = MAX_SCREEN_HEIGHT;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError(), 'e');
		return;
	}

	max_height = dm.h;
	max_width = dm.w;
}

void ModuleWindow::SetFullscreen(bool set)
{
	if (set == fullscreen)
		return;

	fullscreen = set;
	if (fullscreen == true)
	{
		if (SDL_SetWindowFullscreen(window, 0) != 0)
			LOG("Could not switch to windowed: %s\n", SDL_GetError(), 'e');
		return;
	}

	if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
		LOG("Could not switch to fullscreen: %s\n", SDL_GetError(), 'e');

	fullscreen_desktop = false;
}

void ModuleWindow::SetResizable(bool set)
{
	// cannot be changed while the program is running, but we can save the change
	resizable = set;
}

void ModuleWindow::SetBorderless(bool set)
{
	if (set == borderless || fullscreen == true || fullscreen_desktop == true)
		return;

	borderless = set;
	SDL_SetWindowBordered(window, (SDL_bool)!borderless);
}

void ModuleWindow::SetFullScreenDesktop(bool set)
{
	if (set == fullscreen_desktop)
		return;

	fullscreen_desktop = set;
	if (fullscreen_desktop == false)
	{
		if (SDL_SetWindowFullscreen(window, 0) != 0)
			LOG("Could not switch to windowed: %s\n", SDL_GetError(), 'e');

		return;
	}

	if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
		LOG("Could not switch to fullscreen desktop: %s\n", SDL_GetError(), 'e');

	fullscreen = false;
}

void ModuleWindow::SetMaximized(bool set)
{
	if (set == maximized)
		return;

	maximized = set;
	if (maximized == false)
	{
		SDL_RestoreWindow(window);
		return;
	}

	SDL_MaximizeWindow(window);
}

//void ModuleWindow::SetIcon(const char * file)
//{
//	if (file != nullptr && file != icon_file)
//	{
//		icon_file = file;
//
//		SDL_Surface *surface = SDL_LoadBMP_RW(App->file_system->Load(file), 1);
//		SDL_SetWindowIcon(window, surface);
//		SDL_FreeSurface(surface);
//	}
//}
