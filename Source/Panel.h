#pragma once
#include "Globals.h"
#include "Icons.h"

#include "Imgui/imgui.h"

class Panel
{
public:
	Panel(const char* Name, const char* Icon, uint id) : name(Name), icon(Icon), ID(id) {};
	virtual ~Panel() {};

	uint GetID() const { return ID; } 
	const char* GetName() const { return name; }
	const char* GetIcon() const { return icon; }

	virtual void Draw() = 0;
	virtual void Shortcuts() = 0;

public:
	bool active = true;
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	float width = 0, height = 0, pos_x = 0, pos_y = 0;

private:
	const char* name;
	const char* icon;
	uint ID;
};