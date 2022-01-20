/*
File:   gy_variable_array.h
Author: Taylor Robbins
Date:   10\12\2021
Description:
	** The VarArray_t structure acts like a stretchy buffer, resizing as needed to hold as many fixed size elements as needed
*/

#ifndef _GY_VARIABLE_ARRAY_H
#define _GY_VARIABLE_ARRAY_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
//TODO: Add support for a maximum length restriction
struct VarArray_t
{
	MemArena_t* allocArena;
	u64 itemSize;
	u64 allocChunkSize;
	bool exponentialChunkSize;
	MyStr_t name;
	
	u64 length;
	u64 allocLength;
	void* items;
	bool wasExpanded; //feel free to clear this in usage code
	u64 numExpansions;
};

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeVarArray(VarArray_t* array)
{
	NotNull(array);
	if (array->items != nullptr)
	{
		NotNull(array->allocArena);
		FreeMem(array->allocArena, array->items, array->itemSize * array->allocLength);
	}
	if (!IsStrEmpty(array->name))
	{
		NotNull(array->allocArena);
		FreeString(array->allocArena, &array->name);
	}
	ClearPointer(array);
}

void CreateVarArray(VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
{
	NotNull(array);
	NotNull(memArena);
	Assert(itemSize > 0);
	Assert(allocChunkSize > 0);
	ClearPointer(array);
	array->itemSize = itemSize;
	array->allocArena = memArena;
	array->allocChunkSize = allocChunkSize;
	array->exponentialChunkSize = exponentialChunkSize;
	array->length = 0;
	
	if (exponentialChunkSize)
	{
		array->allocLength = allocChunkSize;
		u64 sanityCounter = 0;
		while (array->allocLength < initialRequiredCapacity)
		{
			Assert(array->allocLength <= UINT64_MAX/2);
			array->allocLength *= 2;
			sanityCounter++;
			Assert(sanityCounter <= initialRequiredCapacity); //Infinite loop sanity check
		}
	}
	else
	{
		array->allocLength = RoundUpToU64(initialRequiredCapacity, allocChunkSize);
	}
	if (array->allocLength > 0)
	{
		array->items = AllocMem(memArena, array->allocLength * itemSize);
		if (array->items == nullptr)
		{
			AssertMsg(false, "Initial Allocation inside CreateVarArray failed!");
			ClearPointer(array);
			return;
		}
	}
}

void VarArrayName(VarArray_t* array, MyStr_t newName)
{
	NotNull(array->allocArena);
	if (array->name.pntr != nullptr)
	{
		FreeString(array->allocArena, &array->name);
	}
	array->name = AllocString(array->allocArena, &newName);
	NotNullStr(&array->name);
}

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
//Returns if the array actually expanded
bool VarArrayExpand(VarArray_t* array, u64 capacityRequired)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	if (array->allocLength >= capacityRequired) { return false; }
	
	u64 newLength = 0;
	if (array->exponentialChunkSize)
	{
		Assert(array->allocLength > 0);
		newLength = array->allocLength;
		u64 sanityCounter = 0;
		while (newLength < capacityRequired)
		{
			Assert(newLength <= UINT64_MAX/2);
			newLength *= 2;
			sanityCounter++;
			Assert(sanityCounter <= capacityRequired); //Infinite loop sanity check
		}
	}
	else
	{
		newLength = RoundUpToU64(capacityRequired, array->allocChunkSize);
	}
	Assert(newLength >= capacityRequired);
	Assert(newLength <= (UINT64_MAX / array->itemSize)); //u64 overflow would be weird
	
	void* newSpace = AllocMem(array->allocArena, newLength * array->itemSize);
	if (newSpace == nullptr)
	{
		GyLibPrintLine_E("Failed to expand variable array %s to %llu items at %llu bytes each", (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", newLength, array->itemSize);
		AssertMsg(false, "Failed to expand variable array");
		return false;
	}
	
	if (array->length > 0)
	{
		MyMemCopy(newSpace, array->items, array->length * array->itemSize);
	}
	if (array->items != nullptr)
	{
		FreeMem(array->allocArena, array->items, array->allocLength * array->itemSize);
	}
	
	array->items = newSpace;
	array->allocLength = newLength;
	array->wasExpanded = true;
	IncrementU64(array->numExpansions);
	return true;
}

bool VarArrayContains_(VarArray_t* array, const void* itemInQuestion, u64 itemSize)
{
	NotNull(array);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayContains. Make sure you're accessing the VarArray with the correct type!");
	if (itemInQuestion == nullptr) { return false; }
	if (array->items == nullptr) { return false; }
	if ((const u8*)itemInQuestion < (u8*)array->items) { return false; }
	if ((const u8*)itemInQuestion >= ((u8*)array->items) + (array->itemSize * array->length)) { return false; }
	#if DEBUG_BUILD
	//Make sure the item is aligned properly
	u64 offsetFromBase = (u64)(((const u8*)itemInQuestion) - (u8*)array->items);
	Assert((offsetFromBase % array->itemSize) == 0);
	#endif
	return true;
}
#define VarArrayContains(array, itemPntr) VarArrayContains_((array), (itemPntr), sizeof(*(itemPntr)))
#define VarArrayContainsTyped(array, itemPntr, type) VarArrayContains_((array), (itemPntr), sizeof(type))

bool VarArrayGetIndexOf_(VarArray_t* array, const void* itemInQuestion, u64* indexOut, u64 itemSize)
{
	if (!VarArrayContains_(array, itemInQuestion, itemSize)) { return false; }
	u64 offsetFromBase = (u64)(((const u8*)itemInQuestion) - (u8*)array->items);
	Assert((offsetFromBase % array->itemSize) == 0);
	NotNull(indexOut);
	*indexOut = (offsetFromBase / array->itemSize);
	return true;
}

#define VarArrayGetIndexOf(array, itemPntr, indexOutPntr, type) VarArrayGetIndexOf_((array), (itemPntr), (indexOutPntr), sizeof(type))

void VarArrayClear(VarArray_t* array, bool deallocate = false)
{
	NotNull(array);
	if (deallocate && array->allocLength > 0)
	{
		NotNull(array->allocArena);
		FreeMem(array->allocArena, array->items, array->allocLength * array->itemSize);
		array->items = nullptr;
		array->allocLength = 0;
	}
	array->length = 0;
}

// +--------------------------------------------------------------+
// |                             Get                              |
// +--------------------------------------------------------------+
void* VarArrayGet_(VarArray_t* array, u64 index, u64 itemSize, bool assertOnFailure)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayGet. Make sure you're accessing the VarArray with the correct type!");
	
	if (index >= array->length)
	{
		if (assertOnFailure)
		{
			GyLibPrintLine_E("Tried to get item %llu/%llu in VarArray %s of itemSize %llu", index, array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
			AssertMsg(false, "VarArrayGet out of bounds!");
		}
		return nullptr;
	}
	
	NotNull(array->items);
	void* result = (((u8*)array->items) + (index * array->itemSize));
	return result;
}
const void* VarArrayGet_(const VarArray_t* array, u64 index, u64 itemSize, bool assertOnFailure) //const-variant
{
	return (const void*)VarArrayGet_((VarArray_t*)array, index, itemSize, assertOnFailure);
}

#define VarArrayGetHard(array, index, type) ((type*)VarArrayGet_((array), (index), sizeof(type), true))
#define VarArrayGetSoft(array, index, type) ((type*)VarArrayGet_((array), (index), sizeof(type), false))
#define VarArrayGet(array, index, type) VarArrayGetHard((array), (index), type)

#define VarArrayGetFirstHard(array, type) VarArrayGetHard((array), 0, type)
#define VarArrayGetFirstSoft(array, type) (((array)->length > 0) ? VarArrayGetSoft((array), 0, type) : nullptr)
#define VarArrayGetFirst(array, type) VarArrayGetFirstHard((array), type) 

#define VarArrayGetLastHard(array, type) VarArrayGetHard((array), (array)->length - 1, type)
#define VarArrayGetLastSoft(array, type) (((array)->length > 0) ? VarArrayGetSoft((array), (array)->length - 1, type) : nullptr)
#define VarArrayGetLast(array, type) VarArrayGetLastHard((array), type) 

#define VarArrayLoop(arrayPntr, indexName) for (u64 indexName = 0; indexName < (arrayPntr)->length; indexName++)
#define VarArrayLoopGet(type, varName, arrayPntr, index) type* varName = VarArrayGet((arrayPntr), (index), type)

// +--------------------------------------------------------------+
// |                             Add                              |
// +--------------------------------------------------------------+
//NOTE: This always asserts on failure to allocate since VarArrayExpand defaults to asserting
void* VarArrayAdd_(VarArray_t* array, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayAdd. Make sure you're accessing the VarArray with the correct type!");
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength >= array->length+1);
	
	void* result = (((u8*)array->items) + (array->itemSize * array->length));
	#if DEBUG_BUILD
	MyMemSet(result, 0xCC, array->itemSize);
	#endif
	array->length++;
	
	return result;
}
#define VarArrayAdd(array, type) (type*)VarArrayAdd_((array), sizeof(type))
#define VarArrayPush(array, type) VarArrayAdd((array), (type))

// +--------------------------------------------------------------+
// |                            Insert                            |
// +--------------------------------------------------------------+
//NOTE: This always asserts on failure to allocate since VarArrayExpand defaults to asserting
void* VarArrayInsert_(VarArray_t* array, u64 index, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayInsert. Make sure you're accessing the VarArray with the correct type!");
	Assert(index <= array->length);
	if (index == array->length) { return VarArrayAdd_(array, itemSize); }
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength >= array->length+1);
	
	//Move all items above the index up by one slot
	for (u64 iIndex = array->length-1; iIndex > index; iIndex--)
	{
		MyMemCopy(VarArrayGet_(array, iIndex, itemSize, true), VarArrayGet_(array, iIndex-1, itemSize, true), array->itemSize);
	}
	
	void* result = (((u8*)array->items) + (array->itemSize * index));
	#if DEBUG_BUILD
	MyMemSet(result, 0xCC, array->itemSize);
	#endif
	array->length++;
	
	return result;
}

