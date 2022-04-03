/*
File:   stddef.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#include <stdint.h>

#ifndef _STDDEF_H
#define _STDDEF_H

// +--------------------------------------------------------------+
// |                       NULL and nullptr                       |
// +--------------------------------------------------------------+
#if __cplusplus < 201103L
#define nullptr 0
#endif

#if __cplusplus >= 201103L
#define NULL nullptr
#elif defined(__cplusplus)
#define NULL 0L
#else
#define NULL ((void*)0)
#endif

// +--------------------------------------------------------------+
// |                        offsetof Macro                        |
// +--------------------------------------------------------------+
#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t)((char*)&(((type*)0)->member) - (char*)0))
#endif

#endif //  _STDDEF_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
NULL
@Types
@Functions
#define offsetof(type, member)
*/
