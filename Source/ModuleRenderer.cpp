#include "ModuleRenderer.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"

#include "ModuleInput.h" //***MOVE TO PANELSCENE (SHORTCUTS)
#include "Config.h"

//#include "PanelScene.h"
//#include "ComponentCamera.h"

#include "Glew/include/glew.h"
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "mmgr/mmgr.h"

ModuleRenderer::ModuleRenderer(bool start_enabled) : Module("Renderer", start_enabled)
{
}

// Destructor
ModuleRenderer::~ModuleRenderer()
{
}

// Called before render is available
bool ModuleRenderer::Init(Config* config)
{
	LOG("Creating 3D Renderer context");

	//Create context
	context = SDL_GL_CreateContext(App->window->GetWindow());
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError(), 'e');
		return false;
	}

	// Initialize glew
	GLenum error = glewInit();
	LOG("Loading glew");

	/*glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);*/
	glEnable(GL_TEXTURE_2D);

	return true;

	//------------------------------------------------------

	//bool ret = false;
	//LOG("Creating Renderer context");

	////Create context
	//context = SDL_GL_CreateContext(App->window->GetWindow());
	//if (context == NULL)
	//{
	//	LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError(), 'e');
	//	ret = false;
	//}
	//if (ret == true)
	//{
	//	//Initialize Projection Matrix
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadIdentity();

	//	//Check for error
	//	GLenum error = glGetError();
	//	if (error != GL_NO_ERROR)
	//	{
	//		LOG("Error initializing OpenGL! %s", gluErrorString(error));
	//		ret = false;
	//	}

	//	//Initialize Modelview Matrix
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();

	//	//Check for error
	//	error = glGetError();
	//	if (error != GL_NO_ERROR)
	//	{
	//		LOG("Error initializing OpenGL! %s", gluErrorString(error));
	//		ret = false;
	//	}

	//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//	glClearDepth(1.0f);

	//	//Initialize clear color
	//	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	//	glClear(GL_COLOR_BUFFER_BIT);

	//	//Check for error
	//	error = glGetError();
	//	if (error != GL_NO_ERROR)
	//	{
	//		LOG("Error initializing OpenGL! %s", gluErrorString(error));
	//		ret = false;
	//	}

	//	// Blend for transparency
	//	glEnable(GL_BLEND);
	//	glBlendEquation(GL_FUNC_ADD);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_CULL_FACE);
	//	glEnable(GL_LIGHTING);
	//	glEnable(GL_COLOR_MATERIAL);
	//	glEnable(GL_TEXTURE_2D);

	//	glShadeModel(GL_SMOOTH);
	//	glEnable(GL_LINE_SMOOTH);
	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//}

	//OnResize();

	//return ret;
}

// PreUpdate: clear buffer
bool ModuleRenderer::PreUpdate(float dt)
{
	//if (camera->update_projection)
	//{
	//	UpdateProjectionMatrix();
	//	camera->update_projection = false;
	//}
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(camera->GetOpenGLViewMatrix());

	return true;
}

// PostUpdate present buffer to screen
bool ModuleRenderer::PostUpdate(float dt)
{
	//DrawScene();
	App->editor->Draw();

	SDL_GL_SwapWindow(App->window->GetWindow());

	return true;
}

// Called before quitting
bool ModuleRenderer::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer::Load(Config * config)
{
	SetVSync(config->GetBool("Vertical Sync", true));
}

void ModuleRenderer::Save(Config * config) const
{
	config->AddBool("Vertical Sync", GetVSync());
}

void ModuleRenderer::SetVSync(bool vsync)
{
	if (this->vsync != vsync)
	{
		this->vsync = vsync;
		if (SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError(), 'e');
	}
}

void ModuleRenderer::DrawScene()
{
	glUseProgram(0);

	//Both draw and input handling
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Grid
	if (drawGrid)
	{
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor4f(0.8f, 0.8f, 0.8f, 0.8f);

		float d = 40.0f;
		for (float i = -d; i <= d; i += 2.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}
		glEnd();
	}

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) //***MOVE TO PANELSCENE (SHORTCUTS)
		drawGrid = !drawGrid;

	// Draw Scene
	App->scene->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
}

void ModuleRenderer::OnResize()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	//camera->SetAspectRatio((float)App->window->GetWidth() / (float)App->window->GetHeight());
	UpdateProjectionMatrix();
	GenerateBuffers();
}

void ModuleRenderer::UpdateProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glLoadMatrixf((GLfloat*)camera->GetOpenGLProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer::GenerateBuffers()
{
	//Generating buffers for scene render
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Generating texture to render to
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->GetWidth(), App->window->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generating the depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->GetWidth(), App->window->GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Configuring frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Error creating screen buffer", 'e');
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
