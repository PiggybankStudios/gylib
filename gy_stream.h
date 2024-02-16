/*
File:   gy_stream.h
Author: Taylor Robbins
Date:   02\15\2024
Description:
	** A Stream_t is something that produces data for someone to consume
	** This could be something like a file that is open and being read from,
	** or a static buffer that is wrapped with a Stream_t, or a socket
	** that is reading data from a server.
	** Each kind of StreamSource_t source has different capabilities and
	** tradeoffs, and rather than the Stream_t completely abstracting away
	** these tradeoffs, it simply serves as a convenient wrapper around
	** all these functionalities, while also allowing the implementation
	** code to poke inside and ask questions about the source as needed.
	** For example, some streams have a finite size that is known up front,
	** while others have to do a bit of work to determine size, and others
	** still have no idea how much data is going to come through in total.
	** So implementations can choose to force a finite size and reject streams
	** with unknown sizes, or they could run a different code path for undefined
	** size streams vs finite streams. i.e. they can ask what kind of stream
	** they have and make decisions based on that.
*/

#ifndef _GY_STREAM_H
#define _GY_STREAM_H

// +--------------------------------------------------------------+
// |                      Enums and Structs                       |
// +--------------------------------------------------------------+
enum StreamSource_t
{
	StreamSource_None = 0,
	StreamSource_Buffer,
	StreamSource_EntireFile,
	StreamSource_OpenFile,
	StreamSource_Socket,
	StreamSource_Other,
	StreamSource_NumSources,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetStreamSourceStr(StreamSource_t enumValue);
#else
const char* GetStreamSourceStr(StreamSource_t enumValue)
{
	switch (enumValue)
	{
		case StreamSource_None:       return "None";
		case StreamSource_Buffer:     return "Buffer";
		case StreamSource_EntireFile: return "EntireFile";
		case StreamSource_OpenFile:   return "OpenFile";
		case StreamSource_Socket:     return "Socket";
		case StreamSource_Other:      return "Other";
		default: return "Unknown";
	}
}
#endif

enum StreamCapability_t
{
	StreamCapability_None         = 0x00,
	
	StreamCapability_FiniteSize   = 0x01, //The size can be known
	StreamCapability_GivenSize    = 0x02, //The size is known, and takes no extra cost to know
	StreamCapability_Backtracking = 0x04, //We can just back to an earlier point in the stream
	StreamCapability_Writable     = 0x08, //The pointers returned by read functions point to writable memory
	
	StreamCapability_All          = 0x0F,
	StreamCapability_NumCapabilities = 4,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetStreamCapabilityStr(StreamCapability_t enumValue);
#else
const char* GetStreamCapabilityStr(StreamCapability_t enumValue)
{
	switch (enumValue)
	{
		case StreamCapability_None:         return "None";
		case StreamCapability_FiniteSize:   return "FiniteSize";
		case StreamCapability_GivenSize:    return "GivenSize";
		case StreamCapability_Backtracking: return "Backtracking";
		case StreamCapability_Writable:     return "Writable";
		case StreamCapability_All:          return "All";
		default: return "Unknown";
	}
}
#endif

struct Stream_t;
#define STREAM_FREE_CALLBACK_DEF(functionName) void functionName(Stream_t* stream)
typedef STREAM_FREE_CALLBACK_DEF(StreamFreeCallback_f);
#define STREAM_GET_SIZE_CALLBACK_DEF(functionName) u64 functionName(Stream_t* stream)
typedef STREAM_GET_SIZE_CALLBACK_DEF(StreamGetSizeCallback_f);
#define STREAM_READ_BUFFER_CALLBACK_DEF(functionName) u64 functionName(Stream_t* stream, u64 numBytes, void* bufferPntr)
typedef STREAM_READ_BUFFER_CALLBACK_DEF(StreamReadBufferCallback_f);
#define STREAM_READ_ALLOC_CALLBACK_DEF(functionName) u64 functionName(Stream_t* stream, u64 numBytes, MemArena_t* memArena, void** outputPntr)
typedef STREAM_READ_ALLOC_CALLBACK_DEF(StreamReadAllocCallback_f);
#define STREAM_READ_STATIC_CALLBACK_DEF(functionName) u64 functionName(Stream_t* stream, u64 numBytes, void** outputPntr)
typedef STREAM_READ_STATIC_CALLBACK_DEF(StreamReadStaticCallback_f);
#define STREAM_MOVE_CALLBACK_DEF(functionName) void functionName(Stream_t* stream, i64 offset)
typedef STREAM_MOVE_CALLBACK_DEF(StreamMoveCallback_f);

struct StreamCallbacks_t
{
	StreamFreeCallback_f* Free;
	StreamGetSizeCallback_f* GetSize;
	StreamReadBufferCallback_f* ReadBuffer;
	StreamReadAllocCallback_f* ReadAlloc;
	StreamReadStaticCallback_f* ReadStatic;
	StreamMoveCallback_f* Move;
};

struct Stream_t
{
	MemArena_t* allocArena;
	StreamSource_t source;
	u16 capabilities; //StreamCapability_t
	StreamCallbacks_t callbacks;
	
