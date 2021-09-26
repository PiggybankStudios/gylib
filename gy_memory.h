/*
File:   gy_memory.h
Author: Taylor Robbins
Date:   09\24\2021
Description:
	** This is where we hold all of the implementations for various kinds of memory arenas
	
	@MemArenaType_Redirect An arena that simply acts as a redirect to alloc and free function pointers.
	The internal allocFunc and freeFunc should be updated after DLL reloads where appropriate.
	The used value only increases since the freeFunc does not provide size information. (unless allocSize is always provided to FreeMem)
	Alignment is not supported/
	
	@MemArenaType_Alias An arena that serves as a redirect to another arena. The size and used information
	will be directly copied from the base arena whenever an action is performed (this means the data could get stale if no actions are happening through this alias)
	The numAllocations count is local to this alias, not a copy of the base arena.
	Alignment is only supported if the base arena supports alignment
	
	@MemArenaType_StdHeap An arena that simply redirects to standard library functions malloc, free, and realloc.
	The used value only increases since the free function does not provide size information. (unless allocSize is always provided to FreeMem)
	Alignment is not supported.
	
	@MemArenaType_FixedHeap An arena that manages a space of fixed size and provides
	general purpose allocation/deallocation functionality. (TODO: Unfinished)
	
	@MemArenaType_PagedHeap An arena that manages pages of memory and can allocate more pages from a base
	arena (or through allocFunc and freeFunc) when it runs out. On the outside this provides the same
	general purpose allocation/deallocation functionality as FixedHeap. (TODO: Unfinished)
	
	@MemArenaType_MarkedStack An arena that acts like a "stack" where direct deallocation is not allowed
	but "marks" can be pushed and then later popped (freeing any memory allocated since the push) (TODO: Unfinished)
	You can check for push/pop mismatches by checking the mark count when you expect it to be 0.
	The numAllocations count only increases since we don't keep track of how many allocations get freed in a pop
	
	@MemArenaType_Buffer An arena that provides a simple first in last out style allocation structure within
	a predefined space (usually a buffer from somewhere). When freeing memory you generally have to pass the
	size of the allocation you are freeing (except for simple single allocation scenarios).
	You can quickly create one of these buffer-backed arenas on the stack with CreateStackBufferArena(arenaName, bufferName, size).
	(TODO: Add support for quickly taking a preallocated pntr and size and turning into a Buffer arena)
*/

#ifndef _GY_MEMORY_H
#define _GY_MEMORY_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_debug.h"

enum MemArenaType_t
{
	MemArenaType_None = 0,
	MemArenaType_Redirect,
	MemArenaType_Alias,
	MemArenaType_StdHeap,
	MemArenaType_FixedHeap,
	MemArenaType_PagedHeap,
	MemArenaType_MarkedStack,
	MemArenaType_Buffer,
};
const char* GetMemArenaTypeStr(MemArenaType_t arenaType)
{
	switch (arenaType)
	{
		case MemArenaType_None:        return "None";
		case MemArenaType_Redirect:    return "Redirect";
		case MemArenaType_Alias:       return "Alias";
		case MemArenaType_StdHeap:     return "StdHeap";
		case MemArenaType_FixedHeap:   return "FixedHeap";
		case MemArenaType_PagedHeap:   return "PagedHeap";
		case MemArenaType_MarkedStack: return "MarkedStack";
		case MemArenaType_Buffer:      return "Buffer";
		default: return "Unknown";
	}
}

enum AllocAlignment_t
{
	AllocAlignment_None    = 0,
	AllocAlignment_4Bytes  = 4,
	AllocAlignment_8Bytes  = 8,
	AllocAlignment_64Bytes = 64,
};

typedef void* AllocationFunction_f(u64 numBytes);
typedef void FreeFunction_f(void* memPntr);

#define HEAP_ALLOC_FILLED_FLAG 0x8000000000000000ULL
#define HEAP_ALLOC_SIZE_MASK   0x7FFFFFFFFFFFFFFFULL
struct HeapAllocPrefix_t
{
	u64 size; //top-bit is used as "filled" flag, includes prefix size
};

struct HeapPageHeader_t
{
	HeapPageHeader_t* next;
	u64 size;
	u64 used;
};

