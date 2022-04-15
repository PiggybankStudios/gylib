/*
File:   string.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#include <stdint.h>

#ifndef _STRING_H
#define _STRING_H

#if GY_WASM_STD_LIB
inline void* memcpy(void* __restrict dest, const void* __restrict src, size_t count) { return __builtin_memcpy(dest, src, count); }
#else
void* memcpy(void* __restrict dest, const void* __restrict src, size_t count)
{
	unsigned char* destPntr = (unsigned char*)dest;
	const unsigned char* srcPntr = (const unsigned char*)src;
	uint32_t word1, word2;
	
	for (; (uintptr_t)srcPntr % 4 && count; count--)
	{
		*destPntr++ = *srcPntr++;
	}
	
	if ((uintptr_t)destPntr % 4 == 0)
	{
		for (; count >= 16; srcPntr += 16, destPntr += 16, count -= 16)
		{
			*(uint32_t*)(destPntr+0) = *(uint32_t*)(srcPntr+0);
			*(uint32_t*)(destPntr+4) = *(uint32_t*)(srcPntr+4);
			*(uint32_t*)(destPntr+8) = *(uint32_t*)(srcPntr+8);
			*(uint32_t*)(destPntr+12) = *(uint32_t*)(srcPntr+12);
		}
		if (count & 8)
		{
			*(uint32_t*)(destPntr+0) = *(uint32_t*)(srcPntr+0);
			*(uint32_t*)(destPntr+4) = *(uint32_t*)(srcPntr+4);
			destPntr += 8; srcPntr += 8;
		}
		if (count & 4)
		{
			*(uint32_t*)(destPntr+0) = *(uint32_t*)(srcPntr+0);
			destPntr += 4; srcPntr += 4;
		}
		if (count & 2)
		{
			*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
		}
		if (count & 1)
		{
			*destPntr = *srcPntr;
		}
		return dest;
	}

	if (count >= 32)
	{
		switch ((uintptr_t)destPntr % 4)
		{
			case 1:
			{
				word1 = *(uint32_t*)srcPntr;
				*destPntr++ = *srcPntr++;
				*destPntr++ = *srcPntr++;
				*destPntr++ = *srcPntr++;
				count -= 3;
				for (; count >= 17; srcPntr += 16, destPntr += 16, count -= 16)
				{
					word2 = *(uint32_t*)(srcPntr + 1);
					*(uint32_t*)(destPntr + 0) = ((word1 >> 24) | (word2 << 8));
					word1 = *(uint32_t*)(srcPntr + 5);
					*(uint32_t*)(destPntr + 4) = ((word2 >> 24) | (word1 << 8));
					word2 = *(uint32_t*)(srcPntr + 9);
					*(uint32_t*)(destPntr + 8) = ((word1 >> 24) | (word2 << 8));
					word1 = *(uint32_t*)(srcPntr + 13);
					*(uint32_t*)(destPntr + 12) = ((word2 >> 24) | (word1 << 8));
				}
			} break;
			case 2:
			{
				word1 = *(uint32_t*)srcPntr;
				*destPntr++ = *srcPntr++;
				*destPntr++ = *srcPntr++;
				count -= 2;
				for (; count >= 18; srcPntr += 16, destPntr += 16, count -= 16)
				{
					word2 = *(uint32_t*)(srcPntr + 2);
					*(uint32_t*)(destPntr + 0) = ((word1 >> 16) | (word2 << 16));
					word1 = *(uint32_t*)(srcPntr + 6);
					*(uint32_t*)(destPntr + 4) = ((word2 >> 16) | (word1 << 16));
					word2 = *(uint32_t*)(srcPntr + 10);
					*(uint32_t*)(destPntr + 8) = ((word1 >> 16) | (word2 << 16));
					word1 = *(uint32_t*)(srcPntr + 14);
					*(uint32_t*)(destPntr + 12) = ((word2 >> 16) | (word1 << 16));
				}
			} break;
			case 3:
			{
				word1 = *(uint32_t*)srcPntr;
				*destPntr++ = *srcPntr++;
				count -= 1;
				for (; count >= 19; srcPntr += 16, destPntr += 16, count -= 16)
				{
					word2 = *(uint32_t*)(srcPntr + 3);
					*(uint32_t*)(destPntr + 0) = ((word1 >> 8) | (word2 << 24));
					word1 = *(uint32_t*)(srcPntr + 7);
					*(uint32_t*)(destPntr + 4) = ((word2 >> 8) | (word1 << 24));
					word2 = *(uint32_t*)(srcPntr + 11);
					*(uint32_t*)(destPntr + 8) = ((word1 >> 8) | (word2 << 24));
					word1 = *(uint32_t*)(srcPntr + 15);
					*(uint32_t*)(destPntr + 12) = ((word2 >> 8) | (word1 << 24));
				}
			} break;
		}
	}
	if (count & 16)
	{
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
	}
	if (count & 8)
	{
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
	}
	if (count & 4)
	{
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
	}
	if (count & 2)
	{
		*destPntr++ = *srcPntr++; *destPntr++ = *srcPntr++;
	}
	if (count & 1)
	{
		*destPntr = *srcPntr;
	}
	return dest;
}
#endif

#if GY_WASM_STD_LIB
inline void* memset(void* ptr, int value, size_t num) { return __builtin_memset(ptr, value, num);  }
#else
void* memset(void* dest, int value, size_t num)
{
	unsigned char* charPntr = (unsigned char*)dest;
	size_t alignment;
	
	// Fill head and tail with minimal branching. Each
	// conditional ensures that all the subsequently used
	// offsets are well-defined and in the dest region.
	if (!num) { return dest; }
	charPntr[0] = value;
	charPntr[num-1] = value;
	if (num <= 2) { return dest; }
	charPntr[1] = value;
	charPntr[2] = value;
	charPntr[num-2] = value;
	charPntr[num-3] = value;
	if (num <= 6) { return dest; }
	charPntr[3] = value;
	charPntr[num-4] = value;
	if (num <= 8) { return dest; }
	
	// Advance pointer to align it at a 4-byte boundary,
	// and truncate num to a multiple of 4. The previous code
	// already took care of any head/tail that get cut off
	// by the alignment.
	alignment = -(uintptr_t)charPntr & 3;
	charPntr += alignment;
	num -= alignment;
	num &= -4;
	
	uint32_t value32 = ((uint32_t)-1)/255 * (unsigned char)value;
	
	// In preparation to copy 32 bytes at a time, aligned on
	// an 8-byte bounary, fill head/tail up to 28 bytes each.
	// As in the initial byte-based head/tail fill, each
	// conditional below ensures that the subsequent offsets
	// are valid (e.g. !(num<=24) implies num>=28).
	*(uint32_t*)(charPntr+0) = value32;
	*(uint32_t*)(charPntr+num-4) = value32;
	if (num <= 8) { return dest; }
	*(uint32_t*)(charPntr+4) = value32;
	*(uint32_t*)(charPntr+8) = value32;
	*(uint32_t*)(charPntr+num-12) = value32;
	*(uint32_t*)(charPntr+num-8) = value32;
	if (num <= 24) { return dest; }
	*(uint32_t*)(charPntr+12) = value32;
	*(uint32_t*)(charPntr+16) = value32;
	*(uint32_t*)(charPntr+20) = value32;
	*(uint32_t*)(charPntr+24) = value32;
	*(uint32_t*)(charPntr+num-28) = value32;
	*(uint32_t*)(charPntr+num-24) = value32;
	*(uint32_t*)(charPntr+num-20) = value32;
	*(uint32_t*)(charPntr+num-16) = value32;
	
	// Align to a multiple of 8 so we can fill 64 bits at a time,
	// and avoid writing the same bytes twice as much as is
	// practical without introducing additional branching.
	alignment = 24 + ((uintptr_t)charPntr & 4);
	charPntr += alignment;
	num -= alignment;
	
	// If this loop is reached, 28 tail bytes have already been
	// filled, so any remainder when num drops below 32 can be
	// safely ignored.
	uint64_t value64 = (value32 | ((uint64_t)value32 << 32));
	for (; num >= 32; num -= 32, charPntr += 32)
	{
		*(uint64_t*)(charPntr+0) = value64;
		*(uint64_t*)(charPntr+8) = value64;
		*(uint64_t*)(charPntr+16) = value64;
		*(uint64_t*)(charPntr+24) = value64;
	}
	
	return dest;
}
#endif

void* memchr(const void* src, int c, size_t numBytes)
{
	const unsigned char* srcCharPntr = (const unsigned char*)src;
	c = (unsigned char)c;
	
	for (; ((uintptr_t)srcCharPntr & PTR_ALIGN) && numBytes && *srcCharPntr != c; srcCharPntr++, numBytes--);
	
	if (numBytes && *srcCharPntr != c)
	{
		const size_t* wordPntr;
		size_t charInverse = SIZET_ONES * c;
		for (wordPntr = (const size_t*)srcCharPntr; numBytes >= PTR_SIZE && !SIZET_HASZERO(*wordPntr ^ charInverse); wordPntr++, numBytes -= PTR_SIZE);
		srcCharPntr = (const unsigned char*)wordPntr;
	}
	
	for (; numBytes && *srcCharPntr != c; srcCharPntr++, numBytes--);
	
	return (numBytes ? (void*)srcCharPntr : 0);
}

void* memmove(void* dest, const void* src, size_t numBytes)
{
	char* destPntr = (char*)dest;
	const char* srcPntr = (const char*)src;
	
	if (destPntr == srcPntr) { return destPntr; }
	if ((uintptr_t)srcPntr - (uintptr_t)destPntr - numBytes <= -2*numBytes) { return memcpy(destPntr, srcPntr, numBytes); }
	
	if (destPntr < srcPntr)
	{
		if ((uintptr_t)srcPntr % sizeof(size_t) == (uintptr_t)destPntr % sizeof(size_t))
		{
			while ((uintptr_t)destPntr % sizeof(size_t))
			{
				if (!numBytes--) { return dest; }
				*destPntr++ = *srcPntr++;
			}
			for (; numBytes >= sizeof(size_t); numBytes -= sizeof(size_t), destPntr += sizeof(size_t), srcPntr += sizeof(size_t))
			{
				*(size_t*)destPntr = *(size_t*)srcPntr;
			}
		}
		for (; numBytes; numBytes--) { *destPntr++ = *srcPntr++; }
	}
	else
	{
		if ((uintptr_t)srcPntr % sizeof(size_t) == (uintptr_t)destPntr % sizeof(size_t))
		{
			while ((uintptr_t)(destPntr + numBytes) % sizeof(size_t))
			{
				if (!numBytes--) { return dest; }
				destPntr[numBytes] = srcPntr[numBytes];
			}
			while (numBytes >= sizeof(size_t))
			{
				*(size_t*)(destPntr + numBytes) = *(size_t*)(srcPntr + numBytes);
				numBytes -= sizeof(size_t);
			}
		}
		while (numBytes)
		{
			destPntr[numBytes] = srcPntr[numBytes];
			numBytes--;
		}
	}
	
	return dest;
}

int memcmp(const void* left, const void* right, size_t numBytes)
{
	const unsigned char* leftPntr = (const unsigned char*)left;
	const unsigned char* rightPntr = (const unsigned char*)right;
	for (; numBytes && *leftPntr == *rightPntr; numBytes--, leftPntr++, rightPntr++);
	return (numBytes ? *leftPntr - *rightPntr : 0);
}

char* strcpy(char* __restrict dest, const char* __restrict src)
{
	size_t* destWord;
	const size_t* srcWord;
	
	if (((uintptr_t)src % sizeof(size_t)) == ((uintptr_t)dest % sizeof(size_t)))
	{
		for (; (uintptr_t)src % sizeof(size_t); src++, dest++)
		{
			if (!(*dest = *src)) { return dest; }
		}
		destWord = (size_t*)dest; srcWord = (const size_t*)src;
		for (; !SIZET_HASZERO(*srcWord); *destWord++ = *srcWord++);
		dest = (char*)destWord; src = (const char*)srcWord;
	}
	
	for (; (*dest = *src); src++, dest++);

	return dest;
}

//TODO: Get rid of the goto use here
char* strncpy(char* __restrict dest, const char* __restrict src, size_t numChars)
{
	typedef size_t __attribute__((__may_alias__)) word;
	word* destWord;
	const word* srcWord;
	
	if (((uintptr_t)src & PTR_ALIGN) == ((uintptr_t)dest & PTR_ALIGN))
	{
		for (; ((uintptr_t)src & PTR_ALIGN) && numChars && (*dest = *src); numChars--, src++, dest++);
		if (!numChars || !*src) { goto tail; }
		destWord = (size_t*)dest; srcWord = (const size_t*)src;
		for (; numChars >= sizeof(size_t) && !SIZET_HASZERO(*srcWord); numChars -= sizeof(size_t), srcWord++, destWord++)
		{
			*destWord = *srcWord;
		}
		dest = (char*)destWord; src = (const char*)srcWord;
	}
	
	for (; numChars && (*dest = *src); numChars--, src++, dest++);
	
	tail:
	memset(dest, 0, numChars);
	return dest;
}

int strcmp(const char* left, const char* right)
{
	for (; *left == *right && *left; left++, right++);
	return (*(unsigned char*)left - *(unsigned char*)right);
}

int strncmp(const char* left, const char* right, size_t numChars)
{
	const unsigned char* leftPntr = (const unsigned char*)left;
	const unsigned char* rightPntr = (const unsigned char*)right;
	if (!numChars--) { return 0; }
	for (; *leftPntr && *rightPntr && numChars && *leftPntr == *rightPntr; leftPntr++, rightPntr++, numChars--);
	return (*leftPntr - *rightPntr);
}

inline size_t strlen(const char* str)
{
	const char* startPntr = str;
	const size_t* wordPntr;
	
	for (; (uintptr_t)str % PTR_SIZE; str++)
	{
		if (!*str) { return (str - startPntr); }
	}
	
	for (wordPntr = (const size_t*)str; !SIZET_HASZERO(*wordPntr); wordPntr++);
	str = (const char*)wordPntr;
	
	for (; *str; str++);
	
	return (str - startPntr);
}

inline size_t wcslen(const wchar_t* str)
{
	const wchar_t* startPntr;
	for (startPntr = str; *str; str++);
	return (str - startPntr);
}

inline char* __strchrnul(const char* string, int character)
{
	character = (unsigned char)character;
	if (character == 0) { return (char*)string + strlen(string); }
	
	const size_t* wordPntr;
	for (; (uintptr_t)string % PTR_SIZE; string++)
	{
		if (!*string || *(unsigned char*)string == character) { return (char*)string; }
	}
	
	size_t mask = SIZET_ONES * character;
	for (wordPntr = (size_t*)string; !SIZET_HASZERO(*wordPntr) && !SIZET_HASZERO(*wordPntr^mask); wordPntr++);
	
	string = (char*)wordPntr;
	for (; *string && *(unsigned char*)string != character; string++);
	return (char*)string;
}

inline char* strchr(const char* s, int character)
{
	char* result = __strchrnul(s, character);
	return (*(unsigned char*)result == (unsigned char)character) ? result : nullptr;
}

char* twobyte_strstr(const unsigned char* str1, const unsigned char* str2)
{
	uint16_t word1 = str1[0]<<8 | str1[1];
	uint16_t word2 = str2[0]<<8 | str2[1];
	for (str1++; *str1 && word1 != word2; word1 = word1<<8 | *++str1);
	return (*str1 ? (char*)str1-1 : nullptr);
}

char* threebyte_strstr(const unsigned char* str1, const unsigned char* str2)
{
	uint32_t word2 = (uint32_t)str2[0]<<24 | str2[1]<<16 | str2[2]<<8;
	uint32_t word1 = (uint32_t)str1[0]<<24 | str1[1]<<16 | str1[2]<<8;
	for (str1 += 2; *str1 && word1 != word2; word1 = (word1 | *++str1)<<8);
	return (*str1 ? (char*)str1-2 : nullptr);
}

char* fourbyte_strstr(const unsigned char* str1, const unsigned char* str2)
{
	uint32_t word2 = (uint32_t)str2[0]<<24 | str2[1]<<16 | str2[2]<<8 | str2[3];
	uint32_t word1 = (uint32_t)str1[0]<<24 | str1[1]<<16 | str1[2]<<8 | str1[3];
	for (str1 += 3; *str1 && word1 != word2; word1 = word1<<8 | *++str1);
	return (*str1 ? (char*)str1-3 : nullptr);
}

char* twoway_strstr(const unsigned char* str1, const unsigned char* str2)
{
	const unsigned char* variableZ;
	size_t strLength, variableIP, variableJP, variableK, variableP, variableMs, variableP0, mem, mem0;
	size_t byteset[32 / sizeof(size_t)] = { 0 };
	size_t shift[256];
	
	//Computing length of needle and fill shift table
	for (strLength = 0; str2[strLength] && str1[strLength]; strLength++)
	{
		shift[str2[strLength]] = strLength+1;
		BITOP(byteset, str2[strLength], |=);
	}
	if (str2[strLength]) { return 0; } //hit the end of str1
	
	//Compute maximal suffix
	variableIP = -1;
	variableJP = 0;
	variableK = 1;
	variableP = 1;
	while (variableJP + variableK < strLength)
	{
		if (str2[variableIP + variableK] == str2[variableJP + variableK])
		{
			if (variableK == variableP)
			{
				variableJP += variableP;
				variableK = 1;
			}
			else { variableK++; }
		}
		else if (str2[variableIP + variableK] > str2[variableJP + variableK])
		{
			variableJP += variableK;
			variableK = 1;
			variableP = variableJP - variableIP;
		}
		else
		{
			variableIP = variableJP++;
			variableK = variableP = 1;
		}
	}
	variableMs = variableIP;
	variableP0 = variableP;
	
	//And with the opposite comparison
	variableIP = -1;
	variableJP = 0;
	variableK = 1;
	variableP = 1;
	while (variableJP + variableK < strLength)
	{
		if (str2[variableIP + variableK] == str2[variableJP + variableK])
		{
			if (variableK == variableP)
			{
				variableJP += variableP;
				variableK = 1;
			}
			else { variableK++; }
		}
		else if (str2[variableIP + variableK] < str2[variableJP + variableK])
		{
			variableJP += variableK;
			variableK = 1;
			variableP = variableJP - variableIP;
		}
		else
		{
			variableIP = variableJP++;
			variableK = variableP = 1;
		}
	}
	if (variableIP+1 > variableMs+1) { variableMs = variableIP; }
	else { variableP = variableP0; }
	
	//Periodic needle?
	if (memcmp(str2, str2 + variableP, variableMs+1))
	{
		mem0 = 0;
		variableP = MAX(variableMs, strLength-variableMs-1) + 1;
	}
	else { mem0 = strLength-variableP; }
	mem = 0;
	
	//Initialize incremental end-of-haystack pointer
	variableZ = str1;
	
	//Search loop
	for (;;)
	{
		//Update incremental end-of-haystack pointer
		if (variableZ-str1 < strLength)
		{
			//Fast estimate for MAX(strLength,63)
			size_t grow = strLength | 63;
			const unsigned char* z2 = (const unsigned char*)memchr(variableZ, 0, grow);
			if (z2)
			{
				variableZ = z2;
				if (variableZ-str1 < strLength) { return 0; }
			}
			else { variableZ += grow; }
		}
		
		//Check last byte first; advance by shift on mismatch
		if (BITOP(byteset, str1[strLength-1], &))
		{
			variableK = strLength - shift[str1[strLength-1]];
			if (variableK)
			{
				if (variableK < mem) { variableK = mem; }
				str1 += variableK;
				mem = 0;
				continue;
			}
		}
		else
		{
			str1 += strLength;
			mem = 0;
			continue;
		}
		
		//Compare right half
		for (variableK = MAX(variableMs+1,mem); str2[variableK] && str2[variableK] == str1[variableK]; variableK++);
		if (str2[variableK])
		{
			str1 += variableK-variableMs;
			mem = 0;
			continue;
		}
		//Compare left half
		for (variableK = variableMs+1; variableK > mem && str2[variableK-1] == str1[variableK-1]; variableK--);
		if (variableK <= mem) { return (char *)str1; }
		str1 += variableP;
		mem = mem0;
	}
}

char* strstr(const char* str1, const char* str2)
{
	// Return immediately on empty needle
	if (!str2[0]) { return (char *)str1; }
	
	// Use faster algorithms for short needles
	str1 = strchr(str1, *str2);
	if (!str1 || !str2[1]) { return (char *)str1; }
	if (!str1[1]) { return 0; }
	if (!str1[2]) { return twobyte_strstr((unsigned char*)str1, (unsigned char*)str2); }
	if (!str1[2]) { return 0; }
	if (!str1[3]) { return threebyte_strstr((unsigned char*)str1, (unsigned char*)str2); }
	if (!str1[3]) { return 0; }
	if (!str1[4]) { return fourbyte_strstr((unsigned char*)str1, (unsigned char*)str2); }
	
	return twoway_strstr((unsigned char*)str1, (unsigned char*)str2);
}
char* strstr(char* str1, char* str2) //non-const-variant
{
	return (char*)strstr((const char*)str1, (const char*)str2);
}

// +--------------------------------------------------------------+
// |    Potentially Important Stuff That We Currently Dont Use    |
// +--------------------------------------------------------------+
#if 0
char* strcat(char* __restrict, const char* __restrict);
char* strncat(char* __restrict, const char* __restrict, size_t);

int strcoll(const char* , const char* );
size_t strxfrm(char* __restrict, const char* __restrict, size_t);

char* strchr(const char* , int);
char* strrchr(const char* , int);

size_t strcspn(const char* , const char* );
size_t strspn(const char* , const char* );
char* strpbrk(const char* , const char* );
char* strtok(char* __restrict, const char* __restrict);

char* strerror(int);

char* strtok_r(char* __restrict, const char* __restrict, char* *__restrict);
int strerror_r(int, char* , size_t);
char* stpcpy(char* __restrict, const char* __restrict);
char* stpncpy(char* __restrict, const char* __restrict, size_t);
size_t strnlen(const char* , size_t);
char* strdup(const char* );
char* strndup(const char* , size_t);
char* strsignal(int);
char* strerror_l(int, locale_t);
int strcoll_l(const char* , const char* , locale_t);
size_t strxfrm_l(char* __restrict, const char* __restrict, size_t, locale_t);

void* memccpy(void* __restrict, const void* __restrict, int, size_t);

char* strsep(char* *, const char* );
size_t strlcat(char* , const char* , size_t);
size_t strlcpy(char* , const char* , size_t);
void explicit_bzero(void* , size_t);

#define	strdupa(x)	strcpy(alloca(strlen(x)+1),x)
int strverscmp(const char* , const char* );
char* strchrnul(const char* , int);
char* strcasestr(const char* , const char* );
void* memmem(const void* , size_t, const void* , size_t);
void* memrchr(const void* , int, size_t);
void* mempcpy(void* , const void* , size_t);
#ifndef __cplusplus
char* basename();
#endif
#endif

#endif //  _STRING_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
void* memcpy(void* __restrict dest, const void* __restrict src, size_t count)
void* memset(void* dest, int value, size_t num)
void* memmove(void* dest, const void* src, size_t numBytes)
int memcmp(const void* left, const void* right, size_t numBytes)
char* strcpy(char* __restrict dest, const char* __restrict src)
char* strncpy(char* __restrict dest, const char* __restrict src, size_t numChars)
int strcmp(const char* left, const char* right)
int strncmp(const char* left, const char* right, size_t numChars)
inline size_t strlen(const char* str)
inline size_t wcslen(const wchar_t* str)
inline char* __strchrnul(const char* string, int character)
inline char* strchr(const char* s, int character)
char* twobyte_strstr(const unsigned char* str1, const unsigned char* str2)
char* threebyte_strstr(const unsigned char* str1, const unsigned char* str2)
char* fourbyte_strstr(const unsigned char* str1, const unsigned char* str2)
char* twoway_strstr(const unsigned char* str1, const unsigned char* str2)
char* strstr(const char* str1, const char* str2)
*/
