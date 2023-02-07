#include "Utils.h"

#include "Imgui/imgui.h"
#include <windows.h>

// -----------------------------------------
// --- STYLE ---
void Utils::SetImGuiStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();

    // --- Dimens
    style->Alpha = 1.0f;                                // Global alpha applies to everything in ImGui

    style->WindowPadding = ImVec2(8, 8);                // Padding within a window
    style->FramePadding = ImVec2(4, 3);                 // Padding within a framed rectangle (used by most widgets)
    style->ItemSpacing = ImVec2(8, 4);                  // Horizontal and vertical spacing between widgets/lines
    style->ItemInnerSpacing = ImVec2(4, 4);             // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    style->TouchExtraPadding = ImVec2(0, 0);            // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    style->IndentSpacing = 21.0f;                       // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    style->ScrollbarSize = 14.0f;                       // Width of the vertical scrollbar, Height of the horizontal scrollbar
    style->GrabMinSize = 10.0f;                         // Minimum width/height of a grab box for slider/scrollbar

    style->WindowBorderSize = 1.0f;                     // Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    style->ChildBorderSize = 1.0f;                      // Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    style->FrameBorderSize = 0.0f;                      // Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
    style->PopupBorderSize = 1.0f;                      // Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    style->TabBorderSize = 0.0f;                        // Thickness of border around tabs.

    style->WindowRounding = 0.0f;                       // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    style->ChildRounding = 0.0f;                        // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
    style->FrameRounding = 0.0f;                        // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    style->PopupRounding = 0.0f;                        // Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
    style->ScrollbarRounding = 0.0f;                    // Radius of grab corners rounding for scrollbar
    style->GrabRounding = 0.0f;                         // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    style->TabRounding = 0.0f;                          // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.

    style->WindowTitleAlign = ImVec2(0.0f, 0.5f);       // Alignment for title bar text
    style->WindowMenuButtonPosition = ImGuiDir_None;    // Position of the collapsing/docking button in the title bar (left/right). Defaults to ImGuiDir_Left.
    style->ColorButtonPosition = ImGuiDir_Left;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
    style->ButtonTextAlign = ImVec2(0.5f, 0.5f);        // Alignment of button text when button is larger than text.
    style->SelectableTextAlign = ImVec2(0.0f, 0.0f);    // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.

    style->WindowMinSize = ImVec2(32, 32);              // Minimum window size
    style->ColumnsMinSpacing = 6.0f;                    // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).

    style->TabMinWidthForUnselectedCloseButton = FLT_MAX;  // Minimum width for close button to appears on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.

    style->DisplayWindowPadding = ImVec2(19, 19);       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    style->DisplaySafeAreaPadding = ImVec2(3, 3);       // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.

    style->MouseCursorScale = 1.0f;                     // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    style->AntiAliasedLines = true;                     // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
    style->AntiAliasedFill = true;                      // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    style->CurveTessellationTol = 1.25f;                // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    style->CircleSegmentMaxError = 1.60f;               // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.


    // --- Colors
    ImVec4* colors = style->Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.17f, 0.81f, 0.68f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.17f, 0.81f, 0.68f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.02f, 0.25f, 0.22f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.02f, 0.40f, 0.34f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.02f, 0.25f, 0.22f, 0.30f);
}

// -----------------------------------------
// --- DOCKSPACE ---
void Utils::Dockspace(float paddingTop)
{
	// --- Viewport ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + paddingTop));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - paddingTop));
	ImGui::SetNextWindowViewport(viewport->ID);

	// --- Window Flags ---
	ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// --- Style Vars ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	// --- Dockspace ---
	ImGui::Begin("DockSpace", NULL, window_flags);
	ImGui::DockSpace(ImGui::GetID("MyDockspace"));
	ImGui::End();
	ImGui::PopStyleVar(3);
}

// -----------------------------------------
// --- LINK ---
void Utils::Link(const char* url)
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL); // Open Link
}

void Utils:: Link(const char* text, const char* url)
{
	const ImVec2 pos = ImGui::GetCursorPos();
	ImVec4 color = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);

	// --- Hover ---
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	}

	// --- Text ---
	ImGui::TextColored(color, text);
	ImGui::SameLine();

	// --- Invisible Button ---
	ImGui::SetCursorPos(pos);
	if (ImGui::InvisibleButton(text, ImGui::CalcTextSize(text)))
	{
		Link(url);
	}
}