struct MarkedStackArenaHeader_t
{
	u64 maxNumMarks;
	u64 numMarks;
	u64 highMarkCount;
};

struct MemArena_t
{
	MemArenaType_t type;
	AllocAlignment_t alignment;
	u64 pageSize;
	u64 maxSize;
	u64 maxNumPages;
	bool telemetryEnabled;
	bool singleAlloc;
	
	u64 size;
	u64 used;
	u64 numPages;
	u64 numAllocations;
	u64 highUsedMark;
	u64 highAllocMark;
	
	void* headerPntr;
	void* mainPntr;
	void* otherPntr;
	AllocationFunction_f* allocFunc;
	FreeFunction_f* freeFunc;
	MemArena_t* sourceArena;
};

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
#define PackAllocPrefixSize(used, size)   ((used) ? HEAP_ALLOC_FILLED_FLAG : 0ULL) | ((size) & HEAP_ALLOC_SIZE_MASK)
#define IsAllocPrefixFilled(packedSize)   IsFlagSet((packedSize), HEAP_ALLOC_FILLED_FLAG)
#define UnpackAllocPrefixSize(packedSize) ((packedSize) & HEAP_ALLOC_SIZE_MASK)

bool IsAlignedTo(const void* memoryPntr, AllocAlignment_t alignment)
{
	if (alignment == AllocAlignment_None) { return true; }
	u64 address = (u64)(memoryPntr);
	return ((address % (u64)alignment) == 0);
}
u8 OffsetToAlign(const void* memoryPntr, AllocAlignment_t alignment)
{
	if (alignment == AllocAlignment_None) { return 0; }
	u64 address = (u64)(memoryPntr);
	if ((address % (u64)alignment) == 0) { return 0; }
	else { return (u8)alignment - (u8)(address % (u64)alignment); }
}

bool IsPntrInsideRange(const void* testPntr, const void* rangeBase, u64 rangeSize)
{
	if (((const u8*)testPntr) < ((const u8*)rangeBase)) { return false; }
	if (((const u8*)testPntr) >= ((const u8*)rangeBase) + rangeSize) { return false; }
	return true;
}

