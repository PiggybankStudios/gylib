/*
File:   stdlib.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <string.h>

#ifndef _STDLIB_H
#define _STDLIB_H

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define RAND_MAX (0x7fffffff)

// +--------------------------------------------------------------+
// |                      Parsing Functions                       |
// +--------------------------------------------------------------+
int atoi(const char* nullTermStr)
{
	int result = 0;
	bool isNegative = 0;
	
	// Eat whitespace
	while (isspace(*nullTermStr))
	{
		nullTermStr++;
	}
	
	// Handle sign characters
	switch (*nullTermStr)
	{
		case '-': isNegative = true; nullTermStr++; break;
		case '+': nullTermStr++; break;
	}
	
	// Actually parse the digits
	// NOTE: We aggregate in the negative space to avoid overflow on INT_MAX
	while (isdigit(*nullTermStr))
	{
		result = 10*result - (*nullTermStr - '0');
		nullTermStr++;
	}
	
	return (isNegative ? result : -result);
}

long atol(const char* nullTermStr)
{
	long result = 0;
	bool isNegative = 0;
	
	// Eat whitespace
	while (isspace(*nullTermStr))
	{
		nullTermStr++;
	}
	
	// Handle sign characters
	switch (*nullTermStr)
	{
		case '-': isNegative = true; nullTermStr++; break;
		case '+': nullTermStr++; break;
	}
	
	// Actually parse the digits
	// NOTE: We aggregate in the negative space to avoid overflow on INT_MAX
	while (isdigit(*nullTermStr))
	{
		result = 10*result - (*nullTermStr - '0');
		nullTermStr++;
	}
	
	return (isNegative ? result : -result);
}

long long atoll(const char* nullTermStr)
{
	long long result = 0;
	bool isNegative = 0;
	
	// Eat whitespace
	while (isspace(*nullTermStr))
	{
		nullTermStr++;
	}
	
	// Handle sign characters
	switch (*nullTermStr)
	{
		case '-': isNegative = true; nullTermStr++; break;
		case '+': nullTermStr++; break;
	}
	
	// Actually parse the digits
	// NOTE: We aggregate in the negative space to avoid overflow on INT_MAX
	while (isdigit(*nullTermStr))
	{
		result = 10*result - (*nullTermStr - '0');
		nullTermStr++;
	}
	
	return (isNegative ? result : -result);
}

// +--------------------------------------------------------------+
// |                    Float Point Conversion                    |
// +--------------------------------------------------------------+
// void __shlim(FILE *, off_t);
// #define shcnt(f) ((f)->shcnt + ((f)->rpos - (f)->buf))
// #define shlim(f, lim) __shlim((f), (lim))
// #define sh_fromstring(f, s) ((f)->buf = (f)->rpos = (unsigned char*)(s), (f)->rend = (unsigned char*)-1)

//TODO: Rename these defines at the very least!
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024

#define LD_B1B_DIG 2
#define LD_B1B_MAX 9007199, 254740991
#define FLOATSCAN_MAX_DIGITS 128

#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384

#define LD_B1B_DIG 3
#define LD_B1B_MAX 18, 446744073, 709551615
#define FLOATSCAN_MAX_DIGITS 2048

#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384

#define LD_B1B_DIG 4
#define LD_B1B_MAX 10384593, 717069655, 257060992, 658440191
#define FLOATSCAN_MAX_DIGITS 2048

#else
#error Unsupported long double representation
#endif

#define MASK (FLOATSCAN_MAX_DIGITS-1)

typedef struct
{
	const char* strPntr;
	size_t strLength;
	size_t readIndex;
} FloatScanState_t;

char _floatScanReadChar(FloatScanState_t* scanState)
{
	if (scanState->readIndex >= scanState->strLength) { scanState->readIndex++; return '\0'; }
	char result = scanState->strPntr[scanState->readIndex];
	scanState->readIndex++;
	return result;
}
void _floatScanStepBack(FloatScanState_t* scanState)
{
	if (scanState->readIndex == 0) { return; }
	scanState->readIndex--;
}
char _floatScanGetChar(FloatScanState_t* scanState, size_t offset)
{
	if (scanState->readIndex + offset >= scanState->strLength) { return '\0'; }
	char result = scanState->strPntr[scanState->readIndex + offset];
	return result;
}

long long scanexp(FloatScanState_t* scanState)
{
	int nextChar;
	int smallAccum;
	long long largeAccum;
	int isNegative = 0;
	
	nextChar = _floatScanReadChar(scanState);
	if (nextChar == '+' || nextChar == '-')
	{
		isNegative = (nextChar == '-');
		nextChar = _floatScanReadChar(scanState);
		// if ((nextChar - '0') >= 10U && pok) { _floatScanStepBack(scanState); }
	}
	if ((nextChar - '0') >= 10U)
	{
		_floatScanStepBack(scanState);
		return LLONG_MIN;
	}
	
	for (smallAccum = 0; (nextChar - '0') < 10U && smallAccum < INT_MAX/10; nextChar = _floatScanReadChar(scanState))
	{
		smallAccum = 10*smallAccum + (nextChar - '0');
	}
	for (largeAccum = smallAccum; (nextChar - '0') < 10U && largeAccum < LLONG_MAX/100; nextChar = _floatScanReadChar(scanState))
	{
		largeAccum = 10*largeAccum + (nextChar - '0');
	}
	for (; (nextChar - '0') < 10U; nextChar = _floatScanReadChar(scanState));
	
	_floatScanStepBack(scanState);
	return (isNegative ? -largeAccum : largeAccum);
}

long double hexfloat(FloatScanState_t* scanState, int bits, int emin, int sign)
{
	//TODO: Implement me!
	return 0;
}

long double decfloat(FloatScanState_t* scanState, int bits, int emin, int sign)
{
	static const uint32_t th[] = { LD_B1B_MAX };
	static const int p10s[] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 }; //up to 100 million
	int emax = (-emin - bits) + 3;
	long long lrp = 0; //TODO: Name this better! (negative power of position we are currently at after period? It's a long long version of rp)
	long long dc = 0; //TODO: Name this better!
	int lnz = 0; //TODO: Name this better!
	
	//Trim leading 0's
	int nextChar = _floatScanReadChar(scanState);
	while (nextChar == '0') { nextChar = _floatScanReadChar(scanState); }
	
	bool foundPeriod = false;
	bool foundDigits = false;
	if (nextChar == '.')
	{
		foundPeriod = true;
		for (nextChar = _floatScanReadChar(scanState); nextChar == '0'; nextChar = _floatScanReadChar(scanState))
		{
			foundDigits = true;
			lrp--;
		}
	}
	
	// Collect digits? TODO: Figure out what this section does for real
	uint32_t digits[FLOATSCAN_MAX_DIGITS];
	digits[0] = 0;
	int digitIndex = 0;
	int j = 0; //TODO: Name this better!
	for (; nextChar - '0' < 10U || nextChar == '.'; nextChar = _floatScanReadChar(scanState))
	{
		if (nextChar == '.')
		{
			if (foundPeriod) { break; }
			foundPeriod = true;
			lrp = dc;
		}
		else if (digitIndex < FLOATSCAN_MAX_DIGITS-3)
		{
			dc++;
			if (nextChar != '0') { lnz = dc; }
			
			if (j)
			{
				digits[digitIndex] = digits[digitIndex]*10 + (nextChar - '0');
			}
			else
			{
				digits[digitIndex] = (nextChar - '0');
			}
			
			if (++j == 9)
			{
				digitIndex++;
				j = 0;
			}
			
			foundDigits = true;
		}
		else
		{
			dc++;
			if (nextChar != '0')
			{
				lnz = (FLOATSCAN_MAX_DIGITS - 4) * 9;
				digits[FLOATSCAN_MAX_DIGITS - 4] |= 1;
			}
		}
	}
	if (!foundPeriod)
	{
		lrp = dc;
	}
	
	long long e10 = 0;
	if (foundDigits && (nextChar | 32) == 'e')
	{
		e10 = scanexp(scanState);
		if (e10 == LLONG_MIN)
		{
			//TODO: The example does shlim here? What does shlim do on the file? Should we consume some number of bytes??
			return 0;
		}
		lrp += e10;
	}
	else if (nextChar >= 0)
	{
		_floatScanStepBack(scanState);
	}
	
	if (foundDigits)
	{
		//TODO: What should do about error states? Set errno?
		//TODO: The example also does shlim here
		return 0;
	}
	
	//Handle 0 to reduce edge cases below
	if (!digits[0]) { return sign * 0.0; }
	
	//Optimize small integers (w/no exponent)
	if (lrp == dc && dc < 10 && (bits > 30 || (digits[0] >> bits) == 0))
	{
		return sign * (long double)digits[0];
	}
	
	//Handle over/under-flow
	if (lrp > -emin/2)
	{
		//TODO: This is an ERANGE errno scenario?
		return sign * LDBL_MAX * LDBL_MAX;
	}
	if (lrp < emin - (2*LDBL_MANT_DIG))
	{
		//TODO: This is an ERANGE errno scenario?
		return sign * LDBL_MIN * LDBL_MIN;
	}
	
	//Align incomplete final B1B digit
	if (j)
	{
		for (; j < 9; j++) { digits[digitIndex] *= 10; }
		digitIndex++;
		j = 0;
	}
	
	int a = 0; //TODO: Name this better!
	int z = digitIndex; //TODO: Name this better!
	int e2 = 0; //TODO: Name this better!
	int rp = lrp; //TODO: Name this better!
	
	//Optimize mid to small size integers (even in exponent notation) (I guess up to 100 million? because that's what we have in p10s above?)
	if (lnz < 9 && lnz <= rp && rp < 18)
	{
		if (rp == 9) { return sign * (long double)digits[0]; }
		if (rp < 9) { return sign * (long double)digits[0] / p10s[8 - rp]; }
		int bitlim = bits - (3 * (int)(rp - 9));
		if (bitlim > 30 || digits[0] >> bitlim == 0)
		{
			return sign * (long double)digits[0] * p10s[rp - 10];
		}
	}
	
	//Drop trailing zeros
	for (; digits[z-1] == 0; z--);
	
	//Align radix point to B1B digit boundary
	if (rp % 9)
	{
		int rpMod9 = (rp >= 0) ? (rp % 9) : ((rp % 9) + 9);
		int p10 = p10s[8 - rpMod9];
		uint32_t carry = 0;
		for (digitIndex = a; digitIndex != z; digitIndex++)
		{
			uint32_t temp = digits[digitIndex] % p10;
			digits[digitIndex] = (digits[digitIndex] / p10) + carry;
			carry = 1000000000 / p10 * temp;
			if (digitIndex == a && !digits[digitIndex])
			{
				a = (a + 1 & MASK); //TODO: Figure out order of operations here
				rp -= 9;
			}
		}
		if (carry) { digits[z++] = carry; }
		rp += (9 - rpMod9);
	}
	
	//Upscale until the desired number of bits are left of radix point
	while (rp < 9*LD_B1B_DIG || (rp == 9*LD_B1B_DIG && digits[a] < th[0]))
	{
		uint32_t carry = 0;
		e2 -= 29;
		for (digitIndex = (z-1 & MASK); ; digitIndex = (digitIndex-1 & MASK))
		{
			uint64_t temp = ((uint64_t)digits[digitIndex] << 29) + carry;
			if (temp > 1000000000)
			{
				carry = temp / 1000000000;
				digits[digitIndex] = temp % 1000000000;
			}
			else
			{
				carry = 0;
				digits[digitIndex] = temp;
			}
			if (digitIndex == (z-1 & MASK) && digitIndex != a && !digits[digitIndex]) { z = digitIndex; }
			if (digitIndex == a) { break; }
		}
		if (carry)
		{
			rp += 9;
			a = (a-1 & MASK);
			if (a == z)
			{
				z = (z-1 & MASK);
				digits[z-1 & MASK] |= digits[z];
			}
			digits[a] = carry;
		}
	}
	
	//Downscale until exactly number of bits are left of radix point (TODO: Grammar here??)
	int i; //TODO: Name this better!
	while (true)
	{
		uint32_t carry = 0;
		int sh = 1; //TODO: Name this better!
		for (i = 0; i < LD_B1B_DIG; i++)
		{
			digitIndex = (a + i & MASK);
			if (digitIndex == z || digits[digitIndex] < th[i])
			{
				i = LD_B1B_DIG;
				break;
			}
			if (digits[a + i & MASK] > th[i]) { break; }
		}
		if (i == LD_B1B_DIG && rp == 9 * LD_B1B_DIG) { break; }
		//TODO: find a way to compute optimal sh
		if (rp > 9 + (9 * LD_B1B_DIG)) { sh = 9; }
		e2 += sh;
		for (digitIndex = a; digitIndex != z; digitIndex = (digitIndex + 1 & MASK))
		{
			uint32_t temp = digits[digitIndex] & (1 << sh) - 1;
			digits[digitIndex] = (digits[digitIndex] >> sh) + carry;
			carry = (1000000000 >> sh) * temp;
			if (digitIndex == a && !digits[digitIndex])
			{
				a = (a + 1 & MASK);
				i--;
				rp -= 9;
			}
		}
		if (carry)
		{
			if ((z + 1 & MASK) != a)
			{
				digits[z] = carry;
				z = (z + 1 & MASK);
			}
			else
			{
				digits[z - 1 & MASK] |= 1;
			}
		}
	}
	
	//Assemble desired bits into floating point variable
	long double result;
	for (result = i = 0; i < LD_B1B_DIG; i++)
	{
		if ((a + i & MASK) == z) digits[(z = (z + 1 & MASK)) - 1] = 0;
		result = 1000000000.0L * result + digits[a + i & MASK];
	}
	result *= sign;
	
	//Limit precision for denormal results
	int denormal = 0;
	if (bits > LDBL_MANT_DIG + e2 - emin)
	{
		bits = LDBL_MANT_DIG + e2 - emin;
		if (bits < 0) { bits = 0; }
		denormal = 1;
	}
	
	//TODO: We need copysignl, scalbn, and fmodl
	
	//Calculate bias term to force rounding, move out lower bits
	long double frac = 0;
	long double bias = 0;
	if (bits < LDBL_MANT_DIG)
	{
		bias = copysignl(scalbn(1, (2 * LDBL_MANT_DIG) - bits - 1), result);
		frac = fmodl(result, scalbn(1, LDBL_MANT_DIG - bits));
		result -= frac;
		result += bias;
	}
	
	//Process tail of decimal input so it can affect rounding
	if ((a + i & MASK) != z)
	{
		uint32_t t = digits[a + i & MASK]; //TODO: Name this better!
		
		if (t < 500000000 && (t || (a + i + 1 & MASK) != z))
		{
			frac += 0.25 * sign;
		}
		else if (t > 500000000)
		{
			frac += 0.75 * sign;
		}
		else if (t == 500000000)
		{
			if ((a + i + 1 & MASK) == z)
			{
				frac += 0.5 * sign;
			}
			else
			{
				frac += 0.75 * sign;
			}
		}
		
		if (LDBL_MANT_DIG - bits >= 2 && !fmodl(frac, 1))
		{
			frac++;
		}
	}
	
	result += frac;
	result -= bias;
	
	if ((e2 + LDBL_MANT_DIG & INT_MAX) > emax-5)
	{
		if (fabsl(result) >= 2 / LDBL_EPSILON)
		{
			if (denormal && bits == LDBL_MANT_DIG + e2 - emin)
			{
				denormal = 0;
			}
			result *= 0.5;
			e2++;
		}
		if (e2 + LDBL_MANT_DIG > emax || (denormal && frac))
		{
			// errno = ERANGE; //TODO: Should we check and treat this scenario as an error?
		}
	}
	
	return scalbnl(result, e2);
}

typedef enum
{
	FloatSize_32 = 0,
	FloatSize_64,
	FloatSize_128,
} FloatSize_t;

long double __floatscan(FloatScanState_t* scanState, FloatSize_t floatSize)
{
	int bits; //number of bits in the mantissa?
	int emin; //minimum exponent?
	switch (floatSize)
	{
		case FloatSize_32:  bits = FLT_MANT_DIG;  emin = (FLT_MIN_EXP  - bits); break;
		case FloatSize_64:  bits = DBL_MANT_DIG;  emin = (DBL_MIN_EXP  - bits); break;
		case FloatSize_128: bits = LDBL_MANT_DIG; emin = (LDBL_MIN_EXP - bits); break;
	}
	
	//Trim leading whitespace
	int nextChar = _floatScanReadChar(scanState);
	while (isspace(nextChar)) { nextChar = _floatScanReadChar(scanState); }
	
	//Consume sign character
	int sign = 1;
	if (nextChar == '+' || nextChar == '-')
	{
		sign -= 2 * (nextChar == '-');
		nextChar = _floatScanReadChar(scanState);
	}
	
	//Check for "infinity"
	const char* infinityString = "infinity";
	size_t numInfinityCharsMatched = 0;
	for (size_t cIndex = 0; cIndex < 8; cIndex++)
	{
		if (_floatScanGetChar(scanState, cIndex) == infinityString[cIndex])
		{
			numInfinityCharsMatched++;
		}
		else { break; }
	}
	if (numInfinityCharsMatched == 3 || numInfinityCharsMatched == 8)
	{
		return sign * INFINITY;
	}
	
	//Check for "nan"
	const char* nanString = "nan";
	size_t numNanCharsFound = 0;
	for (size_t cIndex = 0; cIndex < 3; cIndex++)
	{
		if (_floatScanGetChar(scanState, cIndex) == nanString[cIndex])
		{
			numNanCharsFound++;
		}
		else { break; }
	}
	if (numNanCharsFound == 3)
	{
		//TODO: Apparently NAN can be followed by parenthesised alphanumeric (and '_') characters?
		//      If we actually encounter this we should probably start consuming it so the endPntrOut in strtox is correct
		return NAN;
	}
	
	if (_floatScanGetChar(scanState, 0) == '0' && _floatScanGetChar(scanState, 'x'))
	{
		scanState->readIndex += 2;
		return hexfloat(scanState, bits, emin, sign);
	}
	else
	{
		return decfloat(scanState, bits, emin, sign);
	}
}

long double strtox(const char* nullTermStr, char** endPntrOut, FloatSize_t floatSize)
{
	// FILE file;
	// sh_fromstring(&file, nullTermStr);
	// shlim(&file, 0);
	FloatScanState_t scanState;
	scanState.strPntr = nullTermStr;
	scanState.strLength = strlen(nullTermStr);
	scanState.readIndex = 0;
	long double y = __floatscan(&scanState, floatSize);
	// off_t cnt = shcnt(&file);
	if (endPntrOut)
	{
		*endPntrOut = (scanState.readIndex ? (char*)nullTermStr + scanState.readIndex : (char*)nullTermStr);
	}
	return y;
}

double strtod(const char* __restrict nullTermStr, char** __restrict endPtrOut)
{
	return strtox(nullTermStr, endPtrOut, FloatSize_64); //1 = double precision
}

double atof(const char* nullTermStr)
{
	return strtod(nullTermStr, nullptr);
}

// +--------------------------------------------------------------+
// |                   Random Number Generation                   |
// +--------------------------------------------------------------+
static uint64_t __stdRandSeed;

void srand(unsigned seed)
{
	__stdRandSeed = (seed - 1);
}

int rand()
{
	__stdRandSeed = (6364136223846793005ULL * __stdRandSeed) + 1;
	return (__stdRandSeed >> 33);
}

// +--------------------------------------------------------------+
// |    Potentially Important Stuff That We Currently Dont Use    |
// +--------------------------------------------------------------+
#if 0
float strtof (const char *__restrict, char **__restrict);
long double strtold (const char *__restrict, char **__restrict);

long strtol (const char *__restrict, char **__restrict, int);
unsigned long strtoul (const char *__restrict, char **__restrict, int);
long long strtoll (const char *__restrict, char **__restrict, int);
unsigned long long strtoull (const char *__restrict, char **__restrict, int);

void *malloc (size_t);
void *calloc (size_t, size_t);
void *realloc (void *, size_t);
void free (void *);
void *aligned_alloc(size_t, size_t);

_Noreturn void abort (void);
int atexit (void (*) (void));
_Noreturn void exit (int);
_Noreturn void _Exit (int);
int at_quick_exit (void (*) (void));
_Noreturn void quick_exit (int);

char *getenv (const char *);

int system (const char *);

void *bsearch (const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
void qsort (void *, size_t, size_t, int (*)(const void *, const void *));

int abs (int);
long labs (long);
long long llabs (long long);

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;

div_t div (int, int);
ldiv_t ldiv (long, long);
lldiv_t lldiv (long long, long long);

int mblen (const char *, size_t);
int mbtowc (wchar_t *__restrict, const char *__restrict, size_t);
int wctomb (char *, wchar_t);
size_t mbstowcs (wchar_t *__restrict, const char *__restrict, size_t);
size_t wcstombs (char *__restrict, const wchar_t *__restrict, size_t);

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

size_t __ctype_get_mb_cur_max(void);
#define MB_CUR_MAX (__ctype_get_mb_cur_max())


#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) \
 || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) \
 || defined(_BSD_SOURCE)

#define WNOHANG    1
#define WUNTRACED  2

#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (!WTERMSIG(s))
#define WIFSTOPPED(s) ((short)((((s)&0xffff)*0x10001)>>8) > 0x7f00)
#define WIFSIGNALED(s) (((s)&0xffff)-1U < 0xffu)

int posix_memalign (void **, size_t, size_t);
int setenv (const char *, const char *, int);
int unsetenv (const char *);
int mkstemp (char *);
int mkostemp (char *, int);
char *mkdtemp (char *);
int getsubopt (char **, char *const *, char **);
int rand_r (unsigned *);

#endif


#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) \
 || defined(_BSD_SOURCE)
char *realpath (const char *__restrict, char *__restrict);
long int random (void);
void srandom (unsigned int);
char *initstate (unsigned int, char *, size_t);
char *setstate (char *);
int putenv (char *);
int posix_openpt (int);
int grantpt (int);
int unlockpt (int);
char *ptsname (int);
char *l64a (long);
long a64l (const char *);
void setkey (const char *);
double drand48 (void);
double erand48 (unsigned short [3]);
long int lrand48 (void);
long int nrand48 (unsigned short [3]);
long mrand48 (void);
long jrand48 (unsigned short [3]);
void srand48 (long);
unsigned short *seed48 (unsigned short [3]);
void lcong48 (unsigned short [7]);
#endif

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#include <alloca.h>
char *mktemp (char *);
int mkstemps (char *, int);
int mkostemps (char *, int, int);
void *valloc (size_t);
void *memalign(size_t, size_t);
int getloadavg(double *, int);
int clearenv(void);
#define WCOREDUMP(s) ((s) & 0x80)
#define WIFCONTINUED(s) ((s) == 0xffff)
void *reallocarray (void *, size_t, size_t);
void qsort_r (void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);
#endif

#ifdef _GNU_SOURCE
int ptsname_r(int, char *, size_t);
char *ecvt(double, int, int *, int *);
char *fcvt(double, int, int *, int *);
char *gcvt(double, int, char *);
char *secure_getenv(const char *);
struct __locale_struct;
float strtof_l(const char *__restrict, char **__restrict, struct __locale_struct *);
double strtod_l(const char *__restrict, char **__restrict, struct __locale_struct *);
long double strtold_l(const char *__restrict, char **__restrict, struct __locale_struct *);
#endif

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define mkstemp64 mkstemp
#define mkostemp64 mkostemp
#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define mkstemps64 mkstemps
#define mkostemps64 mkostemps
#endif
#endif

#endif

#endif //  _STDLIB_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
RAND_MAX
FloatSize_32
FloatSize_64
FloatSize_128
@Types
FloatSize_t
@Functions
int atoi(const char* nullTermStr)
long atol(const char* nullTermStr)
long long atoll(const char* nullTermStr)
double strtod(const char* __restrict nullTermStr, char** __restrict endPtrOut)
double atof(const char* nullTermStr)
void srand(unsigned seed)
int rand()
*/
