/*
File:   gy_defines_check.h
Author: Taylor Robbins
Date:   09\14\2021
*/

#ifndef _GY_DEFINES_CHECK_H
#define _GY_DEFINES_CHECK_H

#if !defined(WINDOWS_COMPILATION) && !defined(OSX_COMPILATION) && !defined(LINUX_COMPILATION) && !defined(WASM_COMPILATION)
#error Either WINDOWS_COMPILATION, OSX_COMPILATION, LINUX_COMPILATION, or WASM_COMPILATION must be defined before including anything from Common
#endif

#ifdef WINDOWS_COMPILATION
#undef WINDOWS_COMPILATION
#define WINDOWS_COMPILATION 1
#else
#define WINDOWS_COMPILATION 0
#endif

#ifdef OSX_COMPILATION
#undef OSX_COMPILATION
#define OSX_COMPILATION 1
#else
#define OSX_COMPILATION 0
#endif

#ifdef LINUX_COMPILATION
#undef LINUX_COMPILATION
#define LINUX_COMPILATION 1
#else
#define LINUX_COMPILATION 0
#endif

#ifdef WASM_COMPILATION
#undef WASM_COMPILATION
#define WASM_COMPILATION 1
#else
#define WASM_COMPILATION 0
#endif

#if defined(GY_CUSTOM_STD_LIB)
#undef GY_CUSTOM_STD_LIB
#define GY_CUSTOM_STD_LIB 1
#else
#define GY_CUSTOM_STD_LIB 0
#endif

#endif //  _GY_DEFINES_CHECK_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
WINDOWS_COMPILATION
OSX_COMPILATION
LINUX_COMPILATION
GY_CUSTOM_STD_LIB
@Types
@Functions
*/
