#pragma once
#include "Panel.h"

#define BUTTON_SIZE 24

class PanelToolbar : public Panel
{
public:
	PanelToolbar();
	~PanelToolbar();

	void Draw() override;
	void Shortcuts() override {};

	void Save(Config* config) const override {};
	void Load(Config* config) override {};
};
