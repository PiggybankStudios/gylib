/*
File:   gy_test_memory.h
Author: Taylor Robbins
Date:   04\15\2024
*/

// +--------------------------------------------------------------+
// |                  Predeclare for gy_memory.h                  |
// +--------------------------------------------------------------+
#ifdef GYLIB_PREDECLARE_ONLY
#undef GYLIB_PREDECLARE_ONLY
#ifndef _GY_TEST_MEMORY_PREDECLARED
#define _GY_TEST_MEMORY_PREDECLARED

struct MemArenaTestSet_t;
enum AllocAlignment_t;
uint64_t MemArenaTestSetGetActionIndex(const MemArenaTestSet_t* set);
void MemArenaTestSetRecordAllocMemAction(MemArenaTestSet_t* set, uint64_t size, AllocAlignment_t alignment, void* resultPntr);
void MemArenaTestSetRecordReallocMemAction(MemArenaTestSet_t* set, void* allocPntr, uint64_t newSize, uint64_t oldSize, AllocAlignment_t alignment, void* resultPntr);
void MemArenaTestSetRecordFreeMemAction(MemArenaTestSet_t* set, void* allocPntr, uint64_t size);

#endif // _GY_TEST_MEMORY_PREDECLARED
#else // GYLIB_PREDECLARE_ONLY

#ifndef _GY_TEST_MEMORY_H
#define _GY_TEST_MEMORY_H

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
enum MemArenaTestActionType_t
{
	MemArenaTestActionType_None = 0,
	MemArenaTestActionType_Free,
	MemArenaTestActionType_Alloc,
	MemArenaTestActionType_Realloc,
	MemArenaTestActionType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetMemArenaTestActionTypeStr(MemArenaTestActionType_t enumValue);
#else
const char* GetMemArenaTestActionTypeStr(MemArenaTestActionType_t enumValue)
{
	switch (enumValue)
	{
		case MemArenaTestActionType_None:    return "None";
		case MemArenaTestActionType_Free:    return "Free";
		case MemArenaTestActionType_Alloc:   return "Alloc";
		case MemArenaTestActionType_Realloc: return "Realloc";
		default: return "Unknown";
	}
}
#endif // GYLIB_HEADER_ONLY

struct MemArenaTestAction_t
{
	MemArenaTestActionType_t type;
	
	u64 allocIndex;
	u64 size;
	AllocAlignment_t alignment;
	
