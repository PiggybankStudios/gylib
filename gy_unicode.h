/*
File:   gy_unicode.h
Author: Taylor Robbins
Date:   09\24\2021
Description:
	** Holds a bunch of functions that help us deal with characters, utf-8, and unicode in general
*/

#ifndef _GY_UNICODE_H
#define _GY_UNICODE_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"

#define UTF8_MAX_CODEPOINT 0x10FFFFUL
#define UTF8_MAX_CHAR_SIZE 4 //bytes

#define UNICODE_LATIN_EXT_START 0x000000A0UL
#define UNICODE_LATIN_EXT_COUNT 96
#define UNICODE_LATIN_EXT_END   (UNICODE_LATIN_EXT_START + UNICODE_LATIN_EXT_COUNT)

#define UNICODE_CYRILLIC_START 0x00000400UL
#define UNICODE_CYRILLIC_COUNT 256
#define UNICODE_CYRILLIC_END   (UNICODE_CYRILLIC_START + UNICODE_CYRILLIC_COUNT)

#define UNICODE_HIRAGANA_START 0x000003041UL
#define UNICODE_HIRAGANA_COUNT 95
#define UNICODE_HIRAGANA_END   (UNICODE_HIRAGANA_START + UNICODE_HIRAGANA_COUNT)

#define UNICODE_KATAKANA_START 0x0000030A0UL
#define UNICODE_KATAKANA_COUNT 96
#define UNICODE_KATAKANA_END   (UNICODE_KATAKANA_START + UNICODE_KATAKANA_COUNT)

// +--------------------------------------------------------------+
// |                   Simple Char Manipulation                   |
// +--------------------------------------------------------------+
char GetLowercaseAnsiiChar(char c)
{
	if (c >= 'A' && c <= 'Z') { return 'a' + (c - 'A'); }
	return c;
}
char GetUppercaseAnsiiChar(char c)
{
	if (c >= 'a' && c <= 'z') { return 'A' + (c - 'a'); }
	return c;
}

u8 CharToU8(char c)
{
	return *((u8*)&c);
}
u32 CharToU32(char c)
{
	return (u32)(*((u8*)&c));
}

char GetHexChar(u8 hexValue, bool upperCase = true)
{
	if (hexValue <= 9) { return '0' + hexValue; }
	else if (hexValue < 16) { return (upperCase ? 'A' : 'a') + (hexValue - 10); }
	else { return '?'; }
}
u8 GetUpperNibble(u8 value)
{
	return ((value & 0xF0) >> 4);
}
u8 GetLowerNibble(u8 value)
{
	return ((value & 0x0F) >> 0);
}

// +--------------------------------------------------------------+
// |                         Char Classes                         |
// +--------------------------------------------------------------+
bool IsCharNumeric(u32 codepoint)
{
	return (codepoint >= '0' && codepoint <= '9');
}
bool IsCharAlphabetic(u32 codepoint)
{
	if (codepoint >= 'a' && codepoint <= 'z') { return true; }
	else if (codepoint >= 'A' && codepoint <= 'Z') { return true; }
	else { return false; }
}
bool IsCharLowercaseAlphabet(u32 codepoint)
{
	return (codepoint >= 'a' && codepoint <= 'z');
}
bool IsCharUppercaseAlphabet(u32 codepoint)
{
	return (codepoint >= 'A' && codepoint <= 'Z');
}
bool IsCharWhitespace(u32 codepoint, bool includeNewLines = false)
{
	if (codepoint == ' ') { return true; }
	else if (codepoint == '\t') { return true; }
	else if (codepoint == '\n' && includeNewLines) { return true; }
	else if (codepoint == '\r' && includeNewLines) { return true; }
	else { return false; }
}
bool IsCharAnsii(u32 codepoint)
{
	return (codepoint < 128);
}
bool IsCharRenderableAnsii(u32 codepoint)
{
	return (codepoint >= ' ' && codepoint <= '~');
}
bool IsCharExtendedAnsii(u32 codepoint)
{
	return (codepoint <= 255);
}

// +--------------------------------------------------------------+
// |                       UTF-8 Functions                        |
// +--------------------------------------------------------------+
//returns number of bytes needed to store this codepoint in UTF-8 and stores the values in byteBufferOut if not nullptr
//NOTE: byteBufferOut is assumed to be 4 bytes or greater and no null-terminating character is written to the buffer
u8 GetUtf8BytesForCode(u32 codepoint, u8* byteBufferOut = nullptr, bool doAssertions = true)
{
	NotNull(byteBufferOut);
	if (codepoint <= 0x7F)
	{
		//0xxx xxxx
		if (byteBufferOut != nullptr) { byteBufferOut[0] = (u8)codepoint; }
		return 1;
	}
	else if (codepoint <= 0x7FF)
	{
		//110x xxxx 10xx xxxx
		if (byteBufferOut != nullptr)
		{
			byteBufferOut[0] = (0xC0 | ((codepoint >> 6) & 0x1F));
			byteBufferOut[1] = (0x80 | (codepoint & 0x3F));
		}
		return 2;
	}
	else if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
	{
		//invalid block
		Assert(!doAssertions || (false && "Invalid codepoint for UTF8"));
		if (byteBufferOut != nullptr) { byteBufferOut[0] = '\0'; }
		return 0;
	}
	else if (codepoint <= 0xFFFF)
	{
		//1110 xxxx 10xx xxxx 10xx xxxx
		if (byteBufferOut != nullptr)
		{
			byteBufferOut[0] = (0xE0 | ((codepoint >> 12) & 0x0F));
			byteBufferOut[1] = (0x80 | ((codepoint>>6) & 0x3F));
			byteBufferOut[2] = (0x80 | (codepoint & 0x3F));
		}
		return 3;
	}
	else if (codepoint <= UTF8_MAX_CODEPOINT)
	{
		//1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
		if (byteBufferOut != nullptr)
		{
			byteBufferOut[0] = (0xF0 | ((codepoint >> 18) & 0x07));
			byteBufferOut[1] = (0x80 | ((codepoint>>12) & 0x3F));
			byteBufferOut[2] = (0x80 | ((codepoint>>6) & 0x3F));
			byteBufferOut[3] = (0x80 | (codepoint & 0x3F));
		}
		return 4;
	}
	else
	{
		//everything above this point is also invalid
		Assert(!doAssertions || (false && "Invalid codepoint value for UTF8"));
		if (byteBufferOut != nullptr) { byteBufferOut[0] = '\0'; }
		return 0;
	}
}

