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
MyStr_t NewStr(u64 length, const char* pntr)
{
	MyStr_t result;
	result.length = length;
	result.pntr = (char*)pntr;
	return result;
}
MyStr_t NewStr(char* nullTermStr)
{
	MyStr_t result;
	result.length = ((nullTermStr != nullptr) ? MyStrLength64(nullTermStr) : 0);
	result.pntr = nullTermStr;
	return result;
}
MyStr_t NewStr(const char* nullTermStr)
{
	MyStr_t result;
	result.length = ((nullTermStr != nullptr) ? MyStrLength64(nullTermStr) : 0);
	result.pntr = (char*)nullTermStr;
	return result;
}

#define MyStr_Empty NewStr((u64)0, (char*)nullptr)

bool IsStrEmpty(MyStr_t target)
{
	Assert(target.pntr != nullptr || target.length == 0);
	return (target.length == 0);
}
bool IsStrNullTerminated(const MyStr_t* target)
{
	NotNull(target);
	if (target->pntr == nullptr) { return false; }
	return (target->pntr[target->length] == '\0');
}

// +--------------------------------------------------------------+
// |                       Assertion Macros                       |
// +--------------------------------------------------------------+
#define NotNullStr(strPntr)   Assert ((strPntr) != nullptr && ((strPntr)->pntr != nullptr || (strPntr)->length == 0))
#define NotNullStr_(strPntr)  Assert_((strPntr) != nullptr && ((strPntr)->pntr != nullptr || (strPntr)->length == 0))
#define NotEmptyStr(strPntr)  Assert ((strPntr) != nullptr && (strPntr)->pntr != nullptr && (strPntr)->length > 0)
#define NotEmptyStr_(strPntr) Assert_((strPntr) != nullptr && (strPntr)->pntr != nullptr && (strPntr)->length > 0)

#define AssertNullTerm(strPntr)  Assert(IsStrNullTerminated(strPntr))
#define AssertNullTerm_(strPntr) Assert_(IsStrNullTerminated(strPntr))

// +--------------------------------------------------------------+
// |                       MemArena Macros                        |
// +--------------------------------------------------------------+
#define AllocString(arena, strPntr)               NewStr((strPntr)->length,          AllocCharsAndFill  ((arena), (strPntr)->length, (strPntr)->pntr))
#define NewStringInArena(arena, length, charPntr) NewStr((length),                   AllocCharsAndFill  ((arena), (length),          (charPntr)))
#define NewStringInArenaNt(arena, nullTermStr)    NewStr(MyStrLength64(nullTermStr), AllocCharsAndFillNt((arena), (nullTermStr)))

#define FreeString(arena, strPntr) do { NotNullStr(strPntr); if ((strPntr)->pntr != nullptr) { FreeMem((arena), (strPntr)->pntr, (strPntr)->length+1); (strPntr)->pntr = nullptr; (strPntr)->length = 0; } } while(0)

MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...)
{
	NotNull(arena);
	NotNull(formatString);
	
	char* result = nullptr;
	va_list args;
	
	va_start(args, formatString);
	int length = MyVaListPrintf(result, 0, formatString, args); //Measure first
	Assert(length >= 0);
	va_end(args);
	
	result = AllocArray(arena, char, length+1); //Allocate
	if (result == nullptr) { return MyStr_Empty; }
	
	va_start(args, formatString);
	MyVaListPrintf(result, (size_t)(length+1), formatString, args); //Real printf
	va_end(args);
	
	result[length] = '\0';
	
	return NewStr((u64)length, result);
}

// +--------------------------------------------------------------+
// |                   Unicode String Functions                   |
// +--------------------------------------------------------------+
#ifdef _GY_UNICODE_H
u8 GetCodepointForUtf8Str(MyStr_t str, u64 index, u32* codepointOut)
{
	Assert(index <= str.length);
	return GetCodepointForUtf8(str.length - index, str.pntr + index, codepointOut);
}

MyStr_t ConvertWideStrToUtf8(MemArena_t* memArena, const wchar_t* wideStrPntr, u64 wideStrLength)
{
	Assert(wideStrPntr != nullptr || wideStrLength == 0);
	MyStr_t result = MyStr_Empty;
	for (u8 pass = 0; pass < 2; pass++)
	{
		u64 byteIndex = 0;
		
		u8 encodeBuffer[UTF8_MAX_CHAR_SIZE];
		for (u64 cIndex = 0; cIndex < wideStrLength; cIndex++)
		{
			wchar_t wideChar = wideStrPntr[cIndex];
			//TODO: Should we just straight convert wide characters to unicode points by casting to u32!?
			u8 encodeSize = GetUtf8BytesForCode((u32)wideChar, &encodeBuffer[0], false);
			if (encodeSize == 0)
			{
				return result;
			}
			if (result.pntr != nullptr)
			{
				Assert(result.length >= byteIndex + encodeSize);
				MyMemCopy(&result.pntr[byteIndex], &encodeBuffer[0], encodeSize);
			}
			byteIndex += encodeSize;
		}
		
		if (pass == 0)
		{
			result.length = byteIndex;
			if (memArena == nullptr) { return result; }
			result.pntr = AllocArray(memArena, char, result.length+1);
			NotNull(result.pntr);
		}
		else
		{
			Assert(byteIndex == result.length);
			result.pntr[result.length] = '\0';
		}
	}
	return result;
}
MyStr_t ConvertWideStrToUtf8Nt(MemArena_t* memArena, const wchar_t* nullTermWideStr)
{
	NotNull(nullTermWideStr);
	return ConvertWideStrToUtf8(memArena, nullTermWideStr, MyWideStrLength(nullTermWideStr));
}
#endif //_GY_UNICODE_H

