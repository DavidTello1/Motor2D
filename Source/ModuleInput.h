#pragma once

#include "Module.h"

#include "MathGeoLib/include/Math/float2.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:

	ModuleInput(bool start_enabled = true);
	~ModuleInput();

	bool Init(Config* config = nullptr);
	bool PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const { return keyboard[id]; }
	KEY_STATE GetMouseButton(int id) const { return mouse_buttons[id]; }

	// Get mouse / axis position
	void GetMouseMotion(int& x, int& y) const { x = mouse_motion_x; y = mouse_motion_y; }
	void GetMousePosition(int& x, int& y) const { x = mouse_x; y = mouse_y; }
	int GetMouseWheel() const { return mouse_wheel; }

public:
	bool quit = false;

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_motion_x = 0;
	int mouse_motion_y = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	int mouse_wheel = 0;
};