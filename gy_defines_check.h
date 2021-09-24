/*
File:   gy_defines_check.h
Author: Taylor Robbins
Date:   09\14\2021
*/

#ifndef _GY_DEFINES_CHECK_H
#define _GY_DEFINES_CHECK_H

#if !defined(WINDOWS_COMPILATION) && !defined(OSX_COMPILATION) && !defined(LINUX_COMPILATION)
#error Either WINDOWS_COMPILATION, OSX_COMPILATION, or LINUX_COMPILATION must be defined before including anything from Common
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

#if defined(GY_STD_LIB_ALLOWED)
#undef GY_STD_LIB_ALLOWED
#define GY_STD_LIB_ALLOWED 1
#else
#define GY_STD_LIB_ALLOWED 0
#endif

#endif //  _GY_DEFINES_CHECK_H
