/*
File:   gy_unicode.h
Author: Taylor Robbins
Date:   09\24\2021
Description:
	** Holds a bunch of functions that help us deal with characters, utf-8, and unicode in general
*/

#ifndef _GY_UNICODE_H
#define _GY_UNICODE_H

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
