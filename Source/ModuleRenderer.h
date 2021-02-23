#pragma once
#include "Module.h"

typedef void *SDL_GLContext;

class ModuleRenderer : public Module
{
public:
	ModuleRenderer(bool start_enabled = true);
	~ModuleRenderer();

	bool Init(Config* config = nullptr);
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void Load(Config* config);
	void Save(Config* config) const;

	void SetVSync(bool vsync);
	bool GetVSync() const { return vsync; }

public:
	SDL_GLContext context = nullptr;
	bool vsync = false;
};