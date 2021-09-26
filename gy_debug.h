/*
File:   gy_debug.h
Author: Taylor Robbins
Date:   09\23\2021
Description:
	** Holds types and macros for the gy library to send debug output.
	** In order to get debug output set GyLibDebugOutputFunc and GyLibDebugPrintFunc to functions defined by GYLIB_DEBUG_OUTPUT_HANDLER_DEF and GYLIB_DEBUG_PRINT_HANDLER_DEF
*/

#ifndef _GY_DEBUG_H
#define _GY_DEBUG_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

enum GyDbgLevel_t
{
	GyDbgLevel_Debug,
	GyDbgLevel_Info,
	GyDbgLevel_Warning,
	GyDbgLevel_Error,
};

const char* GetGyDbgLevelStr(GyDbgLevel_t gyDbgLevel)
{
	switch (gyDbgLevel)
	{
		case GyDbgLevel_Debug:   return "Debug";
		case GyDbgLevel_Info:    return "Info";
		case GyDbgLevel_Warning: return "Warning";
		case GyDbgLevel_Error:   return "Error";
		default: return "Unknown";
	};
}

#define GYLIB_DEBUG_OUTPUT_HANDLER_DEF(functionName) void functionName(const char* filePath, u32 lineNumber, const char* funcName, GyDbgLevel_t level, bool newLine, const char* message)
typedef GYLIB_DEBUG_OUTPUT_HANDLER_DEF(GyLibDebugOutput_f);
#define GYLIB_DEBUG_PRINT_HANDLER_DEF(functionName) void functionName(const char* filePath, u32 lineNumber, const char* funcName, GyDbgLevel_t level, bool newLine, const char* formatString, ...)
typedef GYLIB_DEBUG_PRINT_HANDLER_DEF(GyLibDebugPrint_f);

GYLIB_DEBUG_OUTPUT_HANDLER_DEF(GyLibDebugOutputHandler_Stub)
{
	UNREFERENCED(filePath);
	UNREFERENCED(lineNumber);
	UNREFERENCED(funcName);
	UNREFERENCED(level);
	UNREFERENCED(newLine);
	UNREFERENCED(message);
	//do nothing
}
GYLIB_DEBUG_PRINT_HANDLER_DEF(GyLibDebugPrintHandler_Stub)
{
	UNREFERENCED(filePath);
	UNREFERENCED(lineNumber);
	UNREFERENCED(funcName);
	UNREFERENCED(level);
	UNREFERENCED(newLine);
	UNREFERENCED(formatString);
	//do nothing
}
static GyLibDebugOutput_f* GyLibDebugOutputFunc = GyLibDebugOutputHandler_Stub;
static GyLibDebugPrint_f*  GyLibDebugPrintFunc  = GyLibDebugPrintHandler_Stub;

#define GyLibWriteAt(level, message)               GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, (level),            false, (message))
#define GyLibWriteLineAt(level, message)           GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, (level),            true,  (message))
#define GyLibPrintAt(level, formatString, ...)     GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, (level),            false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLineAt(level, formatString, ...) GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, (level),            true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_D(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Debug,   false, (message))
#define GyLibWriteLine_D(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Debug,   true,  (message))
#define GyLibPrint_D(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Debug,   false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_D(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Debug,   true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_I(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Info,    false, (message))
#define GyLibWriteLine_I(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Info,    true,  (message))
#define GyLibPrint_I(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Info,    false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_I(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Info,    true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_W(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Warning, false, (message))
#define GyLibWriteLine_W(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Warning, true,  (message))
#define GyLibPrint_W(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Warning, false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_W(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Warning, true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_E(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Error,   false, (message))
#define GyLibWriteLine_E(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, GyDbgLevel_Error,   true,  (message))
#define GyLibPrint_E(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Error,   false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_E(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, GyDbgLevel_Error,   true,  (formatString), ##__VA_ARGS__)

#endif //  _GY_DEBUG_H
