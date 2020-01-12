#pragma once

#include "Globals.h"
#include <string>

#define MODULE_NAME_LENGTH 25

class Application;
class Config;

class Module
{
private:
	bool enabled;
	char name[MODULE_NAME_LENGTH];

public:
	Module(const char* name, bool start_enabled = true) : enabled(start_enabled) { strcpy_s(this->name, MODULE_NAME_LENGTH, name); }
	virtual ~Module() {}

	const char* GetName() const { return name; }
	bool IsActive() const { return enabled; }

	void SetActive(bool active)
	{
		if (enabled != active)
		{
			enabled = active;
			if (active == true)
				Start();
			else
				CleanUp();
		}
	}

	virtual bool Init(Config* config = nullptr) { return true; }
	virtual bool Start(Config* config = nullptr) { return true; }
	virtual bool PreUpdate(float dt) { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }
	virtual bool CleanUp() { return true; }

	virtual void Save(Config* config) const	{}
	virtual void Load(Config* config) {}
};