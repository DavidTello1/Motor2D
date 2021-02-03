#include "Application.h"
#include "ModuleWindow.h"
#include "Config.h"

#include "mmgr/mmgr.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module("Window", start_enabled)
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(Config* config)
{
	LOG("Init SDL window & surface");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}
	else
	{
		SDL_DisplayMode DM;
		SDL_GetDesktopDisplayMode(0, &DM);

		//Create window
		screen_width = config->GetUInt("Width", uint(DM.w*0.75f));
		screen_height = config->GetUInt("Height", uint(DM.h*0.75f));

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 3.2
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		//Init window options
		fullscreen = config->GetBool("Fullscreen", false);
		resizable = config->GetBool("Resizable", true);
		borderless = config->GetBool("Borderless", false);
		fullscreen_desktop = config->GetBool("Fullscreen Desktop", false);
		maximized = config->GetBool("Maximized", false);

		if (fullscreen == true)
			flags |= SDL_WINDOW_FULLSCREEN;

		if (resizable == true)
			flags |= SDL_WINDOW_RESIZABLE;

		if (borderless == true)
			flags |= SDL_WINDOW_BORDERLESS;

		if (fullscreen_desktop == true)
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		if (maximized == true)
			flags |= SDL_WINDOW_MAXIMIZED;

		window = SDL_CreateWindow(App->GetAppName(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, flags);

		if (window == nullptr)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError(), 'e');
			return false;
		}
		else
			screen_surface = SDL_GetWindowSurface(window); //Get window surface
	}

	return true;
}

bool ModuleWindow::Start(Config* config)
{
	//SetDefaultIcon();

	//std::string icon_file = config->GetString("Icon", "");
	//if (icon_file.size() > 1)
	//	SetIcon(icon_file.c_str());

	// Force to trigger a chain of events to refresh aspect ratios	
	SDL_SetWindowSize(window, screen_width, screen_height);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != nullptr)
		SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::Save(Config* config) const
{
	//config->AddString("Icon", icon_file.c_str());
	config->AddUInt("Width", GetWidth());
	config->AddUInt("Height", this->GetHeight());
	config->AddBool("Fullscreen", IsFullscreen());
	config->AddBool("Resizable", IsResizable());
	config->AddBool("Borderless", IsBorderless());
	config->AddBool("Fullscreen Desktop", IsFullscreenDesktop());
	config->AddBool("Maximized", IsMaximized());
}

void ModuleWindow::Load(Config* config)
{
	//SetIcon(config->GetString("Icon", ""));
	SetMaximized(config->GetBool("Maximized", false));

	if (IsMaximized())
		UpdateSize(config->GetUInt("Width", 1280), config->GetUInt("Height", 1024));
	else
	{
		SetWidth(config->GetUInt("Width", 1280));
		SetHeigth(config->GetUInt("Height", 1024));
	}

	SetFullscreen(config->GetBool("Fullscreen", false));
	SetResizable(config->GetBool("Resizable", false));
	SetBorderless(config->GetBool("Borderless", false));
	SetFullScreenDesktop(config->GetBool("Fullscreen Desktop", false));
}

uint ModuleWindow::GetRefreshRate() const
{
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError(), 'e');
	}
	else
		return (uint)dm.refresh_rate;

	return 0;
}

void ModuleWindow::GetMaxMinSize(uint & min_width, uint & min_height, uint & max_width, uint & max_height) const
{
	min_width = MIN_SCREEN_WIDTH;
	min_height = MIN_SCREEN_HEIGHT;
	max_width = MAX_SCREEN_WIDTH;
	max_height = MAX_SCREEN_HEIGHT;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError(), 'e');
	}
	else
	{
		max_width = dm.w;
		max_height = dm.h;
	}
}

void ModuleWindow::SetFullscreen(bool set)
{
	if (set != fullscreen)
	{
		fullscreen = set;
		if (fullscreen == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
				LOG("Could not switch to fullscreen: %s\n", SDL_GetError(), 'e');
			fullscreen_desktop = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				LOG("Could not switch to windowed: %s\n", SDL_GetError(), 'e');
		}
	}
}

void ModuleWindow::SetResizable(bool set)
{
	// cannot be changed while the program is running, but we can save the change
	resizable = set;
}

void ModuleWindow::SetBorderless(bool set)
{
	if (set != borderless && fullscreen == false && fullscreen_desktop == false)
	{
		borderless = set;
		SDL_SetWindowBordered(window, (SDL_bool)!borderless);
	}
}

void ModuleWindow::SetFullScreenDesktop(bool set)
{
	if (set != fullscreen_desktop)
	{
		fullscreen_desktop = set;
		if (fullscreen_desktop == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
				LOG("Could not switch to fullscreen desktop: %s\n", SDL_GetError(), 'e');
			fullscreen = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				LOG("Could not switch to windowed: %s\n", SDL_GetError(), 'e');
		}
	}
}

void ModuleWindow::SetMaximized(bool set)
{
	if (set != maximized)
	{
		maximized = set;
		if (maximized == true)
			SDL_MaximizeWindow(window);
		else
			SDL_RestoreWindow(window);
	}
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