// +--------------------------------------------------------------+
// |                Helpful Manipulation Functions                |
// +--------------------------------------------------------------+
u64 TrimLeadingWhitespace(MyStr_t* target, bool trimNewLines = false)
{
	NotNullStr(target);
	u64 result = 0;
	while (target->length > 0)
	{
		if (target->pntr[0] == ' ' || target->pntr[0] == '\t' ||
			(trimNewLines && (target->pntr[0] == '\n' || target->pntr[0] == '\r')))
		{
			target->length--;
			target->pntr++;
			result++;
		}
		else { break; }
	}
	return result;
}
u64 TrimTrailingWhitespace(MyStr_t* target, bool trimNewLines = false)
{
	NotNullStr(target);
	u64 result = 0;
	while (target->length > 0)
	{
		if (target->pntr[target->length-1] == ' ' || target->pntr[target->length-1] == '\t' ||
			(trimNewLines && (target->pntr[target->length-1] == '\n' || target->pntr[target->length-1] == '\r')))
		{
			target->length--;
			result++;
		}
		else { break; }
	}
	return result;
}
u64 TrimWhitespace(MyStr_t* target, bool trimNewLines = false)
{
	NotNullStr(target);
	u64 result = 0;
	result += TrimLeadingWhitespace(target, trimNewLines);
	result += TrimTrailingWhitespace(target, trimNewLines);
	return result;
}

bool FindNextCharInStr(MyStr_t target, u64 startIndex, MyStr_t searchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	NotNullStr(&target);
	bool inString = false;
	u32 previousCodepoint = 0;
	for (u64 cIndex = startIndex; cIndex < target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(target, cIndex, &codepoint);
		if (codepointSize == 0) { cIndex++; continue; } //invalid utf-8 encoding in target
		for (u64 sIndex = 0; sIndex < searchCharsStr.length; )
		{
			u32 searchCodepoint = 0;
			u8 searchCodepointSize = GetCodepointForUtf8Str(searchCharsStr, sIndex, &searchCodepoint);
			DebugAssert(searchCodepointSize > 0);
			if (searchCodepointSize == 0) { return false; } //invalid utf-8 encoding in searchCharsStr
			if (codepoint == searchCodepoint && !inString)
			{
				if (indexOut != nullptr) { *indexOut = cIndex; }
				return true;
			}
			sIndex += searchCodepointSize;
		}
		if (ignoreCharsInQuotes && codepoint == '"' && !(inString && previousCodepoint == '\\'))
		{
			inString = !inString;
		}
		previousCodepoint = codepoint;
		cIndex += codepointSize;
	}
	return false;
}
bool FindNextCharInStr(MyStr_t target, u64 startIndex, const char* nullTermSearchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	return FindNextCharInStr(target, startIndex, NewStr(nullTermSearchCharsStr), indexOut, ignoreCharsInQuotes);
}
bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, MyStr_t knownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	NotNullStr(&target);
	bool inString = false;
	u32 previousCodepoint = 0;
	for (u64 cIndex = startIndex; cIndex < target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(target, cIndex, &codepoint);
		if (codepointSize == 0) { cIndex++; continue; } //invalid utf-8 encoding in target
		bool isUnknownChar = true;
		for (u64 sIndex = 0; sIndex < knownCharsStr.length; )
		{
			u32 knownCodepoint = 0;
			u8 knownCodepointSize = GetCodepointForUtf8Str(knownCharsStr, sIndex, &knownCodepoint);
			DebugAssert(knownCodepointSize > 0);
			if (knownCodepointSize == 0) { return false; } //invalid utf-8 encoding in knownCharsStr
			if (codepoint == knownCodepoint && !inString)
			{
				isUnknownChar = false;
				break;
			}
			sIndex += knownCodepointSize;
		}
		if (isUnknownChar)
		{
			if (indexOut != nullptr) { *indexOut = cIndex; }
			return true;
		}
		if (ignoreCharsInQuotes && codepoint == '"' && !(inString && previousCodepoint == '\\'))
		{
			inString = !inString;
		}
		previousCodepoint = codepoint;
		cIndex += codepointSize;
	}
	return false;
}
bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, const char* nullTermKnownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	return FindNextUnknownCharInStr(target, startIndex, NewStr(nullTermKnownCharsStr), indexOut, ignoreCharsInQuotes);
}
bool FindNextWhitespaceInStr(MyStr_t target, u64 startIndex, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	NotNullStr(&target);
	bool inString = false;
	u32 previousCodepoint = 0;
	for (u64 cIndex = startIndex; cIndex < target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(target, cIndex, &codepoint);
		if (codepointSize == 0) { cIndex++; continue; } //invalid utf-8 encoding in target
		if (IsCharWhitespace(codepoint))
		{
			if (indexOut != nullptr) { *indexOut = cIndex; }
			return true;
		}
		if (ignoreCharsInQuotes && codepoint == '"' && !(inString && previousCodepoint == '\\'))
		{
			inString = !inString;
		}
		previousCodepoint = codepoint;
		cIndex += codepointSize;
	}
	return false;
}

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

MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2)
{
	NotNull(memArena);
	NotNullStr(&str1);
	NotNullStr(&str2);
	
	MyStr_t result;
	result.length = str1.length + str2.length;
	result.pntr = AllocArray(memArena, char, result.length + 1);
	NotNull(result.pntr);
	
	MyMemCopy(&result.pntr[0], str1.pntr, str1.length);
	MyMemCopy(&result.pntr[str1.length], str2.pntr, str2.length);
	result.pntr[result.length] = '\0';
	
	return result;
}
MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2, MyStr_t str3)
{
	NotNull(memArena);
	NotNullStr(&str1);
	NotNullStr(&str2);
	NotNullStr(&str3);
	
	MyStr_t result;
	result.length = str1.length + str2.length + str3.length;
	result.pntr = AllocArray(memArena, char, result.length + 1);
	NotNull(result.pntr);
	
	MyMemCopy(&result.pntr[0], str1.pntr, str1.length);
	MyMemCopy(&result.pntr[str1.length], str2.pntr, str2.length);
	MyMemCopy(&result.pntr[str1.length + str2.length], str3.pntr, str3.length);
	result.pntr[result.length] = '\0';
	
	return result;
}
MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2, MyStr_t str3, MyStr_t str4)
{
	NotNull(memArena);
	NotNullStr(&str1);
	NotNullStr(&str2);
	NotNullStr(&str3);
	NotNullStr(&str4);
	
	MyStr_t result;
	result.length = str1.length + str2.length + str3.length + str4.length;
	result.pntr = AllocArray(memArena, char, result.length + 1);
	NotNull(result.pntr);
	
	MyMemCopy(&result.pntr[0], str1.pntr, str1.length);
	MyMemCopy(&result.pntr[str1.length], str2.pntr, str2.length);
	MyMemCopy(&result.pntr[str1.length + str2.length], str3.pntr, str3.length);
	MyMemCopy(&result.pntr[str1.length + str2.length + str3.length], str4.pntr, str4.length);
	result.pntr[result.length] = '\0';
	
	return result;
}

