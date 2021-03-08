#include "ModuleCamera.h"
#include "mmgr/mmgr.h"

ModuleCamera::ModuleCamera(bool start_enabled) : Module("Camera", start_enabled)
{
	editor_camera = CreateCamera();
}

// Destructor
ModuleCamera::~ModuleCamera()
{}

// Called before render is available
bool ModuleCamera::Init(Config* config)
{
	return true;
}

bool ModuleCamera::Update(float dt)
{
	UpdateEditorCameraInput();

	return true;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	DestroyAllCameras();

	return true;
}

void ModuleCamera::Load(Config* config)
{
}

void ModuleCamera::Save(Config* config) const
{
}

Camera* ModuleCamera::CreateCamera() 
{
	//Camera* ret = new Camera();
	//cameras.push_back(ret);
	//return ret;
	return nullptr;
}

void ModuleCamera::DestroyCamera(Camera* camera)
{
	//for (std::vector<Camera*>::iterator it = cameras.begin(); it != cameras.end();)
	//{
	//	if (cam == *it)
	//	{
	//		(*it)->CleanUp();
	//		RELEASE(*it);
	//		cameras.erase(it);
	//		break;
	//	}
	//	else
	//		++it;
	//}
}

void ModuleCamera::UpdateEditorCameraInput() 
{

}

void ModuleCamera::DestroyAllCameras() 
{

}
