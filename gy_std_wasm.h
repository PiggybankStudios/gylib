/*
File:   gy_std_wasm.h
Author: Taylor Robbins
Date:   03\19\2022
Description:
	** Contains the Web Assembly implementations for various standard library functions
*/

#ifndef _GY_STD_WASM_H
#define _GY_STD_WASM_H

// +--------------------------------------------------------------+
// |               Custom stdint.h Implementations                |
// +--------------------------------------------------------------+
//TODO: This is probably inaccurate. We just need it to get us off the ground for now. We should test the size of these types and do some research later
typedef char          int8_t;
typedef short         int16_t;
typedef int           int32_t;
typedef long long     int64_t;
typedef long          size_t;
typedef unsigned long uintptr_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

#define INT8_MIN  -128
#define INT16_MIN -32768
#define INT32_MIN -2147483648L
#define INT64_MIN -9223372036854775808LL

#define INT8_MAX  127
#define INT16_MAX 32767
#define INT32_MAX 2147483647L
#define INT64_MAX 9223372036854775807LL

#define UINT8_MAX  255U
#define UINT16_MAX 65535U
#define UINT32_MAX 4294967295UL
#define UINT64_MAX 18446744073709551615ULL

#define PTR_SIZE  (sizeof(size_t))
#define PTR_ALIGN (sizeof(size_t)-1)
#define SIZET_ONES ((size_t)-1/UINT8_MAX)
#define SIZET_HIGHS (SIZET_ONES * (UINT8_MAX/2+1))
#define SIZET_HASZERO(x) ((x)-SIZET_ONES & ~(x) & SIZET_HIGHS)

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#define BITOP(a, b, op) ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

// +--------------------------------------------------------------+
// |               Custom stdarg.h Implementations                |
// +--------------------------------------------------------------+
//This comes from LLVM's implementation of stdarg.h
typedef __builtin_va_list va_list;
#define _VA_LIST
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
#define va_copy(dest, src)  __builtin_va_copy(dest, src)

// +--------------------------------------------------------------+
// |                Custom stdlib Implementations                 |
// +--------------------------------------------------------------+
inline void* memset(void* ptr, int value, size_t num)          { return __builtin_memset(ptr, value, num);  }
inline void* memcpy(void* dest, const void* src, size_t count) { return __builtin_memcpy(dest, src, count); }

//NOTE: These implementations are currently heavily based on musl lib implementations
//TODO: Check the license on musl lib. Maybe we should just use it directly

inline int memcmp(const void* ptr1, const void* ptr2, size_t size)
{
	const unsigned char* p1 = (const unsigned char*)ptr1;
	const unsigned char* p2 = (const unsigned char*)ptr2;
	for (; size && *p1 == *p2; size--, p1++, p2++);
	return (size ? ((*p1) - (*p2)) : 0);
}

inline void* memmove(void* dest, const void* src, size_t count)
{
	char* destPtr = (char*)dest;
	const char* srcPtr = (const char*)src;
	
	if (destPtr == srcPtr) { return destPtr; }
	if ((uintptr_t)srcPtr - (uintptr_t)destPtr - count <= -2*count)
	{
		return memcpy(destPtr, srcPtr, count);
	}
	
	if (destPtr < srcPtr)
	{
		if ((uintptr_t)srcPtr % PTR_SIZE == (uintptr_t)destPtr % PTR_SIZE)
		{
			while ((uintptr_t)destPtr % PTR_SIZE)
			{
				if (!count--) { return dest; }
				*destPtr++ = *srcPtr++;
			}
			for (; count >= PTR_SIZE; count -= PTR_SIZE, destPtr += PTR_SIZE, srcPtr += PTR_SIZE)
			{
				*(size_t*)destPtr = *(size_t*)srcPtr;
			}
		}
		for (; count; count--) { *destPtr++ = *srcPtr++; }
	}
	else
	{
		if ((uintptr_t)srcPtr % PTR_SIZE == (uintptr_t)destPtr % PTR_SIZE)
		{
			while ((uintptr_t)(destPtr+count) % PTR_SIZE)
			{
				if (!count--) { return dest; }
				destPtr[count] = srcPtr[count];
			}
			while (count >= PTR_SIZE)
			{
				*(size_t*)(destPtr+count) = *(size_t*)(srcPtr+count);
				count -= PTR_SIZE;
			}
		}
		while (count)
		{
			destPtr[count] = srcPtr[count];
			count--;
		}
	}
	
	return dest;
}

