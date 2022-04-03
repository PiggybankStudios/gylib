/*
File:   ctype.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#ifndef _CTYPE_H
#define _CTYPE_H

// +--------------------------------------------------------------+
// |                       is[x] Functions                        |
// +--------------------------------------------------------------+
int isdigit(int c)
{
	return (((unsigned)c) - '0') < 10;
}

int isxdigit(int c)
{
	return (isdigit(c) || ((((unsigned)c) | 32) - 'a') < 6);
}

int isspace(int c)
{
	return (c == ' ' || (((unsigned)c) - '\t') < 5);
}

int islower(int c)
{
	return ((((unsigned)c) - 'a') < 26);
}

int isupper(int c)
{
	return ((((unsigned)c) - 'A') < 26);
}

int isalpha(int c)
{
	return (((((unsigned)c) | 32) - 'a') < 26);
}

int isalnum(int c)
{
	return (isalpha(c) || isdigit(c));
}

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}

int iscntrl(int c)
{
	return (((unsigned)c) < 0x20 || c == 0x7F);
}

int isgraph(int c)
{
	return ((((unsigned)c) - 0x21) < 0x5E);
}

int isprint(int c)
{
	return ((((unsigned)c) - 0x20) < 0x5F);
}

int ispunct(int c)
{
	return (isgraph(c) && !isalnum(c));
}

int isascii(int c)
{
	return ((c & ~0x7F) == 0);
}

// +--------------------------------------------------------------+
// |                       to[x] Functions                        |
// +--------------------------------------------------------------+
int toupper(int c)
{
	if (islower(c)) { return (c & 0x5F); }
	return c;
}

int tolower(int c)
{
	if (isupper(c)) { return (c | 32); }
	return c;
}

int toascii(int c)
{
	return (c & 0x7F);
}

#endif //  _CTYPE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
int isdigit(int c)
int isxdigit(int c)
int isspace(int c)
int islower(int c)
int isupper(int c)
int isalpha(int c)
int isalnum(int c)
int isblank(int c)
int iscntrl(int c)
int isgraph(int c)
int isprint(int c)
int ispunct(int c)
int isascii(int c)
int toupper(int c)
int tolower(int c)
int toascii(int c)
*/
