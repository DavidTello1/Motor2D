#pragma once
#include "Panel.h"

#include <vector>

class Config;

struct OnOpenPanelConfiguration;

class PanelConfiguration : public Panel
{
private:
	enum class Index {
		APPLICATION = 0,
		INPUT_DRAW,
	};

public:
	PanelConfiguration(bool active = true);
	~PanelConfiguration();

	void Draw() override;
	void Shortcuts() override {};

private:
	void Save(Config* config) const override;
	void Load(Config* config) override;

	void AddFPS(float fps, float ms); //***MESSAGE

	// --- SECTIONS ---
	void DrawApplication();
	void DrawInput();

	// --- MESSAGES ---
	void OpenPanel(OnOpenPanelConfiguration* m) { this->active = true; }

private:
	static constexpr int FPS_LOG_SIZE = 100;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	Index curr_index = Index::APPLICATION;

	Config* config = nullptr;
};
