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

#endif //  _GY_UNICODE_H
