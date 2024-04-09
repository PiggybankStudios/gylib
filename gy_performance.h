/*
File:   gy_performance.h
Author: Taylor Robbins
Date:   04\09\2024
Description:
	** Holds functions PerfTime_t type and GetPerfTime and GetPerfTimeDiff functions
*/

#ifndef _GY_PERFORMANCE_H
#define _GY_PERFORMANCE_H

struct PerfTime_t
{
	#if WINDOWS_COMPILATION
	LARGE_INTEGER perfCount;
	u64 cycleCount;
	#else
	//TODO: Implement linux/OSX/wasm variants of PerfTime!
	#error Unsupported platform in gy_performance.h PerfTime_t
	#endif
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	PerfTime_t GetPerfTime();
	r64 GetPerfTimeDiff(const PerfTime_t* tStart, const PerfTime_t* tEnd);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
PerfTime_t GetPerfTime()
{
	PerfTime_t result = {};
	BOOL queryResult = QueryPerformanceCounter(&result.perfCount);
	Assert_(queryResult != 0);
	result.cycleCount = __rdtsc();
	return result;
}

r64 GetPerfTimeDiff(const PerfTime_t* tStart, const PerfTime_t* tEnd)
{
	NotNull_(tStart);
	NotNull_(tEnd);
	LARGE_INTEGER perfCountFrequencyLarge;
	QueryPerformanceFrequency(&perfCountFrequencyLarge);
	r64 resultSecs = ((r64)(tEnd->perfCount.QuadPart - tStart->perfCount.QuadPart) / (r64)perfCountFrequencyLarge.QuadPart);
	return resultSecs * 1000.0;
}

#endif // GYLIB_HEADER_ONLY

#endif //  _GY_PERFORMANCE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Types
PerfTime_t
@Functions
PerfTime_t GetPerfTime()
r64 GetPerfTimeDiff(const PerfTime_t* tStart, const PerfTime_t* tEnd)
*/