// +--------------------------------------------------------------+
// |                        Init Functions                        |
// +--------------------------------------------------------------+
void InitMemArena_Redirect(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc)
{
	NotNull(arena);
	NotNull(allocFunc);
	ClearPointer(arena);
	arena->type = MemArenaType_Redirect;
	arena->singleAlloc = false;
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
	arena->used = 0; //NOTE: This only tracks allocations, not deallocations, so it only goes up
	arena->numAllocations = 0;
	
	arena->telemetryEnabled = true;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_Alias(MemArena_t* arena, MemArena_t* sourceArena)
{
	NotNull(arena);
	NotNull(sourceArena);
	ClearPointer(arena);
	arena->type = MemArenaType_Alias;
	arena->sourceArena = sourceArena;
	arena->singleAlloc = false;
	arena->used = 0; //NOTE: This MAY not track deallocations, it depends on if the sourceArena supports returning the size on FreeMem
	arena->numAllocations = 0;
	
	arena->telemetryEnabled = true;
	arena->highUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_StdHeap(MemArena_t* arena)
{
	NotNull(arena);
	ClearPointer(arena);
	arena->type = MemArenaType_StdHeap;
	arena->singleAlloc = false;
	arena->used = 0; //NOTE: This only tracks allocations, not deallocations, so it only goes up
	arena->numAllocations = 0;
	
	arena->telemetryEnabled = true;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_FixedHeap(MemArena_t* arena, u64 size, void* memoryPntr, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	u64 prefixSize = sizeof(HeapAllocPrefix_t);
	Assert(size > prefixSize);
	NotNull(memoryPntr);
	Assert(IsAlignedTo(memoryPntr, alignment));
	ClearPointer(arena);
	arena->type = MemArenaType_FixedHeap;
	arena->alignment = alignment;
	arena->singleAlloc = false;
	arena->mainPntr = memoryPntr;
	arena->size = size;
	arena->otherPntr = memoryPntr;
	HeapAllocPrefix_t* firstAlloc = (HeapAllocPrefix_t*)arena->otherPntr;
	ClearPointer(firstAlloc);
	firstAlloc->size = PackAllocPrefixSize(false, arena->size);
	arena->used = prefixSize;
	arena->numAllocations = 0;
	
	arena->telemetryEnabled = true;
	arena->highUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_PagedHeapFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, AllocationFunction_f* freeFunc, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None)
{
	UNREFERENCED(arena);
	UNREFERENCED(pageSize);
	UNREFERENCED(allocFunc);
	UNREFERENCED(freeFunc);
	UNREFERENCED(maxNumPages);
	UNREFERENCED(alignment);
	//TODO: Implement me!
}
void InitMemArena_PagedHeapArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None)
{
	UNREFERENCED(arena);
	UNREFERENCED(pageSize);
	UNREFERENCED(sourceArena);
	UNREFERENCED(maxNumPages);
	UNREFERENCED(alignment);
	//TODO: Implement me!
}
void InitMemArena_MarkedStack(MemArena_t* arena, u64 size, void* memoryPntr, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(memoryPntr);
	Assert(size > 0);
	Assert(maxNumMarks > 0);
	Assert(size > sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	
	ClearPointer(arena);
	arena->type = MemArenaType_MarkedStack;
	arena->alignment = alignment;
	arena->singleAlloc = false;
	arena->headerPntr = (((u8*)memoryPntr) + 0);
	arena->otherPntr  = (((u8*)memoryPntr) + sizeof(MarkedStackArenaHeader_t));
	arena->mainPntr   = (((u8*)memoryPntr) + sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	arena->size = size - (sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	arena->used = 0;
	arena->numAllocations = 0;
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	ClearPointer(stackHeader);
	stackHeader->maxNumMarks = maxNumMarks;
	stackHeader->numMarks = 0;
	
	arena->telemetryEnabled = true;
	arena->highUsedMark = 0;
	stackHeader->highMarkCount = 0;
}
void InitMemArena_Buffer(MemArena_t* arena, u64 bufferSize, void* bufferPntr, bool singleAlloc = false, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(bufferPntr);
	ClearPointer(arena);
	arena->type = MemArenaType_Buffer;
	arena->alignment = alignment;
	arena->singleAlloc = singleAlloc;
	arena->mainPntr = bufferPntr;
	arena->size = bufferSize;
	arena->used = 0;
	arena->numAllocations = 0;
	
	arena->telemetryEnabled = true;
	arena->highUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}

#define CreateStackBufferArena(arenaName, bufferName, size) MemArena_t arenaName; u8 bufferName[size]; InitMemArena_Buffer(&arenaName, (size), &bufferName[0])

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
bool MemArenaVerify(MemArena_t* arena, bool assertOnFailure = false)
{
	NotNull(arena);
	if (arena->type == MemArenaType_None)
	{
		AssertIfMsg(assertOnFailure, false, "Tried to verify uninitialized arena");
		return false;
	}
	
	switch (arena->type)
	{
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		// case MemArenaType_Redirect:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		// case MemArenaType_StdHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		// case MemArenaType_FixedHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		// case MemArenaType_PagedHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		// case MemArenaType_MarkedStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		// case MemArenaType_Buffer:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +====================================+
		// | Unsupported or Corrupt Arena Type  |
		// +====================================+
		default:
		{
			AssertIfMsg(assertOnFailure, false, "Unsupported or corrupt arena type found in MemArenaVerify");
			return false;
		} break;
	}
	
	return true;
}

u64 GetNumMemMarks(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	return stackHeader->numMarks;
}

// +--------------------------------------------------------------+
// |                      Allocate Function                       |
// +--------------------------------------------------------------+
void* AllocMem(MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to allocate from uninitialized arena");
	
	if (numBytes == 0) { return nullptr; }
	if (arena->singleAlloc && arena->numAllocations > 0)
	{
		GyLibPrintLine_W("Attempted second allocation of %llu out of single alloc arena (type: %s, size: %llu, used: %llu)", numBytes, GetMemArenaTypeStr(arena->type), arena->size, arena->used);
		return nullptr;
	}
	AllocAlignment_t alignment = (alignOverride != AllocAlignment_None) ? alignOverride : arena->alignment;
	
	u8* result = nullptr;
	switch (arena->type)
	{
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		case MemArenaType_Redirect:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in Redirect arena type");
			NotNull(arena->allocFunc);
			result = (u8*)arena->allocFunc(numBytes);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->used += numBytes;
			if (arena->telemetryEnabled)
			{
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = (u8*)AllocMem(arena->sourceArena, numBytes, alignment);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
			if (arena->telemetryEnabled)
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in StdHeap arena type");
			result = (u8*)malloc(numBytes);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->used += numBytes;
			if (arena->telemetryEnabled)
			{
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		// case MemArenaType_FixedHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		// case MemArenaType_PagedHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		case MemArenaType_MarkedStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			u8 alignmentOffset = OffsetToAlign(((u8*)arena->mainPntr) + arena->used, alignment);
			if (arena->used + alignmentOffset + numBytes > arena->size) { return nullptr; }
			result = ((u8*)arena->mainPntr) + arena->used + alignmentOffset;
			arena->used += alignmentOffset + numBytes;
			arena->numAllocations++;
			if (arena->telemetryEnabled)
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
			}
		} break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		case MemArenaType_Buffer:
		{
			u8* basePntr = (u8*)arena->mainPntr;
			u64 neededSize = numBytes;
			result = basePntr + arena->used;
			u8 alignOffset = OffsetToAlign(result, alignment);
			neededSize += alignOffset;
			if (arena->size - arena->used < neededSize) { result = nullptr; break; }
			result += alignOffset;
			IncrementU64(arena->numAllocations);
			arena->used += neededSize;
			if (arena->telemetryEnabled)
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			GyLibPrintLine_E("Unsuported arena type in AllocMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in AllocMem. Maybe the arena is corrupted?");
		} break;
	}
	
	AssertMsg(IsAlignedTo(result, alignment), "An arena has a bug where it tried to return mis-aligned memory");
	return (void*)result;
}

#define AllocStruct(arena, structName)          (structName*)AllocMem((arena), sizeof(structName))
#define AllocArray(arena, structName, numItems) (structName*)AllocMem((arena), sizeof(structName) * (numItems))
#define AllocBytes(arena, numBytes)             (u8*)AllocMem((arena), (numBytes))

MemArena_t AllocBufferArena(MemArena_t* sourceArena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None)
{
	MemArena_t result;
	void* allocatedMemory = AllocMem(sourceArena, numBytes, alignOverride);
	NotNull(allocatedMemory);
	InitMemArena_Buffer(&result, numBytes, allocatedMemory, true);
	return result;
}

char* AllocCharsAndFill(MemArena_t* arena, u64 numChars, const char* dataForFill, bool addNullTerm = true)
{
	NotNull(arena);
	Assert(dataForFill != nullptr || numChars == 0);
	if (numChars == 0 && !addNullTerm) { return nullptr; }
	char* result = (char*)AllocMem(arena, numChars + (addNullTerm ? 1 : 0));
	if (result == nullptr) { return nullptr; }
	if (numChars > 0)
	{
		MyMemCopy(result, dataForFill, numChars);
	}
	if (addNullTerm)
	{
		result[numChars] = '\0';
	}
	return result;
}
char* AllocCharsAndFillNt(MemArena_t* arena, const char* nullTermStr, bool addNullTerm = true)
{
	NotNull(arena);
	NotNull(nullTermStr);
	u64 numChars = MyStrLength64(nullTermStr);
	return AllocCharsAndFill(arena, numChars, nullTermStr, addNullTerm);
}

// +--------------------------------------------------------------+
// |                        Free Function                         |
// +--------------------------------------------------------------+
//NOTE: oldSizeOut may not be written. In some cases it is written to allocSize if provided and no sanity checks are done
bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize = 0, bool ignoreNullptr = false, u64* oldSizeOut = nullptr)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to free from uninitialized arena");
	Assert(ignoreNullptr || allocPntr != nullptr);
	if (allocPntr == nullptr) { return 0; }
	
	bool result = false;
	switch (arena->type)
	{
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		case MemArenaType_Redirect:
		{
			NotNull(arena->freeFunc);
			arena->freeFunc(allocPntr);
			result = true;
			Decrement(arena->numAllocations);
			DecrementBy(arena->used, allocSize);
			if (oldSizeOut != nullptr && allocSize != 0) { *oldSizeOut = allocSize; }
		} break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = FreeMem(arena->sourceArena, allocPntr, allocSize, ignoreNullptr, oldSizeOut);
			Decrement(arena->numAllocations);
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			free(allocPntr);
			Decrement(arena->numAllocations);
			DecrementBy(arena->used, allocSize);
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		// case MemArenaType_FixedHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		// case MemArenaType_PagedHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		case MemArenaType_MarkedStack:
		{
			AssertMsg(false, "FreeMem is not supported for a MarkedStack. Are you trying to free memory allocated on the TempArena");
		} break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		case MemArenaType_Buffer:
		{
			NotNull(arena->mainPntr);
			u8* basePntr = (u8*)arena->mainPntr;
			Assert(IsPntrInsideRange(allocPntr, arena->mainPntr, arena->size));
			if (allocSize == 0 && allocPntr == arena->mainPntr && arena->numAllocations == 1) //the one scenario where allocSize can be assumed
			{
				allocSize = arena->used;
			}
			AssertMsg(allocSize != 0, "Tried to deallocate from Buffer type arena without specifying allocSize and old size could not be assumed");
			AssertMsg(((u8*)allocPntr + allocSize) == (basePntr + arena->used), "Tried to deallocate out of order in Buffer type arena. Ordered frees only please!");
			arena->used -= allocSize;
			Decrement(arena->numAllocations);
			result = true;
		} break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			GyLibPrintLine_E("Unsuported arena type in FreeMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in FreeMem. Maybe the arena is corrupted?");
		} break;
	}
	
	return result;
}

#define HardFreeMem(arena, allocPntr) FreeMem((arena), (allocPntr), 0, false)
#define SoftFreeMem(arena, allocPntr) FreeMem((arena), (allocPntr), 0, true)
#define FreeBufferArena(bufferArena, sourceArena) FreeMem((sourceArena), (bufferArena)->mainPntr, (bufferArena)->size)

// +--------------------------------------------------------------+
// |                     Reallocate Function                      |
// +--------------------------------------------------------------+
void* ReallocMem(MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize = 0, AllocAlignment_t alignOverride = AllocAlignment_None, bool ignoreNullptr = false, u64* oldSizeOut = nullptr)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to realloc from uninitialized arena");
	Assert(ignoreNullptr || allocPntr != nullptr);
	
	AllocAlignment_t alignment = (alignOverride != AllocAlignment_None) ? alignOverride : arena->alignment;
	if (newSize == oldSize && (allocPntr != nullptr || oldSize != 0) && IsAlignedTo(allocPntr, alignment)) //not resizing, just keep the memory where it's at
	{
		return allocPntr;
	}
	if (newSize == 0) //Resizing to 0 is basically freeing
	{
		bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr);
		AssertMsg(freeSuccess, "Failed attempt to free memory in arena when Realloc'd to 0 bytes");
		return nullptr;
	}
	
	bool knownOldSize = (oldSize != 0 || allocPntr == nullptr);
	bool isRealigning = !IsAlignedTo(allocPntr, alignment);
	bool increasingSize = (knownOldSize && newSize > oldSize);
	bool decreasingSize = (knownOldSize && newSize < oldSize);
	u64 sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
	
	UNREFERENCED(isRealigning); //TODO: Remove me!
	u8* result = nullptr;
	switch (arena->type)
	{
		// +======================================+
		// | Temporary AllocMem+FreeMem Solution  |
		// +======================================+
		case MemArenaType_Redirect:
		case MemArenaType_FixedHeap:
		case MemArenaType_PagedHeap:
		case MemArenaType_Buffer:
		{
			result = (u8*)AllocMem(arena, newSize, alignOverride);
			if (result == nullptr)
			{
				if (allocPntr != nullptr)
				{
					u64 reportedOldSize = oldSize;
					bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
					AssertMsg(freeSuccess, "Failed to FreeMem after a failed AllocMem in ReallocMem! Something is probably wrong with this arena");
					Assert(oldSize == 0 || oldSize == reportedOldSize);
					oldSize = reportedOldSize;
					increasingSize = (newSize > oldSize);
					decreasingSize = (newSize < oldSize);
					sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
				}
				if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
				break;
			}
			if (allocPntr != nullptr)
			{
				// if (oldSize == 0) { oldSize = GetAllocSize(arena, allocPntr); } //TODO: Uncomment me!
				Assert(oldSize != 0);
				MyMemCopy(result, allocPntr, oldSize);
			}
			{
				u64 reportedOldSize = oldSize;
				bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
				AssertMsg(freeSuccess, "Failed to FreeMem in ReallocMem! Does this arena type support freeing memory?");
				Assert(oldSize == 0 || oldSize == reportedOldSize);
				oldSize = reportedOldSize;
				increasingSize = (newSize > oldSize);
				decreasingSize = (newSize < oldSize);
				sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
				if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
			}
		} break;
		
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		// case MemArenaType_Redirect:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			u64 reportedOldSize = oldSize;
			result = (u8*)ReallocMem(arena->sourceArena, allocPntr, newSize, oldSize, alignment, ignoreNullptr, &reportedOldSize);
			Assert(oldSize == 0 || oldSize == reportedOldSize);
			oldSize = reportedOldSize;
			increasingSize = (newSize > oldSize);
			decreasingSize = (newSize < oldSize);
			sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
			if (oldSizeOut != nullptr) { *oldSizeOut = reportedOldSize; }
			if (result == nullptr)
			{
				Decrement(arena->numAllocations);
				arena->size = arena->sourceArena->size;
				arena->used = arena->sourceArena->used;
				break;
			}
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
			if (arena->telemetryEnabled)
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in StdHeap type arena");
			result = (u8*)realloc(allocPntr, newSize);
			if (result == nullptr)
			{
				DecrementBy(arena->used, oldSize);
				Decrement(arena->numAllocations);
				return nullptr;
			}
			if (increasingSize) { arena->used += sizeChangeAmount; }
			else if (decreasingSize) { DecrementBy(arena->used, sizeChangeAmount); }
			if (arena->telemetryEnabled)
			{
				if (increasingSize && arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
			}
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		// case MemArenaType_FixedHeap:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		// case MemArenaType_PagedHeap:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		// case MemArenaType_MarkedStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		// case MemArenaType_Buffer:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			GyLibPrintLine_E("Unsuported arena type in ReallocMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in ReallocMem. Maybe the arena is corrupted?");
		} break;
	}
	
	return result;
}

