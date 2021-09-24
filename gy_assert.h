/*
File:   gy_assert.h
Author: Taylor Robbins
Date:   09\23\2021
Description:
	** This file contains definitions for the Assert and related macros
	
	If @GYLIB_ASSERTIONS_ENABLED is not defined it will default to true
	otherwise the truthiness of this #define will be considered in order
	to decide whether assertions should be checked or compiled out
	
	If @GYLIB_USE_ASSERT_FAILURE_FUNC is defined then the file will declare
	a function GyLibAssertFailure that you must implement somewhere
*/

#ifndef _GY_ASSERT_H
#define _GY_ASSERT_H

#ifndef GYLIB_ASSERTIONS_ENABLED
#define GYLIB_ASSERTIONS_ENABLED 1
#endif

#ifdef GYLIB_USE_ASSERT_FAILURE_FUNC
#define GYLIB_USE_ASSERT_FAILURE_FUNC 1
#else
#define GYLIB_USE_ASSERT_FAILURE_FUNC 0
#endif

// +--------------------------------------------------------------+
// |                   MyBreak and MyDebugBreak                   |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION
#define MyBreak() __debugbreak()
#elif OSX_COMPILATION
#define MyBreak() raise(SIGINT)
#elif LINUX_COMPILATION
#define MyBreak() raise(SIGINT)
#endif

#if DEBUG_BUILD
#define MyDebugBreak() MyBreak()
#else
#define MyDebugBreak() //nothing
#endif

// +--------------------------------------------------------------+
// |                    Actual Assertion Macro                    |
// +--------------------------------------------------------------+
#if GYLIB_ASSERTIONS_ENABLED

#define AssertMsg_(Expression, message) do { if (!(Expression)) { MyDebugBreak(); } } while(0)

#if GYLIB_USE_ASSERT_FAILURE_FUNC
extern void GyLibAssertFailure(const char* filePath, int lineNumber, const char* funcName, const char* expressionStr, const char* messageStr);
#define AssertMsg(Expression, message) do { if (!(Expression)) { GyLibAssertFailure(__FILE__, __LINE__, __func__, #Expression, (message)); MyDebugBreak(); } } while(0)
#else
#define AssertMsg(Expression, message) do { if (!(Expression)) { MyDebugBreak(); } } while(0)
#endif

#else //!GYLIB_ASSERTIONS_ENABLED

#define AssertMsg_(Expression, message) //nothing
#define AssertMsg(Expression, message)  //nothing

#endif //GYLIB_ASSERTIONS_ENABLED

// +--------------------------------------------------------------+
// |                      Assertion Aliases                       |
// +--------------------------------------------------------------+
#define Assert(Expression)  AssertMsg((Expression), nullptr)
#define Assert_(Expression) AssertMsg_((Expression), nullptr)

#if DEBUG_BUILD
#define DebugAssert(Expression)              Assert((Expression))
#define DebugAssert_(Expression)             Assert_((Expression))
#define DebugAssertMsg(Expression, message)  AssertMsg((Expression), (message))
#define DebugAssertMsg_(Expression, message) AssertMsg_((Expression), (message))
#else
#define DebugAssert(Expression)              //null
#define DebugAssert_(Expression)             //null
#define DebugAssertMsg(Expression, message)  //null
#define DebugAssertMsg_(Expression, message) //null
#endif

#define NotNull(variable)                                    Assert((variable) != nullptr)
#define NotNull2(variable1, variable2)                       Assert((variable1) != nullptr && (variable2) != nullptr)
#define NotNull3(variable1, variable2, variable3)            Assert((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr)
#define NotNull4(variable1, variable2, variable3, variable4) Assert((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr)

#define NotNull_(variable)                                    Assert_((variable) != nullptr)
#define NotNull2_(variable1, variable2)                       Assert_((variable1) != nullptr && (variable2) != nullptr)
#define NotNull3_(variable1, variable2, variable3)            Assert_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr)
#define NotNull4_(variable1, variable2, variable3, variable4) Assert_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr)

#define NotNullMsg(variable, message)                                    AssertMsg((variable) != nullptr, (message))
#define NotNull2Msg(variable1, variable2, message)                       AssertMsg((variable1) != nullptr && (variable2) != nullptr, (message))
#define NotNull3Msg(variable1, variable2, variable3, message)            AssertMsg((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr, (message))
#define NotNull4Msg(variable1, variable2, variable3, variable4, message) AssertMsg((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr, (message))

#define NotNullMsg_(variable, message)                                    AssertMsg_((variable) != nullptr, (message))
#define NotNull2Msg_(variable1, variable2, message)                       AssertMsg_((variable1) != nullptr && (variable2) != nullptr, (message))
#define NotNull3Msg_(variable1, variable2, variable3, message)            AssertMsg_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr, (message))
#define NotNull4Msg_(variable1, variable2, variable3, variable4, message) AssertMsg_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr, (message))


#endif //  _GY_ASSERT_H
