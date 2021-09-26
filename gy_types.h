/*
File:   gy_types.h
Author: Taylor Robbins
Date:   09\14\2021
Description:
	** Includes a bunch of typedefs and macros that support our preferred naming conventions in the codebase
*/

#ifndef _GY_TYPES_H
#define _GY_TYPES_H

#include "gy_defines_check.h"
#include "gy_std.h"

// +--------------------------------------------------------------+
// |                       Global Constants                       |
// +--------------------------------------------------------------+
//Actual Value of Pi: 3.1415926535897932384626433832795...
#define Pi64          3.14159265358979311599796346854      //accurate to 15 digits
#define Pi32          3.1415927410125732421875f            //accurate to 6 digits
//Actual Value of e:  2.7182818284590452353602874713526...
#define e64           2.71828182845904509079559829843      //accurate to 15 digits
#define e32           2.71828174591064453125f              //accurate to 6 digits

// +--------------------------------------------------------------+
// |                  Integer and Float Typedefs                  |
// +--------------------------------------------------------------+
typedef uint8_t     uint8;
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef uint64_t    uint64;

typedef int8_t 	    int8;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;

typedef int32_t	    bool32;

typedef float       real32;
typedef double      real64;

typedef uint8_t     flags8;
typedef uint16_t    flags16;
typedef uint32_t    flags32;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef float       r32;
typedef double      r64;

// +--------------------------------------------------------------+
// |                        General Macros                        |
// +--------------------------------------------------------------+
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define IsFlagSet(BitwiseField, Bit) (((BitwiseField) & (Bit)) != 0)
#define FlagSet(BitwiseField, Bit)   (BitwiseField) |= (Bit)
#define FlagUnset(BitwiseField, Bit) (BitwiseField) &= ~(Bit)
#define FlagToggle(BitwiseField, Bit) ((BitwiseField) ^= (Bit))
#define FlagSetTo(BitwiseField, Bit, condition) if (condition) { FlagSet((BitwiseField), (Bit)); } else { FlagUnset((BitwiseField), (Bit)); }

#define Kilobytes(value) ((value) * 1024UL)
#define Megabytes(value) (Kilobytes((value)) * 1024UL)
#define Gigabytes(value) (Megabytes((value)) * 1024UL)

#define Increment(variable, max)           if ((variable) < (max)) { (variable)++; } else { (variable) = (max); }
#define IncrementU8(variable)              if ((variable) < 0xFF) { (variable)++; } else { (variable) = 0xFF; }
#define IncrementU16(variable)             if ((variable) < 0xFFFF) { (variable)++; } else { (variable) = 0xFFFF; }
#define IncrementU32(variable)             if ((variable) < 0xFFFFFFFFUL) { (variable)++; } else { (variable) = 0xFFFFFFFFUL; }
#define IncrementU64(variable)             if ((variable) < 0xFFFFFFFFFFFFFFFFULL) { (variable)++; } else { (variable) = 0xFFFFFFFFFFFFFFFFULL; }
#define IncrementBy(variable, amount, max) if ((variable) + (amount) < (max) && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = (max); }
#define IncrementU8By(variable, amount)    if ((variable) + (amount) < 0xFF && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFF; }
#define IncrementU16By(variable, amount)   if ((variable) + (amount) < 0xFFFF && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFFFF; }
#define IncrementU32By(variable, amount)   if ((variable) + (amount) < 0xFFFFFFFFUL && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFFFFFFFFUL; }
#define IncrementU64By(variable, amount)   if ((variable) + (amount) < 0xFFFFFFFFFFFFFFFFULL && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFFFFFFFFFFFFFFFFULL; }
#define Decrement(variable)                if ((variable) > 0) { (variable)--; } else { (variable) = 0; }
#define DecrementBy(variable, amount)      if ((variable) >= (amount)) { (variable) -= (amount); } else { (variable) = 0; }

#define ToRadians32(degrees)		((degrees)/180.0f * Pi32)
#define ToRadians64(degrees)		((degrees)/180.0 * Pi64)
#define ToDegrees32(radians)		((radians)/Pi32 * 180.0f)
#define ToDegrees64(radians)		((radians)/Pi64 * 180.0)