bool StrEquals(MyStr_t target, MyStr_t comparison)
{
	NotNullStr(&target);
	NotNullStr(&comparison);
	if (target.pntr == nullptr && comparison.pntr == nullptr) { return true; }
	if (target.length != comparison.length) { return false; }
	if (target.length == 0 && comparison.length == 0) { return true; }
	if (target.pntr == nullptr || comparison.pntr == nullptr) { AssertMsg(false, "this case is only for malformed MyStr_t structs (size != 0 but pntr is nullptr)"); return false; }
	return (MyStrCompare(target.pntr, comparison.pntr, target.length) == 0);
}
bool StrEquals(MyStr_t target, const char* comparisonNt)
{
	MyStr_t comparisonStr = NewStr(comparisonNt);
	return StrEquals(target, comparisonStr);
}
bool StrEquals(MyStr_t target, u64 comparisonLength, const char* comparisonPntr)
{
	MyStr_t comparisonStr = NewStr(comparisonLength, comparisonPntr);
	return StrEquals(target, comparisonStr);
}
bool StrEquals(const char* comparisonNt, MyStr_t target)
{
	MyStr_t comparisonStr = NewStr(comparisonNt);
	return StrEquals(comparisonStr, target);
}
bool StrEquals(u64 comparisonLength, const char* comparisonPntr, MyStr_t target)
{
	MyStr_t comparisonStr = NewStr(comparisonLength, comparisonPntr);
	return StrEquals(comparisonStr, target);
}

//TODO: These currently don't support unicode's idea of "lowercase/uppercase" equivalent characters
i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2, u64 compareLength)
{
	NotNullStr(&str1);
	NotNullStr(&str2);
	for (u64 cIndex = 0; cIndex < compareLength; cIndex++)
	{
		if (cIndex >= str1.length && cIndex >= str2.length)
		{
			return 0; //they were equal all the way up until they both ended early
		}
		else if (cIndex >= str1.length)
		{
			return -1; //str1 ended earlier than str2
		}
		else if (cIndex >= str2.length)
		{
			return 1; //str2 ended earlier than str1
		}
		else
		{
			char char1 = GetLowercaseAnsiiChar(str1.pntr[cIndex]);
			char char2 = GetLowercaseAnsiiChar(str2.pntr[cIndex]);
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
i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2)
{
	if (str1.length > str2.length) { return 1; }
	if (str1.length < str2.length) { return -1; }
	return StrCompareIgnoreCase(str1, str2, str1.length);
}
i32 StrCompareIgnoreCase(MyStr_t str1, const char* nullTermStr, u64 compareLength)
{
	MyStr_t str2 = NewStr(nullTermStr);
	return StrCompareIgnoreCase(str1, str2, compareLength);
}
i32 StrCompareIgnoreCase(MyStr_t str1, const char* nullTermStr)
{
	MyStr_t str2 = NewStr(nullTermStr);
	return StrCompareIgnoreCase(str1, str2);
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

bool StrStartsWith(MyStr_t str, MyStr_t prefix, bool ignoreCase = false)
{
	NotNullStr_(&str);
	NotNullStr_(&prefix);
	if (prefix.length == 0) { return true; }
	if (str.length < prefix.length) { return false; }
	if (ignoreCase)
	{
		if (StrCompareIgnoreCase(str, prefix, prefix.length) == 0) { return true; }
	}
	else
	{
		if (MyStrCompare(str.pntr, prefix.pntr, prefix.length) == 0) { return true; }
	}
	return false;
}
bool StrStartsWith(MyStr_t str, const char* nullTermPrefixStr, bool ignoreCase = false)
{
	NotNull_(nullTermPrefixStr);
	return StrStartsWith(str, NewStr(nullTermPrefixStr), ignoreCase);
}
bool StrStartsWith(const char* nullTermStr, MyStr_t prefix, bool ignoreCase = false)
{
	NotNull_(nullTermStr);
	return StrStartsWith(NewStr(nullTermStr), prefix, ignoreCase);
}
bool StrStartsWith(const char* nullTermStr, const char* nullTermPrefixStr, bool ignoreCase = false)
{
	NotNull_(nullTermStr);
	NotNull_(nullTermPrefixStr);
	return StrStartsWith(NewStr(nullTermStr), NewStr(nullTermPrefixStr), ignoreCase);
}

bool StrEndsWith(MyStr_t str, MyStr_t suffix, bool ignoreCase = false)
{
	NotNullStr_(&str);
	NotNullStr_(&suffix);
	if (suffix.length == 0) { return true; }
	if (str.length < suffix.length) { return false; }
	if (ignoreCase)
	{
		if (StrCompareIgnoreCase(StrSubstring(&str, str.length - suffix.length), suffix, suffix.length) == 0) { return true; }
	}
	else
	{
		if (MyStrCompare(&str.pntr[str.length - suffix.length], suffix.pntr, suffix.length) == 0) { return true; }
	}
	return false;
}
bool StrEndsWith(MyStr_t str, const char* nullTermSuffix, bool ignoreCase = false)
{
	return StrEndsWith(str, NewStr(nullTermSuffix), ignoreCase);
}
bool StrEndsWith(const char* nullTermStr, MyStr_t suffix, bool ignoreCase = false)
{
	return StrEndsWith(NewStr(nullTermStr), suffix, ignoreCase);
}
bool StrEndsWith(const char* nullTermStr, const char* nullTermSuffix, bool ignoreCase = false)
{
	return StrEndsWith(NewStr(nullTermStr), NewStr(nullTermSuffix), ignoreCase);
}
//TODO: Add StrEndsWith

MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	NotNull(memArena);
	NotNullStr(&target);
	NotEmptyStr(&delineator);
	
	u64 numPieces = 0;
	u64 prevDelineator = 0;
	for (u64 cIndex = 0; cIndex <= target.length+1 - delineator.length; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator.length > target.length) { match = true; }
		else if (ignoreCase && StrCompareIgnoreCase(&target.pntr[cIndex], delineator.pntr, delineator.length) == 0) { match = true; }
		else if (!ignoreCase && MyStrCompare(&target.pntr[cIndex], delineator.pntr, delineator.length) == 0) { match = true; }
		if (match)
		{
			numPieces++;
			prevDelineator = cIndex + delineator.length;
			cIndex += delineator.length-1;
		}
	}
	AssertMsg(prevDelineator >= target.length, "We missed a piece at the end somehow? We should be going 1 past the end and considering it a match");
	
	if (numPieces == 0)
	{
		if (numPiecesOut != nullptr) { *numPiecesOut = 0; }
		return nullptr;
	}
	MyStr_t* results = AllocArray(memArena, MyStr_t, numPieces);
	NotNull(results);
	
	u64 pIndex = 0;
	prevDelineator = 0;
	for (u64 cIndex = 0; cIndex <= target.length+1 - delineator.length; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator.length > target.length) { match = true; }
		else if (ignoreCase && StrCompareIgnoreCase(&target.pntr[cIndex], delineator.pntr, delineator.length) == 0) { match = true; }
		else if (!ignoreCase && MyStrCompare(&target.pntr[cIndex], delineator.pntr, delineator.length) == 0) { match = true; }
		if (match)
		{
			Assert(pIndex < numPieces);
			Assert(cIndex >= prevDelineator);
			results[pIndex].pntr = &target.pntr[prevDelineator];
			results[pIndex].length = cIndex - prevDelineator;
			pIndex++;
			prevDelineator = cIndex + delineator.length;
			cIndex += delineator.length-1;
		}
	}
	AssertMsg(pIndex == numPieces, "Expected the same number of pieces in both runs through the loop");
	
	if (numPiecesOut != nullptr) { *numPiecesOut = numPieces; }
	return results;
}
MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t delineator = NewStr(delineatorNt);
	return SplitString(memArena, target, delineator, numPiecesOut, ignoreCase);
}
MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t target = NewStr(targetNt);
	return SplitString(memArena, target, delineator, numPiecesOut, ignoreCase);
}
MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t target = NewStr(targetNt);
	MyStr_t delineator = NewStr(delineatorNt);
	return SplitString(memArena, target, delineator, numPiecesOut, ignoreCase);
}

