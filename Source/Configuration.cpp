#include "Application.h"
#include "Configuration.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"

#include "gpudetect/DeviceId.h"
//#include "Devil/include/IL/il.h"

#include "mmgr/mmgr.h"

Configuration::Configuration() : Panel("Configuration"), fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	active = false;

	GetHardware(); //init hardware detection
	info_hw = GetHardwareInfo();
}

Configuration::~Configuration()
{
}

void Configuration::Draw()
{
	// Child Index
	ImGui::BeginChild("Index", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 0), true);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 15));
	static int selected = 0;
	if (ImGui::Selectable("Application", selected == 0)) {
		curr_index = Index::APPLICATION;
		selected = 0;
	}
	else if (ImGui::Selectable("Memory", selected == 1)) {
		curr_index = Index::MEMORY;
		selected = 1;
	}
	else if (ImGui::Selectable("Hardware", selected == 2)) {
		curr_index = Index::HARDWARE;
		selected = 2;
	}
	else if (ImGui::Selectable("Software", selected == 3)) {
		curr_index = Index::SOFTWARE;
		selected = 3;
	}
	else if (ImGui::Selectable("Window", selected == 4)) {
		curr_index = Index::WINDOW;
		selected = 4;
	}
	else if (ImGui::Selectable("Input", selected == 5)) {
		curr_index = Index::INPUT_DRAW;
		selected = 5;
	}
	else if (ImGui::Selectable("File System", selected == 6)) {
		curr_index = Index::FILESYSTEM;
		selected = 6;
	}
	else if (ImGui::Selectable("Resources", selected == 7)) {
		curr_index = Index::RESOURCES;
		selected = 7;
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::SameLine();

	// Child Content
	ImGui::BeginChild("Content");
	switch (curr_index)
	{
	case Index::APPLICATION:
		DrawApplication();
		break;
	case Index::MEMORY:
		DrawMemory();
		break;
	case Index::HARDWARE:
		DrawHardware();
		break;
	case Index::SOFTWARE:
		DrawSoftware();
		break;
	case Index::WINDOW:
		DrawWindow();
		break;
	case Index::INPUT_DRAW:
		DrawInput();
		break;
	case Index::FILESYSTEM:
		DrawFileSystem();
		break;
	case Index::RESOURCES:
		//DrawResources();
		break;
	default:
		break;
	}

	// Buttons
	static ImVec2 size = ImGui::GetContentRegionAvail();
	static float pos = 0.0f;

	pos = ImGui::GetCursorPosX();
	ImGui::SetCursorPosY(float(height - 60));
	if (ImGui::Button("Apply", ImVec2(size.x / 3, 22)))
	{
		LOG("Saved Configuration");
		App->SavePrefs();
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();

	ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
	pos = ImGui::GetCursorPosX();
	if (ImGui::Button("Reset", ImVec2(size.x / 3, 22)))
	{
		LOG("Setting Default Configuration");
		App->LoadPrefs(true);
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();

	ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
	if (ImGui::Button("Cancel", ImVec2(size.x / 3, 22)))
	{
		App->LoadPrefs();
		active = false;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndChild();
}

void Configuration::DrawApplication()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("APPLICATION").x / 2));
	ImGui::Text("APPLICATION");
	ImGui::Separator();
	ImGui::Separator();

	// App name
	static char app_name[120];
	strcpy_s(app_name, 120, App->GetAppName());
	if (ImGui::InputText("App Name", app_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		App->SetAppName(app_name);

	// Organization name
	static char org_name[120];
	strcpy_s(org_name, 120, App->GetOrganizationName());
	if (ImGui::InputText("Organization", org_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		App->SetOrganizationName(org_name);
	ImGui::NewLine();

	// Style
	const char* items[] = { "Black", "Classic", "Dark" };
	int item_current = (int)App->editor->GetStyle();
	ImGui::PushItemWidth(80);
	ImGui::Combo("Style", &item_current, items, IM_ARRAYSIZE(items));
	App->editor->ChangeStyle(item_current);

	// AutoSelect Windows
	ImGui::Checkbox("Auto-Select windows", &App->editor->is_auto_select);
	ImGui::NewLine();

	// VSync
	bool vsync = App->renderer->GetVSync();
	if (ImGui::Checkbox("Vertical Sync", &vsync))
		App->renderer->SetVSync(vsync);
	ImGui::NewLine();

	// Limit Framerate
	ImGui::Text("Limit Framerate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", App->GetFramerateLimit());
	int max_fps = App->GetFramerateLimit();
	ImGui::PushItemWidth(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x / 3));
	if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120))
		App->SetFramerateLimit(max_fps);

	// Plot Framerate
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
}

void Configuration::DrawMemory()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("MEMORY").x / 2));
	ImGui::Text("MEMORY");
	ImGui::Separator();
	ImGui::Separator();

	// Get Stats
	sMStats stats = m_getMemoryStatistics();
	static int speed = 0;
	static std::vector<float> memory(100);
	if (++speed > 20)
	{
		speed = 0;
		if (memory.size() == 100)
		{
			for (uint i = 0; i < 100 - 1; ++i)
				memory[i] = memory[i + 1];

			memory[100 - 1] = (float)stats.totalReportedMemory;
		}
		else
			memory.push_back((float)stats.totalReportedMemory);
	}

	// Plot Graph
	ImGui::PlotHistogram("##memory", &memory[0], memory.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

	// Stats Data
	IMGUI_PRINT("Total Reported Mem:", "%u", stats.totalReportedMemory);
	IMGUI_PRINT("Total Actual Mem:", "%u", stats.totalActualMemory);
	ImGui::NewLine();
	IMGUI_PRINT("Peak Reported Mem:", "%u", stats.peakReportedMemory);
	IMGUI_PRINT("Peak Actual Mem:", "%u", stats.peakActualMemory);
	ImGui::NewLine();
	IMGUI_PRINT("Accumulated Reported Mem:", "%u", stats.accumulatedReportedMemory);
	IMGUI_PRINT("Accumulated Actual Mem:", "%u", stats.accumulatedActualMemory);
	ImGui::NewLine();
	IMGUI_PRINT("Accumulated Alloc Unit Count:", "%u", stats.accumulatedAllocUnitCount);
	IMGUI_PRINT("Total Alloc Unit Count:", "%u", stats.totalAllocUnitCount);
	IMGUI_PRINT("Peak Alloc Unit Count:", "%u", stats.peakAllocUnitCount);
}

