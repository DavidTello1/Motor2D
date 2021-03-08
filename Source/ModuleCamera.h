#pragma once
#include "Module.h"

#include <vector>

struct Camera;

class ModuleCamera : public Module
{
public:
	ModuleCamera(bool start_enabled = true);
	~ModuleCamera();

	bool Init(Config* config = nullptr) override;
	bool Update(float dt) override;
	bool CleanUp() override;

	void Load(Config* config) override;
	void Save(Config* config) const override;


	Camera* CreateCamera();
	void DestroyCamera(Camera* camera);

	std::vector<Camera*> GetCameras() { return cameras; }
	Camera* GetEditorCamera() const { return editor_camera; }
	Camera* GetGameCamera() const { return game_camera; }
	const float GetWheelSpeed() const { return wheel_speed; }

	void SetGameCamera(Camera* set) { game_camera = set; }
	void SetWheelSpeed(const float& set) { wheel_speed = set; }

private:
	void UpdateEditorCameraInput();
	void DestroyAllCameras();

private:
	std::vector<Camera*> cameras;
	Camera* editor_camera = nullptr;
	Camera* game_camera = nullptr;

	// Camera Movement
	bool  mouse_movement = false;
	float wheel_speed = 0.0f;

	bool dragging = false;
};
