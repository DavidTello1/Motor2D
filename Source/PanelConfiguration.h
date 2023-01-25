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
		INPUT,
		FILESYSTEM
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

	// --- CHILD WINDOWS ---
	void DrawChildIndex(ImVec2 windowSize);
	void DrawChildContent(ImVec2 windowSize);
	void DrawButtons(ImVec2 windowSize);

	// --- SECTIONS ---
	void DrawApplication();
	void DrawInput();
	void DrawFilesystem();

	// --- MESSAGES ---
	void OpenPanel(OnOpenPanelConfiguration* m) { this->active = true; }

private:
	static constexpr int FPS_LOG_SIZE = 100;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	Index current_index = Index::APPLICATION;

	Config* config = nullptr;
};
