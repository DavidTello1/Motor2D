#pragma once
#include "Globals.h"

#include "Icons.h"
#include "Imgui/imgui.h"
#include "SDL/include/SDL_scancode.h"

class Panel
{
public:
	Panel(const char* Name, const char* Icon, SDL_Scancode Shortcut = SDL_SCANCODE_UNKNOWN) : name(Name), icon(Icon), shortcut(Shortcut) {};
	virtual ~Panel() {};

	void SwitchActive() { active = !active; }
	bool IsActive() const { return active; }

	SDL_Scancode GetShortCut() const { return shortcut; }
	const char* GetName() const { return name; }
	const char* GetIcon() const { return icon; }

	virtual void Draw() = 0;
	virtual void Shortcuts() {};

public:
	bool active = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	int width = 0, height = 0, pos_x = 0, pos_y = 0;

private:
	const char* name;
	const char* icon;
	SDL_Scancode shortcut = SDL_SCANCODE_UNKNOWN;
};