/*
File:   gy_parsing.h
Author: Taylor Robbins
Date:   01\11\2022
Description:
	** Holds a bunch of functions that help us parse strings that contain
	** numbers, booleans, or other data
*/

#ifndef _GY_PARSING_H
#define _GY_PARSING_H

#include "gylib/gy_std.h"
#include "gylib/gy_assert.h"
#include "gylib/gy_types.h"
#include "gylib/gy_intrinsics.h"
#include "gylib/gy_unicode.h"
#include "gylib/gy_string.h"
#include "gylib/gy_vectors.h"

#define GY_MAX_FLOAT_PARSE_LENGTH 64 //characters

// +--------------------------------------------------------------+
// |                  Structure/Type Definitions                  |
// +--------------------------------------------------------------+
enum TryParseFailureReason_t
{
	TryParseFailureReason_None = 0,
	TryParseFailureReason_InvalidCharacter,
	TryParseFailureReason_InvalidUtf8,
	TryParseFailureReason_Underflow,
	TryParseFailureReason_Overflow,
	TryParseFailureReason_NoNumbers,
	TryParseFailureReason_StringOverflow,
	TryParseFailureReason_Infinity,
	TryParseFailureReason_FloatParseFailure,
	TryParseFailureReason_UnknownString,
	TryParseFailureReason_NotEnoughCommas,
	TryParseFailureReason_TooManyCommas,
	TryParseFailureReason_NumReasons,
};

const char* GetTryParseFailureReasonStr(TryParseFailureReason_t reason)
{
	switch (reason)
	{
		case TryParseFailureReason_None:              return "None";
		case TryParseFailureReason_InvalidCharacter:  return "InvalidCharacter";
		case TryParseFailureReason_InvalidUtf8:       return "InvalidUtf8";
		case TryParseFailureReason_Underflow:         return "Underflow";
		case TryParseFailureReason_Overflow:          return "Overflow";
		case TryParseFailureReason_NoNumbers:         return "NoNumbers";
		case TryParseFailureReason_StringOverflow:    return "StringOverflow";
		case TryParseFailureReason_Infinity:          return "Infinity";
		case TryParseFailureReason_FloatParseFailure: return "FloatParseFailure";
		case TryParseFailureReason_UnknownString:     return "UnknownString";
		case TryParseFailureReason_NotEnoughCommas:   return "NotEnoughCommas";
		case TryParseFailureReason_TooManyCommas:     return "TooManyCommas";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                        Parse Integer                         |
// +--------------------------------------------------------------+
//TODO: For some reason 18446744073709551616-18446744073709551619 incorrectly succeed parsing and return 0-3
bool TryParseU64(MyStr_t str, u64* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	NotNullStr(&str);
	TrimWhitespace(&str);
	u64 result = 0;
	bool foundHexDesignation = false;
	bool foundBinaryDesignation = false;
	if (allowHex && StrStartsWith(str, "0x", true)) { foundHexDesignation = true; str = StrSubstring(&str, 2); }
	else if (allowBinary && StrStartsWith(str, "0b", true)) { foundBinaryDesignation = true; str = StrSubstring(&str, 2); }
	
	bool foundNumbers = false;
	while (str.length > 0)
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(str, 0, &codepoint);
		if (codepointSize == 0)
		{
			if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_InvalidUtf8; }
			return false;
		}
		
		if (foundHexDesignation && codepoint >= '0' && codepoint <= '9')
		{
			if (result > UINT64_MAX / 16ULL)
			{
				if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
				return false;
			}
			result = result * 16ULL + (u64)(codepoint - '0');
			foundNumbers = true;
		}
		else if (foundHexDesignation && codepoint >= 'A' && codepoint <= 'F')
		{
			if (result > UINT64_MAX / 16ULL)
			{
				if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
				return false;
			}
			result = result * 16ULL + (u64)((codepoint - 'A') + 10);
			foundNumbers = true;
		}
		else if (foundHexDesignation && codepoint >= 'a' && codepoint <= 'f')
		{
			if (result > UINT64_MAX / 16ULL)
			{
				if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
				return false;
			}
			result = result * 16ULL + (u64)((codepoint - 'a') + 10);
			foundNumbers = true;
		}
		else if (foundBinaryDesignation && codepoint >= '0' && codepoint <= '1')
		{
			if (result > UINT64_MAX / 2ULL)
			{
				if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
				return false;
			}
			result = result * 2ULL + (u64)((codepoint == '1') ? 1 : 0);
			foundNumbers = true;
		}
		else if (!foundHexDesignation && !foundBinaryDesignation && codepoint >= '0' && codepoint <= '9')
		{
			if (result > UINT64_MAX / 10ULL)
			{
				if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
				return false;
			}
			result = result * 10ULL + (u64)(codepoint - '0');
			foundNumbers = true;
		}
		else 
		{
			if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_InvalidCharacter; }
			return false;
		}
		
		str.pntr += codepointSize;
		str.length -= codepointSize;
	}
	
	if (!foundNumbers)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_NoNumbers; }
		return false;
	}
	
	if (valueOut != nullptr) { *valueOut = result; }
	return true;
}
bool TryParseU32(MyStr_t str, u32* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	u64 resultU64 = 0;
	if (!TryParseU64(str, &resultU64, reasonOut, allowHex, allowBinary)) { return false; }
	if (resultU64 > UINT32_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = (u32)resultU64; }
	return true;
}
bool TryParseU16(MyStr_t str, u16* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	u64 resultU64 = 0;
	if (!TryParseU64(str, &resultU64, reasonOut, allowHex, allowBinary)) { return false; }
	if (resultU64 > UINT16_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = (u16)resultU64; }
	return true;
}
bool TryParseU8(MyStr_t str, u16* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	u64 resultU64 = 0;
	if (!TryParseU64(str, &resultU64, reasonOut, allowHex, allowBinary)) { return false; }
	if (resultU64 > UINT8_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = (u8)resultU64; }
	return true;
}

