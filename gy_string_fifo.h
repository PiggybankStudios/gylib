/*
File:   gy_string_fifo.h
Author: Taylor Robbins
Date:   01\01\2022
Description:
	** Holds a bunch of functions that help us create and work with StringFifo_t structure.
	** This data structure is essentially a linked list in a fixed size buffer that we treat as a FIFO.
	** Each string can be an arbitrary length and have a meta information structure tacked to the front
	** with some options for arbitrary length strings to feed those meta structures as well.
	** This is primarily used to hold the data for a debug output console window which also stores the
	** filePath and functionName of the code that generated each line of output.
*/

#ifndef _GY_STRING_FIFO_H
#define _GY_STRING_FIFO_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"

struct StringFifoLine_t
{
	StringFifoLine_t* prev;
	StringFifoLine_t* next;
	u64 lineNumber;
	
	//These can be found immediately following this structure
	u64 metaStructSize;
	u64 metaStringLength;
	u64 textLength;
};

struct StringFifo_t
{
	MemArena_t* allocArena; //if nullptr then DestroyStringFifo does nothing. Someone external is managing this memory
	
	u64 numLines;
	u64 nextLineNumber;
	StringFifoLine_t* firstLine;
	StringFifoLine_t* lastLine;
	
	u64 bufferSize;
	u64 used;
	u8* buffer;
};

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
u64 GetFifoLineTotalSize(const StringFifoLine_t* line)
{
	NotNull_(line);
	return sizeof(StringFifoLine_t) + line->metaStructSize + line->metaStringLength+1 + line->textLength+1;
}
void* GetFifoLineMetaStruct_(StringFifoLine_t* line, u64 expectedStructSize)
{
	NotNull_(line);
	Assert_(line->metaStructSize == expectedStructSize);
	return (void*)(line + 1);
}
const void* GetFifoLineMetaStruct_(const StringFifoLine_t* line, u64 expectedStructSize) //const-variant
{
	return (const void*)GetFifoLineMetaStruct_((StringFifoLine_t*)line, expectedStructSize);
}
//TODO: This define technically throws away const correctness because of the non-const type* cast
#define GetFifoLineMetaStruct(linePntr, type) (type*)GetFifoLineMetaStruct_((linePntr), sizeof(type))
MyStr_t GetFifoLineMetaString(const StringFifoLine_t* line)
{
	NotNull_(line);
	MyStr_t result;
	result.pntr = (char*)(line + 1) + line->metaStructSize;
	result.length = line->metaStringLength;
	return result;
}
MyStr_t GetFifoLineText(const StringFifoLine_t* line)
{
	NotNull_(line);
	MyStr_t result;
	result.pntr = (char*)(line + 1) + line->metaStructSize + line->metaStringLength+1;
	result.length = line->textLength;
	return result;
}
const void* GetFifoLineEndPntr(const StringFifoLine_t* line)
{
	NotNull_(line);
	return (const void*)((const char*)(line + 1) + line->metaStructSize + line->metaStringLength+1 + line->textLength+1);
}

u64 GetStringFifoPntrIndex(const StringFifo_t* fifo, const void* pntr)
{
	NotNull_(fifo);
	NotNull_(pntr);
	Assert_(((const u8*)pntr) >= fifo->buffer && ((const u8*)pntr) < fifo->buffer + fifo->bufferSize);
	return (u64)(((const u8*)pntr) - ((const u8*)fifo->buffer));
}
u64 GetStringFifoTailIndex(const StringFifo_t* fifo)
{
	if (fifo->firstLine == nullptr)
	{
		Assert_(fifo->lastLine == nullptr);
		return 0;
	}
	return GetStringFifoPntrIndex(fifo, fifo->firstLine);
}
u64 GetStringFifoHeadIndex(const StringFifo_t* fifo)
{
	if (fifo->lastLine == nullptr)
	{
		Assert_(fifo->firstLine == nullptr);
		return 0;
	}
	return GetStringFifoPntrIndex(fifo, GetFifoLineEndPntr((const StringFifoLine_t*)fifo->lastLine));
}

// +--------------------------------------------------------------+
// |                Creation/Destruction Functions                |
// +--------------------------------------------------------------+
void DestroyStringFifo(StringFifo_t* fifo)
{
	NotNull_(fifo);
	if (fifo->allocArena != nullptr)
	{
		FreeMem(fifo->allocArena, fifo->buffer);
	}
	ClearPointer(fifo);
}

void CreateStringFifo(StringFifo_t* fifo, u64 bufferSize, u8* bufferPntr)
{
	NotNull_(fifo);
	NotNull_(bufferPntr);
	Assert_(bufferSize > 0);
	ClearPointer(fifo);
	fifo->allocArena = nullptr;
	fifo->bufferSize = bufferSize;
	fifo->buffer = bufferPntr;
	fifo->used = 0;
	fifo->numLines = 0;
	fifo->firstLine = nullptr;
	fifo->lastLine = nullptr;
	fifo->nextLineNumber = 1;
}
void CreateStringFifoInArena(StringFifo_t* fifo, MemArena_t* memArena, u64 bufferSize)
{
	NotNull_(fifo);
	Assert_(bufferSize > 0);
	ClearPointer(fifo);
	fifo->allocArena = memArena;
	fifo->bufferSize = bufferSize;
	fifo->used = 0;
	fifo->numLines = 0;
	fifo->firstLine = nullptr;
	fifo->lastLine = nullptr;
	fifo->nextLineNumber = 1;
	
	fifo->buffer = AllocArray(memArena, u8, bufferSize);
	NotNull_(fifo->buffer);
	MyMemSet(&fifo->buffer[0], 0x00, bufferSize);
}