#define ReverseByteArray(array, size) do                  \
{                                                         \
	u8 _tempByte; u32 _bIndex = 0;                        \
	while (_bIndex < (u32)(size)/2)                       \
	{                                                     \
		_tempByte = (array)[_bIndex];                     \
		(array)[_bIndex] = (array)[(size) - 1 - _bIndex]; \
		(array)[(size) - 1 - _bIndex] = _tempByte;        \
		_bIndex++;                                        \
	}                                                     \
} while(0)

#define IsVersionBelow(versionMajor, versionMinor, numberMajor, numberMinor) (((versionMajor) < (numberMajor)) || ((versionMajor) == (numberMajor) && (versionMinor) < (numberMinor)))
#define IsVersionAbove(versionMajor, versionMinor, numberMajor, numberMinor) (((versionMajor) > (numberMajor)) || ((versionMajor) == (numberMajor) && (versionMinor) > (numberMinor)))

#define IsEqualXor(variable1, variable2, condition1, condition2) (((variable1) == (condition1) && (variable2) == (condition2)) || ((variable1) == (condition2) && (variable2) == (condition1)))

#define FlipEndianU32(variable) variable = (((*(((const u8*)&(variable)) + 0)) & 0xFF) << 24) | (((*(((const u8*)&(variable)) + 1)) & 0xFF) << 16) | (((*(((const u8*)&(variable)) + 2)) & 0xFF) << 8) | (((*(((const u8*)&(variable)) + 3)) & 0xFF) << 0);
#define OnesComplimentU32(variable) (variable ^ 0xFFFFFFFFL)

#define STRUCT_VAR_SIZE(structureName, variableName) sizeof(((const structureName*)0)->variableName)
#define STRUCT_VAR_OFFSET(structureName, variableName) (u32)((const u8*)&((const structureName*)0)->variableName - (const u8*)((const structureName*)0))
#define STRUCT_VAR_END_OFFSET(structureName, variableName) (u32)(((const u8*)&((const structureName*)0)->variableName + sizeof(((const structureName*)0)->variableName)) - (const u8*)((const structureName*)0))
#define IS_VAR_IN_X_BYTES_OF_STRUCT(structureName, numBytes, variableName) ((numBytes) >= STRUCT_VAR_END_OFFSET((structureName), (variableName)))

#define VOID_FUNC_DEF(functionName) void functionName()
typedef VOID_FUNC_DEF(VoidFunc_f);
#define BOOL_FUNC_DEF(functionName) bool functionName()
typedef BOOL_FUNC_DEF(BoolFunc_f);
#define I32_FUNC_DEF(functionName) i32 functionName()
typedef I32_FUNC_DEF(I32Func_f);

#define UNREFERENCED(varName) (void)(varName)

// +--------------------------------------------------------------+
// |                   Packed and Export Macros                   |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION
	#define PACKED(class_to_pack) __pragma( pack(push, 1) ) class_to_pack __pragma(pack(pop))
	#define START_PACK  __pragma(pack(push, 1))
	#define END_PACK    __pragma(pack(pop))
	#define ATTR_PACKED //nothing
	#define EXPORT __declspec(dllexport)
	#define IMPORT __declspec(dllimport)
	#define __func__ __FUNCTION__
#elif OSX_COMPILATION
	#define PACKED(class_to_pack) class_to_pack __attribute__((__packed__))
	#define START_PACK  //nothing
	#define END_PACK    //nothing
	#define ATTR_PACKED __attribute__((__packed__))
	//TODO: Figure out how to do EXPORT and IMPORT on OSX
#else
	#define PACKED(class_to_pack) class_to_pack __attribute__((__packed__))
	#define START_PACK  //nothing
	#define END_PACK    //nothing
	#define ATTR_PACKED __attribute__((__packed__))
	//TODO: Figure out how to do EXPORT and IMPORT on OSX
#endif

// +--------------------------------------------------------------+
// |                        Memset Macros                         |
// +--------------------------------------------------------------+
#if GY_STD_LIB_ALLOWED
#if WINDOWS_COMPILATION
#include <string.h>
#elif OSX_COMPILATION
//TODO: Add support for OSX compilation
#elif LINUX_COMPILATION
//TODO: Add support for OSX compilation
#endif

#define ClearArray(Array)      MyMemSet((Array), '\0', sizeof((Array)))
#define ClearStruct(Structure) MyMemSet(&(Structure), '\0', sizeof((Structure)))
#define ClearPointer(Pointer)  MyMemSet((Pointer), '\0', sizeof(*(Pointer)));

#endif //GY_STD_LIB_ALLOWED

#endif //  _GY_TYPES_H
