/*
File:   stdint.h
Author: Taylor Robbins
Date:   03\27\2022
*/

#ifndef _STDINT_H
#define _STDINT_H

// +--------------------------------------------------------------+
// |                         Base Defines                         |
// +--------------------------------------------------------------+
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321

#if GY_WASM_STD_LIB
	#define _Addr long
	#define _Int64 long long
	#define _Reg long
	#define __BYTE_ORDER __LITTLE_ENDIAN
	#define __LONG_MAX 0x7FFFFFFFFFFFFFFFL
	#ifndef __cplusplus
	#ifdef __WCHAR_TYPE__
	typedef __WCHAR_TYPE__ wchar_t;
	#else
	typedef long wchar_t;
	#endif
	#endif
#else
	#define _Addr int
	#define _Int64 long long
	#define _Reg long long
	#define __BYTE_ORDER __LITTLE_ENDIAN
	#define __LONG_MAX 0x7FFFFFFFL
	#ifndef __cplusplus
	typedef int wchar_t;
	#endif
#endif

typedef struct { long long __ll; long double __ld; } max_align_t;

// +--------------------------------------------------------------+
// |                         Common Types                         |
// +--------------------------------------------------------------+
typedef float float_t;
typedef double double_t;

typedef unsigned _Addr size_t;
typedef unsigned _Addr uintptr_t;
typedef _Addr ptrdiff_t;
typedef _Addr ssize_t;
typedef _Addr intptr_t;
typedef _Addr regoff_t;
typedef _Reg register_t;
typedef _Int64 time_t;
typedef _Int64 suseconds_t;

//TODO: Double check that "int" results in a 32-bit integer on all platforms
typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
typedef signed _Int64   int64_t;
typedef signed _Int64   intmax_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned _Int64 uint64_t;
typedef unsigned _Int64 u_int64_t;
typedef unsigned _Int64 uintmax_t;

typedef unsigned mode_t;
typedef unsigned _Reg nlink_t;
typedef _Int64 off_t;
typedef unsigned _Int64 ino_t;
typedef unsigned _Int64 dev_t;
typedef long blksize_t;
typedef _Int64 blkcnt_t;
typedef unsigned _Int64 fsblkcnt_t;
typedef unsigned _Int64 fsfilcnt_t;

typedef unsigned wint_t;
typedef unsigned long wctype_t;

typedef void * timer_t;
typedef int clockid_t;
typedef long clock_t;
struct timeval { time_t tv_sec; suseconds_t tv_usec; };
struct timespec { time_t tv_sec; int :8*(sizeof(time_t)-sizeof(long))*(__BYTE_ORDER==__BIG_ENDIAN); long tv_nsec; int :8*(sizeof(time_t)-sizeof(long))*(__BYTE_ORDER!=__BIG_ENDIAN); };

typedef int pid_t;
typedef unsigned id_t;
typedef unsigned uid_t;
typedef unsigned gid_t;
typedef int key_t;
typedef unsigned useconds_t;

struct _IO_FILE
{
	unsigned flags;
	unsigned char *rpos, *rend;
	int (*close)(_IO_FILE *);
	unsigned char *wend, *wpos;
	unsigned char *mustbezero_1;
	unsigned char *wbase;
	size_t (*read)(_IO_FILE *, unsigned char *, size_t);
	size_t (*write)(_IO_FILE *, const unsigned char *, size_t);
	off_t (*seek)(_IO_FILE *, off_t, int);
	unsigned char *buf;
	size_t buf_size;
	_IO_FILE *prev, *next;
	int fd;
	int pipe_pid;
	long lockcount;
	int mode;
	volatile int lock;
	int lbf;
	void *cookie;
	off_t off;
	char *getln_buf;
	void *mustbezero_2;
	unsigned char *shend;
	off_t shlim, shcnt;
	_IO_FILE *prev_locked, *next_locked;
	struct __locale_struct *locale;
};
typedef struct _IO_FILE FILE;

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;

// +--------------------------------------------------------------+
// |                     Min and Max Defines                      |
// +--------------------------------------------------------------+
#define INT8_MIN   (-1-0x7f)
#define INT16_MIN  (-1-0x7fff)
#define INT32_MIN  (-1-0x7fffffff)
#define INT64_MIN  (-1-0x7fffffffffffffff)

#define INT8_MAX   (0x7f)
#define INT16_MAX  (0x7fff)
#define INT32_MAX  (0x7fffffff)
#define INT64_MAX  (0x7fffffffffffffff)

#define UINT8_MAX  (0xff)
#define UINT16_MAX (0xffff)
#define UINT32_MAX (0xffffffffu)
#define UINT64_MAX (0xffffffffffffffffu)

#if GY_WASM_STD_LIB
	#define INTPTR_MIN      INT32_MIN
	#define INTPTR_MAX      INT32_MAX
	#define UINTPTR_MAX     UINT32_MAX
	#define PTRDIFF_MIN     INT32_MIN
	#define PTRDIFF_MAX     INT32_MAX
	#define SIZE_MAX        UINT32_MAX
#else
	#define INTPTR_MIN      INT64_MIN
	#define INTPTR_MAX      INT64_MAX
	#define UINTPTR_MAX     UINT64_MAX
	#define PTRDIFF_MIN     INT64_MIN
	#define PTRDIFF_MAX     INT64_MAX
	#define SIZE_MAX        UINT64_MAX
#endif

// +--------------------------------------------------------------+
// |                    Numeric Suffix Macros                     |
// +--------------------------------------------------------------+
#define INT8_C(c)  c
#define INT16_C(c) c
#define INT32_C(c) c

#define UINT8_C(c)  c
#define UINT16_C(c) c
#define UINT32_C(c) c ## U

