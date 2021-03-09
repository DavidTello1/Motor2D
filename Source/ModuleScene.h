#pragma once
#include "Module.h"

#include "GameObject.h"

//struct SystemCamera;
//struct SystemRender;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(Config* config = nullptr) override;
	bool Start(Config* config = nullptr) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	void Load(Config* config) override;
	void Save(Config* config) const override;

	void Draw();

private:
	int GetGameObjectByID(UID id)
	{
		for (size_t index = 0, size = objects.ids.size(); index < size; ++index)
		{
			if (objects.ids[index] == id)
				return index;
		}
		return -1;
	}

	int GetGameObjectByName(std::string name)
	{
		for (size_t index = 0, size = objects.ids.size(); index < size; ++index)
		{
			if (objects.names[index] == name)
				return index;
		}
		return -1;
	}

	size_t CreateGameObject(UID id_, std::string name_, int flags_, int components_)
	{
		objects.ids.push_back(id_);
		objects.names.push_back(name_);
		objects.flags.push_back(flags_);
		objects.components.push_back(components_);

		return objects.ids.size() - 1;
	}

	void DeleteGameObject(size_t index)
	{
		objects.ids.erase(objects.ids.begin() + index);
		objects.names.erase(objects.names.begin() + index);
		objects.flags.erase(objects.flags.begin() + index);
		objects.components.erase(objects.components.begin() + index);

		//unsubscribe from components' systems
	}

	void DeleteAllGameObjects()
	{
		objects.ids.clear();
		objects.names.clear();
		objects.flags.clear();
		objects.components.clear();

		objects.ids.shrink_to_fit();
		objects.names.shrink_to_fit();
		objects.flags.shrink_to_fit();
		objects.components.shrink_to_fit();
	}


public:
	UID current_scene;
	
private:
	GameObjects objects;

	//SystemRender render_system;
	//SystemCamera camera_system;
};
