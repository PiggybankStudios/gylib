/*
File:   gy_std.h
Author: Taylor Robbins
Date:   09\14\2021
*/

#ifndef _GY_STD_H
#define _GY_STD_H

#include "gy_defines_check.h"

#if !GY_CUSTOM_STD_LIB

#include <stdbool.h>
#include <stdint.h>
#include <algorithm> //Used for min and max functions

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#if WINDOWS_COMPILATION
#include <intrin.h>
#elif OSX_COMPILATION
#include <signal.h>
#elif LINUX_COMPILATION
#include <signal.h>
#endif

#else //GY_CUSTOM_STD_LIB

// +--------------------------------------------------------------+
// |                Custom stdint Implementations                 |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION

#if (_MSC_VER < 1300)
	typedef signed char       int8_t;
	typedef signed short      int16_t;
	typedef signed int        int32_t;
	typedef unsigned char     uint8_t;
	typedef unsigned short    uint16_t;
	typedef unsigned int      uint32_t;
#else
	typedef signed __int8     int8_t;
	typedef signed __int16    int16_t;
	typedef signed __int32    int32_t;
	typedef unsigned __int8   uint8_t;
	typedef unsigned __int16  uint16_t;
	typedef unsigned __int32  uint32_t;
#endif

//TODO: Add support for all the other standard library functions we might use (like we did in gy_std_wasm.h)

#elif OSX_COMPILATION

//TODO: Add support for OSX

#elif LINUX_COMPILATION

//TODO: Add support for Linux
	
#elif WASM_COMPILATION
#include "gy_std_wasm.h"
#endif

#endif //GY_CUSTOM_STD_LIB

// +--------------------------------------------------------------+
// |                    Our Reroute Functions                     |
// +--------------------------------------------------------------+
#ifndef __cplusplus
#define bool   _Bool
#define false  0
#define true   1
#endif

#ifndef MyMemSet
#define MyMemSet(dest, value, length)     memset(dest, value, length)
#endif
#ifndef MyMemCompare
#define MyMemCompare(ptr1, ptr2, length)  memcmp(ptr1, ptr2, length)
#endif
#ifndef MyMemCopy
#define MyMemCopy(dest, source, length)   memcpy(dest, source, length)
#endif
#ifndef MyMemMove
#define MyMemMove(dest, source, length)   memmove(dest, source, length)
#endif
#ifndef MyStrCopyNt
#define MyStrCopyNt(dest, source)         strcpy(dest, source)
#endif
#ifndef MyStrCompareNt
#define MyStrCompareNt(str1, str2)        strcmp(str1, str2)
#endif
#ifndef MyStrCompare
#define MyStrCompare(str1, str2, length)  strncmp(str1, str2, length)
#endif
#ifndef MyStrLength
#define MyStrLength(str)                  strlen(str)
#endif
#ifndef MyStrLength32
#define MyStrLength32(str)                ((u32)strlen(str))
#endif
#ifndef MyStrLength64
#define MyStrLength64(str)                ((u64)strlen(str))
#endif
#ifndef MyWideStrLength
#define MyWideStrLength(str)              wcslen(str)
#endif
#ifndef MyWideStrLength32
#define MyWideStrLength32(str)            ((u32)wcslen(str))
#endif
#ifndef MyStrStrNt
#define MyStrStrNt(str1, str2)            strstr(str1, str2)
#endif
#ifndef MyVaListPrintf
	#if 0//TODO: Do we want to default back to vsnprintf in some cases?
	#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) vsnprintf_s(buffer, bufferSize, _TRUNCATE, formatStr, vaList)
	#else
	#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) vsnprintf(buffer, bufferSize, formatStr, vaList)
	#endif
#endif

#endif // _GY_STD_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
#define MyMemSet(dest, value, length)
#define MyMemCompare(ptr1, ptr2, length)
#define MyMemCopy(dest, source, length)
#define MyMemMove(dest, source, length)
#define MyStrCopyNt(dest, source)
#define MyStrCompareNt(str1, str2)
#define MyStrCompare(str1, str2, length)
#define MyStrLength(str)
#define MyStrLength32(str)
#define MyStrLength64(str)
#define MyWideStrLength(str)
#define MyWideStrLength32(str)
#define MyStrStrNt(str1, str2)
#define MyMalloc(size)
#define MyRealloc(pntr, newSize)
#define MyFree(size)
#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList)
*/
