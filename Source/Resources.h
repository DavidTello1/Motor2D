#pragma once
#include "Panel.h"

enum class ResourceType;

class Resources : public Panel
{
public:
	Resources();
	~Resources();

	void Draw() override;
	void Shortcuts() override;

	void Save(Config* config) const override;
	void Load(Config* config) override;

private:
	void DrawResourceData(ResourceType type, size_t index);
	void DrawImportOptions(ResourceType type, size_t index);
	void DrawPreview(ResourceType type, size_t index);

public:
	static const uint default_width = 265;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

public:
	UID selected_resource;
};

