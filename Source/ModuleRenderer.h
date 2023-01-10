#pragma once
#include "Module.h"

typedef void *SDL_GLContext;

struct OnGetRendererContext;

class ModuleRenderer : public Module
{
public:
	ModuleRenderer(bool start_enabled = true);
	~ModuleRenderer();

	bool Init() override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	// --- Context ---
	SDL_GLContext GetContext() { return context; }

	// --- VSync ---
	void SetVSync(bool value);
	const bool GetVSync() const { return vsync; }

private:
	SDL_GLContext context = nullptr;

	bool vsync = false;
};