struct SplitStringContext_t
{
	u64 lastSeparatorIndex;
	MyStr_t piece;
};
bool SplitStringFast(SplitStringContext_t* context, MyStr_t target, char separatorChar, bool includeEmptyPieces = false)
{
	for (u64 cIndex = context->lastSeparatorIndex; cIndex <= target.length; cIndex++)
	{
		if (cIndex == target.length || target.pntr[cIndex] == separatorChar)
		{
			if (includeEmptyPieces || cIndex > context->lastSeparatorIndex)
			{
				context->piece.pntr = &target.pntr[context->lastSeparatorIndex];
				context->piece.length = cIndex - context->lastSeparatorIndex;
				context->lastSeparatorIndex = cIndex+1;
				return true;
			}
		}
	}
	context->lastSeparatorIndex = target.length;
	return false;
}

MyStr_t* SplitStringBySpacesFastTemp(MemArena_t* tempArena, MyStr_t target, u64* numPiecesOut)
{
	DebugAssert(tempArena != nullptr);
	DebugAssert(tempArena->type == MemArenaType_MarkedStack);
	DebugAssert(target.pntr != nullptr || target.length == 0);
	DebugAssert(numPiecesOut != nullptr);
	*numPiecesOut = 0;
	MyStr_t* result = nullptr;
	u64 previousSplitIndex = 0;
	for (u64 cIndex = 0; cIndex <= target.length; cIndex++)
	{
		if (cIndex == target.length || target.pntr[cIndex] == ' ')
		{
			if (cIndex > previousSplitIndex)
			{
				MyStr_t* newStr = AllocStruct(tempArena, MyStr_t);
				DebugAssert(newStr != nullptr);
				DebugAssert(result == nullptr || newStr == result + (*numPiecesOut));
				newStr->length = cIndex - previousSplitIndex;
				newStr->pntr = &target.pntr[previousSplitIndex];
				if (result == nullptr) { result = newStr; }
				*numPiecesOut = (*numPiecesOut) + 1;
			}
			previousSplitIndex = cIndex+1;
		}
	}
	return result;
}
MyStr_t* SplitStringBySlashesFastTemp(MemArena_t* tempArena, MyStr_t target, u64* numPiecesOut)
{
	DebugAssert(tempArena != nullptr);
	DebugAssert(tempArena->type == MemArenaType_MarkedStack);
	DebugAssert(target.pntr != nullptr || target.length == 0);
	DebugAssert(numPiecesOut != nullptr);
	*numPiecesOut = 0;
	MyStr_t* result = nullptr;
	u64 previousSplitIndex = 0;
	for (u64 cIndex = 0; cIndex <= target.length; cIndex++)
	{
		if (cIndex == target.length || target.pntr[cIndex] == '/')
		{
			if (cIndex > previousSplitIndex)
			{
				MyStr_t* newStr = AllocStruct(tempArena, MyStr_t);
				DebugAssert(newStr != nullptr);
				DebugAssert(result == nullptr || newStr == result + (*numPiecesOut));
				newStr->length = cIndex - previousSplitIndex;
				newStr->pntr = &target.pntr[previousSplitIndex];
				if (result == nullptr) { result = newStr; }
				*numPiecesOut = (*numPiecesOut) + 1;
			}
			previousSplitIndex = cIndex+1;
		}
	}
	return result;
}