bool TryParseI64(MyStr_t str, i64* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	NotNullStr(&str);
	TrimWhitespace(&str);
	bool isNegative = false;
	if (StrStartsWith(str, "-")) { isNegative = true; str = StrSubstring(&str, 1); }
	else if (StrStartsWith(str, "+")) { isNegative = false; str = StrSubstring(&str, 1); }
	u64 resultU64 = 0;
	if (!TryParseU64(str, &resultU64, reasonOut, allowHex, allowBinary)) { return false; }
	if (!isNegative && resultU64 > INT64_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (isNegative && resultU64 > (u64)INT64_MAX + 1)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Underflow; }
		return false;
	}
	if (valueOut != nullptr)
	{
		if (isNegative)
		{
			resultU64 = (~resultU64) + 1;
			*valueOut = *((i64*)&resultU64);
		}
		else
		{
			*valueOut = (i64)resultU64;
		}
	}
	return true;
}
bool TryParseI32(MyStr_t str, i32* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	NotNullStr(&str);
	i64 resultI64 = 0;
	if (!TryParseI64(str, &resultI64, reasonOut, allowHex, allowBinary)) { return false; }
	if (resultI64 > INT32_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (resultI64 < INT32_MIN)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Underflow; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = (i32)resultI64; }
	return true;
}
bool TryParseI16(MyStr_t str, i16* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	NotNullStr(&str);
	i64 resultI64 = 0;
	if (!TryParseI64(str, &resultI64, reasonOut, allowHex, allowBinary)) { return false; }
	if (resultI64 > INT16_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (resultI64 < INT16_MIN)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Underflow; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = (i16)resultI64; }
	return true;
}
bool TryParseI8(MyStr_t str, i8* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
{
	NotNullStr(&str);
	i64 resultI64 = 0;
	if (!TryParseI64(str, &resultI64, reasonOut, allowHex, allowBinary)) { return false; }
	if (resultI64 > INT8_MAX)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Overflow; }
		return false;
	}
	if (resultI64 < INT8_MIN)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Underflow; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = (i8)resultI64; }
	return true;
}

