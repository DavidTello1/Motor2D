#pragma once
#include "Panel.h"

class PanelResources : public Panel
{
public:
	PanelResources();
	~PanelResources();

	void Draw() override;
	void Shortcuts() override;

	void Save(Config* config) const override;
	void Load(Config* config) override;

private:
	void DockSpace();
	//void DrawResourceData(ResourceType type, size_t index);

	//// Import Options
	//void DrawOptionsScene(size_t index);
	//void DrawOptionsPrefab(size_t index);
	//void DrawOptionsTexture(size_t index);
	//void DrawOptionsMaterial(size_t index);
	//void DrawOptionsAnimation(size_t index);
	//void DrawOptionsTilemap(size_t index);
	//void DrawOptionsAudio(size_t index);
	//void DrawOptionsScript(size_t index);
	//void DrawOptionsShader(size_t index);

	//// Preview
	//void DrawPreviewScene(size_t index);
	//void DrawPreviewPrefab(size_t index);
	//void DrawPreviewTexture(size_t index);
	//void DrawPreviewMaterial(size_t index);
	//void DrawPreviewAnimation(size_t index);
	//void DrawPreviewTilemap(size_t index);
	//void DrawPreviewAudio(size_t index);
	//void DrawPreviewScript(size_t index);
	//void DrawPreviewShader(size_t index);

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	bool is_init = false; //used for dockspace

};