//TODO: This doesn't play SUPER nice with unicode strings. It shouldn't really "fail" though
//TODO: Add support for hex or unicode sequences
//Returns the number of bytes that the string shrunk by after unescaping
u64 UnescapeQuotedStringInPlace(MyStr_t* target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
{
	NotNullStr(target);
	if (target->length == 0) { return 0; }
	u64 numBytesSmaller = 0;
	u64 writeIndex = 0;
	for (u64 readIndex = 0; readIndex < target->length; readIndex++)
	{
		char currChar = target->pntr[readIndex];
		char nextChar = (readIndex+1 < target->length) ? target->pntr[readIndex+1] : '\0';
		if (currChar == '\\')
		{
			if (nextChar == '\\')
			{
				target->pntr[writeIndex] = '\\';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == '\"')
			{
				target->pntr[writeIndex] = '\"';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == '\'')
			{
				target->pntr[writeIndex] = '\'';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 't')
			{
				target->pntr[writeIndex] = '\t';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'n' && allowNewLineEscapes)
			{
				target->pntr[writeIndex] = '\n';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'r' && allowNewLineEscapes)
			{
				target->pntr[writeIndex] = '\r';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'b' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\b';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'a' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\a';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'b' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\b';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'f' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\f';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'v' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\v';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == '?' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\?';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else
			{
				//Invalid escape code
				//Just leave this escape sequence
				target->pntr[writeIndex] = target->pntr[readIndex];
				writeIndex++;
			}
		}
		else if (currChar == '\"' && (readIndex == 0 || readIndex == target->length-1) && removeQuotes)
		{
			numBytesSmaller++;
			//don't increment writeIndex
		}
		else
		{
			target->pntr[writeIndex] = target->pntr[readIndex];
			writeIndex++;
		}
	}
	Assert(numBytesSmaller < target->length);
	Assert(numBytesSmaller == target->length - writeIndex);
	target->length -= numBytesSmaller;
	if (numBytesSmaller > 0) 
	{
		//If we shrunk at all then we should have space to insert a new null-term char
		//Otherwise we rely on the null-term pre-existing if the string didn't change at all
		target->pntr[target->length] = '\0';
	}
	return numBytesSmaller;
}
MyStr_t UnescapeQuotedStringInArena(MemArena_t* memArena, MyStr_t target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
{
	NotNull(memArena);
	MyStr_t result = AllocString(memArena, &target);
	u64 numBytesSmaller = UnescapeQuotedStringInPlace(&result, removeQuotes, allowNewLineEscapes, allowOtherEscapeCodes);
	UNUSED(numBytesSmaller);
	return result;
}

// if extensionOut is not passed then the extension will be included in the fileNameOut
// Output MyStr_t are not reallocated so they are not all null-terminated
// extensionOut will include the '.' character
// If the path is actually a directory only we may interpret the last folder name as fileName w/ extension.
// Only use full file paths in order to avoid this, or have trailing "/"
void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr)
{
	NotNullStr(&fullPath);
	if (fullPath.length == 0)
	{
		if (directoryOut != nullptr) { *directoryOut = MyStr_Empty; }
		if (fileNameOut != nullptr) { *fileNameOut = MyStr_Empty; }
		if (extensionOut != nullptr) { *extensionOut = MyStr_Empty; }
		return;
	}
	
	bool foundSlash = false;
	bool foundPeriod = false;
	u64 lastSlashIndex = 0; //index after char
	u64 lastPeriodIndex = fullPath.length; //index before char
	for (u64 cIndex = 0; cIndex < fullPath.length; cIndex++)
	{
		if (fullPath.pntr[cIndex] == '\\' || fullPath.pntr[cIndex] == '/')
		{
			foundSlash = true;
			lastSlashIndex = cIndex+1;
		}
		if (fullPath.pntr[cIndex] == '.')
		{
			foundPeriod = true;
			lastPeriodIndex = cIndex;
		}
	}
	if (foundPeriod && lastPeriodIndex < lastSlashIndex) //periods in the directory portion are relative directives like ".\..\" 
	{
		foundPeriod = false;
		lastPeriodIndex = fullPath.length;
	}
	Assert(lastPeriodIndex >= lastSlashIndex);
	
	if (directoryOut != nullptr)
	{
		*directoryOut = NewStr(lastSlashIndex, &fullPath.pntr[0]);
	}
	if (fileNameOut != nullptr)
	{
		if (extensionOut != nullptr)
		{
			*fileNameOut = NewStr(lastPeriodIndex - lastSlashIndex, &fullPath.pntr[lastSlashIndex]);
			*extensionOut = NewStr(fullPath.length - lastPeriodIndex, &fullPath.pntr[lastPeriodIndex]);
		}
		else
		{
			*fileNameOut = NewStr(fullPath.length - lastSlashIndex, &fullPath.pntr[lastSlashIndex]);
		}
	}
}
MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true)
{
	MyStr_t result;
	MyStr_t extensionThrowAway;
	SplitFilePath(filePath, nullptr, &result, (includeExtension ? nullptr : &extensionThrowAway));
	NotNullStr(&result);
	return result;
}

MyStr_t GetDirectoryPart(MyStr_t filePath)
{
	MyStr_t result;
	SplitFilePath(filePath, &result, nullptr, nullptr);
	NotNullStr(&result);
	return result;
}

const char* GetFileNamePartNt(const char* filePath)
{
	NotNull(filePath);
	MyStr_t result;
	SplitFilePath(NewStr(filePath), nullptr, &result);
	NotNullStr(&result);
	return result.pntr;
}

//Returns the number of instances replaced
u64 StrReplaceInPlace(MyStr_t str, MyStr_t target, MyStr_t replacement, bool ignoreCase = false)
{
	NotNullStr(&str);
	NotNullStr(&target);
	NotNullStr(&replacement);
	Assert(target.length == replacement.length); //TODO: Add support for replacement being shorter?
	if (target.length == 0) { return 0; } //nothing to replace
	
	u64 result = 0;
	for (u64 cIndex = 0; cIndex + target.length <= str.length; cIndex++)
	{
		if ((ignoreCase && StrCompareIgnoreCase(&str.pntr[cIndex], target.pntr, target.length) == 0) ||
			(!ignoreCase && MyStrCompare(&str.pntr[cIndex], target.pntr, target.length) == 0))
		{
			for (u64 cIndex2 = 0; cIndex2 < target.length; cIndex2++)
			{
				str.pntr[cIndex + cIndex2] = replacement.pntr[cIndex2];
			}
			cIndex += replacement.length-1;
			result++;
		}
	}
	
	return result;
}
u64 StrReplaceInPlace(MyStr_t str, const char* target, const char* replacement, bool ignoreCase = false)
{
	NotNullStr(&str);
	NotNull(target);
	NotNull(replacement);
	return StrReplaceInPlace(str, NewStr(target), NewStr(replacement), ignoreCase);
}

//TODO: Add ignoreCase support to this implementation!
MyStr_t StrReplace(MyStr_t str, MyStr_t target, MyStr_t replacement, MemArena_t* memArena)
{
	NotNullStr(&str);
	NotNullStr(&target);
	NotNullStr(&replacement);
	Assert(target.length > 0);
	
	MyStr_t result = {};
	for (u8 pass = 0; pass < 2; pass++)
	{
		u64 numBytesNeeded = 0;
		
		for (u64 bIndex = 0; bIndex < str.length; )
		{
			if (bIndex + target.length <= str.length)
			{
				bool foundTarget = true;
				for (u64 tIndex = 0; tIndex < target.length; )
				{
					u32 strCodepoint = 0;
					u8 strByteSize = GetCodepointForUtf8Str(str, bIndex + tIndex, &strCodepoint);
					if (strByteSize == 0) { strByteSize = 1; strCodepoint = CharToU32(str.pntr[bIndex + tIndex]); } //invalid unicode
					u32 targetCodepoint = 0;
					u8 targetByteSize = GetCodepointForUtf8Str(target, tIndex, &targetCodepoint);
					if (targetByteSize == 0) { targetByteSize = 1; targetCodepoint = target.pntr[tIndex]; } //invalid unicode
					if (strCodepoint != targetCodepoint)
					{
						foundTarget = false;
						break;
					}
					Assert(targetByteSize == strByteSize); //TODO: Get rid of this assumption by having 2 iteration variables, not just tIndex
					tIndex += strByteSize;
				}
				
				if (foundTarget)
				{
					if (result.pntr != nullptr)
					{
						Assert(numBytesNeeded + replacement.length <= result.length);
						MyMemCopy(&result.pntr[numBytesNeeded], replacement.pntr, replacement.length);
					}
					numBytesNeeded += replacement.length;
					bIndex += target.length;
				}
				else
				{
					u32 codepoint = 0;
					u8 charByteSize = GetCodepointForUtf8Str(str, bIndex, &codepoint);
					if (charByteSize == 0) { charByteSize = 1; codepoint = CharToU32(str.pntr[bIndex]); } //invalid unicode
					if (result.pntr != nullptr)
					{
						Assert(numBytesNeeded + charByteSize <= result.length);
						MyMemCopy(&result.pntr[numBytesNeeded], &str.pntr[bIndex], charByteSize);
					}
					numBytesNeeded += charByteSize;
					bIndex += charByteSize;
				}
			}
			else
			{
				u32 codepoint = 0;
				u8 charByteSize = GetCodepointForUtf8Str(str, bIndex, &codepoint);
				if (charByteSize == 0) { charByteSize = 1; codepoint = CharToU32(str.pntr[bIndex]); } //invalid unicode
				if (result.pntr != nullptr)
				{
					Assert(numBytesNeeded + charByteSize <= result.length);
					MyMemCopy(&result.pntr[numBytesNeeded], &str.pntr[bIndex], charByteSize);
				}
				numBytesNeeded += charByteSize;
				bIndex += charByteSize;
			}
		}
		
		if (pass == 0)
		{
			result.length = numBytesNeeded;
			result.pntr = AllocArray(memArena, char, result.length+1);
			Assert(result.pntr);
		}
		else
		{
			Assert(numBytesNeeded == result.length);
			result.pntr[result.length] = '\0';
		}
	}
	return result;
}
MyStr_t StrReplace(MyStr_t str, const char* target, const char* replacement, MemArena_t* memArena)
{
	return StrReplace(str, NewStr(target), NewStr(replacement), memArena);
}

bool FindSubstring(MyStr_t target, MyStr_t substring, u64* indexOut = nullptr, bool ignoreCase = false)
{
	NotNullStr(&target);
	NotNullStr(&substring);
	if (substring.length >= target.length) { return false; }
	
	for (u64 cIndex = 0; cIndex + substring.length <= target.length; )
	{
		bool allMatched = true;
		u64 cSubIndex = 0;
		for (u64 subIndex = 0; subIndex < substring.length; )
		{
			u32 targetCodepoint = 0;
			u8 targetCodepointSize = GetCodepointForUtf8Str(target, cIndex + cSubIndex, &targetCodepoint);
			u32 subCodepoint = 0;
			u8 subCodepointSize = GetCodepointForUtf8Str(substring, subIndex, &subCodepoint);
			if (targetCodepointSize == 0 || subCodepointSize == 0)
			{
				return false;
			}
			
			if ((!ignoreCase && targetCodepoint != subCodepoint) ||
				(ignoreCase && GetLowercaseCodepoint(targetCodepoint) != GetLowercaseCodepoint(subCodepoint)))
			{
				allMatched = false;
				break;
			}
			
			subIndex += subCodepointSize;
			cSubIndex += targetCodepointSize;
		}
		if (allMatched)
		{
			if (indexOut != nullptr) { *indexOut = cIndex; }
			return true;
		}
		else
		{
			u8 targetCodepointSize = GetCodepointForUtf8Str(target, cIndex, nullptr);
			if (targetCodepointSize == 0)
			{
				return false;
			}
			cIndex += targetCodepointSize;
		}
	}
	
	return false;
}
bool FindSubstring(MyStr_t target, const char* nullTermSubstring, u64* indexOut= nullptr, bool ignoreCase = false)
{
	return FindSubstring(target, NewStr(nullTermSubstring), indexOut, ignoreCase);
}
bool FindSubstring(const char* nullTermTarget, MyStr_t substring, u64* indexOut= nullptr, bool ignoreCase = false)
{
	return FindSubstring(NewStr(nullTermTarget), substring, indexOut, ignoreCase);
}
bool FindSubstring(const char* nullTermTarget, const char* nullTermSubstring, u64* indexOut= nullptr, bool ignoreCase = false)
{
	return FindSubstring(NewStr(nullTermTarget), NewStr(nullTermSubstring), indexOut, ignoreCase);
}

MyStr_t FindStrParensPart(MyStr_t target, char openParensChar = '(', char closeParensChar = ')')
{
	NotNullStr(&target);
	u64 openParensIndex = target.length;
	u64 parensLevel = 0;
	for (u64 cIndex = 0; cIndex < target.length; cIndex++)
	{
		if (target.pntr[cIndex] == openParensChar)
		{
			if (parensLevel == 0)
			{
				openParensIndex = cIndex;
			}
			parensLevel++;
		}
		else if (target.pntr[cIndex] == closeParensChar)
		{
			if (parensLevel > 1)
			{
				parensLevel--;
			}
			else if (parensLevel == 1)
			{
				return NewStr(cIndex+1 - openParensIndex, &target.pntr[openParensIndex]);
			}
		}
	}
	return MyStr_Empty;
}
MyStr_t FindStrParensPart(const char* nullTermTarget, char openParensChar = '(', char closeParensChar = ')')
{
	return FindStrParensPart(NewStr(nullTermTarget), openParensChar, closeParensChar);
}

//takes the str and returns a string that repeats that str some number of times
MyStr_t StringRepeat(MemArena_t* memArena, MyStr_t str, u64 numRepetitions)
{
	NotNull(memArena);
	NotNullStr(&str);
	MyStr_t result;
	result.length = str.length * numRepetitions;
	result.pntr = AllocArray(memArena, char, result.length+1);
	NotNull(result.pntr);
	for (u64 rIndex = 0; rIndex < numRepetitions; rIndex++)
	{
		MyMemCopy(&result.pntr[rIndex * str.length], str.pntr, str.length);
	}
	result.pntr[result.length] = '\0';
	return result;
}
MyStr_t StringRepeat(MemArena_t* memArena, const char* nullTermStr, u64 numRepetitions)
{
	return StringRepeat(memArena, NewStr(nullTermStr), numRepetitions);
}

MyStr_t FormatBytes(u64 numBytes, MemArena_t* memArena)
{
	u64 gigabytes = numBytes/Gigabytes(1);
	u64 megabytes = (numBytes%Gigabytes(1))/Megabytes(1);
	u64 kilobytes = (numBytes%Megabytes(1))/Kilobytes(1);
	u64 remainder = (numBytes % Kilobytes(1));
	if (numBytes >= Kilobytes(1))
	{
		if (numBytes >= Megabytes(1))
		{
			if (numBytes >= Gigabytes(1))
			{
				return PrintInArenaStr(memArena, "%lluG %lluM %lluk %llub", gigabytes, megabytes, kilobytes, remainder);
			}
			else
			{
				return PrintInArenaStr(memArena, "%lluM %lluk %llub", megabytes, kilobytes, remainder);
			}
		}
		else
		{
			return PrintInArenaStr(memArena, "%lluk %llub", kilobytes, remainder);
		}
	}
	else
	{
		return PrintInArenaStr(memArena, "%llub", remainder);
	}
}
const char* FormatBytesNt(u64 numBytes, MemArena_t* memArena)
{
	return FormatBytes(numBytes, memArena).pntr;
}

// +--------------------------------------------------------------+
// |                    Time String Functions                     |
// +--------------------------------------------------------------+
#ifdef _GY_TIME_H
MyStr_t FormatRealTime(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
{
	NotNull(realTime);
	NotNull(memArena);
	MyStr_t result;
	if (includeDayOfWeek)
	{
		if (includeHourMinuteSecond)
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena,
					"%s %u:%02u:%02u%s (%s %s, %u)",
					GetDayOfWeekStr(realTime->dayOfWeek),
					Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second,
					IsPostMeridian(realTime->hour) ? "pm" : "am",
					GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year
				);
			}
			else
			{
				result = PrintInArenaStr(memArena,
					"%s %u:%02u:%02u%s",
					GetDayOfWeekStr(realTime->dayOfWeek),
					Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second,
					IsPostMeridian(realTime->hour) ? "pm" : "am"
				);
			}
		}
		else
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena,
					"%s (%s %s, %u)",
					GetDayOfWeekStr(realTime->dayOfWeek),
					GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year
				);
			}
			else
			{
				result = PrintInArenaStr(memArena, "%s", GetDayOfWeekStr(realTime->dayOfWeek));
			}
		}
	}
	else
	{
		if (includeHourMinuteSecond)
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena,
					"%u:%02u:%02u%s (%s %s, %u)",
					Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second,
					IsPostMeridian(realTime->hour) ? "pm" : "am",
					GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year
				);
			}
			else
			{
				result = PrintInArenaStr(memArena, "%u:%02u:%02u%s", Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second, IsPostMeridian(realTime->hour) ? "pm" : "am");
			}
		}
		else
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena, "(%s %s, %u)", GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year);
			}
			else
			{
				return NewStringInArenaNt(memArena, "");
			}
		}
	}
	return result;
}
const char* FormatRealTimeNt(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
{
	return FormatRealTime(realTime, memArena, includeDayOfWeek, includeHourMinuteSecond, includeMonthDayYear).pntr;
}

MyStr_t FormatMilliseconds(u64 milliseconds, MemArena_t* memArena)
{
	NotNull(memArena);
	if (milliseconds < NUM_MS_PER_DAY)
	{
		if (milliseconds < NUM_MS_PER_HOUR)
		{
			if (milliseconds < NUM_MS_PER_MINUTE)
			{
				if (milliseconds < NUM_MS_PER_SECOND)
				{
					return PrintInArenaStr(memArena, "%llums", milliseconds);
				}
				else
				{
					return PrintInArenaStr(memArena, "%llus %llums",
						milliseconds/NUM_MS_PER_SECOND,
						milliseconds%NUM_MS_PER_SECOND
					);
				}
			}
			else
			{
				return PrintInArenaStr(memArena, "%llum %llus %llums",
					milliseconds/NUM_MS_PER_MINUTE,
					(milliseconds%NUM_MS_PER_MINUTE)/NUM_MS_PER_SECOND,
					milliseconds%NUM_MS_PER_SECOND
				);
			}
		}
		else
		{
			return PrintInArenaStr(memArena, "%lluh %llum %llus %llums",
				milliseconds/NUM_MS_PER_HOUR,
				(milliseconds%NUM_MS_PER_HOUR)/NUM_MS_PER_MINUTE,
				(milliseconds%NUM_MS_PER_MINUTE)/NUM_MS_PER_SECOND,
				milliseconds%NUM_MS_PER_SECOND
			);
		}
	}
	else
	{
		return PrintInArenaStr(memArena, "%llud %lluh %llum %llus %llums",
			milliseconds/NUM_MS_PER_DAY,
			(milliseconds%NUM_MS_PER_DAY)/NUM_MS_PER_HOUR,
			(milliseconds%NUM_MS_PER_HOUR)/NUM_MS_PER_MINUTE,
			(milliseconds%NUM_MS_PER_MINUTE)/NUM_MS_PER_SECOND,
			milliseconds%NUM_MS_PER_SECOND
		);
	}
}
const char* FormatMillisecondsNt(u64 milliseconds, MemArena_t* memArena)
{
	return FormatMilliseconds(milliseconds, memArena).pntr;
}
#endif //_GY_TIME_H

#endif //  _GY_STRING_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
MyStr_Empty
@Types
MyStr_t
@Functions
MyStr_t NewStr(u64 length, char* pntr)
bool IsStrEmpty(MyStr_t target)
bool IsStrNullTerminated(const MyStr_t* target)
#define NotNullStr(strPntr)
#define NotNullStr_(strPntr)
#define NotEmptyStr(strPntr)
#define NotEmptyStr_(strPntr)
#define AssertNullTerm(strPntr)
#define AssertNullTerm_(strPntr)
#define AllocString(arena, strPntr)
#define NewStringInArena(arena, length, charPntr)
#define NewStringInArenaNt(arena, nullTermStr)
#define FreeString(arena, strPntr)
MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...)
u64 TrimLeadingWhitespace(MyStr_t* target, bool trimNewLines = false)
u64 TrimTrailingWhitespace(MyStr_t* target, bool trimNewLines = false)
u64 TrimWhitespace(MyStr_t* target, bool trimNewLines = false)
bool FindNextCharInStr(MyStr_t target, u64 startIndex, MyStr_t searchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, MyStr_t knownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
bool FindNextWhitespaceInStr(MyStr_t target, u64 startIndex, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
MyStr_t StrSubstring(MyStr_t* target, u64 startIndex, u64 endIndex)
MyStr_t StrSubstringLength(MyStr_t* target, u64 startIndex, u64 length)
MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2)
bool StrEquals(MyStr_t target, MyStr_t comparison)
i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2)
bool StrStartsWith(MyStr_t str, MyStr_t prefix, bool ignoreCase = false)
bool StrEndsWith(MyStr_t str, MyStr_t suffix, bool ignoreCase = false)
MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
u64 UnescapeQuotedStringInPlace(MyStr_t* target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
MyStr_t UnescapeQuotedStringInArena(MemArena_t* memArena, MyStr_t target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr)
MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true)
MyStr_t GetDirectoryPart(MyStr_t filePath)
const char* GetFileNamePartNt(const char* filePath)
u64 StrReplaceInPlace(MyStr_t str, MyStr_t target, MyStr_t replacement, bool ignoreCase = false)
MyStr_t StrReplace(MyStr_t str, MyStr_t target, MyStr_t replacement, MemArena_t* memArena)
bool FindSubstring(MyStr_t target, MyStr_t substring, u64* indexOut = nullptr, bool ignoreCase = false)
MyStr_t FindStrParensPart(MyStr_t target, char openParensChar = '[', char closeParensChar = ']')
MyStr_t StringRepeat(MemArena_t* memArena, MyStr_t str, u64 numRepetitions)
u8 GetCodepointForUtf8Str(MyStr_t str, u64 index, u32* codepointOut)
MyStr_t ConvertWideStrToUtf8(MemArena_t* memArena, const wchar_t* wideStrPntr, u64 wideStrLength)
MyStr_t ConvertWideStrToUtf8Nt(MemArena_t* memArena, const wchar_t* nullTermWideStr)
MyStr_t FormatBytes(u64 numBytes, MemArena_t* memArena)
const char* FormatBytesNt(u64 numBytes, MemArena_t* memArena)
MyStr_t FormatRealTime(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
const char* FormatRealTimeNt(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
MyStr_t FormatMilliseconds(u64 milliseconds, MemArena_t* memArena)
const char* FormatMillisecondsNt(u64 milliseconds, MemArena_t* memArena)
*/
