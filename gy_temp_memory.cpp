/*
File:   gy_temp_memory.cpp
Author: Taylor Robbins
Date:   09\25\2021
Description:
	** If you include this file it will define a global TempArena that should be
	** assigned to a memory arena for code to implicitly use as "Temporary Memory"
	
	** Alternatively you can set GetTempArena to your own function that decides where
	** TempArena should be sourced from. This function pointer is intentionally declared
	** static so that it will auto reset everytime the compilation unit (DLL) reloads.
	** If you are using your own function make sure you reset this function pointer
	** after every reload.
*/

#ifndef _GY_TEMP_MEMORY_CPP
#define _GY_TEMP_MEMORY_CPP

#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
#define GYLIB_GET_TEMP_ARENA_DEF(functionName) MemArena_t* functionName()
typedef GYLIB_GET_TEMP_ARENA_DEF(GetTempArena_f);

MemArena_t* TempArena = nullptr;
MemArena_t* GetTempArena_Stub()
{
	return TempArena;
}
static GetTempArena_f* GetTempArena = GetTempArena_Stub;

// +--------------------------------------------------------------+
// |                         Alloc Macros                         |
// +--------------------------------------------------------------+
#define TempStruct(structName)          (structName*)AllocMem(GetTempArena(), sizeof(structName))
#define TempArray(structName, numItems) (structName*)AllocMem(GetTempArena(), sizeof(structName) * (numItems))
#define TempBytes(numBytes)             (u8*)AllocMem(GetTempArena(), (numBytes))

#define TempPushMark() PushMemMark(GetTempArena())
#define TempPopMark()  PopMemMark(GetTempArena())

#define TempPrint(formatString, ...) PrintInArena(GetTempArena(), (formatString), ##__VA_ARGS__)
#define TempPrintVa(resultName, resultLengthName, formatString) PrintInArenaVa(GetTempArena(), resultName, resultLengthName, formatString)
#define TempPrintStr(formatString, ...) PrintInArenaStr(GetTempArena(), formatString, ##__VA_ARGS__)

#endif //  _GY_TEMP_MEMORY_CPP

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Globals
TempArena
@Functions
#define GYLIB_GET_TEMP_ARENA_DEF(functionName)
MemArena_t* GetTempArena()
#define TempStruct(structName)
#define TempArray(structName, numItems)
#define TempBytes(numBytes)
#define TempPushMark()
#define TempPopMark()
#define TempPrint(formatString, ...)
#define TempPrintVa(resultName, resultLengthName, formatString)
#define TempPrintStr(formatString, ...)
*/