// +--------------------------------------------------------------+
// |                         Parse Float                          |
// +--------------------------------------------------------------+
//TODO: Move away from using atof to parse floating point numbers
bool TryParseR64(MyStr_t str, r64* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowInfinity = false)
{
	NotNullStr(&str);
	TrimWhitespace(&str);
	if (str.length > GY_MAX_FLOAT_PARSE_LENGTH)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_StringOverflow; }
		return false;
	}
	if (str.length == 0)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_NoNumbers; }
		return false;
	}
	
	//We need a local buffer to ensure the string is null-terminated (a requirement of atof)
	char localBuffer[GY_MAX_FLOAT_PARSE_LENGTH+1];
	MyMemCopy(&localBuffer[0], str.pntr, str.length);
	localBuffer[str.length] = '\0';
	MyStr_t tempStr = NewStr(str.length, &localBuffer[0]);
	
	r64 result = atof(tempStr.pntr);
	if (!allowInfinity && IsInfiniteR64(result))
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Infinity; }
		return false;
	}
	//atof doesn't really give us errors. When it fails it gives us 0.0.
	//We are going to do some sanity checks to try and tell between a failed parse and an actual parse of 0.0
	if (result == 0.0)
	{
		bool containsZero = false;
		bool containsOtherNumbers = false;
		for (u64 cIndex = 0; cIndex < tempStr.length; cIndex++)
		{
			if (tempStr.pntr[cIndex] == '0') { containsZero = true; break; }
			if (tempStr.pntr[cIndex] >= '1' && tempStr.pntr[cIndex] <= '9') { containsOtherNumbers = true; break; }
		}
		if (!containsZero || containsOtherNumbers)
		{
			if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_FloatParseFailure; }
			return false;
		}
	}
	
	if (valueOut != nullptr) { *valueOut = result; }
	return true;
}
bool TryParseR32(MyStr_t str, r32* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowSuffix = true, bool allowInfinity = false)
{
	NotNullStr(&str);
	TrimWhitespace(&str);
	if (allowSuffix && str.length > 0 && str.pntr[str.length-1] == 'f')
	{
		str.length--;
	}
	if (str.length > GY_MAX_FLOAT_PARSE_LENGTH)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_StringOverflow; }
		return false;
	}
	if (str.length == 0)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_NoNumbers; }
		return false;
	}
	
	//We need a local buffer to ensure the string is null-terminated (a requirement of atof)
	char localBuffer[GY_MAX_FLOAT_PARSE_LENGTH+1];
	MyMemCopy(&localBuffer[0], str.pntr, str.length);
	localBuffer[str.length] = '\0';
	MyStr_t tempStr = NewStr(str.length, &localBuffer[0]);
	
	r64 resultR64 = atof(tempStr.pntr);
	if (!allowInfinity && IsInfiniteR64(resultR64))
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_Infinity; }
		return false;
	}
	//atof doesn't really give us errors. When it fails it gives us 0.0.
	//We are going to do some sanity checks to try and tell between a failed parse and an actual parse of 0.0
	if (resultR64 == 0.0)
	{
		bool containsZero = false;
		bool containsOtherNumbers = false;
		for (u64 cIndex = 0; cIndex < tempStr.length; cIndex++)
		{
			if (tempStr.pntr[cIndex] == '0') { containsZero = true; break; }
			if (tempStr.pntr[cIndex] >= '1' && tempStr.pntr[cIndex] <= '9') { containsOtherNumbers = true; break; }
		}
		if (!containsZero || containsOtherNumbers)
		{
			if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_FloatParseFailure; }
			return false;
		}
	}
	
	if (valueOut != nullptr) { *valueOut = (r32)resultR64; }
	return true;
}

// +--------------------------------------------------------------+
// |                      Parse Other Types                       |
// +--------------------------------------------------------------+
bool TryParseBool(MyStr_t str, bool* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	TrimWhitespace(&str);
	bool result = false;
	if (StrCompareIgnoreCase(str, "true"))       { result = true;  }
	else if (StrCompareIgnoreCase(str, "false")) { result = false; }
	else if (StrCompareIgnoreCase(str, "t"))     { result = true;  }
	else if (StrCompareIgnoreCase(str, "f"))     { result = false; }
	else if (StrCompareIgnoreCase(str, "1"))     { result = true;  }
	else if (StrCompareIgnoreCase(str, "0"))     { result = false; }
	else if (StrCompareIgnoreCase(str, "yes"))   { result = true;  }
	else if (StrCompareIgnoreCase(str, "no"))    { result = false; }
	else if (StrCompareIgnoreCase(str, "y"))     { result = true;  }
	else if (StrCompareIgnoreCase(str, "n"))     { result = false; }
	else
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_UnknownString; }
		return false;
	}
	if (valueOut != nullptr) { *valueOut = result; }
	return true;
}

