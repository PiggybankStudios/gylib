/*
File:   gy_str_hash_dictionary.h
Author: Taylor Robbins
Date:   07\29\2022
Description:
	** Holds the StrHashDict_t type and all the related functions for it
*/

#ifndef _GY_STR_HASH_DICTIONARY_H
#define _GY_STR_HASH_DICTIONARY_H

#define GY_STR_HASH_DICTIONARY_DEFEAULT_INITIAL_SIZE    32

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
struct StrHashDictItem_t
{
	u64 hash;
};

struct StrHashDict_t
{
	MemArena_t* allocArena;
	u64 numItems;
	u64 itemSize;
	
	u64 numItemsAlloc;
	void* base;
};

struct StrHashDictIter_t
{
	StrHashDict_t* dict;
	u64 slotIndex;
	u64 index;
	u64 foundIndex;
	u64 itemAndHeaderSize;
	u8* pntr;
};

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeStrHashDict(StrHashDict_t* dict)
{
	NotNull(dict);
	if (dict->base != nullptr)
	{
		NotNull(dict->allocArena);
		u64 itemAndHeaderSize = sizeof(StrHashDictItem_t) + dict->itemSize;
		FreeMem(dict->allocArena, dict->base, dict->numItemsAlloc * itemAndHeaderSize);
	}
	ClearPointer(dict);
}

void CreateStrHashDict(StrHashDict_t* dict, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0)
{
	NotNull(dict);
	NotNull(memArena);
	
	ClearPointer(dict);
	dict->allocArena = memArena;
	dict->itemSize = itemSize;
	dict->numItems = 0;
	u64 itemAndHeaderSize = sizeof(StrHashDictItem_t) + itemSize;
	
	dict->numItemsAlloc = initialRequiredCapacity*2;
	if (dict->numItemsAlloc == 0) { dict->numItemsAlloc = GY_STR_HASH_DICTIONARY_DEFEAULT_INITIAL_SIZE; }
	
	if (dict->numItemsAlloc > 0)
	{
		dict->base = AllocMem(memArena, dict->numItemsAlloc * itemAndHeaderSize);
		Assert(dict->base != nullptr);
		MyMemSet(dict->base, 0x00, dict->numItemsAlloc * itemAndHeaderSize);
	}
	else { dict->base = nullptr; }
}

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
bool StrHashExpand(StrHashDict_t* dict, u64 numItemsRequired)
{
	NotNull(dict);
	NotNull(dict->allocArena);
	
	if (dict->numItemsAlloc < numItemsRequired*2)
	{
		u8* baseBytePntr = (u8*)dict->base;
		u64 itemAndHeaderSize = sizeof(StrHashDictItem_t) + dict->itemSize;
		u64 newNumItemsAlloc = dict->numItemsAlloc*2;
		if (newNumItemsAlloc < numItemsRequired*2) { newNumItemsAlloc = numItemsRequired*2; }
		
		u8* newBase = (u8*)AllocMem(dict->allocArena, newNumItemsAlloc * itemAndHeaderSize);
		NotNull(newBase);
		MyMemSet(newBase, 0x00, newNumItemsAlloc * itemAndHeaderSize);
		
		for (u64 iIndex = 0; iIndex < dict->numItemsAlloc; iIndex++)
		{
			StrHashDictItem_t* item = (StrHashDictItem_t*)(baseBytePntr + (iIndex * itemAndHeaderSize));
			if (item->hash != 0)
			{
				u64 newExpectedIndex = (item->hash % newNumItemsAlloc);
				u64 offset = 0;
				while (offset < newNumItemsAlloc)
				{
					StrHashDictItem_t* newSlot = (StrHashDictItem_t*)(newBase + (((newExpectedIndex + offset) % newNumItemsAlloc) * itemAndHeaderSize));
					if (newSlot->hash == 0)
					{
						MyMemCopy(newSlot, item, itemAndHeaderSize);
						break;
					}
					offset++;
				}
				Assert(offset < newNumItemsAlloc);
			}
		}
		
		if (dict->base != nullptr) { FreeMem(dict->allocArena, dict->base, dict->numItemsAlloc * itemAndHeaderSize); }
		dict->base = (void*)newBase;
		dict->numItemsAlloc = newNumItemsAlloc;
		
		return true;
	}
	else { return false; }
}

