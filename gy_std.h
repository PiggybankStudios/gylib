/*
File:   gy_std.h
Author: Taylor Robbins
Date:   09\14\2021
*/

#ifndef _GY_STD_H
#define _GY_STD_H

#include "gy_defines_check.h"

#if GY_STD_LIB_ALLOWED

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

#else //!GY_STD_LIB_ALLOWED

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
#elif OSX_COMPILATION
//TODO: Add support for OSX
#elif LINUX_COMPILATION
//TODO: Add support for Linux	
#endif

//TODO: Add custom implementations for all of our commonly used stdlib functions
// MyMemSet
// MyMemCompare
// MyMemCopy
// MyMemMove
// MyStrCopyNt
// MyStrCompareNt
// MyStrCompare
// MyStrLength
// MyStrLength32
// MyStrLength64
// MyWideStrLength
// MyWideStrLength32
// MyStrStrNt
// MyMalloc
// MyRealloc
// MyFree
// MyVaListPrintf

#endif //!GY_STD_LIB_ALLOWED

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
#ifndef MyMalloc
#define MyMalloc(size)                    malloc(size)
#endif
#ifndef MyRealloc
#define MyRealloc(pntr, newSize)          realloc(pntr, size)
#endif
#ifndef MyFree
#define MyFree(size)                      free(size)
#endif
#ifndef MyVaListPrintf
	#if 0//TODO: Do we want to default back to vsnprintf in some cases?
	#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) vsnprintf_s(buffer, bufferSize, _TRUNCATE, formatStr, vaList)
	#else
	#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) vsnprintf(buffer, bufferSize, formatStr, vaList)
	#endif
#endif

#endif // _GY_STD_H
