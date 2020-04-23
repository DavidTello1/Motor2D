#pragma once
#include "Panel.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_LOG_STRING "[Debug]" //used for engine/editor logs
#define GEOMETRY_LOG_STRING "[Geometry]" //used for loading/unloading assets
#define ERROR_LOG_STRING "[Error]" //used for errors
#define WARNING_LOG_STRING "[Warning]" //used for warnings
#define VERBOSE_LOG_STRING "[Verbose]" //used for everything else

class Console : public Panel
{
public:
	Console();
	~Console();

	// Portable helpers
	static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
	static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
	static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); }
	static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

	static void UpdateFilters();
	void Draw();
	void Shortcuts();

	static void ClearLog()
	{
		for (int i = 0; i < Items.Size; ++i)
			free(Items[i]);
		Items.clear();
	}

	static void AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		// FIXME-OPT
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		Items.push_back(Strdup(buf));
	}

public:
	static const uint default_width = 1400;
	static const uint default_height = 220;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 560;

	static char InputBuf[256];
	static ImVector<char*> Items;
	static ImVector<char*> History;

	static int HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	static ImGuiTextFilter Filter;
	static ImGuiTextFilter Searcher;

	static bool	AutoScroll;
	static bool	ShowVerboseLog;
	static bool	ShowGeometryLog;
	static bool	ShowDebugLog;
	static bool	ShowWarningLog;
	static bool	EnableFileName;
	static bool	ScrollToBottom;
	static bool ShowSearch;
	static bool ClearOnPlay;
};
