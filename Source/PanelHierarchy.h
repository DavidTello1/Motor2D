#pragma once
#include "Panel.h"
#include "HierarchyNode.h"

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();
	~PanelHierarchy();

	void Draw() override;
	void Shortcuts() override;

	void Save(Config* config) const override;
	void Load(Config* config) override;

private:
	void DrawSceneNode();
	void DrawConnectorLines(size_t index, ImDrawList* draw_list); //draw connector lines when node is open

	void DrawRightClick(); //only draws if right click is pressed, returns true if drawn
	void ShowSceneOptions(size_t index); //popup when options button is clicked

	void CreateMenu(); //imgui menu for creating nodes (folder, gameobject, scene, etc)
	void Scroll(ImVec2 pos);

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	std::string scene_name = "Default Scene";
	int scene_index = -1;
	bool is_scene_saved = true;
	bool is_scene_hidden = false;

	HierarchyNode nodes;

	//std::vector<std::string> hidden_childs;
};
