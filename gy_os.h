/*
File:   gy_os.h
Author: Taylor Robbins
Date:   08\09\2023
Description:
	** A collection of platform-dependent operations that we'd like to be able
	** to do on all shipping platforms in a consistent way.
*/

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_debug.h"
#include "gy_memory.h"
#include "gy_scratch_arenas.h"
#include "gy_string.h"

#ifndef _GY_OS_H
#define _GY_OS_H

// +--------------------------------------------------------------+
// |                      Error Enumeration                       |
// +--------------------------------------------------------------+
enum OsError_t
{
	OsError_None = 0,
	OsError_UnsupportedPlatform,
	OsError_FailedToAllocateMemory,
	OsError_EmptyPath,
	OsError_ExePathTooLong,
	OsError_NumErrors,
};
const char* GetOsErrorStr(OsError_t enumValue)
{
	switch (enumValue)
	{
		case OsError_None:                   return "None";
		case OsError_UnsupportedPlatform:    return "UnsupportedPlatform";
		case OsError_FailedToAllocateMemory: return "FailedToAllocateMemory";
		case OsError_EmptyPath:              return "EmptyPath";
		case OsError_ExePathTooLong:         return "ExePathTooLong";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                      GetExecutablePath                       |
// +--------------------------------------------------------------+
#if GYLIB_SCRATCH_ARENA_AVAILABLE

#define MAX_EXECUTABLE_PATH_LENGTH 1024

// Always returns path with forward slashes: /
MyStr_t OsGetExecutablePath(MemArena_t* memArena, OsError_t* errorOut = nullptr)
{
	NotNull(memArena);
	SetOptionalOutPntr(errorOut, OsError_None);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		MemArena_t* scratch = GetScratchArena(memArena);
		char* tempBuffer = AllocArray(scratch, char, MAX_EXECUTABLE_PATH_LENGTH);
		NotNull(tempBuffer);
		DWORD pathLength = GetModuleFileNameA(
			0,
			tempBuffer,
			MAX_EXECUTABLE_PATH_LENGTH
		);
		FreeScratchArena(scratch);
		
		if (pathLength <= 0)
		{
			SetOptionalOutPntr(errorOut, OsError_EmptyPath);
			return MyStr_Empty;
		}
		if (pathLength >= MAX_EXECUTABLE_PATH_LENGTH-1)
		{
			GyLibPrintLine_W("Our executable path is %u characters or more. Please put the executable in a shorter directory", MAX_EXECUTABLE_PATH_LENGTH-1);
			SetOptionalOutPntr(errorOut, OsError_ExePathTooLong);
			return MyStr_Empty;
		}
		
		char* resultBuffer = AllocArray(memArena, char, pathLength+1);
		if (resultBuffer == nullptr)
		{
			SetOptionalOutPntr(errorOut, OsError_FailedToAllocateMemory);
			return MyStr_Empty;
		}
		
		DWORD resultLength = GetModuleFileNameA(
			0,
			resultBuffer,
			pathLength+1
		);
		Assert(resultLength == pathLength);
		resultBuffer[resultLength] = '\0';
		
		MyStr_t result = NewStr(resultLength, resultBuffer);
		StrReplaceInPlace(result, "\\", "/");
		return result;
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	// #elif LINUX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |     Unsupported Platform     |
	// +==============================+
	#else
	#error GetExecutablePath does not support the current platform yet!
	#endif
}

#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

// +--------------------------------------------------------------+
// |                     GetWorkingDirectory                      |
// +--------------------------------------------------------------+
// Always returns path with forward slashes: /
// Guarantees that the path ends with a forward slash
MyStr_t OsGetWorkingDirectory(MemArena_t* memArena, OsError_t* errorOut = nullptr)
{
	NotNull(memArena);
	SetOptionalOutPntr(errorOut, OsError_None);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		DWORD bufferSizeNeeded = GetCurrentDirectory(0, nullptr);
		if (bufferSizeNeeded <= 0)
		{
			SetOptionalOutPntr(errorOut, OsError_EmptyPath);
			return MyStr_Empty;
		}
		
		char* resultBuffer = AllocArray(memArena, char, bufferSizeNeeded+1);
		if (resultBuffer == nullptr)
		{
			SetOptionalOutPntr(errorOut, OsError_FailedToAllocateMemory);
			return MyStr_Empty;
		}
		
		DWORD resultLength = GetCurrentDirectoryA(
			bufferSizeNeeded,
			resultBuffer
		);
		Assert(resultLength == bufferSizeNeeded-1);
		
		if (resultBuffer[resultLength] != '\\' && resultBuffer[resultLength] != '/')
		{
			resultBuffer[resultLength] = '\\';
			resultLength++;
		}
		else
		{
			ShrinkMem(memArena, resultBuffer, bufferSizeNeeded+1, bufferSizeNeeded);
		}
		
		resultBuffer[resultLength] = '\0';
		MyStr_t result = NewStr(resultLength, resultBuffer);
		StrReplaceInPlace(result, "\\", "/");
		return result;
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	// #elif LINUX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	#else
	#error GetWorkingDirectory does not support the current platform yet!
	#endif
}

#endif //  _GY_OS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Functions
MyStr_t OsGetExecutablePath(MemArena_t* memArena)
MyStr_t OsGetWorkingDirectory(MemArena_t* memArena)
*/