/*
File:   gy_string_builder.h
Author: Taylor Robbins
Date:   05\12\2023
Description:
	** A string builder is something that efficiently manages building a string
	** without knowing how long the string is going to be beforehand. Often this
	** is serialization code that wants to use printf style formatting a bunch
	** of times in a row and end up with a large string holding the serialized data.
*/

#ifndef _GY_STRING_BUILDER_H
#define _GY_STRING_BUILDER_H

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct StringBuilder_t
{
	MemArena_t* allocArena;
	u64 length; //doesn't include null-term char so should always be strictly less than allocLength
	u64 allocLength;
	char* chars;
};

// +--------------------------------------------------------------+
// |                    New and Free Functions                    |
// +--------------------------------------------------------------+
void FreeStringBuilder(StringBuilder_t* builder)
{
	NotNull(builder);
	if (builder->chars != nullptr)
	{
		NotNull(builder->allocArena);
		FreeMem(builder->allocArena, builder->chars, builder->allocLength);
	}
	ClearPointer(builder);
}
void NewStringBuilder(StringBuilder_t* builder, MemArena_t* memArena, u64 initialRequiredCapacity = 0)
{
	NotNull(builder);
	NotNull(memArena);
	ClearPointer(builder);
	builder->allocArena = memArena;
	builder->allocLength = 0;
	if (initialRequiredCapacity > 0) { builder->allocLength = PowerOfTwoGreaterThanOrEqualTo(initialRequiredCapacity); }
	if (builder->allocLength > 0)
	{
		builder->chars = AllocArray(builder->allocArena, char, builder->allocLength);
		NotNull(builder->chars);
		builder->chars[0] = '\0';
	}
}

//returns the number of bytes at the end of the allocation that are currently unused (with the option to query the memory arena for space that is following our allocation that is also unallocated)
u64 StringBuilderGetNumUnusedBytes(StringBuilder_t* builder, bool includeMemArenaSpace = false, MemArenaSpaceAfterFixupInfo_t* fixupInfoOut = nullptr)
{
	NotNull2(builder, builder->allocArena);
	u64 memArenaSpace = 0;
	if (includeMemArenaSpace && builder->chars != nullptr)
	{
		NotNull(fixupInfoOut);
		memArenaSpace = GrowMemQuery(builder->allocArena, builder->chars, builder->allocLength, fixupInfoOut);
	}
	return (builder->allocLength-1 - builder->length) + memArenaSpace;
}

void StringBuilderAppend(StringBuilder_t* builder, MyStr_t str)
{
	u64 spaceAvailable = StringBuilderGetNumUnusedBytes(builder, false);
	if (spaceAvailable >= str.length)
	{
		MyMemCopy(&builder->chars[builder->length], str.chars, str.length);
		builder->length += str.length;
		builder->chars[builder->length] = '\0';
	}
	else
	{
		MemArenaSpaceAfterFixupInfo_t arenaFixupInfo = {};
		u64 allocatedSpaceAvailable = spaceAvailable;
		spaceAvailable = StringBuilderGetNumUnusedBytes(builder, true, &arenaFixupInfo);
		if (spaceAvailable >= str.length)
		{
			u64 numNewBytesUsed = (str.length - allocatedSpaceAvailable);
			MyMemCopy(&builder->chars[builder->length], str.chars, str.length);
			builder->length += str.length;
			builder->chars[builder->length] = '\0';
			GrowMem(builder->allocArena, builder->chars, builder->allocLength, builder->allocLength + numNewBytesUsed, &arenaFixupInfo);
			builder->allocLength += numNewBytesUsed;
		}
		else
		{
			u64 newAllocLength = PowerOfTwoGreaterThanOrEqualTo(builder->length + str.length + 1);
			char* newChars = AllocArray(builder->allocArena, char, newAllocLength);
			NotNull(newChars);
			if (builder->allocLength > 0)
			{
				MyMemCopy(newChars, builder->chars, builder->allocLength);
				FreeMem(builder->allocArena, builder->chars);
			}
			builder->chars = newChars;
			builder->allocLength = newAllocLength;
			
			MyMemCopy(&builder->chars[builder->length], str.chars, str.length);
			builder->length += str.length;
			builder->chars[builder->length] = '\0';
		}
	}
}

#endif //  _GY_STRING_BUILDER_H
