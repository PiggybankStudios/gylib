/*
File:   stdarg.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#include <stdint.h>

#ifndef _STDARG_H
#define _STDARG_H

// +--------------------------------------------------------------+
// |                     Function-Like Macros                     |
// +--------------------------------------------------------------+
#define va_start(args, formatStr) __builtin_va_start(args, formatStr)
#define va_end(args)              __builtin_va_end(args)
#define va_arg(args, index)       __builtin_va_arg(args, index)
#define va_copy(dest, src)        __builtin_va_copy(dest, src)

#endif //  _STDARG_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
#define va_start(args, formatStr)
#define va_end(args)
#define va_arg(args, formatStr)
#define va_copy(dest, src)
*/
