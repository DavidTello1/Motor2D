#pragma once

#include "Globals.h"


class Module
{
public:
	Module(bool start_enabled = true) : enabled(start_enabled) {};
	virtual ~Module() {};

	bool IsActive() const { return enabled; }
	void SetActive(bool active)
	{
		if (enabled != active)
		{
			enabled = active;
			(active == true) ? Start() : CleanUp();
		}
	}

	virtual bool Init() { return true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate(float dt) { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }
	virtual bool CleanUp() { return true; }

private:
	bool enabled;
};
