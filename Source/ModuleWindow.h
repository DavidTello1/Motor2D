#pragma once
#include "Module.h"

#include "SDL/include/SDL.h"

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	SDL_Window* GetWindow() const { return window; }

	// --- SIZE ---
	uint GetWidth() const { return screen_width; }
	void SetWidth(uint width) { SDL_SetWindowSize(window, width, GetHeight()); screen_width = width; }

	uint GetHeight() const { return screen_height; }
	void SetHeigth(uint height) { SDL_SetWindowSize(window, GetWidth(), height); screen_height = height; }

	void GetMaxMinSize(uint& min_width, uint& min_height, uint& max_width, uint& max_height) const;
	void UpdateSize(uint width, uint height) { screen_width = width; screen_height = height; }

	// --- WINDOW OPTIONS ---
	bool IsFullscreen() const { return fullscreen; }
	void SetFullscreen(bool set);

	bool IsResizable() const { return resizable; }
	void SetResizable(bool set);

	bool IsBorderless() const { return borderless; }
	void SetBorderless(bool set);

	bool IsFullscreenDesktop() const { return fullscreen_desktop; }
	void SetFullScreenDesktop(bool set);

	bool IsMaximized() const { return maximized; }
	void SetMaximized(bool set);

	// --- REFRESH RATE ---
	uint GetRefreshRate() const;

	// --- TITLE & ICON ---
	void SetTitle(const char* title) { SDL_SetWindowTitle(window, title); }
	//void SetIcon(const char* file);

private:
	static constexpr int DEFAULT_SCREEN_WIDTH = 1280;
	static constexpr int DEFAULT_SCREEN_HEIGHT = 1024;
	static constexpr int MIN_SCREEN_WIDTH = 480;
	static constexpr int MIN_SCREEN_HEIGHT = 320;
	static constexpr int MAX_SCREEN_WIDTH = 4800;
	static constexpr int MAX_SCREEN_HEIGHT = 3200;

	SDL_Window* window = nullptr; // The window we'll be rendering to
	SDL_Surface* screen_surface = nullptr; // The surface contained by the window

	int screen_width = DEFAULT_SCREEN_WIDTH;
	int screen_height = DEFAULT_SCREEN_HEIGHT;

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;
	bool maximized = false;
};