StrHashDictIter_t StrHashDictGetIter_(const StrHashDict_t* dict, u64 itemSize)
{
	NotNull(dict);
	Assert(dict->itemSize == itemSize);
	StrHashDictIter_t result = {};
	result.dict = (StrHashDict_t*)dict;
	result.itemAndHeaderSize = sizeof(StrHashDictItem_t) + dict->itemSize;
	result.pntr = nullptr;
	return result;
}

#define StrHashDictGetIter(dict, type) StrHashDictGetIter_((dict), sizeof(type))

bool StrHashDictIter_(StrHashDictIter_t* iter, u64 itemSize, void** itemPntrOut)
{
	NotNull(iter);
	NotNull(iter->dict);
	NotNull(itemPntrOut);
	Assert(iter->dict->itemSize == itemSize);
	
	iter->index = iter->foundIndex;
	while (iter->slotIndex+1 < iter->dict->numItemsAlloc)
	{
		if (iter->pntr == nullptr)
		{
			if (iter->dict->numItemsAlloc == 0) { return false; }
			NotNull(iter->dict->base);
			iter->pntr = (u8*)iter->dict->base;
			iter->slotIndex = 0;
		}
		else
		{
			iter->pntr += iter->itemAndHeaderSize;
			iter->slotIndex++;
		}
		
		StrHashDictItem_t* slotHeader = (StrHashDictItem_t*)iter->pntr;
		if (slotHeader->hash != 0)
		{
			iter->foundIndex++;
			*itemPntrOut = (void*)(slotHeader + 1);
			return true;
		}
	}
	return false;
}

#define StrHashDictIter(iter, type, itemPntrOut) StrHashDictIter_((iter), sizeof(type), (void**)(itemPntrOut))

// +--------------------------------------------------------------+
// |                             Add                              |
// +--------------------------------------------------------------+
void* StrHashDictAdd_(StrHashDict_t* dict, MyStr_t key, u64 itemSize, bool assertOnDuplicate)
{
	NotNull(dict);
	Assert(dict->itemSize == itemSize);
	NotNull(dict->allocArena);
	
	StrHashExpand(dict, dict->numItems+1);
	
	u64 keyHash = FnvHashStr(key);
	Assert(keyHash != 0);
	u64 expectedIndex = (keyHash % dict->numItemsAlloc);
	u64 itemAndHeaderSize = sizeof(StrHashDictItem_t) + dict->itemSize;
	u8* baseBytePntr = (u8*)dict->base;
	
	void* result = nullptr;
	u64 offset = 0;
	while (offset < dict->numItemsAlloc)
	{
		StrHashDictItem_t* newSlot = (StrHashDictItem_t*)(baseBytePntr + (((expectedIndex + offset) % dict->numItemsAlloc) * itemAndHeaderSize));
		if (newSlot->hash == keyHash)
		{
			AssertIfMsg(assertOnDuplicate, false, "Duplicate key added to StrHashDict!");
			newSlot->hash = keyHash;
			result = (void*)(newSlot + 1);
			break;
		}
		else if (newSlot->hash == 0)
		{
			newSlot->hash = keyHash;
			result = (void*)(newSlot + 1);
			break;
		}
		offset++;
	}
	Assert(result != nullptr);
	
	dict->numItems++;
	return result;
}
void* StrHashDictAdd_(StrHashDict_t* dict, const char* nullTermStr, u64 itemSize, bool assertOnFailure)
{
	return StrHashDictAdd_(dict, NewStr(nullTermStr), itemSize, assertOnFailure);
}

#define StrHashDictAddHard(dict, key, type) (type*)StrHashDictAdd_((dict), (key), sizeof(type), true)
#define StrHashDictAddSoft(dict, key, type) (type*)StrHashDictAdd_((dict), (key), sizeof(type), false)
#define StrHashDictAdd(dict, key, type)     StrHashDictAddHard(dict, key, type)
#define StrHashDictAddEmptyHard(dict, key)  StrHashDictAdd_((dict), (key), 0, true)
#define StrHashDictAddEmptySoft(dict, key)  StrHashDictAdd_((dict), (key), 0, false)
#define StrHashDictAddEmpty(dict, key)      StrHashDictAddEmptyHard(dict, key)

