#pragma once
#include "Module.h"

#include <vector>

class GameObject;
class Hierarchy;
//class ModuleResources;
//class ComponentRenderer;
//class ComponentMesh;
//class ComponentCamera;

//enum SceneState {
//	EDIT,
//	START,
//	PLAY,
//	PAUSE,
//	STOP
//};

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	//bool Init(Config * config);
	//bool Start(Config* config = nullptr);
	//bool Update(float dt);
	//bool PostUpdate(float dt);
	//bool CleanUp();

	//bool Draw();

	//const char* GetName() { return scene_name.c_str(); }
	//void SetSceneName(const char* name) { scene_name = name; }

	//GameObject* GetGameObject(UID id);
	//GameObject* CreateGameObject(const char* name = "GameObject", GameObject* parent = nullptr, bool visible = false);
	//void DeleteGameObject(GameObject* obj);

	//void DeleteSelected();
	//void UnSelectAll(GameObject* keep_selected = nullptr);

public:
	std::string scene_name;

	//static SceneState state;
	//static const char* state_to_string[STOP + 1];

	//std::vector<ResourceScene*> scenes;
	//ResourceScene* current_scene;
};
