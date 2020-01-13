#pragma once

#include "Globals.h"

#include "SDL/include/SDL_scancode.h"
#include <string>

class Panel
{
public:
	Panel(const char* Name, SDL_Scancode Shortcut = SDL_SCANCODE_UNKNOWN) : name(Name), shortcut(Shortcut) {};
	virtual ~Panel() {};

	void SwitchActive() { active = !active; }
	bool IsActive() const { return active; }

	SDL_Scancode GetShortCut() const { return shortcut; }
	const char* GetName() const { return name; }

	virtual void Draw() = 0;

public:
	bool active = true;
	bool has_menubar = false;
	int width, height, pos_x, pos_y;

private:
	const char* name;
	SDL_Scancode shortcut = SDL_SCANCODE_UNKNOWN;
};