	MyStr_t filePath;
	void* mainPntr;
	void* otherPntr;
	
	u64 readIndex;
	u64 numBytesRead;
	bool isTotalSizeFilled;
	u64 totalSize;
};

// +--------------------------------------------------------------+
// |              StreamSource_Buffer Implementation              |
// +--------------------------------------------------------------+
// +========================================+
// | StreamSourceBuffer_ReadBufferCallback  |
// +========================================+
// u64 StreamSourceBuffer_ReadBufferCallback(Stream_t* stream, u64 numBytes, void* bufferPntr)
STREAM_READ_BUFFER_CALLBACK_DEF(StreamSourceBuffer_ReadBufferCallback)
{
	NotNull(stream);
	u64 numBytesToRead = MinU64(numBytes, stream->totalSize - stream->readIndex);
	if (numBytesToRead > 0)
	{
		NotNull(bufferPntr);
		MyMemCopy(bufferPntr, ((u8*)stream->mainPntr) + stream->readIndex, numBytesToRead);
		stream->readIndex += numBytesToRead;
	}
	return numBytesToRead;
}
// +======================================+
// | StreamSourceBuffer_ReadAllocCallback |
// +======================================+
// u64 StreamSourceBuffer_ReadAllocCallback(Stream_t* stream, u64 numBytes, MemArena_t* memArena, void** outputPntr)
STREAM_READ_ALLOC_CALLBACK_DEF(StreamSourceBuffer_ReadAllocCallback)
{
	NotNull(stream);
	u64 numBytesToRead = MinU64(numBytes, stream->totalSize - stream->readIndex);
	if (numBytesToRead > 0)
	{
		NotNull(memArena);
		NotNull(outputPntr);
		u8* result = AllocArray(memArena, u8, numBytesToRead);
		NotNull(result);
		MyMemCopy(result, ((u8*)stream->mainPntr) + stream->readIndex, numBytesToRead);
		stream->readIndex += numBytesToRead;
		*outputPntr = result;
	}
	return numBytesToRead;
}
// +========================================+
// | StreamSourceBuffer_ReadStaticCallback  |
// +========================================+
// u64 StreamSourceBuffer_ReadStaticCallback(Stream_t* stream, u64 numBytes, void** outputPntr)
STREAM_READ_STATIC_CALLBACK_DEF(StreamSourceBuffer_ReadStaticCallback)
{
	NotNull(stream);
	u64 numBytesToRead = MinU64(numBytes, stream->totalSize - stream->readIndex);
	SetOptionalOutPntr(outputPntr, (void*)(((u8*)stream->mainPntr) + stream->readIndex));
	stream->readIndex += numBytesToRead;
	return numBytesToRead;
}
// +==================================+
// | StreamSourceBuffer_MoveCallback  |
// +==================================+
// void StreamSourceBuffer_MoveCallback(Stream_t* stream, i64 offset)
STREAM_MOVE_CALLBACK_DEF(StreamSourceBuffer_MoveCallback)
{
	NotNull(stream);
	AssertIf(offset < 0, stream->readIndex >= (u64)AbsI64(offset));
	AssertIf(offset > 0, stream->readIndex + (u64)offset <= stream->totalSize);
	if (offset > 0) { stream->readIndex += (u64)offset; }
	else { stream->readIndex -= (u64)AbsI64(offset); }
}

// +--------------------------------------------------------------+
// |                         Free and New                         |
// +--------------------------------------------------------------+
void FreeStream(Stream_t* stream)
{
	NotNull(stream);
	if (stream->callbacks.Free != nullptr)
	{
		stream->callbacks.Free(stream);
	}
	if (!IsEmptyStr(stream->filePath))
	{
		NotNull(stream->allocArena);
		FreeString(stream->allocArena, &stream->filePath);
	}
	ClearPointer(stream);
}

Stream_t NewStream(StreamSource_t source, u16 capabilities, const StreamCallbacks_t* callbacks, MemArena_t* memArena, MyStr_t filePath)
{
	AssertIf(!IsEmptyStr(filePath), memArena != nullptr);
	Stream_t result = {};
	result.source = source;
	result.capabilities = capabilities;
	MyMemCopy(&result.callbacks, &callbacks, sizeof(StreamCallbacks_t));
	result.allocArena = memArena;
	if (result.allocArena != nullptr && !IsEmptyStr(filePath))
	{
		result.filePath = AllocString(result.allocArena, &filePath);
	}
	else { result.filePath = MyStr_Empty; }
	return result;
}

Stream_t NewBufferStream(const void* bufferPntr, u64 bufferSize, bool isWritable)
{
	StreamCallbacks_t callbacks = {};
	callbacks.Free = nullptr;
	callbacks.ReadBuffer = StreamSourceBuffer_ReadBufferCallback;
	callbacks.ReadAlloc = StreamSourceBuffer_ReadAllocCallback;
	callbacks.ReadStatic = StreamSourceBuffer_ReadStaticCallback;
	callbacks.GetSize = nullptr;
	callbacks.Move = StreamSourceBuffer_MoveCallback;
	
	u16 streamCaps = (
		StreamCapability_FiniteSize |
		StreamCapability_GivenSize |
		StreamCapability_Backtracking |
		(isWritable ? StreamCapability_Writable : 0)
	);
	
	Stream_t result = NewStream(StreamSource_Buffer, streamCaps, &callbacks, nullptr, MyStr_Empty);
	result.mainPntr = (void*)bufferPntr;
	result.totalSize = bufferSize;
	result.isTotalSizeFilled = true;
	result.numBytesRead = result.totalSize;
	return result;
}

Stream_t NewMyStrStream(MyStr_t bufferStr, bool isWritable)
{
	return NewBufferStream(bufferStr.chars, bufferStr.length, isWritable);
}

// +--------------------------------------------------------------+
// |                        Info Functions                        |
// +--------------------------------------------------------------+
bool StreamIsFinite(Stream_t* stream)
{
	return IsFlagSet(stream->capabilities, StreamCapability_FiniteSize);
}
bool StreamIsGivenSize(Stream_t* stream)
{
	return IsFlagSet(stream->capabilities, StreamCapability_GivenSize);
}
bool StreamCanBacktrack(Stream_t* stream)
{
	return IsFlagSet(stream->capabilities, StreamCapability_Backtracking);
}
bool StreamIsWritable(Stream_t* stream)
{
	return IsFlagSet(stream->capabilities, StreamCapability_Writable);
}

u64 StreamGetSize(Stream_t* stream)
{
	NotNull(stream);
	Assert(IsFlagSet(stream->capabilities, StreamCapability_FiniteSize));
	if (stream->isTotalSizeFilled) { return stream->totalSize; }
	Assert(stream->callbacks.GetSize != nullptr);
	stream->totalSize = stream->callbacks.GetSize(stream);
	stream->isTotalSizeFilled = true;
	return stream->totalSize;
}

u64 StreamGetRemainingSize(Stream_t* stream)
{
	NotNull(stream);
	Assert(IsFlagSet(stream->capabilities, StreamCapability_FiniteSize));
	if (stream->isTotalSizeFilled) { return stream->totalSize - stream->readIndex; }
	Assert(stream->callbacks.GetSize != nullptr);
	stream->totalSize = stream->callbacks.GetSize(stream);
	stream->isTotalSizeFilled = true;
	Assert(stream->readIndex <= stream->totalSize);
	return stream->totalSize - stream->readIndex;
}

bool StreamIsOver(Stream_t* stream)
{
	NotNull(stream);
	if (stream->source == StreamSource_None) { return true; }
	if (!stream->isTotalSizeFilled && IsFlagSet(stream->capabilities, StreamCapability_FiniteSize) && stream->callbacks.GetSize != nullptr)
	{
		stream->totalSize = stream->callbacks.GetSize(stream);
		stream->isTotalSizeFilled = true;
	}
	if (stream->isTotalSizeFilled && stream->readIndex >= stream->totalSize) { return true; }
	return false;
}

// +--------------------------------------------------------------+
// |                        Read Functions                        |
// +--------------------------------------------------------------+
void* StreamRead(Stream_t* stream, u64 numBytes)
{
	NotNull(stream);
	Assert(stream->callbacks.ReadStatic != nullptr);
	void* result = nullptr;
	u64 numBytesRead = stream->callbacks.ReadStatic(stream, numBytes, &result);
	Assert(numBytesRead == numBytes);
	AssertIf(numBytesRead > 0, result != nullptr);
	return result;
}
void* StreamRead(Stream_t* stream, u64 numBytes, u64* numBytesReadOut)
{
	NotNull(stream);
	Assert(stream->callbacks.ReadStatic != nullptr);
	void* result = nullptr;
	u64 numBytesRead = stream->callbacks.ReadStatic(stream, numBytes, &result);
	AssertIf(numBytesRead > 0, result != nullptr);
	SetOptionalOutPntr(numBytesReadOut, numBytesRead);
	return result;
}
u64 StreamReadInto(Stream_t* stream, u64 bufferSize, void* bufferPntr)
{
	NotNull(stream);
	Assert(stream->callbacks.ReadBuffer != nullptr);
	return stream->callbacks.ReadBuffer(stream, bufferSize, bufferPntr);
}
void* StreamReadInArena(Stream_t* stream, u64 numBytes, MemArena_t* memArena)
{
	NotNull2(stream, memArena);
	Assert(stream->callbacks.ReadAlloc != nullptr);
	void* result = nullptr;
	u64 numBytesRead = stream->callbacks.ReadAlloc(stream, numBytes, memArena, &result);
	Assert(numBytesRead == numBytes);
	AssertIf(numBytesRead > 0, result != nullptr);
	return result;
}
void* StreamReadInArena(Stream_t* stream, u64 numBytes, MemArena_t* memArena, u64* numBytesReadOut)
{
	NotNull2(stream, memArena);
	Assert(stream->callbacks.ReadAlloc != nullptr);
	void* result = nullptr;
	u64 numBytesRead = stream->callbacks.ReadAlloc(stream, numBytes, memArena, &result);
	AssertIf(numBytesRead > 0, result != nullptr);
	SetOptionalOutPntr(numBytesReadOut, numBytesRead);
	return result;
}
void* StreamReadRemaining(Stream_t* stream, u64* numBytesReadOut)
{
	NotNull(stream);
	return StreamRead(stream, StreamGetRemainingSize(stream), numBytesReadOut);
}
u64 StreamReadRemainingInto(Stream_t* stream, u64 bufferSize, void* bufferPntr)
{
	NotNull(stream);
	u64 numBytesRemaining = StreamGetRemainingSize(stream);
	Assert(bufferSize >= numBytesRemaining);
	return StreamReadInto(stream, numBytesRemaining, bufferPntr);
}
void* StreamReadRemainingInArena(Stream_t* stream, MemArena_t* memArena, u64* numBytesReadOut)
{
	NotNull(stream);
	return StreamReadInArena(stream, StreamGetRemainingSize(stream), memArena, numBytesReadOut);
}

// +--------------------------------------------------------------+
// |                        Move Functions                        |
// +--------------------------------------------------------------+
void StreamSkip(Stream_t* stream, u64 amount, bool allowLess = true)
{
	NotNull(stream);
	if (allowLess && stream->isTotalSizeFilled)
	{
		amount = MinU64(amount, stream->totalSize - stream->readIndex);
	}
	if (stream->callbacks.Move)
	{
		stream->callbacks.Move(stream, (i64)amount);
	}
	else
	{
		u64 numBytesRead = StreamReadInto(stream, amount, nullptr);
		AssertIf(!allowLess, numBytesRead == amount);
	}
}

void StreamReset(Stream_t* stream)
{
	NotNull(stream);
	if (stream->readIndex == 0) { return; }
	Assert(IsFlagSet(stream->capabilities, StreamCapability_Backtracking));
	Assert(stream->callbacks.Move != nullptr);
	stream->callbacks.Move(stream, -(i64)stream->readIndex);
	DebugAssert(stream->readIndex == 0);
}

void StreamMoveToEnd(Stream_t* stream)
{
	NotNull(stream);
	Assert(IsFlagSet(stream->capabilities, StreamCapability_FiniteSize));
	if (!stream->isTotalSizeFilled)
	{
		Assert(stream->callbacks.GetSize != nullptr);
		stream->totalSize = stream->callbacks.GetSize(stream);
		stream->isTotalSizeFilled = true;
	}
	if (stream->readIndex >= stream->totalSize) { return; }
	Assert(stream->callbacks.Move != nullptr);
	stream->callbacks.Move(stream, (i64)(stream->totalSize - stream->readIndex));
}

void StreamMove(Stream_t* stream, i64 amount)
{
	NotNull(stream);
	Assert(stream->callbacks.Move != nullptr);
	stream->callbacks.Move(stream, amount);
}

void StreamMoveBack(Stream_t* stream, u64 amount)
{
	NotNull(stream);
	if (stream->readIndex == 0) { return; }
	Assert(IsFlagSet(stream->capabilities, StreamCapability_Backtracking));
	Assert(stream->callbacks.Move != nullptr);
	stream->callbacks.Move(stream, -(i64)amount);
	DebugAssert(stream->readIndex == 0);
}

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
StreamSource_None
StreamSource_Buffer
StreamSource_EntireFile
StreamSource_OpenFile
StreamSource_Socket
StreamSource_Other
StreamSource_NumSources
StreamCapability_None
StreamCapability_FiniteSize
StreamCapability_GivenSize
StreamCapability_Backtracking
StreamCapability_Writable
StreamCapability_All
StreamCapability_NumCapabilities
@Types
StreamSource_t
StreamCapability_t
StreamFreeCallback_f
StreamGetSizeCallback_f
StreamReadBufferCallback_f
StreamReadAllocCallback_f
StreamReadStaticCallback_f
StreamMoveCallback_f
StreamCallbacks_t
Stream_t
@Functions
const char* GetStreamSourceStr(StreamSource_t enumValue)
const char* GetStreamCapabilityStr(StreamCapability_t enumValue)
void STREAM_FREE_CALLBACK_DEF(Stream_t* stream)
u64 STREAM_GET_SIZE_CALLBACK_DEF(Stream_t* stream)
u64 STREAM_READ_BUFFER_CALLBACK_DEF(Stream_t* stream, u64 numBytes, void* bufferPntr)
u64 STREAM_READ_ALLOC_CALLBACK_DEF(Stream_t* stream, u64 numBytes, MemArena_t* memArena, void** outputPntr)
u64 STREAM_READ_STATIC_CALLBACK_DEF(Stream_t* stream, u64 numBytes, void** outputPntr)
void STREAM_MOVE_CALLBACK_DEF(Stream_t* stream, i64 offset)
void StreamSourceBuffer_FreeCallback(Stream_t* stream)
u64 StreamSourceBuffer_GetSizeCallback(Stream_t* stream)
u64 StreamSourceBuffer_ReadBufferCallback(Stream_t* stream, u64 numBytes, void* bufferPntr)
u64 StreamSourceBuffer_ReadAllocCallback(Stream_t* stream, u64 numBytes, MemArena_t* memArena, void** outputPntr)
u64 StreamSourceBuffer_ReadStaticCallback(Stream_t* stream, u64 numBytes, void** outputPntr)
void StreamSourceBuffer_MoveCallback(Stream_t* stream, i64 offset)
void FreeStream(Stream_t* stream)
Stream_t NewStream(StreamSource_t source, u16 capabilities, const StreamCallbacks_t* callbacks, MemArena_t* memArena, MyStr_t filePath)
Stream_t NewBufferStream(const void* bufferPntr, u64 bufferSize, bool isWritable)
Stream_t NewMyStrStream(MyStr_t bufferStr, bool isWritable)
bool StreamIsFinite(Stream_t* stream)
bool StreamIsGivenSize(Stream_t* stream)
bool StreamCanBacktrack(Stream_t* stream)
bool StreamIsWritable(Stream_t* stream)
u64 StreamGetSize(Stream_t* stream)
u64 StreamGetRemainingSize(Stream_t* stream)
bool StreamIsOver(Stream_t* stream)
void* StreamRead(Stream_t* stream, u64 numBytes, u64* numBytesReadOut = nullptr)
u64 StreamReadInto(Stream_t* stream, u64 bufferSize, void* bufferPntr)
void* StreamReadInArena(Stream_t* stream, u64 numBytes, MemArena_t* memArena, u64* numBytesReadOut = nullptr)
void* StreamReadRemaining(Stream_t* stream, u64* numBytesReadOut)
u64 StreamReadRemainingInto(Stream_t* stream, u64 bufferSize, void* bufferPntr)
void* StreamReadRemainingInArena(Stream_t* stream, MemArena_t* memArena, u64* numBytesReadOut)
void StreamSkip(Stream_t* stream, u64 amount, bool allowLess = true)
void StreamReset(Stream_t* stream)
void StreamMoveToEnd(Stream_t* stream)
void StreamMove(Stream_t* stream, i64 amount)
void StreamMoveBack(Stream_t* stream, u64 amount)
*/

#endif //  _GY_STREAM_H