// +--------------------------------------------------------------+
// |                            Remove                            |
// +--------------------------------------------------------------+
bool StrHashDictRemove_(StrHashDict_t* dict, MyStr_t key, u64 itemSize, bool assertOnFailure)
{
	NotNull(dict);
	Assert(dict->itemSize == itemSize);
	NotNull(dict->allocArena);
	
	u64 keyHash = FnvHashStr(key);
	Assert(keyHash != 0);
	u64 expectedIndex = (keyHash % dict->numItemsAlloc);
	u64 itemAndHeaderSize = sizeof(StrHashDictItem_t) + dict->itemSize;
	u8* baseBytePntr = (u8*)dict->base;
	
	bool result = false;
	u64 offset = 0;
	while (offset < dict->numItemsAlloc)
	{
		StrHashDictItem_t* newSlot = (StrHashDictItem_t*)(baseBytePntr + (((expectedIndex + offset) % dict->numItemsAlloc) * itemAndHeaderSize));
		if (newSlot->hash == keyHash)
		{
			newSlot->hash = 0;
			dict->numItems--;
			result = true;
			break;
		}
		offset++;
	}
	
	AssertIf(assertOnFailure, result);
	return result;
}
bool StrHashDictRemove_(StrHashDict_t* dict, const char* nullTermStr, u64 itemSize, bool assertOnFailure)
{
	return StrHashDictRemove_(dict, NewStr(nullTermStr), itemSize, assertOnFailure);
}

#define StrHashDictRemoveHard(dict, key, type) StrHashDictRemove_((dict), (key), sizeof(type), true)
#define StrHashDictRemoveSoft(dict, key, type) StrHashDictRemove_((dict), (key), sizeof(type), false)
#define StrHashDictRemove(dict, key, type)     StrHashDictRemoveHard(dict, key, type)
#define StrHashDictRemoveEmptyHard(dict, key)  StrHashDictRemove_((dict), (key), 0, true)
#define StrHashDictRemoveEmptySoft(dict, key)  StrHashDictRemove_((dict), (key), 0, false)
#define StrHashDictRemoveEmpty(dict, key)      StrHashDictRemoveEmptyHard(dict, key)

// +--------------------------------------------------------------+
// |                             Get                              |
// +--------------------------------------------------------------+
void* StrHashDictGet_(StrHashDict_t* dict, MyStr_t key, u64 itemSize, bool assertOnFailure)
{
	NotNull(dict);
	Assert(dict->itemSize == itemSize);
	NotNull(dict->allocArena);
	
	u64 keyHash = FnvHashStr(key);
	Assert(keyHash != 0);
	u64 expectedIndex = (keyHash % dict->numItemsAlloc);
	u64 itemAndHeaderSize = sizeof(StrHashDictItem_t) + dict->itemSize;
	u8* baseBytePntr = (u8*)dict->base;
	
	void* result = nullptr;
	u64 offset = 0;
	while (offset < dict->numItemsAlloc)
	{
		StrHashDictItem_t* itemPntr = (StrHashDictItem_t*)(baseBytePntr + (((expectedIndex + offset) % dict->numItemsAlloc) * itemAndHeaderSize));
		if (itemPntr->hash == keyHash)
		{
			result = (void*)(itemPntr + 1);
			break;
		}
		offset++;
	}
	AssertIfMsg(assertOnFailure, result != nullptr, "Key not found in StrHashDict!");
	
	return result;
}
void* StrHashDictGet_(StrHashDict_t* dict, const char* nullTermStr, u64 itemSize, bool assertOnFailure)
{
	return StrHashDictGet_(dict, NewStr(nullTermStr), itemSize, assertOnFailure);
}

#define StrHashDictGetHard(dict, key, type)  (type*)StrHashDictGet_((dict), (key), sizeof(type), true)
#define StrHashDictGetSoft(dict, key, type)  (type*)StrHashDictGet_((dict), (key), sizeof(type), false)
#define StrHashDictGet(dict, key, type)      StrHashDictGetHard(dict, key, type)
#define StrHashDictContains(dict, key, type) (StrHashDictGet_((dict), (key), sizeof(type), false) != nullptr)
#define StrHashDictContainsEmpty(dict, key)  (StrHashDictGet_((dict), (key), 0, false) != nullptr)

#endif //  _GY_STR_HASH_DICTIONARY_H