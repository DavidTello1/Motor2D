#include "ModuleRenderer.h"

#include "Application.h"
#include "MessageBus.h"

//*** MESSAGES
#include "ModuleWindow.h"
#include "ModuleEditor.h"
// ---

#include "Glew/include/glew.h"

#pragma comment(lib, "glew/x64/glew32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

#include "mmgr/mmgr.h"

ModuleRenderer::ModuleRenderer(bool start_enabled) : Module(start_enabled)
{
}

ModuleRenderer::~ModuleRenderer()
{
}

// Called before render is available
bool ModuleRenderer::Init()
{
	LOG("Creating 3D Renderer context", 'i');

	// --- Create Context ---
	context = SDL_GL_CreateContext(App->window->GetWindow()); //***
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}

	// --- Initialize Glew ---
	LOG("Loading Glew", 'i');
	GLenum error = glewInit();

	// --- Create Viewport ---
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight()); //***
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- Messages ---
	//App->message->Subscribe(this, &ModuleRenderer::OnGetContext);

	return true;
}

bool ModuleRenderer::PreUpdate(float dt)
{
	// --- Clear Screen ---
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool ModuleRenderer::PostUpdate(float dt)
{
	//// --- Draw Scene ---
	//DrawScene();

	// --- Draw Editor ---
	App->editor->Draw(); //***

	SDL_GL_SwapWindow(App->window->GetWindow()); //***

	return true;
}

bool ModuleRenderer::CleanUp()
{
	LOG("Destroying 3D Renderer", 'i');

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer::SetVSync(bool value)
{
	if (vsync == value)
		return;

	vsync = value;

	if (SDL_GL_SetSwapInterval(value ? 1 : 0) < 0)
		LOG("Unable to set VSync: %s\n", SDL_GetError(), 'w');
}