void Configuration::DrawHardware()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("HARDWARE").x / 2));
	ImGui::Text("HARDWARE");
	ImGui::Separator();
	ImGui::Separator();

	IMGUI_PRINT("CPUs:", "%u", info_hw.cpu_count);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Cache:", "%ukb", info_hw.l1_cachekb);
	ImGui::NewLine();

	IMGUI_PRINT("System RAM:", "%.1fGb", info_hw.ram_gb);
	ImGui::NewLine();

	float pos = ImGui::GetCursorPosX();
	IMGUI_PRINT("Caps:", "%s%s%s%s%s%s",
		info_hw.rdtsc ? "RDTSC," : "",
		info_hw.altivec ? "AltiVec," : "",
		info_hw.mmx ? "MMX," : "",
		info_hw.now3d ? "3DNow," : "",
		info_hw.sse ? "SSE," : "",
		info_hw.sse2 ? "SSE2," : "");
	ImGui::SameLine();
	ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x);
	IMGUI_PRINT("", "%s%s%s%s%s",
		info_hw.sse3 ? "SSE3," : "",
		info_hw.sse41 ? "SSE41," : "",
		info_hw.sse42 ? "SSE42," : "",
		info_hw.avx ? "AVX," : "",
		info_hw.avx2 ? "AVX2" : "");
	ImGui::NewLine();

	ImGui::Text("GPU:");
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Vendor", "%u", info_hw.gpu_vendor);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Device", "%u", info_hw.gpu_device);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Brand:", info_hw.gpu_brand);
	ImGui::NewLine();

	ImGui::Text("VRAM:");
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Budget:", "%.1f Mb", info_hw.vram_mb_budget);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Usage:", "%.1f Mb", info_hw.vram_mb_usage);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Available:", "%.1f Mb", info_hw.vram_mb_available);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Reserved:", "%.1f Mb", info_hw.vram_mb_reserved);
}

