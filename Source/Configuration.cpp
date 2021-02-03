#include "Application.h"
#include "Configuration.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"

#include "Assets.h"

#include "Imgui/imgui_internal.h"
#include "gpudetect/DeviceId.h"
#include "Glew/include/glew.h"
//#include "Devil/include/IL/il.h"

#include "mmgr/mmgr.h"

Configuration::Configuration() : Panel("Configuration", ICON_CONFIGURATION, 1), fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	current_layout = "Default";
	selected_layout = current_layout;
	layouts = GetLayouts();

	active = true; //***CHANGE TO FALSE
	flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

	GetHardware(); //init hardware detection
	info_hw = GetHardwareInfo();
}

Configuration::~Configuration()
{
}

void Configuration::Draw()
{
	pos_x = ImGui::GetWindowPos().x;
	pos_y = ImGui::GetWindowPos().y;

	// Child Index
	ImGui::BeginChild("Index", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 0), true);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 15));
	static int selected = 0;
	if (ImGui::Selectable("Application", selected == 0)) {
		curr_index = Index::APPLICATION;
		layouts = GetLayouts();
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
	else if (ImGui::Selectable("Window", selected == 3)) {
		curr_index = Index::WINDOW;
		selected = 3;
	}
	else if (ImGui::Selectable("Input", selected == 4)) {
		curr_index = Index::INPUT_DRAW;
		selected = 4;
	}
	else if (ImGui::Selectable("File System", selected == 5)) {
		curr_index = Index::FILESYSTEM;
		selected = 5;
	}
	else if (ImGui::Selectable("Resources", selected == 6)) {
		curr_index = Index::RESOURCES;
		selected = 6;
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
		DrawResources();
		break;
	default:
		break;
	}

	// Buttons
	static ImVec2 size = ImGui::GetContentRegionAvail();
	static float pos = 0.0f;

	ImGui::SetCursorPos(ImVec2(size.x / 3, float(height - 57))); // accept
	pos = ImGui::GetCursorPosX();
	if (ImGui::Button("Accept", ImVec2(size.x / 3, 22)))
	{
		LOG("Saved Configuration", 'v');
		App->SavePrefs(current_layout.c_str());
		active = false;
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();

	ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1); // cancel
	pos = ImGui::GetCursorPosX();
	if (ImGui::Button("Cancel", ImVec2(size.x / 3, 22))) 
	{
		App->LoadPrefs(current_layout.c_str());
		active = false;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndChild();
}

void Configuration::Save(Config* config) const
{
}

void Configuration::Load(Config* config)
{
}

//--- DRAWS ---
void Configuration::DrawApplication()
{
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

	// Limit Framerate
	ImGui::Text("Limit Framerate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", App->GetFramerateLimit());
	int max_fps = App->GetFramerateLimit();
	ImGui::PushItemWidth(190);
	if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120))
		App->SetFramerateLimit(max_fps);
	ImGui::SameLine(0.0f, 20.0f);

	// VSync
	static bool vsync = App->renderer->GetVSync();
	if (ImGui::Checkbox("Vertical Sync", &vsync))
		App->renderer->SetVSync(vsync);

	// Plot Framerate
	static char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	// Layout
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	ImGui::Text("Layout:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), current_layout.c_str());

	ImGui::BeginChild("Layout List", ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y - 30), true);
	for (std::string layout : layouts)
	{
		if (ImGui::Selectable(layout.c_str(), selected_layout == layout))
			selected_layout = layout;
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::BeginChild("Layout Options", ImVec2(0, ImGui::GetContentRegionAvail().y - 30));
	static bool is_save_layout = false;
	static float size = ImGui::GetContentRegionAvail().x;

	float pos = ImGui::GetCursorPosX();
	if (ImGui::Button("Load", ImVec2(size / 3, 0)))
	{
		if (App->LoadPrefs(selected_layout.c_str()))
			current_layout = selected_layout;
		layouts = GetLayouts();
	}

	//if (selected_layout == "Default")
	//{
	//	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	//	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	//}
	ImGui::SameLine();
	ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
	pos = ImGui::GetCursorPosX();
	if (ImGui::Button("Save", ImVec2(size / 3, 0)))
	{
		App->SavePrefs(selected_layout.c_str());
		current_layout = selected_layout;
		layouts = GetLayouts();
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(pos + ImGui::GetItemRectSize().x + 1);
	pos = ImGui::GetCursorPosX();
	if (ImGui::Button("Delete", ImVec2(size / 3, 0)))
	{
		App->file_system->Remove(std::string(SETTINGS_FOLDER + selected_layout + ".json").c_str());

		if (selected_layout == current_layout)
			current_layout = selected_layout = "Default";
		layouts = GetLayouts();
	}
	//else if (selected_layout == "Default")
	//{
	//	ImGui::PopItemFlag();
	//	ImGui::PopStyleVar();
	//}

	ImGui::Separator();
	static char buffer[180];
	if (ImGui::Button("New Layout"))
	{
		is_save_layout = true;
		strcpy_s(buffer, 180, "New Layout");
	}

	if (is_save_layout)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::SetNextItemWidth(size * 2 / 3);
		if (ImGui::InputText("##LayoutName", buffer, 180, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			App->SavePrefs(buffer);
			current_layout = selected_layout = buffer;
			layouts = GetLayouts();
			is_save_layout = false;
		}		

		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::Button("Add", ImVec2(size / 3, 0)))
		{
			App->SavePrefs(buffer);
			current_layout = selected_layout = buffer;
			layouts = GetLayouts();
			is_save_layout = false;
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void Configuration::DrawMemory()
{
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
	ImGui::NewLine();

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
	IMGUI_PRINT("Vendor:", "%u", info_hw.gpu_vendor);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	IMGUI_PRINT("Device:", "%u", info_hw.gpu_device);
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


void Configuration::DrawWindow()
{
	// Width & Height
	uint min_w, min_h, max_w, max_h;
	App->window->GetMaxMinSize(min_w, min_h, max_w, max_h);
	int width = (int)App->window->GetWidth();
	int height = (int)App->window->GetHeight();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::SetNextItemWidth(200);
	if (ImGui::DragInt("##Width", &width, 1, min_w, max_w))
	{
		if ((uint)width > max_w)
			width = int(max_w);
		else if ((uint)width < min_w)
			width = int(min_w);

		App->window->SetWidth((uint)width);
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::Text("Width");

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::SetNextItemWidth(200);
	if (ImGui::DragInt("##Height", &height, 1, min_h, max_h))
	{
		if ((uint)height > max_h)
			height = max_h;
		else if ((uint)height < min_h)
			height = min_h;

		App->window->SetHeigth((uint)height);
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::Text("Height");
	ImGui::NewLine();

	// Refresh Rate
	ImGui::Text("Refresh rate:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%u", App->window->GetRefreshRate());
	ImGui::NewLine();

	// Window Options
	static bool fullscreen = App->window->IsFullscreen();
	static bool resizable = App->window->IsResizable();
	static bool borderless = App->window->IsBorderless();
	static bool full_desktop = App->window->IsFullscreenDesktop();

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreen(fullscreen);
	ImGui::SameLine(0.0f, 30.0f);

	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizable(resizable);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Restart to apply");

	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderless(borderless);
	ImGui::SameLine(0.0f, 30.0f);

	if (ImGui::Checkbox("Full Desktop", &full_desktop))
		App->window->SetFullScreenDesktop(full_desktop);
}

void Configuration::DrawInput()
{
	//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

	// Mouse Position
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	ImGui::Text("Mouse Position:");
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

	// Mouse Wheel
	int wheel = App->input->GetMouseWheel();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", wheel);
	ImGui::NewLine();

	// Focused Panel
	ImGui::Text("Focused Panel:");
	if (App->editor->focused_panel)
	{
		const char* panel_name = App->editor->focused_panel->GetName();
		if (panel_name == "###Console")
			panel_name = "Console";

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), panel_name);
	}

	// Local Mouse Position
	App->input->GetMousePosition(mouse_x, mouse_y);
	ImGui::Text("Local Mouse Position:");
	if (App->editor->focused_panel)
	{
		float local_pos_x = mouse_x - App->editor->focused_panel->pos_x;
		float local_pos_y = mouse_y - App->editor->focused_panel->pos_y;
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.f", local_pos_x);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
		ImGui::Text("x");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.f", local_pos_y);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
		ImGui::Text("y");
	}
	ImGui::NewLine();

	// Input Log
	ImGui::Text("Input Log");
	ImGui::BeginChild("InputLog", ImVec2(0, ImGui::GetContentRegionAvail().y - 30), true);
	

	ImGui::EndChild();
}

void Configuration::DrawFileSystem()
{
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

void Configuration::DrawResources()
{
	// Selected Resource
	uint image = 0;
	//if (selected_resource)
	//	image = selected_resource->texture;
	ImGui::Image((ImTextureID)image, ImVec2(120,120), ImVec2(0, 1), ImVec2(1, 0)); // Image
	ImGui::SameLine();

	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::Text("Name:");
	//if (selected_resource)
	//{
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selected_resource->name);
	//}

	ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 20));
	ImGui::Text("UID:");
	//if (selected_resource)
	//{
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selected_resource->UID);
	//}

	ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 40));
	ImGui::Text("Size:");
	//if (selected_resource)
	//{
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selected_resource->size);
	//}

	ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 60));
	ImGui::Text("Times Loaded:");
	//if (selected_resource)
	//{
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selected_resource->times_loaded);
	//}

	ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 80));
	ImGui::Text("Original File:");
	//if (selected_resource)
	//{
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selected_resource->original_file);
	//}

	ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 100));
	ImGui::Text("Exported File:");
	//if (selected_resource)
	//{
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selected_resource->exported_file);
	//}

	// Resources List
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::BeginChild("ResourcesList", ImVec2(0, ImGui::GetContentRegionAvail().y - 30), true);
	ImGui::EndChild();
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

std::vector<std::string> Configuration::GetLayouts()
{
	layouts.clear();
	if (App->file_system->Exists(SETTINGS_FOLDER))
	{
		std::vector<std::string> file_list, dir_list;
		App->file_system->GetFolderContent(SETTINGS_FOLDER, file_list, dir_list);

		for (std::string file : file_list)
		{
			if (App->editor->panel_assets->GetExtension(file.c_str()) == "json")
			{
				std::string name = file.substr(0, file.find("."));
				layouts.push_back(name);
			}
		}
	}
	return layouts;
}
