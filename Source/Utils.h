#pragma once

/* --- UTILS FOR IMGUI --- */
class Utils {
public:
	// --- Style
	static void SetImGuiStyle();

	// --- Dockspace
	static void Dockspace(float paddingTop = 0.0f);

	// --- Link
	static void Link(const char* url);
	static void Link(const char* text, const char* url);


};
