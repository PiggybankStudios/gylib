/*
File:   gy_sorting.h
Author: Taylor Robbins
Date:   01\08\2022
Description:
	** This file holds all of the sorting algorithms that we have implemented.
	** Some algorithms are implemented in different ways to provide different ways of using them
*/

//TODO: These functions have not been tested since they were imported in from mylib

#ifndef _GY_SORTING_H
#define _GY_SORTING_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                  Structure/Type Definitions                  |
// +--------------------------------------------------------------+
//-1: <    0: ==   1: >
#define COMPARE_FUNC_DEFINITION(functionName) i32 functionName(const void* left, const void* right, void* contextPntr)
typedef COMPARE_FUNC_DEFINITION(CompareFunc_f);

// +--------------------------------------------------------------+
// |                          Quick Sort                          |
// +--------------------------------------------------------------+
i64 QuickSortPartition(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
{
	NotNull(arrayPntr);
	Assert(elementSize > 0);
	NotNull(workingSpace);
	NotNull(compareFunc);
	u8* bytePntr = (u8*)arrayPntr;
	
	u8* tempSpace = ((u8*)workingSpace) + 0;
	u8* pivotCopy = ((u8*)workingSpace) + elementSize;
	u8* pivotPntr = bytePntr + elementSize*(numElements-1);
	MyMemCopy(pivotCopy, pivotPntr, elementSize);
	
	i64 smallIndex = -1;
	u8* smallPntr = bytePntr - elementSize;
	for (u64 eIndex = 0; eIndex < numElements-1; eIndex++)
	{
		u8* elementPntr = bytePntr + elementSize*(eIndex);
		i32 compareResult = compareFunc((const void*)elementPntr, (const void*)pivotCopy, contextPntr);
		if (compareResult < 0)
		{
			smallIndex++;
			smallPntr += elementSize;
			
			//swap smallPntr and elementPntr
			MyMemCopy(tempSpace, smallPntr, elementSize);
			MyMemCopy(smallPntr, elementPntr, elementSize);
			MyMemCopy(elementPntr, tempSpace, elementSize);
		}
	}
	
	smallIndex++;
	smallPntr += elementSize;
	Assert((u64)smallIndex < numElements);
	MyMemCopy(pivotPntr, smallPntr, elementSize);
	MyMemCopy(smallPntr, pivotCopy, elementSize);
	
	return smallIndex;
}

//NOTE: workingSpace must be a space large enough to hold two elements. This space is used to perform swaps and to hold the pivot element
void QuickSort(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr)
{
	NotNull(arrayPntr);
	Assert(elementSize > 0);
	NotNull(workingSpace);
	NotNull(compareFunc);
	if (numElements <= 1) { return; } //nothing to sort
	u8* bytePntr = (u8*)arrayPntr;
	
	#if DEBUG
	//make sure the workingSpace is large enough by writing it to zero to make sure we don't have any memory access exceptions happen
	MyMemSet(workingSpace, 0x00, elementSize*2);
	#endif
	
	//parition the array
	i64 pi = QuickSortPartition(arrayPntr, numElements, elementSize, workingSpace, compareFunc, contextPntr);
	
	//sort lower partition
	QuickSort(arrayPntr, (u64)pi, elementSize, workingSpace, compareFunc, contextPntr);
	
	//sort upper partition
	QuickSort((void*)(bytePntr + elementSize*(pi+1)), (u64)(numElements - (pi+1)), elementSize, workingSpace, compareFunc, contextPntr);
}

//Allocates 2x elementSize space in workingAllocArena. This space is used to perform swaps and to hold the pivot element
void QuickSortAlloc(void* arrayPntr, u64 numElements, u64 elementSize, MemArena_t* workingAllocArena, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
{
	Assert(elementSize > 0);
	Assert(workingAllocArena != nullptr);
	u8* workingSpace = AllocArray(workingAllocArena, u8, elementSize*2);
	Assert(workingSpace != nullptr);
	QuickSort(arrayPntr, numElements, elementSize, workingSpace, compareFunc, contextPntr);
	FreeMem(workingAllocArena, workingSpace, elementSize*2);
}

#endif //  _GY_SORTING_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
CompareFunc_f
@Functions
#define COMPARE_FUNC_DEFINITION(functionName)
i64 QuickSortPartition(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
void QuickSort(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr)
void QuickSortAlloc(void* arrayPntr, u64 numElements, u64 elementSize, MemArena_t* workingAllocArena, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
*/
