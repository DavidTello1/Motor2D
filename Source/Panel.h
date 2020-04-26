#pragma once
#include "Globals.h"

#include "Imgui/imgui.h"
#include "SDL/include/SDL_scancode.h"

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
	virtual void Shortcuts() {};

public:
	bool active = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	int width, height, pos_x, pos_y = 0;

private:
	const char* name;
	SDL_Scancode shortcut = SDL_SCANCODE_UNKNOWN;
};