bool TryParseV2i(MyStr_t str, v2i* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	if (StrStartsWith(str, "(")) { str = StrSubstring(&str, 1); }
	if (StrEndsWith(str, ")")) { str = StrSubstring(&str, 0, str.length-1); }
	u64 commaIndex = 0;
	bool strContainsComma = FindSubstring(str, ",", &commaIndex);
	if (!strContainsComma)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_NotEnoughCommas; }
		return false;
	}
	MyStr_t xStr = StrSubstring(&str, 0, commaIndex);
	MyStr_t yStr = StrSubstring(&str, commaIndex+1);
	v2i vector = Vec2i_Zero;
	if (!TryParseI32(xStr, &vector.x, reasonOut))
	{
		return false;
	}
	if (!TryParseI32(yStr, &vector.y, reasonOut))
	{
		return false;
	}
	if (valueOut != nullptr) { *valueOut = vector; }
	return true;
}
bool TryParseV2(MyStr_t str, v2* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	if (StrStartsWith(str, "(")) { str = StrSubstring(&str, 1); }
	if (StrEndsWith(str, ")")) { str = StrSubstring(&str, 0, str.length-1); }
	u64 commaIndex = 0;
	bool strContainsComma = FindSubstring(str, ",", &commaIndex);
	if (!strContainsComma)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_NotEnoughCommas; }
		return false;
	}
	MyStr_t xStr = StrSubstring(&str, 0, commaIndex);
	MyStr_t yStr = StrSubstring(&str, commaIndex+1);
	v2 vector = Vec2_Zero;
	if (!TryParseR32(xStr, &vector.x, reasonOut))
	{
		return false;
	}
	if (!TryParseR32(yStr, &vector.y, reasonOut))
	{
		return false;
	}
	if (valueOut != nullptr) { *valueOut = vector; }
	return true;
}

bool TryParseReci(MyStr_t str, reci* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	if (StrStartsWith(str, "(")) { str = StrSubstring(&str, 1); }
	if (StrEndsWith(str, ")")) { str = StrSubstring(&str, 0, str.length-1); }
	u8 numCommasFound = 0;
	u64 commaIndices[3];
	for (u64 cIndex = 0; cIndex < str.length; cIndex++)
	{
		if (str.pntr[cIndex] == ',')
		{
			if (numCommasFound >= 3)
			{
				if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_TooManyCommas; }
				return false;
			}
			commaIndices[numCommasFound] = cIndex;
			numCommasFound++;
		}
	}
	if (numCommasFound < 3)
	{
		if (reasonOut != nullptr) { *reasonOut = TryParseFailureReason_NotEnoughCommas; }
		return false;
	}
	MyStr_t xStr = StrSubstring(&str, 0, commaIndices[0]);
	MyStr_t yStr = StrSubstring(&str, commaIndices[0]+1, commaIndices[1]);
	MyStr_t widthStr = StrSubstring(&str, commaIndices[1]+1, commaIndices[2]);
	MyStr_t heightStr = StrSubstring(&str, commaIndices[2]+1);
	reci rectangle = Reci_Zero;
	if (!TryParseI32(xStr, &rectangle.x, reasonOut))
	{
		return false;
	}
	if (!TryParseI32(yStr, &rectangle.y, reasonOut))
	{
		return false;
	}
	if (!TryParseI32(widthStr, &rectangle.width, reasonOut))
	{
		return false;
	}
	if (!TryParseI32(heightStr, &rectangle.height, reasonOut))
	{
		return false;
	}
	if (valueOut != nullptr) { *valueOut = rectangle; }
	return true;
}

#endif //  _GY_PARSING_H

