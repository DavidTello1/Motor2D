#pragma once
#include "Panel.h"

class PanelScene : public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Draw() override;
	void Shortcuts() override;

	void Save(Config* config) const override;
	void Load(Config* config) override;

	void OnResize(ImVec2 new_size);

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	ImVec2 img_corner = { 0,0 };
	ImVec2 img_size = { 0,0 };
	ImVec2 cornerPos = { 0,0 };

	ImVec2 win_size = { 0,0 };
	ImVec2 img_offset = {0,0};

};
