#pragma once
#include "Imgui/imgui.h"

class Config;

class Panel
{
public:
	Panel(bool active = true) : active(active) {};
	virtual ~Panel() {};

	virtual void Draw() = 0;
	virtual void Shortcuts() = 0;

	virtual void Save(Config* config) const = 0;
	virtual void Load(Config* config) = 0;

	const bool IsActive() const { return active; }
	void SetActive(bool value) { active = value; }

protected:
	bool active = true;

	const char* icon = "";
	const char* name = "";
	ImGuiWindowFlags flags = 0;

	ImVec2 position = ImVec2(0.0f, 0.0f);
	ImVec2 size = ImVec2(0.0f, 0.0f);
};
