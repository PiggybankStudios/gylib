/*
File:   gy_string.h
Author: Taylor Robbins
Date:   09\24\2021
*/

#ifndef _GY_STRING_H
#define _GY_STRING_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_memory.h"
#include "gy_unicode.h"

struct MyStr_t
{
	u64 length;
	union
	{
		char* pntr;
		char* chars;
		u8* bytePntr;
	};
};

MyStr_t NewStr(u64 length, char* pntr)
{
	MyStr_t result;
	result.length = length;
	result.pntr = pntr;
	return result;
}
MyStr_t NewStr(char* nullTermStr)
{
	MyStr_t result;
	result.length = MyStrLength64(nullTermStr);
	result.pntr = nullTermStr;
	return result;
}
MyStr_t NewStr(const char* nullTermStr)
{
	MyStr_t result;
	result.length = MyStrLength64(nullTermStr);
	result.pntr = (char*)nullTermStr;
	return result;
}

// +--------------------------------------------------------------+
// |                       Assertion Macros                       |
// +--------------------------------------------------------------+
#define NotNullStr(strPntr) NotNull2(strPntr, strPntr->pntr)
#define NotNullStr_(strPntr) NotNull2_(strPntr, strPntr->pntr)
#define NotEmptyStr(strPntr) Assert(strPntr != nullptr && strPntr->pntr != nullptr && strPntr->pntr[0] != '\0')
#define NotEmptyStr_(strPntr) Assert_(strPntr != nullptr && strPntr->pntr != nullptr && strPntr->pntr[0] != '\0')

// +--------------------------------------------------------------+
// |                       MemArena Macros                        |
// +--------------------------------------------------------------+
#define AllocString(arena, strPntr)               NewStr((strPntr)->length,          AllocCharsAndFill  ((arena), (strPntr)->length, (strPntr)->pntr))
#define NewStringInArena(arena, length, charPntr) NewStr((length),                   AllocCharsAndFill  ((arena), (length),          (charPntr)))
#define NewStringInArenaNt(arena, nullTermStr)    NewStr(MyStrLength64(nullTermStr), AllocCharsAndFillNt((arena), (nullTermStr)))

// +--------------------------------------------------------------+
// |                Helpful Manipulation Functions                |
// +--------------------------------------------------------------+
MyStr_t StrSubstring(MyStr_t* target, u64 startIndex)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = target->length - startIndex;
	return result;
}
MyStr_t StrSubstring(MyStr_t* target, u64 startIndex, u64 endIndex)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex <= target->length);
	Assert(endIndex >= startIndex);
	Assert(endIndex <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = endIndex - startIndex;
	return result;
}
MyStr_t StrSubstringLength(MyStr_t* target, u64 startIndex, u64 length)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex + length <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = length;
	return result;
}

//TODO: These currently don't support unicode's idea of "lowercase/uppercase" equivalent characters
i32 StrCompareIgnoreCase(const MyStr_t* str1, const MyStr_t* str2, u64 compareLength)
{
	NotNullStr(str1);
	NotNullStr(str2);
	for (u64 cIndex = 0; cIndex < compareLength; cIndex++)
	{
		if (cIndex >= str1->length && cIndex >= str2->length)
		{
			return 0; //they were equal all the way up until they both ended early
		}
		else if (cIndex >= str1->length)
		{
			return -1; //str1 ended earlier than str2
		}
		else if (cIndex >= str2->length)
		{
			return 1; //str2 ended earlier than str1
		}
		else
		{
			char char1 = GetLowercaseAnsiiChar(str1->pntr[cIndex]);
			char char2 = GetLowercaseAnsiiChar(str2->pntr[cIndex]);
			if (char1 > char2)
			{
				return 1; //str1 comes after str2 alphabetically (str1 > str2)
			}
			else if (char2 > char1)
			{
				return -1; //str2 comes after str1 alphabetically (str1 < str2)
			}
			else { continue; }
		}
	}
	return 0;
}
i32 StrCompareIgnoreCase(const MyStr_t* str1, const MyStr_t* str2)
{
	if (str1->length > str2->length) { return 1; }
	if (str1->length < str2->length) { return -1; }
	return StrCompareIgnoreCase(str1, str2, str1->length);
}
i32 StrCompareIgnoreCase(const MyStr_t* str1, const char* nullTermStr, u64 compareLength)
{
	MyStr_t str2 = NewStr(nullTermStr);
	return StrCompareIgnoreCase(str1, &str2, compareLength);
}
i32 StrCompareIgnoreCase(const MyStr_t* str1, const char* nullTermStr)
{
	MyStr_t str2 = NewStr(nullTermStr);
	return StrCompareIgnoreCase(str1, &str2);
}
i32 StrCompareIgnoreCase(const char* str1, const char* str2, u64 compareLength)
{
	NotNull(str1);
	NotNull(str2);
	for (u64 cIndex = 0; cIndex < compareLength; cIndex++)
	{
		char char1 = GetLowercaseAnsiiChar(str1[cIndex]);
		char char2 = GetLowercaseAnsiiChar(str2[cIndex]);
		if (char1 == '\0' && char2 == '\0')
		{
			return 0; //they were equal all the way up until they both ended
		}
		else if (char1 == '\0')
		{
			return -1; //str1 ended earlier than str2
		}
		else if (char2 == '\0')
		{
			return 1; //str2 ended earlier than str1
		}
		else
		{
			if (char1 > char2)
			{
				return 1; //str1 comes after str2 alphabetically (str1 > str2)
			}
			else if (char2 > char1)
			{
				return -1; //str2 comes after str1 alphabetically (str1 < str2)
			}
			else { continue; }
		}
	}
	return 0;
}