	u64 oldAllocIndex;
	u64 oldSize;
};
struct MemArenaTestAllocation_t
{
	u64 originalSize;
	void* pntr;
};

struct MemArenaTestSet_t
{
	MemArena_t* allocArena;
	u64 numTotalAllocations;
	VarArray_t allocations; //MemArenaTestAllocation_t
	VarArray_t actions; //MemArenaTestAction_t
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeMemArenaTestSet(MemArenaTestSet_t* set);
	void NewMemArenaTestSet(MemArena_t* memArena, MemArenaTestSet_t* set, u64 numAllocationsExpected = 0, u64 numActionsExpected = 0);
	u64 FindMemArenaTestSetAllocIndex(MemArenaTestSet_t* set, void* allocPntr);
	u64 MemArenaTestSetGetActionIndex(const MemArenaTestSet_t* set);
	void MemArenaTestSetRecordAllocMemAction(MemArenaTestSet_t* set, u64 size, AllocAlignment_t alignment, void* resultPntr);
	void MemArenaTestSetRecordReallocMemAction(MemArenaTestSet_t* set, void* allocPntr, u64 newSize, u64 oldSize, AllocAlignment_t alignment, void* resultPntr);
	void MemArenaTestSetRecordFreeMemAction(MemArenaTestSet_t* set, void* allocPntr, u64 size);
	void MemArenaTestSetPrepare(MemArenaTestSet_t* set);
	void MemArenaTestPerformAction(MemArena_t* memArena, MemArenaTestAllocation_t* allocations, u64 numAllocations, MemArenaTestAction_t* action);
	void MemArenaTestSetPerformAllActions(MemArena_t* memArena, MemArenaTestSet_t* set);
#else

// +--------------------------------------------------------------+
// |                         New and Free                         |
// +--------------------------------------------------------------+
void FreeMemArenaTestSet(MemArenaTestSet_t* set)
{
	NotNull(set);
	FreeVarArray(&set->allocations);
	FreeVarArray(&set->actions);
	ClearPointer(set);
}
void NewMemArenaTestSet(MemArena_t* memArena, MemArenaTestSet_t* set, u64 numAllocationsExpected = 0, u64 numActionsExpected = 0)
{
	NotNull2(memArena, set);
	ClearPointer(set);
	set->allocArena = memArena;
	CreateVarArray(&set->allocations, memArena, sizeof(MemArenaTestAllocation_t), numAllocationsExpected);
	CreateVarArray(&set->actions, memArena, sizeof(MemArenaTestAction_t), numActionsExpected);
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
u64 FindMemArenaTestSetAllocIndex(MemArenaTestSet_t* set, void* allocPntr)
{
	if (allocPntr == nullptr) { return UINT64_MAX; }
	VarArrayLoop(&set->allocations, aIndex)
	{
		VarArrayLoopGet(MemArenaTestAllocation_t, allocation, &set->allocations, aIndex);
		if (allocation->pntr == allocPntr) { return aIndex; }
	}
	return UINT64_MAX;
}

u64 MemArenaTestSetGetActionIndex(const MemArenaTestSet_t* set)
{
	return set->actions.length;
}

// +--------------------------------------------------------------+
// |                        Record Actions                        |
// +--------------------------------------------------------------+
void MemArenaTestSetRecordAllocMemAction(MemArenaTestSet_t* set, u64 size, AllocAlignment_t alignment, void* resultPntr)
{
	NotNull2(set, set->allocArena);
	MemArenaTestAction_t* newAction = VarArrayAdd(&set->actions, MemArenaTestAction_t);
	NotNull(newAction);
	ClearPointer(newAction);
	newAction->type = MemArenaTestActionType_Alloc;
	newAction->size = size;
	newAction->alignment = alignment;
	newAction->allocIndex = set->allocations.length;
	MemArenaTestAllocation_t* newAllocation = VarArrayAdd(&set->allocations, MemArenaTestAllocation_t);
	NotNull(newAllocation);
	ClearPointer(newAllocation);
	newAllocation->originalSize = size;
	newAllocation->pntr = resultPntr;
}
void MemArenaTestSetRecordReallocMemAction(MemArenaTestSet_t* set, void* allocPntr, u64 newSize, u64 oldSize, AllocAlignment_t alignment, void* resultPntr)
{
	NotNull2(set, set->allocArena);
	MemArenaTestAction_t* newAction = VarArrayAdd(&set->actions, MemArenaTestAction_t);
	NotNull(newAction);
	ClearPointer(newAction);
	newAction->type = MemArenaTestActionType_Realloc;
	newAction->size = newSize;
	newAction->oldSize = oldSize;
	newAction->alignment = alignment;
	newAction->oldAllocIndex = FindMemArenaTestSetAllocIndex(set, allocPntr);
	if (resultPntr != allocPntr)
	{
		if (newAction->oldAllocIndex < set->allocations.length)
		{
			MemArenaTestAllocation_t* oldAllocation = VarArrayGet(&set->allocations, newAction->oldAllocIndex, MemArenaTestAllocation_t);
			oldAllocation->pntr = nullptr;
		}
		newAction->allocIndex = set->allocations.length;
		MemArenaTestAllocation_t* newAllocation = VarArrayAdd(&set->allocations, MemArenaTestAllocation_t);
		NotNull(newAllocation);
		ClearPointer(newAllocation);
		newAllocation->originalSize = newSize;
		newAllocation->pntr = resultPntr;
	}
	else { newAction->allocIndex = newAction->oldAllocIndex; }
}
void MemArenaTestSetRecordFreeMemAction(MemArenaTestSet_t* set, void* allocPntr, u64 size)
{
	NotNull2(set, set->allocArena);
	MemArenaTestAction_t* newAction = VarArrayAdd(&set->actions, MemArenaTestAction_t);
	NotNull(newAction);
	ClearPointer(newAction);
	newAction->type = MemArenaTestActionType_Free;
	newAction->size = size;
	newAction->allocIndex = FindMemArenaTestSetAllocIndex(set, allocPntr);
	Assert(newAction->allocIndex < set->allocations.length);
	MemArenaTestAllocation_t* allocation = VarArrayGet(&set->allocations, newAction->allocIndex, MemArenaTestAllocation_t);
	allocation->pntr = nullptr;
}

// +--------------------------------------------------------------+
// |                       Perform Actions                        |
// +--------------------------------------------------------------+
void MemArenaTestSetPrepare(MemArenaTestSet_t* set)
{
	NotNull(set);
	set->numTotalAllocations = set->allocations.length;
	VarArrayClear(&set->allocations);
}
void MemArenaTestPerformAction(MemArena_t* memArena, MemArenaTestAllocation_t* allocations, u64 numAllocations, MemArenaTestAction_t* action)
{
	switch (action->type)
	{
		case MemArenaTestActionType_Alloc:
		{
			void* newPntr = AllocMem(memArena, action->size, action->alignment);
			NotNull(newPntr);
			Assert(action->allocIndex < numAllocations);
			allocations[action->allocIndex].pntr = newPntr;
			allocations[action->allocIndex].originalSize = action->size;
		} break;
		case MemArenaTestActionType_Realloc:
		{
			void* newPntr = nullptr;
			if (action->oldAllocIndex < numAllocations)
			{
				newPntr = ReallocMem(memArena, allocations[action->oldAllocIndex].pntr, action->size, action->oldSize, action->alignment);
			}
			else
			{
				Assert(action->oldSize == 0);
				newPntr = ReallocMem(memArena, nullptr, action->size, action->oldSize, action->alignment, true);
			}
			Assert(action->allocIndex < numAllocations);
			if (action->allocIndex != action->oldAllocIndex)
			{
				allocations[action->allocIndex].pntr = newPntr;
				allocations[action->allocIndex].originalSize = action->size;
			}
			else { Assert(newPntr == allocations[action->allocIndex].pntr); }
			
		} break;
		case MemArenaTestActionType_Free:
		{
			Assert(action->allocIndex < numAllocations);
			FreeMem(memArena, allocations[action->allocIndex].pntr, action->size);
		} break;
	}
}
void MemArenaTestSetPerformAllActions(MemArena_t* memArena, MemArenaTestSet_t* set)
{
	NotNull2(memArena, set);
	AssertMsg(set->allocations.length == 0, "You must call MemArenaTestSetPrepare before MemArenaTestSetPerformAllActions");
	MemArenaTestAllocation_t* allocations = VarArrayAddRange(&set->allocations, 0, set->numTotalAllocations, MemArenaTestAllocation_t);
	MyMemSet(allocations, 0x00, sizeof(MemArenaTestAllocation_t) * set->numTotalAllocations);
	VarArrayLoop(&set->actions, aIndex)
	{
		VarArrayLoopGet(MemArenaTestAction_t, action, &set->actions, aIndex);
		MemArenaTestPerformAction(memArena, allocations, set->numTotalAllocations, action);
	}
}

#endif // GYLIB_HEADER_ONLY

#endif //  _GY_TEST_MEMORY_H

#endif // GYLIB_PREDECLARE_ONLY

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
MemArenaTestActionType_None
MemArenaTestActionType_Free
MemArenaTestActionType_Alloc
MemArenaTestActionType_Realloc
MemArenaTestActionType_NumTypes
@Types
MemArenaTestActionType_t
MemArenaTestAction_t
MemArenaTestAllocation_t
MemArenaTestSet_t
@Functions
const char* GetMemArenaTestActionTypeStr(MemArenaTestActionType_t enumValue)
void FreeMemArenaTestSet(MemArenaTestSet_t* set)
void NewMemArenaTestSet(MemArena_t* memArena, MemArenaTestSet_t* set, u64 numAllocationsExpected = 0, u64 numActionsExpected = 0)
u64 FindMemArenaTestSetAllocIndex(MemArenaTestSet_t* set, void* allocPntr)
u64 MemArenaTestSetGetActionIndex(const MemArenaTestSet_t* set)
void MemArenaTestSetRecordAllocMemAction(MemArenaTestSet_t* set, u64 size, AllocAlignment_t alignment, void* resultPntr)
void MemArenaTestSetRecordReallocMemAction(MemArenaTestSet_t* set, void* allocPntr, u64 newSize, u64 oldSize, AllocAlignment_t alignment, void* resultPntr)
void MemArenaTestSetRecordFreeMemAction(MemArenaTestSet_t* set, void* allocPntr, u64 size)
void MemArenaTestSetPrepare(MemArenaTestSet_t* set)
void MemArenaTestPerformAction(MemArena_t* memArena, MemArenaTestAllocation_t* allocations, u64 numAllocations, MemArenaTestAction_t* action)
void MemArenaTestSetPerformAllActions(MemArena_t* memArena, MemArenaTestSet_t* set)
*/
