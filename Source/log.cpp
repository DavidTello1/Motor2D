#pragma once
#include "Globals.h"
#include "Console.h"

#define GEOMETRY_LOG_STRING "[Geometry]"
#define DEBUG_LOG_STRING "[Debug]"
#define VERBOSE_LOG_STRING "[Verbose]"
#define ERROR_LOG_STRING "[Error]"
#define WARNING_LOG_STRING "[Warning]"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char final_log[4096];
	static char log_type[4096];

	static va_list  ap;
	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);

	// Filtering
	strcpy(log_type, format);
	strcat(log_type, "%c");
	vsprintf(final_log, log_type, ap);
	char char_type = final_log[strlen(final_log)-1];

	va_end(ap);

	//Default visual studio logging
	sprintf_s(final_log, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(final_log);

	//Console log system

	//Adding log string distinguishment in front of the log itself
	if (char_type == 'g')		sprintf_s(log_type, 4096, "%s ", GEOMETRY_LOG_STRING);
	else if (char_type == 'd')	sprintf_s(log_type, 4096, "%s ", DEBUG_LOG_STRING);
	else if (char_type == 'w')	sprintf_s(log_type, 4096, "%s ", WARNING_LOG_STRING);
	else if (char_type == 'e')	sprintf_s(log_type, 4096, "%s ", ERROR_LOG_STRING);
	else						sprintf_s(log_type, 4096, "%s ", VERBOSE_LOG_STRING);

	if (Console::EnableFileName)
	{
		// 92 stands for '\' character
		const char* short_file = strrchr(file, 92);

		sprintf_s(final_log, 4096, "%s%s(%d) : %s", log_type, short_file+1, line, tmp_string);
	}
	else
	{
		sprintf_s(final_log, 4096, "%s%s", log_type, tmp_string);
	}

	Console::AddLog(final_log);
}


//OLD CONSOLE
//// geometry logs
//if (logtype == 'g')
//{
//	if (Console::ShowGeometryLog)
//	{
//		sprintf_s(extendedlog, 4096, "[Geometry] %s(%d) : %s", short_file, line, tmp_string);
//		Console::AddLog(extendedlog);
//	} // else { ignore log }
//}
//// debug logs
//else if (logtype == 'd')
//{
//	if (Console::ShowDebugLog)
//	{
//		sprintf_s(extendedlog, 4096, "[Debug] %s(%d) : %s", short_file, line, tmp_string);
//		Console::AddLog(extendedlog);
//	} // else { ignore log }
//}
//// warning logs
//else if (logtype == 'w')
//{
//	if (Console::ShowDebugLog)
//	{
//		sprintf_s(extendedlog, 4096, "[Warn] %s(%d) : %s", short_file, line, tmp_string);
//		Console::AddLog(extendedlog);
//	}
//}
//else if (logtype == 'e') // errors can not be ignored, there's always something to learn
//{
//	sprintf_s(extendedlog, 4096, "[Error] %s(%d) : %s", short_file, line, tmp_string);
//	Console::AddLog(extendedlog);
//}
//// verbose logs
//else if (Console::ShowVerboseLog) {
//	sprintf_s(extendedlog, 4096, "[Verbose] %s(%d) : %s", short_file, line, tmp_string);
//	Console::AddLog(extendedlog);
//} // else { ignore log }