void Configuration::DrawSoftware()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("SOFTWARE").x / 2));
	ImGui::Text("SOFTWARE");
	ImGui::Separator();
	ImGui::Separator();

	// SDL
	SDL_version compiled;
	SDL_VERSION(&compiled);

	ImGui::BulletText("SDL Version:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);

	//// OpenGL
	//ImGui::BulletText("OpenGL Version:");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));

	//// Glew
	//ImGui::BulletText("Glew Version:");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glewGetString(GLEW_VERSION));

	//ImGui
	ImGui::BulletText("ImGui Version:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", IMGUI_VERSION);

	//// Devil
	//ImGui::BulletText("DevIL Version:");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", IL_VERSION);
}

void Configuration::DrawWindow()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("WINDOW").x / 2));
	ImGui::Text("WINDOW");
	ImGui::Separator();
	ImGui::Separator();

	//// Icon
	//static bool waiting_to_load_icon = false;

	//if (waiting_to_load_icon == true && App->editor->FileDialog("bmp"))
	//{
	//	const char* file = App->editor->CloseFileDialog();
	//	if (file != nullptr)
	//		App->window->SetIcon(file);
	//	waiting_to_load_icon = false;
	//}

	//ImGui::Text("Icon: ");
	//ImGui::SameLine();
	//if (ImGui::Selectable(App->window->GetIcon()))
	//	waiting_to_load_icon = true;

	// Brightness
	float brightness = App->window->GetBrightness();
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		App->window->SetBrightness(brightness);

	// Width & Height
	uint min_w, min_h, max_w, max_h;
	App->window->GetMaxMinSize(min_w, min_h, max_w, max_h);
	static int width = (int)App->window->GetWidth();
	static int height = (int)App->window->GetHeight();
	if (ImGui::DragInt("Width", &width, 1, min_w, max_w))
	{
		if ((uint)width > max_w)
			width = int(max_w);
		else if ((uint)width < min_w)
			width = int(min_w);

		App->window->SetWidth((uint)width);
	}
	if (ImGui::DragInt("Height", &height, 1, min_h, max_h))
	{
		if ((uint)height > max_h)
			height = max_h;
		else if ((uint)height < min_h)
			height = min_h;

		App->window->SetHeigth((uint)height);
	}
	ImGui::NewLine();

	// Refresh Rate
	ImGui::Text("Refresh rate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%u", App->window->GetRefreshRate());
	ImGui::NewLine();

	// Window Options
	bool fullscreen = App->window->IsFullscreen();
	bool resizable = App->window->IsResizable();
	bool borderless = App->window->IsBorderless();
	bool full_desktop = App->window->IsFullscreenDesktop();

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreen(fullscreen);

	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizable(resizable);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Restart to apply");

	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderless(borderless);

	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &full_desktop))
		App->window->SetFullScreenDesktop(full_desktop);
}

void Configuration::DrawInput()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("INPUT").x / 2));
	ImGui::Text("INPUT");
	ImGui::Separator();
	ImGui::Separator();

	// Mouse Position
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	ImGui::Text("Mouse Position:");
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_x);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
	ImGui::Text("x");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_y);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
	ImGui::Text("y");
	ImGui::NewLine();

	if (App->editor->focused_panel)
	{
		ImGui::Text("Mouse relative to %s:", App->editor->focused_panel->GetName());
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_x - App->editor->focused_panel->pos_x);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
		ImGui::Text("x");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_y - App->editor->focused_panel->pos_y);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
		ImGui::Text("y");
	}
	ImGui::NewLine();

	// Mouse Motion
	App->input->GetMouseMotion(mouse_x, mouse_y);
	ImGui::Text("Mouse Motion:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_x);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
	ImGui::Text("x");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mouse_y);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
	ImGui::Text("y");
	ImGui::NewLine();

	// Mouse Wheel
	int wheel = App->input->GetMouseWheel();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", wheel);

	//TODO : Add options to change mouse/camera speed in viewport
}

