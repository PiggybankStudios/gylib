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
#define QuarterPi64    (Pi64/4.0)
#define ThirdPi64      (Pi64/3.0)
#define HalfPi64       (Pi64/2.0)
#define ThreeHalfsPi64 (Pi64*(3.0/2.0))
#define TwoPi64        (2*Pi64)
#define QuarterPi32    (Pi32/4.0f)
#define ThirdPi32      (Pi32/3.0f)
#define HalfPi32       (Pi32/2.0f)
#define ThreeHalfsPi32 (Pi32*(3.0f/2.0f))
#define TwoPi32        (2*Pi32)
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

#define FlagEnumSet(BitwiseField, Bit, enumType, castType)   (BitwiseField) = (enumType)(((castType)(BitwiseField)) | (castType)(Bit))
#define FlagEnumUnset(BitwiseField, Bit, enumType, castType) (BitwiseField) = (enumType)(((castType)(BitwiseField)) & ~((castType)(Bit)))
#define FlagEnumToggle(BitwiseField, Bit, enumType, castType) (BitwiseField) = (enumType)(((castType)(BitwiseField)) ^ ((castType)(Bit)))
#define FlagEnumSetTo(BitwiseField, Bit, condition, enumType, castType) if (condition) { FlagEnumSet((BitwiseField), (Bit), (enumType), (castType)); } else { FlagEnumUnset((BitwiseField), (Bit), (enumType), (castType)); }

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

#if WINDOWS_COMPILATION
#define UNUSED(varName)        (void)(varName)
#define UNREFERENCED(varName)  (void)(varName)
#else
#define UNUSED(varName)        (void)sizeof(varName)
#define UNREFERENCED(varName)  (void)sizeof(varName)
#endif

#define SWAP_VARIABLES(varType, var1, var2) do { varType tempVarWithLongNameThatWontConflict = (var2); (var2) = (var1); (var1) = tempVarWithLongNameThatWontConflict; } while(0)
#define SWAP_POINTERS(varType, pntr1, pntr2) do                                \
{                                                                              \
	varType tempVarWithLongNameThatWontConflict;                               \
	MyMemCopy(&tempVarWithLongNameThatWontConflict, (pntr2), sizeof(varType)); \
	MyMemCopy((pntr2), (pntr1), sizeof(varType));                              \
	MyMemCopy((pntr1), &tempVarWithLongNameThatWontConflict, sizeof(varType)); \
	varType* tempPntrWithLongNameThatWontConflict;                             \
	tempPntrWithLongNameThatWontConflict = (pntr2);                            \
	(pntr2) = (pntr1);                                                         \
	(pntr1) = tempPntrWithLongNameThatWontConflict;                            \
} while(0)

#define EXTERN_C_START extern "C" {
#define EXTERN_C_END }

// +--------------------------------------------------------------+
// |                   Packed and Export Macros                   |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION
	#define PACKED(class_to_pack) __pragma( pack(push, 1) ) class_to_pack __pragma(pack(pop))
	#define START_PACK()  __pragma(pack(push, 1))
	#define END_PACK()    __pragma(pack(pop))
	#define ATTR_PACKED //nothing
	#define EXPORT __declspec(dllexport)
	#define IMPORT __declspec(dllimport)
	#define __func__ __FUNCTION__
#elif OSX_COMPILATION
	#define PACKED(class_to_pack) class_to_pack __attribute__((__packed__))
	#define START_PACK()  //nothing
	#define END_PACK()    //nothing
	#define ATTR_PACKED __attribute__((__packed__))
	//TODO: Figure out how to do EXPORT and IMPORT on OSX
#else
	#define PACKED(class_to_pack) class_to_pack __attribute__((__packed__))
	#define START_PACK()  //nothing
	#define END_PACK()    //nothing
	#define ATTR_PACKED __attribute__((__packed__))
	//TODO: Figure out how to do EXPORT and IMPORT on OSX
#endif

// +--------------------------------------------------------------+
// |                        Memset Macros                         |
// +--------------------------------------------------------------+
#define ClearArray(Array)      MyMemSet((Array), '\0', sizeof((Array)))
#define ClearStruct(Structure) MyMemSet(&(Structure), '\0', sizeof((Structure)))
#define ClearPointer(Pointer)  MyMemSet((Pointer), '\0', sizeof(*(Pointer)));

#endif //  _GY_TYPES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Pi64
Pi32
QuarterPi64
ThirdPi64
HalfPi64
ThreeHalfsPi64
TwoPi64
QuarterPi32
ThirdPi32
HalfPi32
ThreeHalfsPi32
TwoPi32
e64
e32
EXPORT
IMPORT
__func__
@Types
VoidFunc_f
BoolFunc_f
I32Func_f
@Functions
#define ArrayCount(Array)
#define IsFlagSet(BitwiseField, Bit)
#define FlagSet(BitwiseField, Bit)
#define FlagUnset(BitwiseField, Bit)
#define FlagToggle(BitwiseField, Bit)
#define FlagSetTo(BitwiseField, Bit, condition)
#define FlagEnumSet(BitwiseField, Bit, enumType, castType)
#define FlagEnumUnset(BitwiseField, Bit, enumType, castType)
#define FlagEnumToggle(BitwiseField, Bit, enumType, castType)
#define FlagEnumSetTo(BitwiseField, Bit, condition, enumType, castType)
#define Kilobytes(value)
#define Megabytes(value)
#define Gigabytes(value)
#define Increment(variable, max)
#define IncrementU8(variable)
#define IncrementU16(variable)
#define IncrementU32(variable)
#define IncrementU64(variable)
#define IncrementBy(variable, amount, max)
#define IncrementU8By(variable, amount)
#define IncrementU16By(variable, amount)
#define IncrementU32By(variable, amount)
#define IncrementU64By(variable, amount)
#define Decrement(variable)
#define DecrementBy(variable, amount)
#define ToRadians32(degrees)
#define ToRadians64(degrees)
#define ToDegrees32(radians)
#define ToDegrees64(radians)
#define ReverseByteArray(array, size)
#define IsVersionBelow(versionMajor, versionMinor, numberMajor, numberMinor)
#define IsVersionAbove(versionMajor, versionMinor, numberMajor, numberMinor)
#define IsEqualXor(variable1, variable2, condition1, condition2)
#define FlipEndianU32(variable)
#define OnesComplimentU32(variable)
#define STRUCT_VAR_SIZE(structureName, variableName)
#define STRUCT_VAR_OFFSET(structureName, variableName)
#define STRUCT_VAR_END_OFFSET(structureName, variableName)
#define IS_VAR_IN_X_BYTES_OF_STRUCT(structureName, numBytes, variableName)
#define VOID_FUNC_DEF(functionName)
#define BOOL_FUNC_DEF(functionName)
#define I32_FUNC_DEF(functionName)
#define UNUSED(varName)
#define UNREFERENCED(varName)
#define SWAP_VARIABLES(varType, var1, var2)
#define SWAP_POINTERS(varType, pntr1, pntr2)
#define PACKED(class_to_pack)
#define START_PACK()
#define END_PACK()
#define ClearArray(Array)
#define ClearStruct(Structure)
#define ClearPointer(Pointer)
*/
