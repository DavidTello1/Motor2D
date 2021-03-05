#include "PanelScene.h"

#include "Application.h"
#include "ModuleRenderer.h"
#include "ModuleWindow.h"

#include "ModuleResources.h"

#include "mmgr/mmgr.h"

// ---------------------------------------------------------
PanelScene::PanelScene() : Panel("Scene", ICON_SCENE, 6)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	flags = ImGuiWindowFlags_NoCollapse;
}

PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ImGui::BeginChild("SceneViewport", ImVec2(0, 0), true);

	ImVec2 window_size = ImGui::GetWindowSize();
	if (window_size.x != win_size.x || window_size.y != win_size.y)
		OnResize(window_size);

	ImGui::SetCursorPos(img_offset);
	img_corner = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + img_size.y);

	ImGui::Image((ImTextureID)0/*App->renderer->renderTexture*/, img_size, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::EndChild();
}

void PanelScene::Shortcuts()
{
}

void PanelScene::Save(Config* config) const
{
}

void PanelScene::Load(Config* config)
{
}

void PanelScene::OnResize(ImVec2 new_size)
{
	win_size = new_size;
	img_size = ImVec2((float)App->window->GetWidth(), (float)App->window->GetHeight());

	while (img_size.x > win_size.x || img_size.y > win_size.y - 15.0f)
	{
		if (img_size.x > win_size.x)
		{
			float factor = img_size.x / (win_size.x - 15.0f);
			img_size.x = img_size.x / factor;
			img_size.y = img_size.y / factor;
		}
		if (img_size.y > win_size.y - 15.0f)
		{
			float factor = img_size.y / (win_size.y - 15.0f);
			img_size.x = (uint)img_size.x / factor;
			img_size.y = (uint)img_size.y / factor;
		}
	}
	img_offset = ImVec2((win_size.x - img_size.x) / 2, (win_size.y - img_size.y) / 2);
}