#if UINTPTR_MAX == UINT64_MAX
	#define INT64_C(c) c ## L
	#define UINT64_C(c) c ## UL
	#define INTMAX_C(c)  c ## L
	#define UINTMAX_C(c) c ## UL
#else
	#define INT64_C(c) c ## LL
	#define UINT64_C(c) c ## ULL
	#define INTMAX_C(c)  c ## LL
	#define UINTMAX_C(c) c ## ULL
#endif

// +--------------------------------------------------------------+
// |                Other Helpful Macros (Custom)                 |
// +--------------------------------------------------------------+
#define PTR_SIZE  (sizeof(size_t))
#define PTR_ALIGN (sizeof(size_t)-1)
#define SIZET_ONES ((size_t)-1/UINT8_MAX)
#define SIZET_HIGHS (SIZET_ONES * (UINT8_MAX/2+1))
#define SIZET_HASZERO(x) ((x)-SIZET_ONES & ~(x) & SIZET_HIGHS)

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#define BITOP(a, b, op) ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

// +--------------------------------------------------------------+
// |    Potentially Important Stuff That We Currently Dont Use    |
// +--------------------------------------------------------------+
#if 0
#ifdef __cplusplus
typedef unsigned long pthread_t;
#else
typedef struct __pthread * pthread_t;
#endif
typedef int pthread_once_t;
typedef unsigned pthread_key_t;
typedef int pthread_spinlock_t;
typedef struct { unsigned __attr; } pthread_mutexattr_t;
typedef struct { unsigned __attr; } pthread_condattr_t;
typedef struct { unsigned __attr; } pthread_barrierattr_t;
typedef struct { unsigned __attr[2]; } pthread_rwlockattr_t;

typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;

typedef struct __locale_struct* locale_t;

typedef struct __sigset_t { unsigned long __bits[128/sizeof(long)]; } sigset_t;

struct iovec { void* iov_base; size_t iov_len; };

struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };

typedef unsigned socklen_t;
typedef unsigned short sa_family_t;

typedef struct { union { int __i[sizeof(long)==8?14:9]; volatile int __vi[sizeof(long)==8?14:9]; unsigned long __s[sizeof(long)==8?7:9]; } __u; } pthread_attr_t;
typedef struct { union { int __i[sizeof(long)==8?10:6]; volatile int __vi[sizeof(long)==8?10:6]; volatile void *volatile __p[sizeof(long)==8?5:6]; } __u; } pthread_mutex_t;
typedef struct { union { int __i[sizeof(long)==8?10:6]; volatile int __vi[sizeof(long)==8?10:6]; volatile void *volatile __p[sizeof(long)==8?5:6]; } __u; } mtx_t;
typedef struct { union { int __i[12]; volatile int __vi[12]; void *__p[12*sizeof(int)/sizeof(void*)]; } __u; } pthread_cond_t;
typedef struct { union { int __i[12]; volatile int __vi[12]; void *__p[12*sizeof(int)/sizeof(void*)]; } __u; } cnd_t;
typedef struct { union { int __i[sizeof(long)==8?14:8]; volatile int __vi[sizeof(long)==8?14:8]; void *__p[sizeof(long)==8?7:8]; } __u; } pthread_rwlock_t;
typedef struct { union { int __i[sizeof(long)==8?8:5]; volatile int __vi[sizeof(long)==8?8:5]; void *__p[sizeof(long)==8?4:5]; } __u; } pthread_barrier_t;

typedef int8_t int_fast8_t;
typedef int64_t int_fast64_t;

typedef int8_t  int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_fast8_t;
typedef uint64_t uint_fast64_t;

typedef uint8_t  uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;

#define INT_FAST16_MIN  INT32_MIN
#define INT_FAST32_MIN  INT32_MIN

#define INT_FAST16_MAX  INT32_MAX
#define INT_FAST32_MAX  INT32_MAX

#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST32_MAX UINT32_MAX
#endif

#endif //  _STDINT_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
__LITTLE_ENDIAN
__BIG_ENDIAN
_Addr
_Int64
_Reg
__BYTE_ORDER
__LONG_MAX
INT8_MIN
INT16_MIN
INT32_MIN
INT64_MIN
INT8_MAX
INT16_MAX
INT32_MAX
INT64_MAX
UINT8_MAX
UINT16_MAX
UINT32_MAX
UINT64_MAX
INTPTR_MIN
INTPTR_MAX
UINTPTR_MAX
PTRDIFF_MIN
PTRDIFF_MAX
SIZE_MAX
PTR_SIZE
PTR_ALIGN
SIZET_ONES
SIZET_HIGHS
@Types
max_align_t
float_t
double_t
size_t
uintptr_t
ptrdiff_t
ssize_t
intptr_t
regoff_t
register_t
time_t
suseconds_t
int8_t
int16_t
int32_t
int64_t
intmax_t
uint8_t
uint16_t
uint32_t
uint64_t
u_int64_t
uintmax_t
mode_t
nlink_t
off_t
ino_t
dev_t
blksize_t
blkcnt_t
fsblkcnt_t
fsfilcnt_t
wint_t
wctype_t
timer_t
clockid_t
clock_t
timeval
timespec
pid_t
id_t
uid_t
gid_t
key_t
useconds_t
_IO_FILE
FILE
va_list
__isoc_va_list
@Functions
INT8_C(c)
INT16_C(c)
INT32_C(c)
UINT8_C(c)
UINT16_C(c)
UINT32_C(c)
INT64_C(c)
UINT64_C(c)
INTMAX_C(c)
UINTMAX_C(c)
#define SIZET_HASZERO(x)
#define MAX(a, b)
#define MIN(a, b)
#define BITOP(a, b, op)
#define offsetof(type, member)
*/