// +--------------------------------------------------------------+
// |                            Tests                             |
// +--------------------------------------------------------------+
#if 0
{
	TryParseFailureReason_t failureReason = TryParseFailureReason_None;
	r32 floatValue = 0; 
	#define TRY_PARSE_R32_TEST(parseString) if (!TryParseR32(NewStr(parseString), &floatValue, &failureReason)) { PrintLine_E("TryParseR32(\"" parseString "\") failed: %s", GetTryParseFailureReasonStr(failureReason)); } else { PrintLine_I("\"" parseString "\" -> %f", floatValue); }
	// TRY_PARSE_R32_TEST("0.0f");
	// TRY_PARSE_R32_TEST("0.1f");
	// TRY_PARSE_R32_TEST("0.2");
	// TRY_PARSE_R32_TEST("0.3");
	// TRY_PARSE_R32_TEST("0.4000");
	// TRY_PARSE_R32_TEST("0.105");
	// TRY_PARSE_R32_TEST("1");
	// TRY_PARSE_R32_TEST("100");
	// TRY_PARSE_R32_TEST("-10");
	// TRY_PARSE_R32_TEST(".5");
	// TRY_PARSE_R32_TEST("-.1");
	// TRY_PARSE_R32_TEST("-0.5628");
	// TRY_PARSE_R32_TEST("10.5628");
	// TRY_PARSE_R32_TEST("+8");
	// TRY_PARSE_R32_TEST("1000");
	// TRY_PARSE_R32_TEST("10000");
	// TRY_PARSE_R32_TEST("100000");
	// TRY_PARSE_R32_TEST("1000000");
	// TRY_PARSE_R32_TEST("10000000");
	// TRY_PARSE_R32_TEST("100000000");
	// TRY_PARSE_R32_TEST("1000000000");
	// TRY_PARSE_R32_TEST("10000000000");
	// TRY_PARSE_R32_TEST("100000000000");
	// TRY_PARSE_R32_TEST("1000000000000");
	// TRY_PARSE_R32_TEST("10000000000000");
	// TRY_PARSE_R32_TEST("100000000000000");
	// TRY_PARSE_R32_TEST("1000000000000000");
	// TRY_PARSE_R32_TEST("10000000000000000");
	// TRY_PARSE_R32_TEST("");
	// TRY_PARSE_R32_TEST("-");
	// TRY_PARSE_R32_TEST("+");
	// TRY_PARSE_R32_TEST(".");
	// TRY_PARSE_R32_TEST("+-10");
	// TRY_PARSE_R32_TEST("NaN");
	// TRY_PARSE_R32_TEST("Inf");
	// TRY_PARSE_R32_TEST("+Inf");
	// TRY_PARSE_R32_TEST("Infinity");
	// TRY_PARSE_R32_TEST("nan");
	// TRY_PARSE_R32_TEST("ab");
	
	r64 doubleValue = 0; 
	#define TRY_PARSE_R64_TEST(parseString) if (!TryParseR64(NewStr(parseString), &doubleValue, &failureReason)) { PrintLine_E("TryParseR64(\"" parseString "\") failed: %s", GetTryParseFailureReasonStr(failureReason)); } else { PrintLine_I("\"" parseString "\" -> %lf", doubleValue); }
	// TRY_PARSE_R64_TEST("0.0f");
	// TRY_PARSE_R64_TEST("0.1f");
	// TRY_PARSE_R64_TEST("0.2");
	// TRY_PARSE_R64_TEST("0.3");
	// TRY_PARSE_R64_TEST("0.4000");
	// TRY_PARSE_R64_TEST("0.105");
	// TRY_PARSE_R64_TEST("1");
	// TRY_PARSE_R64_TEST("100");
	// TRY_PARSE_R64_TEST("-10");
	// TRY_PARSE_R64_TEST(".5");
	// TRY_PARSE_R64_TEST("-.1");
	// TRY_PARSE_R64_TEST("-0.5628");
	// TRY_PARSE_R64_TEST("10.5628");
	// TRY_PARSE_R64_TEST("+8");
	// TRY_PARSE_R64_TEST("1000");
	// TRY_PARSE_R64_TEST("10000");
	// TRY_PARSE_R64_TEST("100000");
	// TRY_PARSE_R64_TEST("1000000");
	// TRY_PARSE_R64_TEST("10000000");
	// TRY_PARSE_R64_TEST("100000000");
	// TRY_PARSE_R64_TEST("1000000000");
	// TRY_PARSE_R64_TEST("10000000000");
	// TRY_PARSE_R64_TEST("100000000000");
	// TRY_PARSE_R64_TEST("1000000000000");
	// TRY_PARSE_R64_TEST("10000000000000");
	// TRY_PARSE_R64_TEST("100000000000000");
	// TRY_PARSE_R64_TEST("1000000000000000");
	// TRY_PARSE_R64_TEST("10000000000000000");
	// TRY_PARSE_R64_TEST("10000000000000000.1");
	// TRY_PARSE_R64_TEST("");
	// TRY_PARSE_R64_TEST("-");
	// TRY_PARSE_R64_TEST("+");
	// TRY_PARSE_R64_TEST(".");
	// TRY_PARSE_R64_TEST("+-10");
	// TRY_PARSE_R64_TEST("NaN");
	// TRY_PARSE_R64_TEST("Inf");
	// TRY_PARSE_R64_TEST("+Inf");
	// TRY_PARSE_R64_TEST("Infinity");
	// TRY_PARSE_R64_TEST("nan");
	// TRY_PARSE_R64_TEST("ab");
	
	u64 valueU64 = 0; 
	#define TRY_PARSE_U64_TEST(parseString) if (!TryParseU64(NewStr(parseString), &valueU64, &failureReason)) { PrintLine_E("TryParseU64(\"" parseString "\") failed: %s", GetTryParseFailureReasonStr(failureReason)); } else { PrintLine_I("\"" parseString "\" -> %llu", valueU64); }
	// TRY_PARSE_U64_TEST("1");
	// TRY_PARSE_U64_TEST("100");
	// TRY_PARSE_U64_TEST("100A");
	// TRY_PARSE_U64_TEST("0xFF");
	// TRY_PARSE_U64_TEST("0123456789");
	// TRY_PARSE_U64_TEST("0xABCDEF");
	// TRY_PARSE_U64_TEST("0xabcdef");
	// TRY_PARSE_U64_TEST("0xF1");
	// TRY_PARSE_U64_TEST("0b1001");
	// TRY_PARSE_U64_TEST("0b1111");
	// TRY_PARSE_U64_TEST("0b1119");
	// TRY_PARSE_U64_TEST("+1");
	// TRY_PARSE_U64_TEST("-10");
	// TRY_PARSE_U64_TEST("");
	// TRY_PARSE_U64_TEST("0x");
	// TRY_PARSE_U64_TEST("10 20");
	// TRY_PARSE_U64_TEST("0xFFFFFFFFFFFFFFFE");
	// TRY_PARSE_U64_TEST("0xFFFFFFFFFFFFFFFF");
	// TRY_PARSE_U64_TEST("18446744073709551615");
	// TRY_PARSE_U64_TEST("18446744073709551616"); //TODO: Incorrectly Succeed
	// TRY_PARSE_U64_TEST("18446744073709551617"); //TODO: Incorrectly Succeed
	// TRY_PARSE_U64_TEST("18446744073709551618"); //TODO: Incorrectly Succeed
	// TRY_PARSE_U64_TEST("18446744073709551619"); //TODO: Incorrectly Succeed
	// TRY_PARSE_U64_TEST("18446744073709551620");
	// TRY_PARSE_U64_TEST("18446744073709551621");
	// TRY_PARSE_U64_TEST("18446744073709551622");
	// TRY_PARSE_U64_TEST("18446744073709551623");
	// TRY_PARSE_U64_TEST("18446744073709551624");
	// TRY_PARSE_U64_TEST("18446744073709551625");
	// TRY_PARSE_U64_TEST("18446744073709551626");
	// TRY_PARSE_U64_TEST("18446744073709551627");
	// TRY_PARSE_U64_TEST("18446744073709551628");
	// TRY_PARSE_U64_TEST("18446744073709551629");
	// TRY_PARSE_U64_TEST("18446744073710000000");
	
	u32 valueU32 = 0; 
	#define TRY_PARSE_U32_TEST(parseString) if (!TryParseU32(NewStr(parseString), &valueU32, &failureReason)) { PrintLine_E("TryParseU32(\"" parseString "\") failed: %s", GetTryParseFailureReasonStr(failureReason)); } else { PrintLine_I("\"" parseString "\" -> %lu", valueU32); }
	// TRY_PARSE_U32_TEST("10");
	// TRY_PARSE_U32_TEST("123456789");
	// TRY_PARSE_U32_TEST("0xABCDEF");
	// TRY_PARSE_U32_TEST("0xabcdef");
	// TRY_PARSE_U32_TEST("0xfF");
	// TRY_PARSE_U32_TEST("4000000000");
	// TRY_PARSE_U32_TEST("4294967295");
	// TRY_PARSE_U32_TEST("4294967296");
	// TRY_PARSE_U32_TEST("4294967297");
	// TRY_PARSE_U32_TEST("4294967298");
	// TRY_PARSE_U32_TEST("4294967299");
	// TRY_PARSE_U32_TEST("4294967300");
	// TRY_PARSE_U32_TEST("0xFFFFFFFF");
	// TRY_PARSE_U32_TEST("0x100000000");
	// TRY_PARSE_U32_TEST("0xFFFFFFFFF");
	// TRY_PARSE_U32_TEST("0x10000000000000001");
	
	i64 valueI64 = 0; 
	#define TRY_PARSE_I64_TEST(parseString) if (!TryParseI64(NewStr(parseString), &valueI64, &failureReason)) { PrintLine_E("TryParseI64(\"" parseString "\") failed: %s", GetTryParseFailureReasonStr(failureReason)); } else { PrintLine_I("\"" parseString "\" -> %lld", valueI64); }
	// TRY_PARSE_I64_TEST("10");
	// TRY_PARSE_I64_TEST("123456789");
	// TRY_PARSE_I64_TEST("-100");
	// TRY_PARSE_I64_TEST("-123456789");
	// TRY_PARSE_I64_TEST("+10");
	// TRY_PARSE_I64_TEST("-");
	// TRY_PARSE_I64_TEST("+");
	// TRY_PARSE_I64_TEST("+0");
	// TRY_PARSE_I64_TEST("4000000000");
	// TRY_PARSE_I64_TEST("9223372036854775807");
	// TRY_PARSE_I64_TEST("-9223372036854775807");
	// TRY_PARSE_I64_TEST("9223372036854775808");
	// TRY_PARSE_I64_TEST("-9223372036854775808");
	// TRY_PARSE_I64_TEST("0x7FFFFFFFFFFFFFFF");
	// TRY_PARSE_I64_TEST("0x8000000000000000");
	// TRY_PARSE_I64_TEST("-0x8000000000000000");
	
	i32 valueI32 = 0; 
	#define TRY_PARSE_I32_TEST(parseString) if (!TryParseI32(NewStr(parseString), &valueI32, &failureReason)) { PrintLine_E("TryParseI32(\"" parseString "\") failed: %s", GetTryParseFailureReasonStr(failureReason)); } else { PrintLine_I("\"" parseString "\" -> %ld", valueI32); }
	// TRY_PARSE_I32_TEST("10");
	// TRY_PARSE_I32_TEST("40000");
	// TRY_PARSE_I32_TEST("-12");
	// TRY_PARSE_I32_TEST("+-12");
	// TRY_PARSE_I32_TEST("-");
	// TRY_PARSE_I32_TEST("+");
	// TRY_PARSE_I32_TEST("+45");
	// TRY_PARSE_I32_TEST("0xFf");
	// TRY_PARSE_I32_TEST("0x7FFFFFFF");
	// TRY_PARSE_I32_TEST("0x80000000");
	// TRY_PARSE_I32_TEST("2147483647");
	// TRY_PARSE_I32_TEST("-2147483647");
	// TRY_PARSE_I32_TEST("2147483648");
	// TRY_PARSE_I32_TEST("-2147483648");
}
#endif

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
GY_MAX_FLOAT_PARSE_LENGTH
TryParseFailureReason_None
TryParseFailureReason_InvalidCharacter
TryParseFailureReason_InvalidUtf8
TryParseFailureReason_Underflow
TryParseFailureReason_Overflow
TryParseFailureReason_NoNumbers
TryParseFailureReason_StringOverflow
TryParseFailureReason_Infinity
TryParseFailureReason_FloatParseFailure
TryParseFailureReason_UnknownString
TryParseFailureReason_NumReasons
@Types
TryParseFailureReason_t
@Functions
const char* GetTryParseFailureReasonStr(TryParseFailureReason_t reason)
bool TryParseU64(MyStr_t str, u64* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseU32(MyStr_t str, u32* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseU16(MyStr_t str, u16* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseU8(MyStr_t str, u16* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseI64(MyStr_t str, i64* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseI32(MyStr_t str, i32* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseI16(MyStr_t str, i16* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseI8(MyStr_t str, i8* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowHex = true, bool allowBinary = true)
bool TryParseR64(MyStr_t str, r64* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowInfinity = false)
bool TryParseR32(MyStr_t str, r32* valueOut, TryParseFailureReason_t* reasonOut = nullptr, bool allowSuffix = true, bool allowInfinity = false)
bool TryParseBool(MyStr_t str, bool* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
bool TryParseV2i(MyStr_t str, v2i* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
bool TryParseV2(MyStr_t str, v2* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
bool TryParseReci(MyStr_t str, reci* valueOut, TryParseFailureReason_t* reasonOut = nullptr)
*/
