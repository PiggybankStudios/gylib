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
	#elif ORCA_COMPILATION
	u64 value;
	#else
	//TODO: Implement linux/OSX/wasm variants of PerfTime!
	#error Unsupported platform in gy_performance.h PerfTime_t
	#endif
};

struct PerfTimeTotal_t
{
	u64 totalCounterAmount;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	PerfTime_t GetPerfTime();
	r64 GetPerfTimeDiff(const PerfTime_t* tStart, const PerfTime_t* tEnd);
	void AddToPerfTimeTotal(const PerfTime_t* tStart, const PerfTime_t* tEnd, PerfTimeTotal_t* totalOut);
	r64 GetPerfTimeTotal(const PerfTimeTotal_t* total);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
PerfTime_t GetPerfTime()
{
	PerfTime_t result = {};
	#if WINDOWS_COMPILATION
	BOOL queryResult = QueryPerformanceCounter(&result.perfCount);
	Assert_(queryResult != 0);
	result.cycleCount = __rdtsc();
	#elif ORCA_COMPILATION
	Unimplemented(); //TODO: Implement me!
	#else
	//TODO: Implement linux/OSX/wasm variants of PerfTime!
	#error Unsupported platform in gy_performance.h GetPerfTime
	#endif
	return result;
}

r64 GetPerfTimeDiff(const PerfTime_t* tStart, const PerfTime_t* tEnd)
{
	NotNull_(tStart);
	NotNull_(tEnd);
	r64 resultSecs;
	#if WINDOWS_COMPILATION
	LARGE_INTEGER perfCountFrequencyLarge;
	QueryPerformanceFrequency(&perfCountFrequencyLarge);
	resultSecs = ((r64)(tEnd->perfCount.QuadPart - tStart->perfCount.QuadPart) / (r64)perfCountFrequencyLarge.QuadPart);
	#elif ORCA_COMPILATION
	Unimplemented(); //TODO: Implement me!
	#else
	//TODO: Implement linux/OSX/wasm variants of PerfTime!
	#error Unsupported platform in gy_performance.h GetPerfTimeDiff
	#endif
	return resultSecs * 1000.0;
}

void AddToPerfTimeTotal(const PerfTime_t* tStart, const PerfTime_t* tEnd, PerfTimeTotal_t* totalOut)
{
	NotNull_(tStart);
	NotNull_(tEnd);
	NotNull_(totalOut);
	#if WINDOWS_COMPILATION
	totalOut->totalCounterAmount += (u64)(tEnd->perfCount.QuadPart - tStart->perfCount.QuadPart);
	#elif ORCA_COMPILATION
	Unimplemented(); //TODO: Implement me!
	#else
	//TODO: Implement linux/OSX/wasm variants of PerfTime!
	#error Unsupported platform in gy_performance.h AddToPerfTimeTotal
	#endif
}

r64 GetPerfTimeTotal(const PerfTimeTotal_t* total)
{
	NotNull_(total);
	r64 resultSecs;
	#if WINDOWS_COMPILATION
	LARGE_INTEGER perfCountFrequencyLarge;
	QueryPerformanceFrequency(&perfCountFrequencyLarge);
	resultSecs = ((r64)total->totalCounterAmount / (r64)perfCountFrequencyLarge.QuadPart);
	#elif ORCA_COMPILATION
	Unimplemented(); //TODO: Implement me!
	#else
	//TODO: Implement linux/OSX/wasm variants of PerfTime!
	#error Unsupported platform in gy_performance.h GetPerfTimeTotal
	#endif
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
PerfTimeTotal_t
@Functions
PerfTime_t GetPerfTime()
r64 GetPerfTimeDiff(const PerfTime_t* tStart, const PerfTime_t* tEnd)
void AddToPerfTimeTotal(const PerfTime_t* tStart, const PerfTime_t* tEnd, PerfTimeTotal_t* totalOut)
r64 GetPerfTimeTotal(const PerfTimeTotal_t* total)
*/

