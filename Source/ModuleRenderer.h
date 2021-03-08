#pragma once
#include "Module.h"

typedef void *SDL_GLContext;
struct Camera;

class ModuleRenderer : public Module
{
public:
	ModuleRenderer(bool start_enabled = true);
	~ModuleRenderer();

	bool Init(Config* config = nullptr) override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	void Load(Config* config);
	void Save(Config* config) const;

	void SetVSync(bool vsync);
	bool GetVSync() const { return vsync; }

	SDL_GLContext GetContext() { return context; }

private:
	void DrawScene();

	void OnResize();
	void UpdateProjectionMatrix();
	void GenerateBuffers();

private:
	Camera* camera = nullptr;
	Camera* culling_camera = nullptr;

	SDL_GLContext context = nullptr;
	bool vsync = false;

	uint frameBuffer = 0;
	uint depthBuffer = 0;
	uint renderTexture = 0;

	bool drawGrid = true;

};