#pragma once
#include "Panel.h"

#include <vector>

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

enum Index {
	APPLICATION = 0,
	MEMORY,
	HARDWARE,
	SOFTWARE,
	WINDOW,
	INPUT_DRAW,
	FILESYSTEM,
	RESOURCES
};

class Configuration : public Panel
{
public:
	Configuration();
	virtual ~Configuration();

	void Draw() override;
	void Shortcuts() override {};

	void DrawApplication();
	void DrawMemory();
	void DrawHardware();
	void DrawSoftware();
	void DrawWindow();
	void DrawInput();
	void DrawFileSystem();

	void AddFPS(float fps, float ms);

	void GetHardware();
	const hardware_info& GetHardwareInfo() const;

public:
	static const uint default_width = 650;
	static const uint default_height = 500;
	static const uint default_pos_x = 360;
	static const uint default_pos_y = 100;

private:
	ImGuiTextBuffer input_buf;
	bool need_scroll = false;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

	mutable hardware_info info_hw;

	bool show_application = true;
	bool show_hardware = false;
	bool show_window = false;
	bool show_input = false;
	bool show_filesystem = false;

	Index curr_index = Index::APPLICATION;
};