inline void* memchr(const void* src, int character, size_t count)
{
	const unsigned char* charPntr = (const unsigned char*)src;
	character = (unsigned char)character;
	for (; ((uintptr_t)charPntr & PTR_ALIGN) && count && *charPntr != character; charPntr++, count--);
	if (count && *charPntr != character)
	{
		const size_t* wordPntr;
		size_t mask = SIZET_ONES * character;
		for (wordPntr = (const size_t*)charPntr; count >= PTR_SIZE && !SIZET_HASZERO(*wordPntr^mask); wordPntr++, count -= PTR_SIZE);
		charPntr = (const unsigned char*)wordPntr;
	}
	for (; count && *charPntr != character; charPntr++, count--);
	return (count ? (void*)charPntr : 0);
}

inline char* strcpy(char* dest, const char* src)
{
	size_t* destWordPntr;
	const size_t* srcWordPntr;
	
	if ((uintptr_t)src % PTR_SIZE == (uintptr_t)dest % PTR_SIZE)
	{
		for (; (uintptr_t)src % PTR_SIZE; src++, dest++)
		{
			if (!(*dest = *src)) { return dest; }
		}
		destWordPntr = (size_t*)dest;
		srcWordPntr = (const size_t*)src;
		for (; !SIZET_HASZERO(*srcWordPntr); *destWordPntr++ = *srcWordPntr++);
		dest = (char*)destWordPntr;
		src = (const char*)srcWordPntr;
	}
	for (; (*dest = *src); src++, dest++);
	
	return dest;
}

inline int strcmp(const char* str1, const char* str2)
{
	for (; (*str1 == *str2) && *str1; str1++, str2++);
	return (*(unsigned char*)str1 - *(unsigned char*)str2);
}