#define VarArrayInsert(array, index, type) (type*)VarArrayInsert_((array), (index), sizeof(type))
#define VarArrayPushFront(array, type) (type*)VarArrayInsert_((array), 0, sizeof(type))

// +--------------------------------------------------------------+
// |                            Remove                            |
// +--------------------------------------------------------------+
void VarArrayRemove_(VarArray_t* array, u64 index, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayRemove. Make sure you're accessing the VarArray with the correct type!");
	
	if (index >= array->length)
	{
		GyLibPrintLine_E("Tried to remove item [%llu]/%llu in VarArray %s of itemSize %llu", index, array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
		AssertMsg(false, "VarArrayRemove out of bounds!");
		return;
	}
	
	//move all items above index down by one
	for (u64 iIndex = index; iIndex+1 < array->length; iIndex++)
	{
		MyMemCopy(VarArrayGet_(array, iIndex, itemSize, true), VarArrayGet_(array, iIndex+1, itemSize, true), array->itemSize);
	}
	array->length--;
}

#define VarArrayRemove(array, index, type) VarArrayRemove_((array), (index), sizeof(type))
#define VarArrayPop(array, type) VarArrayRemove_((array), (array)->length - 1, sizeof(type))
#define VarArrayPopFront(array, type) VarArrayRemove_((array), 0, sizeof(type))

// +--------------------------------------------------------------+
// |                         RemoveByPntr                         |
// +--------------------------------------------------------------+
void VarArrayRemoveByPntr_(VarArray_t* array, const void* itemToRemove, u64 itemSize)
{
	u64 itemIndex = 0;
	bool itemInArray = VarArrayGetIndexOf_(array, itemToRemove, &itemIndex, itemSize);
	Assert(itemInArray);
	VarArrayRemove_(array, itemIndex, itemSize);
}

#define VarArrayRemoveByPntr(array, itemToRemove) VarArrayRemoveByPntr_((array), (itemToRemove), sizeof(*(itemToRemove)))
#define VarArrayRemoveByPntrTyped(array, itemToRemove, type) VarArrayRemoveByPntr_((array), (itemToRemove), sizeof(type))

// +--------------------------------------------------------------+
// |                           AddRange                           |
// +--------------------------------------------------------------+
void* VarArrayAddRange_(VarArray_t* array, u64 index, u64 newItemsCount, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayAddRange. Make sure you're accessing the VarArray with the correct type!");
	Assert(index <= array->length);
	if (newItemsCount == 0) { return nullptr; }
	if (newItemsCount == 1) { return VarArrayInsert_(array, index, itemSize); }
	
	VarArrayExpand(array, array->length+newItemsCount);
	Assert(array->allocLength >= array->length+newItemsCount);
	
	//Move all items above the index up by however many slots are being inserted
	if (array->length > 0)
	{
		for (u64 iIndex = array->length-1; iIndex >= index; iIndex--)
		{
			MyMemCopy(VarArrayGet_(array, iIndex + newItemsCount, itemSize, true), VarArrayGet_(array, iIndex, itemSize, true), array->itemSize);
		}
	}
	
	void* result = (((u8*)array->items) + (array->itemSize * index));
	#if DEBUG_BUILD
	MyMemSet(result, 0xCC, newItemsCount * array->itemSize);
	#endif
	array->length += newItemsCount;
	
	return result;
}

#define VarArrayAddRange(array, index, newItemsCount, type) (type*)VarArrayAddRange_((array), (index), (newItemsCount), sizeof(type))

// +--------------------------------------------------------------+
// |                         AddVarArray                          |
// +--------------------------------------------------------------+
void VarArrayAddVarArray(VarArray_t* destArray, const VarArray_t* sourceArray, u64 destIndex, u64 sourceIndex = 0, u64 sourceCount = UINT64_MAX)
{
	NotNull(destArray);
	NotNull(sourceArray);
	AssertMsg(destArray->itemSize == sourceArray->itemSize, "Incompatible arrays passed to VarArrayAddVarArray. Make sure the two arrays are the same type!");
	Assert(destIndex <= destArray->length);
	Assert(sourceIndex <= sourceArray->length);
	Assert(destArray != sourceArray);
	if (sourceCount == UINT64_MAX) { sourceCount = sourceArray->length; }
	Assert(sourceIndex+sourceCount <= sourceArray->length);
	if (sourceCount == 0) { return; }
	
	void* newSpace = VarArrayAddRange_(destArray, destIndex, sourceCount, destArray->itemSize);
	NotNull(newSpace);
	MyMemCopy(newSpace, VarArrayGet_(sourceArray, sourceIndex, sourceArray->itemSize, true), sourceCount * destArray->itemSize);
}

#define VarArrayMerge(destArray, sourceArray) VarArrayAddVarArray((destArray), (sourceArray), (destArray)->length, 0, (sourceArray)->length)

// +--------------------------------------------------------------+
// |                         RemoveRange                          |
// +--------------------------------------------------------------+
void VarArrayRemoveRange_(VarArray_t* array, u64 index, u64 numItemsToRemove, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayRemoveRange. Make sure you're accessing the VarArray with the correct type!");
	if (numItemsToRemove == 0) { return; }
	if (numItemsToRemove == 1) { VarArrayRemove_(array, index, itemSize); return; }
	
	if (index >= array->length || index + numItemsToRemove > array->length)
	{
		GyLibPrintLine_E("Tried to remove items [%llu-%llu]/%llu in VarArray %s of itemSize %llu", index, index + (numItemsToRemove-1), array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
		AssertMsg(false, "VarArrayRemoveRange out of bounds!");
		return;
	}
	
	//copy all items above the removed range down by numItemsToRemove
	for (u64 iIndex = index+numItemsToRemove; iIndex < array->length; iIndex++)
	{
		MyMemCopy(VarArrayGet_(array, iIndex-numItemsToRemove, itemSize, true), VarArrayGet_(array, iIndex, itemSize, true), array->itemSize);
	}
	array->length -= numItemsToRemove;
}

#define VarArrayRemoveRange(array, index, numItemsToRemove, type) VarArrayRemoveRange_((array), (index), (numItemsToRemove), sizeof(type))

// +--------------------------------------------------------------+
// |                             Copy                             |
// +--------------------------------------------------------------+
void VarArrayCopy(VarArray_t* destArray, const VarArray_t* sourceArray, MemArena_t* memArena)
{
	NotNull(memArena);
	NotNull(destArray);
	NotNull(sourceArray);
	Assert(destArray != sourceArray);
	CreateVarArray(destArray, memArena, sourceArray->itemSize, sourceArray->length, sourceArray->exponentialChunkSize, sourceArray->allocChunkSize);
	if (sourceArray->length > 0)
	{
		VarArrayMerge(destArray, sourceArray);
	}
}

// +--------------------------------------------------------------+
// |                             Move                             |
// +--------------------------------------------------------------+
void* VarArrayMove(VarArray_t* array, u64 fromIndex, u64 toIndex, bool swapWithTarget = true)
{
	NotNull(array);
	NotNull(array->items);
	Assert(fromIndex < array->length);
	Assert(toIndex < array->length);
	if (fromIndex == toIndex) { return VarArrayGet_(array, fromIndex, array->itemSize, true); } //no work to do
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength > array->length);
	void* tempSpace = (void*)(((u8*)array->items) + (array->length * array->itemSize));
	
	void* movingItem = VarArrayGet_(array, fromIndex, array->itemSize, true);
	void* replaceItem = VarArrayGet_(array, toIndex, array->itemSize, true);
	if (swapWithTarget)
	{
		MyMemCopy(tempSpace, movingItem, array->itemSize);
		MyMemCopy(movingItem, replaceItem, array->itemSize);
		MyMemCopy(replaceItem, tempSpace, array->itemSize);
	}
	else
	{
		MyMemCopy(tempSpace, movingItem, array->itemSize);
		if (toIndex > fromIndex)
		{
			//shift things in-between down by one
			for (u64 index = fromIndex; index < toIndex; index++)
			{
				void* thisItem = VarArrayGet_(array, index, array->itemSize, true);
				void* nextItem = VarArrayGet_(array, index+1, array->itemSize, true);
				MyMemCopy(thisItem, nextItem, array->itemSize);
			}
		}
		else
		{
			//shift things in-between up by one
			for (u64 index = fromIndex; index > toIndex; index--)
			{
				void* thisItem = VarArrayGet_(array, index, array->itemSize, true);
				void* prevItem = VarArrayGet_(array, index-1, array->itemSize, true);
				MyMemCopy(thisItem, prevItem, array->itemSize);
			}
		}
		MyMemCopy(replaceItem, tempSpace, array->itemSize);
	}
	
	return replaceItem;
}

//TODO: Add VarArraySort if gy_sorting.h is included?

#endif //  _GY_VARIABLE_ARRAY_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
VarArray_t
@Functions
void FreeVarArray(VarArray_t* array)
void CreateVarArray(VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
void VarArrayName(VarArray_t* array, MyStr_t newName)
bool VarArrayExpand(VarArray_t* array, u64 capacityRequired)
#define VarArrayContains(array, itemPntr)
#define VarArrayContainsTyped(array, itemPntr, type)
#define VarArrayGetIndexOf(array, itemPntr, indexOutPntr, type)
void VarArrayClear(VarArray_t* array, bool deallocate = false)
#define VarArrayGetHard(array, index, type)
#define VarArrayGetSoft(array, index, type)
#define VarArrayGet(array, index, type)
#define VarArrayGetFirstHard(array, type)
#define VarArrayGetFirstSoft(array, type)
#define VarArrayGetFirst(array, type)
#define VarArrayGetLastHard(array, type)
#define VarArrayGetLastSoft(array, type)
#define VarArrayGetLast(array, type)
#define VarArrayLoop(arrayPntr, indexName)
#define VarArrayLoopGet(type, varName, arrayPntr, index)
#define VarArrayAdd(array, type)
#define VarArrayPush(array, type)
#define VarArrayInsert(array, index, type)
#define VarArrayPushFront(array, type)
#define VarArrayRemove(array, index, type)
#define VarArrayPop(array, type)
#define VarArrayPopFront(array, type)
#define VarArrayRemoveByPntr(array, itemToRemove)
#define VarArrayRemoveByPntrTyped(array, itemToRemove, type)
#define VarArrayAddRange(array, index, newItemsCount, type)
void VarArrayAddVarArray(VarArray_t* destArray, const VarArray_t* sourceArray, u64 destIndex, u64 sourceIndex = 0, u64 sourceCount = UINT64_MAX)
#define VarArrayMerge(destArray, sourceArray)
#define VarArrayRemoveRange(array, index, numItemsToRemove, type)
void VarArrayCopy(VarArray_t* destArray, const VarArray_t* sourceArray, MemArena_t* memArena)
void* VarArrayMove(VarArray_t* array, u64 fromIndex, u64 toIndex, bool swapWithTarget = true)
*/
