#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"

//#include "ComponentRenderer.h"
//#include "ComponentMaterial.h"
//#include "ComponentCamera.h"
//#include "ComponentMesh.h"

//#include "ResourceModel.h"
//#include "Viewport.h"

#include "mmgr/mmgr.h"

//GameObject* ModuleScene::root_object;
//SceneState ModuleScene::state = EDIT;
//const char* ModuleScene::state_to_string[STOP + 1] = { "EDIT","START","PLAY","PAUSE","STOP" };

ModuleScene::ModuleScene(bool start_enabled) : Module("Scene", start_enabled)
{

}


ModuleScene::~ModuleScene()
{
	//delete root_object;
}
