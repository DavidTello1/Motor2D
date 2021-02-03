#pragma once
#include "Module.h"

#include "SDL/include/SDL.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 1024

#define MIN_SCREEN_WIDTH 480
#define MIN_SCREEN_HEIGHT 320
#define MAX_SCREEN_WIDTH 4800
#define MAX_SCREEN_HEIGHT 3200


struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool CleanUp();

	void Save(Config* config) const override;
	void Load(Config* config) override;

	SDL_Window* GetWindow() const { return window; }
	uint GetWidth() const { return screen_width; }
	uint GetHeight() const { return screen_height; }
	uint GetRefreshRate() const;
	void GetMaxMinSize(uint& min_width, uint& min_height, uint& max_width, uint& max_height) const;

	bool IsFullscreen() const { return fullscreen; }
	bool IsResizable() const { return resizable; }
	bool IsBorderless() const { return borderless; }
	bool IsFullscreenDesktop() const { return fullscreen_desktop; }
	bool IsMaximized() const { return maximized; }
	//const char* GetIcon() const { return icon_file.c_str(); }

	void SetTitle(const char* title) { SDL_SetWindowTitle(window, title); }
	void SetWidth(uint width) { SDL_SetWindowSize(window, width, GetHeight()); screen_width = width; }
	void SetHeigth(uint height) { SDL_SetWindowSize(window, GetWidth(), height); screen_height = height; }
	void UpdateSize(uint width, uint height) { screen_width = width; screen_height = height; }

	void SetFullscreen(bool set);
	void SetResizable(bool set);
	void SetBorderless(bool set);
	void SetFullScreenDesktop(bool set);
	void SetMaximized(bool set);
	//void SetIcon(const char* file);

private:
	SDL_Window* window = nullptr; 	//The window we'll be rendering to
	SDL_Surface* screen_surface = nullptr; 	//The surface contained by the window

	int screen_width = DEFAULT_SCREEN_WIDTH;
	int screen_height = DEFAULT_SCREEN_HEIGHT;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;
	bool maximized = false;
	std::string icon_file;
};