//Returns the number of bytes consumed to complete the UTF-8 encoded character pointed to by strPntr
//If more bytes are needed for a full UTF-8 character than is specified by maxNumChars then we return 0
//If an invalid encoding is detected then we return 0. If maxNumChars == 0 then we return 0.
//TODO: Should we accept characters that are technically valid but encoded in a larger number of bytes than needed?
//      Like a 4-byte encoding of \0 would be dumb because it could cause non-null-terminated UTF-8 strings to become null terminated collapsed ASCII strings
u8 GetCodepointForUtf8(u64 maxNumChars, const char* strPntr, u32* codepointOut = nullptr) //somewhat tested
{
	Assert(strPntr != nullptr || maxNumChars == 0);
	if (codepointOut != nullptr) { *codepointOut = 0; }
	if (maxNumChars == 0) { return 0; }
	const u8* bytePntr = (const u8*)strPntr;
	if (bytePntr[0] <= 127)
	{
		if (codepointOut != nullptr) { *codepointOut = (u32)bytePntr[0]; }
		return 1;
	}
	else if (bytePntr[0] < 0xC0)
	{
		//An 10xx xxxx format for the first byte of a character is invalid
		return 0;
	}
	else if (bytePntr[0] < 0xE0)
	{
		if (maxNumChars < 2) { return 0; }
		if (bytePntr[1] < 0x80 || bytePntr[1] >= 0xC0) { return 0; }
		if (codepointOut != nullptr) { *codepointOut = ((u32)(bytePntr[0] & 0x1F) << 6) | ((u32)(bytePntr[1] & 0x3F) << 0); }
		return 2;
	}
	else if (bytePntr[0] < 0xF0)
	{
		if (maxNumChars < 3) { return 0; }
		if (bytePntr[1] < 0x80 || bytePntr[1] >= 0xC0) { return 0; }
		if (bytePntr[2] < 0x80 || bytePntr[2] >= 0xC0) { return 0; }
		if (codepointOut != nullptr) { *codepointOut = ((u32)(bytePntr[0] & 0x0F) << 12) | ((u32)(bytePntr[1] & 0x3F) << 6) | ((u32)(bytePntr[2] & 0x3F) << 0); }
		return 3;
	}
	else if (bytePntr[0] < 0xF8)
	{
		if (maxNumChars < 4) { return 0; }
		if (bytePntr[1] < 0x80 || bytePntr[1] >= 0xC0) { return 0; }
		if (bytePntr[2] < 0x80 || bytePntr[2] >= 0xC0) { return 0; }
		if (bytePntr[3] < 0x80 || bytePntr[3] >= 0xC0) { return 0; }
		if (codepointOut != nullptr) { *codepointOut = ((u32)(bytePntr[0] & 0x07) << 18) | ((u32)(bytePntr[1] & 0x3F) << 12) | ((u32)(bytePntr[2] & 0x3F) << 6) | ((u32)(bytePntr[3] & 0x3F) << 0); }
		return 4;
	}
	else
	{
		//Everything above this point is considered an invalid character to exist in UTF-8 encoded strings
		return 0;
	}
}

// +--------------------------------------------------------------+
// |                       String Functions                       |
// +--------------------------------------------------------------+
//NOTE: This technically works 'okay' for unicode strings, it's a bit strict in which characters are valid
bool IsStringValidIdentifier(u64 strLength, const char* strPntr, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false)
{
	Assert(strPntr != nullptr || strLength == 0);
	if (strLength == 0)
	{
		return allowEmpty;
	}
	
	for (u64 cIndex = 0; cIndex < strLength; cIndex++)
	{
		char nextChar = strPntr[cIndex];
		if ((nextChar >= 'a' && nextChar <= 'z') ||
			(nextChar >= 'A' && nextChar <= 'Z'))
		{
			continue;
		}
		else if (nextChar >= '0' && nextChar <= '9' && allowNumbers)
		{
			if (cIndex == 0 && !allowLeadingNumbers) { return false; }
			continue;
		}
		else if (nextChar == '_' && allowUnderscores)
		{
			continue;
		}
		else if (nextChar == ' ' && allowSpaces)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

#endif //  _GY_UNICODE_H
