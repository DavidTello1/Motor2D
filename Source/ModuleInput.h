#pragma once
#include "Module.h"

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

	bool Init() override;
	bool PreUpdate(float dt) override;
	bool CleanUp() override;

	// Get Key / Mouse Button
	KEY_STATE GetKey(int id) const { return keyboard[id]; }
	KEY_STATE GetMouseButton(int id) const { return mouse_buttons[id]; }

	// Get mouse / axis position
	void GetMouseMotion(int& x, int& y) const { x = mouse_motion_x; y = mouse_motion_y; }
	void GetMousePosition(int& x, int& y) const { x = mouse_x; y = mouse_y; }
	int GetMouseWheel() const { return mouse_wheel; }

private:
	void GetKeyboardInput();
	void GetMouseInput();

	void MouseEvents();

public:
	bool quit = false; //***

private:
	static constexpr int MAX_MOUSE_BUTTONS = 5;
	static constexpr int MAX_KEYS = 300;

	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];

	int mouse_motion_x = 0;
	int mouse_motion_y = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	int mouse_wheel = 0;
};