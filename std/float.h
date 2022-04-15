/*
File:   float.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#ifndef _FLOAT_H
#define _FLOAT_H

extern "C" int __flt_rounds();
#define FLT_ROUNDS (__flt_rounds())

// +--------------------------------------------------------------+
// |                       Numeric Defines                        |
// +--------------------------------------------------------------+
#define FLT_RADIX        2

#define FLT_TRUE_MIN     1.40129846432481707092e-45F
#define FLT_MIN          1.17549435082228750797e-38F
#define FLT_MAX          3.40282346638528859812e+38F
#define FLT_EPSILON      1.1920928955078125e-07F

#define FLT_MANT_DIG     24
#define FLT_MIN_EXP      (-125)
#define FLT_MAX_EXP      128
#define FLT_HAS_SUBNORM  1

#define FLT_DIG          6
#define FLT_DECIMAL_DIG  9
#define FLT_MIN_10_EXP   (-37)
#define FLT_MAX_10_EXP   38

#define DBL_TRUE_MIN     4.94065645841246544177e-324
#define DBL_MIN          2.22507385850720138309e-308
#define DBL_MAX          1.79769313486231570815e+308
#define DBL_EPSILON      2.22044604925031308085e-16

#define DBL_MANT_DIG     53
#define DBL_MIN_EXP      (-1021)
#define DBL_MAX_EXP      1024
#define DBL_HAS_SUBNORM  1

#define DBL_DIG          15
#define DBL_DECIMAL_DIG  17
#define DBL_MIN_10_EXP   (-307)
#define DBL_MAX_10_EXP   308

#define LDBL_HAS_SUBNORM 1
#define LDBL_DECIMAL_DIG DECIMAL_DIG

#if GY_WASM_STD_LIB
	//TODO: Is this really true for WASM?
	#define LDBL_TRUE_MIN 6.47517511943802511092443895822764655e-4966L
	#define LDBL_MIN 3.36210314311209350626267781732175260e-4932L
	#define LDBL_MAX 1.18973149535723176508575932662800702e+4932L
	#define LDBL_EPSILON 1.92592994438723585305597794258492732e-34L
	
	#define LDBL_MANT_DIG 113
	#define LDBL_MIN_EXP (-16381)
	#define LDBL_MAX_EXP 16384
	
	#define LDBL_DIG 33
	#define LDBL_MIN_10_EXP (-4931)
	#define LDBL_MAX_10_EXP 4932
	
	#define DECIMAL_DIG 36
#else
	#define LDBL_TRUE_MIN 6.47517511943802511092443895822764655e-4966L
	#define LDBL_MIN 3.36210314311209350626267781732175260e-4932L
	#define LDBL_MAX 1.18973149535723176508575932662800702e+4932L
	#define LDBL_EPSILON 1.92592994438723585305597794258492732e-34L
	
	#define LDBL_MANT_DIG 113
	#define LDBL_MIN_EXP (-16381)
	#define LDBL_MAX_EXP 16384
	
	#define LDBL_DIG 33
	#define LDBL_MIN_10_EXP (-4931)
	#define LDBL_MAX_10_EXP 4932
	
	#define DECIMAL_DIG 36
#endif

// TODO: Can we use these macros that seem to be defined by the compiler?
// #define __FLT_DENORM_MIN__ 1.40129846e-45F
// #define __FLT_HAS_DENORM__ 1
// #define __FLT_DIG__ 6
// #define __FLT_DECIMAL_DIG__ 9
// #define __FLT_EPSILON__ 1.19209290e-7F
// #define __FLT_HAS_INFINITY__ 1
// #define __FLT_HAS_QUIET_NAN__ 1
// #define __FLT_MANT_DIG__ 24
// #define __FLT_MAX_10_EXP__ 38
// #define __FLT_MAX_EXP__ 128
// #define __FLT_MAX__ 3.40282347e+38F
// #define __FLT_MIN_10_EXP__ (-37)
// #define __FLT_MIN_EXP__ (-125)
// #define __FLT_MIN__ 1.17549435e-38F
// #define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// #define __DBL_HAS_DENORM__ 1
// #define __DBL_DIG__ 15
// #define __DBL_DECIMAL_DIG__ 17
// #define __DBL_EPSILON__ 2.2204460492503131e-16
// #define __DBL_HAS_INFINITY__ 1
// #define __DBL_HAS_QUIET_NAN__ 1
// #define __DBL_MANT_DIG__ 53
// #define __DBL_MAX_10_EXP__ 308
// #define __DBL_MAX_EXP__ 1024
// #define __DBL_MAX__ 1.7976931348623157e+308
// #define __DBL_MIN_10_EXP__ (-307)
// #define __DBL_MIN_EXP__ (-1021)
// #define __DBL_MIN__ 2.2250738585072014e-308
// #define __LDBL_DENORM_MIN__ 6.47517511943802511092443895822764655e-4966L
// #define __LDBL_HAS_DENORM__ 1
// #define __LDBL_DIG__ 33
// #define __LDBL_DECIMAL_DIG__ 36
// #define __LDBL_EPSILON__ 1.92592994438723585305597794258492732e-34L
// #define __LDBL_HAS_INFINITY__ 1
// #define __LDBL_HAS_QUIET_NAN__ 1
// #define __LDBL_MANT_DIG__ 113
// #define __LDBL_MAX_10_EXP__ 4932
// #define __LDBL_MAX_EXP__ 16384
// #define __LDBL_MAX__ 1.18973149535723176508575932662800702e+4932L
// #define __LDBL_MIN_10_EXP__ (-4931)
// #define __LDBL_MIN_EXP__ (-16381)
// #define __LDBL_MIN__ 3.36210314311209350626267781732175260e-4932L

#endif //  _FLOAT_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
FLT_ROUNDS
FLT_RADIX
FLT_TRUE_MIN
FLT_MIN
FLT_MAX
FLT_EPSILON
FLT_MANT_DIG
FLT_MIN_EXP
FLT_MAX_EXP
FLT_HAS_SUBNORM
FLT_DIG
FLT_DECIMAL_DIG
FLT_MIN_10_EXP
FLT_MAX_10_EXP
DBL_TRUE_MIN
DBL_MIN
DBL_MAX
DBL_EPSILON
DBL_MANT_DIG
DBL_MIN_EXP
DBL_MAX_EXP
DBL_HAS_SUBNORM
DBL_DIG
DBL_DECIMAL_DIG
DBL_MIN_10_EXP
DBL_MAX_10_EXP
LDBL_HAS_SUBNORM
LDBL_DECIMAL_DIG
LDBL_TRUE_MIN
LDBL_MIN
LDBL_MAX
LDBL_EPSILON
LDBL_MANT_DIG
LDBL_MIN_EXP
LDBL_MAX_EXP
LDBL_DIG
LDBL_MIN_10_EXP
LDBL_MAX_10_EXP
DECIMAL_DIG
@Types
@Functions
int __flt_rounds()
*/