inline int strncmp(const char* str1, const char* str2, size_t count)
{
	const unsigned char* _str1 = (const unsigned char*)str1;
	const unsigned char* _str2 = (const unsigned char*)str2;
	if (!count--) { return 0; }
	for (; *_str1 && *_str2 && count && (*_str1 == *_str2); _str1++, _str2++, count--);
	return (*_str1 - *_str2);
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

inline char* twobyte_strstr(const unsigned char* str1, const unsigned char* str2)
{
	uint16_t word1 = str1[0]<<8 | str1[1];
	uint16_t word2 = str2[0]<<8 | str2[1];
	for (str1++; *str1 && word1 != word2; word1 = word1<<8 | *++str1);
	return (*str1 ? (char*)str1-1 : nullptr);
}

inline char* threebyte_strstr(const unsigned char* str1, const unsigned char* str2)
{
	uint32_t word2 = (uint32_t)str2[0]<<24 | str2[1]<<16 | str2[2]<<8;
	uint32_t word1 = (uint32_t)str1[0]<<24 | str1[1]<<16 | str1[2]<<8;
	for (str1 += 2; *str1 && word1 != word2; word1 = (word1 | *++str1)<<8);
	return (*str1 ? (char*)str1-2 : nullptr);
}

inline char* fourbyte_strstr(const unsigned char* str1, const unsigned char* str2)
{
	uint32_t word2 = (uint32_t)str2[0]<<24 | str2[1]<<16 | str2[2]<<8 | str2[3];
	uint32_t word1 = (uint32_t)str1[0]<<24 | str1[1]<<16 | str1[2]<<8 | str1[3];
	for (str1 += 3; *str1 && word1 != word2; word1 = word1<<8 | *++str1);
	return (*str1 ? (char*)str1-3 : nullptr);
}

inline char* twoway_strstr(const unsigned char* str1, const unsigned char* str2)
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

inline char* strstr(const char* str1, const char* str2)
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
inline char* strstr(char* str1, char* str2) //non-const-variant
{
	return (char*)strstr((const char*)str1, (const char*)str2);
}

#ifndef vsnprintf
#error vsnprintf must be defined before including gy_std.h while defining GY_CUSTOM_STD_LIB
#endif

// +--------------------------------------------------------------+
// |              Custom Instrinsic Implementations               |
// +--------------------------------------------------------------+
//If we need SIMD types for WASM we can use these typedefs
typedef char i8x16 __attribute((vector_size(16)));
typedef short i16x8 __attribute((vector_size(16)));
typedef int i32x4 __attribute((vector_size(16)));
typedef long long i64x2 __attribute((vector_size(16)));
typedef unsigned char u8x16 __attribute((vector_size(16)));
typedef unsigned short u16x8 __attribute((vector_size(16)));
typedef unsigned int u32x4 __attribute((vector_size(16)));
typedef unsigned long long u64x2 __attribute((vector_size(16)));
typedef float f32x4 __attribute((vector_size(16)));
typedef double f64x2 __attribute((vector_size(16)));

inline float  fminf(float value1, float value2)  { return __builtin_fminf(value1, value2); }
inline double fmin(double value1, double value2) { return __builtin_fmin(value1, value2);  }
inline float  fmaxf(float value1, float value2)  { return __builtin_fmaxf(value1, value2); }
inline double fmax(double value1, double value2) { return __builtin_fmax(value1, value2);  }

inline float  absf(float value)                  { return __builtin_fabsf(value);          }
inline double abs(double value)                  { return __builtin_fabs(value);           }
inline float  fmodf(float numer, float denom)    { return __builtin_fmodf(numer, denom);   }
inline double fmod(double numer, double denom)   { return __builtin_fmod(numer, denom);    }

inline float  roundf(float value)                { return __builtin_roundf(value);         }
inline double round(double value)                { return __builtin_round(value);          }
inline float  floorf(float value)                { return __builtin_floorf(value);         }
inline double floor(double value)                { return __builtin_floor(value);          }
inline float  ceilf(float value)                 { return __builtin_ceilf(value);          }
inline double ceil(double value)                 { return __builtin_ceil(value);           }

inline float  sinf(float value)                  { return __builtin_sinf(value);           }
inline double sin(double value)                  { return __builtin_sin(value);            }
inline float  asinf(float value)                 { return __builtin_asinf(value);          }
inline double asin(double value)                 { return __builtin_asin(value);           }
inline float  cosf(float value)                  { return __builtin_cosf(value);           }
inline double cos(double value)                  { return __builtin_cos(value);            }
inline float  acosf(float value)                 { return __builtin_acosf(value);          }
inline double acos(double value)                 { return __builtin_acos(value);           }
inline float  tanf(float value)                  { return __builtin_tanf(value);           }
inline double tan(double value)                  { return __builtin_tan(value);            }
inline float  atanf(float value)                 { return __builtin_atanf(value);          }
inline double atan(double value)                 { return __builtin_atan(value);           }
inline float  atan2f(float numer, float denom)   { return __builtin_atan2f(numer, denom);  }
inline double atan2(double numer, double denom)  { return __builtin_atan2(numer, denom);   }

inline float  powf(float value, float exponent)  { return __builtin_powf(value, exponent); }
inline double pow(double value, double exponent) { return __builtin_pow(value, exponent);  }
inline float  sqrtf(float value)                 { return __builtin_sqrtf(value);          }
inline double sqrt(double value)                 { return __builtin_sqrt(value);           }
inline float  cbrtf(float value)                 { return __builtin_cbrtf(value);          }
inline double cbrt(double value)                 { return __builtin_cbrt(value);           }

inline bool   isnan(float value)                 { return __builtin_isnan(value);          }
inline bool   isinf(float value)                 { return __builtin_isinf(value);          }

#define RAND_MAX INT32_MAX
void srand(unsigned int value)
{
	//TODO: Implement me!
}
int rand()
{
	//TODO: Implement me!
	return 0;
}

double atof(const char* str)
{
	//TODO: Implement me!
	return 0;
}

#endif //  _GY_STD_WASM_H
