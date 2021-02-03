#pragma once
#include "Panel.h"

#define BUTTON_SIZE 24

class Toolbar : public Panel
{
public:
	Toolbar();
	~Toolbar();

	void Draw() override;
	void Shortcuts() override {};

	void Save(Config* config) const override {};
	void Load(Config* config) override {};
};