void StringFifoPopLine(StringFifo_t* fifo)
{
	NotNull_(fifo);
	Assert_(fifo->numLines > 0);
	NotNull_(fifo->buffer);
	NotNull_(fifo->firstLine);
	NotNull_(fifo->lastLine);
	
	//TODO: Add a callback to let the code that manages this fifo know that a line is getting popped off
	
	u64 poppedLineSize = GetFifoLineTotalSize(fifo->firstLine);
	
	StringFifoLine_t* secondLine = fifo->firstLine->next;
	fifo->firstLine = secondLine;
	if (secondLine != nullptr)
	{
		secondLine->prev = nullptr;
	}
	fifo->numLines--;
	if (fifo->numLines == 0)
	{
		fifo->lastLine = nullptr;
	}
	Assert_(fifo->used >= poppedLineSize);
	fifo->used -= poppedLineSize;
}

StringFifoLine_t* StringFifoPushLineExt(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
{
	NotNull_(fifo);
	NotNull_(fifo->buffer);
	NotNullStr(&text);
	NotNullStr(&metaString);
	
	StringFifoLine_t dummyLine = {};
	dummyLine.metaStructSize = metaStructSize;
	dummyLine.metaStringLength = metaString.length;
	dummyLine.textLength = text.length;
	u64 allocationSize = GetFifoLineTotalSize(&dummyLine);
	
	if (allocationSize > fifo->bufferSize)
	{
		return nullptr;
	}
	
	StringFifoLine_t* result = nullptr;
	
	bool triedEmptyArena = false;
	while (fifo->numLines > 0 || !triedEmptyArena)
	{
		if (fifo->numLines == 0) { triedEmptyArena = true; }
		DebugAssert_(fifo->used <= fifo->bufferSize);
		
		u64 fifoHeadIndex = GetStringFifoHeadIndex(fifo);
		u64 fifoTailIndex = GetStringFifoTailIndex(fifo);
		
		u64 nextAvailableSize = 0;
		u64 otherAvailableSize = 0;
		if (fifoHeadIndex > fifoTailIndex || (fifoHeadIndex == fifoTailIndex && fifo->numLines == 0))
		{
			//other   T      H   next
			//[-------<######>------]
			nextAvailableSize = fifo->bufferSize - fifoHeadIndex;
			otherAvailableSize = fifoTailIndex;
		}
		else
		{
			//     H  next   T       (no other)
			//[####>---------<######] 
			// (also includes scenario where we are perfectly caught up with our tail)
			nextAvailableSize = fifoTailIndex - fifoHeadIndex;
			otherAvailableSize = 0;
		}
		
		if (nextAvailableSize >= allocationSize)
		{
			result = (StringFifoLine_t*)(&fifo->buffer[fifoHeadIndex]);
			MyMemSet(result, 0x00, allocationSize);
			result->prev = fifo->lastLine;
			if (fifo->lastLine != nullptr) { fifo->lastLine->next = result; }
			else { fifo->firstLine = result; }
			fifo->numLines++;
			fifo->lastLine = result;
			fifo->used += allocationSize;
			Assert_(fifo->used <= fifo->bufferSize);
			break;
		}
		else if (otherAvailableSize >= allocationSize)
		{
			result = (StringFifoLine_t*)(&fifo->buffer[0]);
			MyMemSet(result, 0x00, allocationSize);
			result->prev = fifo->lastLine;
			if (fifo->lastLine != nullptr) { fifo->lastLine->next = result; }
			else { fifo->firstLine = result; }
			fifo->numLines++;
			fifo->lastLine = result;
			fifo->used += allocationSize;
			Assert_(fifo->used <= fifo->bufferSize);
			break;
		}
		else
		{
			if (fifo->numLines > 0) { StringFifoPopLine(fifo); }
		}
	}
	
	if (result != nullptr)
	{
		result->metaStructSize = metaStructSize;
		result->metaStringLength = metaString.length;
		result->textLength = text.length;
		
		result->lineNumber = fifo->nextLineNumber;
		fifo->nextLineNumber++;
		
		//Copy the data into the correct places
		void* metaStructSpace = GetFifoLineMetaStruct_(result, metaStructSize);
		MyStr_t metaStringSpace = GetFifoLineMetaString(result);
		MyStr_t textSpace = GetFifoLineText(result);
		if (metaStructSize > 0)
		{
			MyMemCopy(metaStructSpace, metaStructPntr, metaStructSize);
		}
		if (metaString.length > 0)
		{
			MyMemCopy(metaStringSpace.pntr, metaString.pntr, metaString.length);
			metaStringSpace.pntr[metaString.length] = '\0';
		}
		if (text.length > 0)
		{
			MyMemCopy(textSpace.pntr, text.pntr, text.length);
			textSpace.pntr[text.length] = '\0';
		}
	}
	
	DebugAssert_(fifo->firstLine == nullptr || fifo->firstLine->prev == nullptr);
	DebugAssert_(fifo->lastLine == nullptr || fifo->lastLine->next == nullptr);
	return result;
}
StringFifoLine_t* StringFifoPushLine(StringFifo_t* fifo, MyStr_t text)
{
	return StringFifoPushLineExt(fifo, text, 0, nullptr, MyStr_Empty);
}

#endif //  _GY_STRING_FIFO_H
