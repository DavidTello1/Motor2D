#include "ModuleScene.h"
#include "Application.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(bool start_enabled) : Module("Scene", start_enabled)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init(Config* config)
{
	return true;
}

bool ModuleScene::Start(Config* config)
{
	return true;
}

bool ModuleScene::Update(float dt)
{
	// update systems
	return true;
}

bool ModuleScene::PostUpdate(float dt)
{
	return true;
}

bool ModuleScene::CleanUp()
{
	return true;
}

void ModuleScene::Load(Config* config)
{
}

void ModuleScene::Save(Config* config) const
{
}

void ModuleScene::Draw()
{

}