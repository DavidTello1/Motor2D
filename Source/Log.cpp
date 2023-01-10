#include "Application.h"

#include <windows.h>
#include <stdio.h>

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_log[4096];
	static char final_log[4096];
	static char log_type[4096];

	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_log, 4096, format, ap);

	// Filtering
	strcpy_s(log_type, format);
	strcat_s(log_type, "%c");
	vsprintf_s(final_log, log_type, ap);
	char char_type = final_log[strlen(final_log) - 1];

	va_end(ap);

	// Default visual studio logging
	sprintf_s(final_log, 4096, "\n%s(%d) : %s", file, line, tmp_log);
	OutputDebugString(final_log);

	//--- Console log ---
	if (App)
		App->AddLog(char_type, tmp_log);
}