void Configuration::DrawFileSystem()
{
	// Title
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("FILE SYSTEM").x / 2));
	ImGui::Text("FILE SYSTEM");
	ImGui::Separator();
	ImGui::Separator();

	// Paths
	ImGui::Text("Base Path:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::SameLine();
	ImGui::TextWrapped(App->file_system->GetBasePath());
	ImGui::PopStyleColor();
	ImGui::NewLine();

	ImGui::Text("Read Paths:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::SameLine();
	ImGui::TextWrapped(App->file_system->GetReadPaths());
	ImGui::PopStyleColor();
	ImGui::NewLine();

	ImGui::Text("Write Path:");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::SameLine();
	ImGui::TextWrapped(App->file_system->GetWritePath());
	ImGui::PopStyleColor();
}

//---------------------------------
void Configuration::AddFPS(float fps, float ms)
{
	static uint count = 0;
	if (count == FPS_LOG_SIZE)
	{
		for (uint i = 0; i < FPS_LOG_SIZE - 1; ++i)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
	}
	else
		++count;

	fps_log[count - 1] = fps;
	ms_log[count - 1] = ms;
}

void Configuration::GetHardware()
{
	SDL_version version;
	SDL_GetVersion(&version);

	info_hw.ram_gb = (float)SDL_GetSystemRAM() / (1024.f);
	info_hw.cpu_count = SDL_GetCPUCount();
	info_hw.l1_cachekb = SDL_GetCPUCacheLineSize();
	info_hw.rdtsc = SDL_HasRDTSC() == SDL_TRUE;
	info_hw.altivec = SDL_HasAltiVec() == SDL_TRUE;
	info_hw.now3d = SDL_Has3DNow() == SDL_TRUE;
	info_hw.mmx = SDL_HasMMX() == SDL_TRUE;
	info_hw.sse = SDL_HasSSE() == SDL_TRUE;
	info_hw.sse2 = SDL_HasSSE2() == SDL_TRUE;
	info_hw.sse3 = SDL_HasSSE3() == SDL_TRUE;
	info_hw.sse41 = SDL_HasSSE41() == SDL_TRUE;
	info_hw.sse42 = SDL_HasSSE42() == SDL_TRUE;
	info_hw.avx = SDL_HasAVX() == SDL_TRUE;
	info_hw.avx2 = SDL_HasAVX2() == SDL_TRUE;

	uint vendor, device_id;
	std::wstring brand;
	unsigned __int64 video_mem_budget;
	unsigned __int64 video_mem_usage;
	unsigned __int64 video_mem_available;
	unsigned __int64 video_mem_reserved;

	if (getGraphicsDeviceInfo(&vendor, &device_id, &brand, &video_mem_budget, &video_mem_usage, &video_mem_available, &video_mem_reserved))
	{
		info_hw.gpu_vendor = vendor;
		info_hw.gpu_device = device_id;
		sprintf_s(info_hw.gpu_brand, 250, "%S", brand.c_str());
		info_hw.vram_mb_budget = float(video_mem_budget) / 1073741824.0f;
		info_hw.vram_mb_usage = float(video_mem_usage) / (1024.f * 1024.f * 1024.f);
		info_hw.vram_mb_available = float(video_mem_available) / (1024.f * 1024.f * 1024.f);
		info_hw.vram_mb_reserved = float(video_mem_reserved) / (1024.f * 1024.f * 1024.f);
	}
}

const hardware_info& Configuration::GetHardwareInfo() const
{
	unsigned __int64 video_mem_budget;
	unsigned __int64 video_mem_usage;
	unsigned __int64 video_mem_available;
	unsigned __int64 video_mem_reserved;

	if (getGraphicsDeviceInfo(nullptr, nullptr, nullptr, &video_mem_budget, &video_mem_usage, &video_mem_available, &video_mem_reserved))
	{
		info_hw.vram_mb_budget = float(video_mem_budget) / (1024.f * 1024.f);
		info_hw.vram_mb_usage = float(video_mem_usage) / (1024.f * 1024.f);
		info_hw.vram_mb_available = float(video_mem_available) / (1024.f * 1024.f);
		info_hw.vram_mb_reserved = float(video_mem_reserved) / (1024.f * 1024.f);
	}

	return info_hw;
}
