#pragma once
#include "Panel.h"

#include <vector>
#include <string>

#define MAX_LAYOUTS 10

#define FPS_LOG_SIZE 100

#define IMGUI_PRINT(field, format, ...) \
	ImGui::Text(field); \
	ImGui::SameLine(); \
	ImGui::TextColored(ImVec4(1.0f, 1.0f,0.0f,1.0f), format, __VA_ARGS__)

class Module;
class ModuleEditor;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer;
class ModuleFileSystem;

class Config;

struct hardware_info {
	float ram_gb = 0.f;
	uint cpu_count = 0;
	uint l1_cachekb = 0;
	bool rdtsc = false;
	bool altivec = false;
	bool now3d = false;
	bool mmx = false;
	bool sse = false;
	bool sse2 = false;
	bool sse3 = false;
	bool sse41 = false;
	bool sse42 = false;
	bool avx = false;
	bool avx2 = false;
	uint gpu_vendor = 0;
	uint gpu_device = 0;
	char gpu_brand[250] = "";
	float vram_mb_budget = 0.f;
	float vram_mb_usage = 0.f;
	float vram_mb_available = 0.f;
	float vram_mb_reserved = 0.f;
};

enum class Index {
	APPLICATION = 0,
	MEMORY,
	HARDWARE,
	WINDOW,
	INPUT_DRAW,
	FILESYSTEM,
	RESOURCES
};

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();

	void Draw() override;
	void Shortcuts() override {};

	void Save(Config* config) const override;
	void Load(Config* config) override;

	void AddFPS(float fps, float ms);
	std::vector<std::string> GetLayouts();

private:
	void DrawApplication();
	void DrawMemory();
	void DrawHardware();
	void DrawWindow();
	void DrawInput();
	void DrawFileSystem();
	void DrawResources();

	void GetHardware();
	const hardware_info& GetHardwareInfo() const;

public:
	static const uint default_width = 650;
	static const uint default_height = 500;
	static const uint default_pos_x = 360;
	static const uint default_pos_y = 100;

private:
	std::string current_layout = "Default";
	std::string selected_layout = "Default";
	std::vector<std::string> layouts;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	mutable hardware_info info_hw;

	Index curr_index = Index::APPLICATION;
	bool show_application = true;
	bool show_hardware = false;
	bool show_window = false;
	bool show_input = false;
	bool show_filesystem = false;
};