MyStr_t* SplitString(MemArena_t* memArena, const MyStr_t* target, const MyStr_t* delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	NotNull(memArena);
	NotNullStr(target);
	NotEmptyStr(delineator);
	
	u64 numPieces = 0;
	u64 prevDelineator = 0;
	for (u64 cIndex = 0; cIndex <= target->length+1 - delineator->length; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator->length > target->length) { match = true; }
		else if (ignoreCase && StrCompareIgnoreCase(&target->pntr[cIndex], delineator->pntr, delineator->length) == 0) { match = true; }
		else if (!ignoreCase && MyStrCompare(&target->pntr[cIndex], delineator->pntr, delineator->length) == 0) { match = true; }
		if (match)
		{
			numPieces++;
			prevDelineator = cIndex + delineator->length;
			cIndex += delineator->length-1;
		}
	}
	AssertMsg(prevDelineator >= target->length, "We missed a piece at the end somehow? We should be going 1 past the end and considering it a match");
	
	if (numPieces == 0)
	{
		if (numPiecesOut != nullptr) { *numPiecesOut = 0; }
		return nullptr;
	}
	MyStr_t* results = AllocArray(memArena, MyStr_t, numPieces);
	NotNull(results);
	
	u64 pIndex = 0;
	prevDelineator = 0;
	for (u64 cIndex = 0; cIndex <= target->length+1 - delineator->length; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator->length > target->length) { match = true; }
		else if (ignoreCase && StrCompareIgnoreCase(&target->pntr[cIndex], delineator->pntr, delineator->length) == 0) { match = true; }
		else if (!ignoreCase && MyStrCompare(&target->pntr[cIndex], delineator->pntr, delineator->length) == 0) { match = true; }
		if (match)
		{
			Assert(pIndex < numPieces);
			Assert(cIndex >= prevDelineator);
			results[pIndex].pntr = &target->pntr[prevDelineator];
			results[pIndex].length = cIndex - prevDelineator;
			pIndex++;
			prevDelineator = cIndex + delineator->length;
			cIndex += delineator->length-1;
		}
	}
	AssertMsg(pIndex == numPieces, "Expected the same number of pieces in both runs through the loop");
	
	if (numPiecesOut != nullptr) { *numPiecesOut = numPieces; }
	return results;
}
MyStr_t* SplitString(MemArena_t* memArena, const MyStr_t* target, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t delineator = NewStr(delineatorNt);
	return SplitString(memArena, target, &delineator, numPiecesOut, ignoreCase);
}
MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, const MyStr_t* delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t target = NewStr(targetNt);
	return SplitString(memArena, &target, delineator, numPiecesOut, ignoreCase);
}
MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t target = NewStr(targetNt);
	MyStr_t delineator = NewStr(delineatorNt);
	return SplitString(memArena, &target, &delineator, numPiecesOut, ignoreCase);
}

#endif //  _GY_STRING_H