#define HardReallocMem(arena, allocPntr, newSize) ReallocMem((arena), (allocPntr), (newSize), 0, AllocAlignment_None, false)
#define SoftReallocMem(arena, allocPntr, newSize) ReallocMem((arena), (allocPntr), (newSize), 0, AllocAlignment_None, true)

// +--------------------------------------------------------------+
// |                 Push And Pop Mark Functions                  |
// +--------------------------------------------------------------+
void PushMemMark(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	NotNull(arena->otherPntr);
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	if (stackHeader->numMarks == stackHeader->maxNumMarks)
	{
		GyLibPrintLine_E("Tried to push mark %llu onto marked stack which only has support for %llu marks", stackHeader->numMarks+1, stackHeader->maxNumMarks);
		AssertMsg(false, "Too many marks pushed onto a MarkedStack");
		return;
	}
	
	u64* marksPntr = (u64*)arena->otherPntr;
	marksPntr[stackHeader->numMarks] = arena->used;
	stackHeader->numMarks++;
	
	if (arena->telemetryEnabled)
	{
		if (stackHeader->highMarkCount < stackHeader->numMarks) { stackHeader->highMarkCount = stackHeader->numMarks; }
	}
}

void PopMemMark(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	NotNull(arena->otherPntr);
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	if (stackHeader->numMarks == 0)
	{
		GyLibWriteLine_E("Tried to pop stack mark when no marks were left");
		AssertMsg(false, "Tried to pop too many times on a MarkedStack");
		return;
	}
	
	u64* marksPntr = (u64*)arena->otherPntr;
	Assert(marksPntr[stackHeader->numMarks-1] <= arena->used);
	Assert(marksPntr[stackHeader->numMarks-1] <= arena->size);
	arena->used = marksPntr[stackHeader->numMarks-1];
	stackHeader->numMarks--;
}

#endif //  _GY_MEMORY_H
