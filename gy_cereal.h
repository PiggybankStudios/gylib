/*
File:   gy_cereal.h
Author: Taylor Robbins
Date:   02\17\2024
Description: 
	** The "Cereal Engine" is a system by which we serialize and deserialize various
	** types to binary file formats in a backwards compatible way (using simple versioning),
	** and with compositional structure (types can exist in multiple file formats)
NOTE: Crl stands for Cereal
*/

//TODO: Pass scratch arena to Deserialize function for each task
//TODO: Remove array count embedding, let the application handle storing array counts somewhere. Or maybe just give them an option? We would need a callback or something to do a bit of work when the array count is read for the first time
//TODO: Struct sizes should be auto-injected into the stream, minimum size for structures should be part of registration
//TODO: Split PushTask functions for Serialization and Deserialization
//TODO: Add support for GYLIB_HEADER_ONLY
//TODO: Add support for context values that get deallocated automatically
//TODO: How do we fill in information from later serialization processes, earlier in the file? Like having file offsets written to a header?
//TODO: Test all the error scenarios to make sure they are working properly (Write some unit tests maybe?)

#ifndef _GY_CEREAL_H
#define _GY_CEREAL_H

#if GYLIB_SCRATCH_ARENA_AVAILABLE

#define CRL_TYPE_DESIGNATION_SIZE 4 //chars
#define CRL_CONTEXT_MAX_VALUE_SIZE 8 //bytes

struct CrlVersion_t
{
	u8 major;
	u8 minor;
};

enum CrlError_t
{
	CrlError_None = 0,
	CrlError_IncompleteData,
	CrlError_InvalidDesignation,
	CrlError_EmptySerialization,
	CrlError_NumErrors,
};
const char* GetCrlErrorStr(CrlError_t enumValue)
{
	switch (enumValue)
	{
		case CrlError_None:               return "None";
		case CrlError_IncompleteData:     return "IncompleteData";
		case CrlError_InvalidDesignation: return "InvalidDesignation";
		case CrlError_EmptySerialization: return "EmptySerialization";
		default: return "Unknown";
	}
}

struct CrlEngine_t;
#define CRL_SERIALIZE_DEF(functionName) bool functionName(CrlEngine_t* crl, const void* inputPntr, u64 inputSize, u64 arrayIndex, void* outputPntr, u64 outputSize, u64* writeIndexPntr)
typedef CRL_SERIALIZE_DEF(CrlSerialize_f);
//TODO: Ideally the CerealEngine can handle struct sizes and pass that in, so the majority of reading (and reading failures) happen in Cereal Engine
// a.k.a. We should inject struct sizes into the stream, read them, and use that to determine deserialized struct sizes
#define CRL_DESERIALIZE_DEF(functionName) bool functionName(CrlEngine_t* crl, const void* inputPntr, u64 inputSize, u64 arrayIndex)
typedef CRL_DESERIALIZE_DEF(CrlDeserialize_f);

struct CrlRegisteredTypeVersion_t
{
	CrlVersion_t version;
	char designation[CRL_TYPE_DESIGNATION_SIZE];
	u64 serializedSize;
	CrlSerialize_f* Serialize;
	CrlDeserialize_f* Deserialize;
	#if DEBUG_BUILD
	MyStr_t debugName;
	#endif
};

struct CrlRegisteredType_t
{
	u64 index;
	VarArray_t versions; //CrlRegisteredTypeVersion_t
};

struct CrlContextEntry_t
{
	u64 index;
	bool keepForSecondPass;
	bool isFilled;
	u64 size;
	union
	{
		u8 valueBytes[CRL_CONTEXT_MAX_VALUE_SIZE];
		void* pntr;
		i8 valueI8;
		i16 valueI16;
		i32 valueI32;
		i64 valueI64;
		u8 valueU8;
		u16 valueU16;
		u32 valueU32;
		u64 valueU64;
		r32 valueR32;
		r64 valueR64;
		bool valueBool;
	};
};

struct CrlTask_t
{
	bool isArray;
	CrlRegisteredType_t* type;
	CrlRegisteredTypeVersion_t* typeVersion;
	
	bool started;
	u64 startFileOffset;
	u64 arraySize;
	bool isRuntimeVarArray;
	u64 runtimeItemSize;
	const void* runtimeItemPntr;
	u64 progressIndex;
};

struct CrlEngine_t
{
	MemArena_t* allocArena;
	MemArena_t* serializedOutputArena;
	MemArena_t* deserOutputArena;
	bool isDeserializing;
	CrlVersion_t version;
	
	//serializing
	u64 outputSize;
	u8* outputPntr;
	u64 writeIndex;
	
	//deserializing
	Stream_t* inputStream;
	ProcessLog_t* log;
	
	u64 numRegisteredTypes;
	u64 numRegisteredTypesAlloc;
	CrlRegisteredType_t* registeredTypes;
	
	u64 numContextEntries;
	CrlContextEntry_t* contextEntries;
	
	VarArray_t taskStack; //CrlTask_t
};

#define CrlVersion_Zero       NewCrlVersion(0, 0)
#define CrlVersion_Zero_Const { 0, 0 }
#define CrlVersion_Max        NewCrlVersion(255, 255)
#define CrlVersion_Max_Const  { 255, 255 }

// +--------------------------------------------------------------+
// |                      Version Functions                       |
// +--------------------------------------------------------------+
CrlVersion_t NewCrlVersion(u8 major, u8 minor)
{
	CrlVersion_t result;
	result.major = major;
	result.minor = minor;
	return result;
}
bool IsCrlVersionGreaterThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
{
	if (left.major > right.major) { return true; }
	else if (left.major == right.major)
	{
		if (left.minor > right.minor) { return true; }
		else if (allowEqual && left.minor == right.minor) { return true; }
		else { return false; }
	}
	else { return false; }
}
bool IsCrlVersionLessThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
{
	if (left.major < right.major) { return true; }
	else if (left.major == right.major)
	{
		if (left.minor < right.minor) { return true; }
		else if (allowEqual && left.minor == right.minor) { return true; }
		else { return false; }
	}
	else { return false; }
}
bool IsCrlVersionEqual(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
{
	return (left.major == right.major && left.minor == right.minor);
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
MyStr_t CrlGetDebugStackString(CrlEngine_t* crl, MemArena_t* memArena)
{
	NotNull(crl);
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	VarArrayLoop(&crl->taskStack, tIndex)
	{
		VarArrayLoopGet(CrlTask_t, task, &crl->taskStack, tIndex);
		if (tIndex > 0) { StringBuilderAppend(&builder, " > "); }
		#if DEBUG_BUILD
		StringBuilderAppend(&builder, task->typeVersion->debugName);
		#else
		// void StringBuilderAppendPrint(StringBuilder_t* builder, const char* formatString, ...)
		StringBuilderAppendPrint(&builder, "%llu", task->type->index);
		#endif
		if (task->isArray)
		{
			StringBuilderAppendPrint(&builder, "[%llu/%llu]", task->progressIndex, task->arraySize);
		}
	}
	return ToMyStr(&builder);
}

// +--------------------------------------------------------------+
// |                       Free and Create                        |
// +--------------------------------------------------------------+
void FreeCrlEngine(CrlEngine_t* crl)
{
	NotNull(crl);
	if (crl->registeredTypes != nullptr)
	{
		NotNull(crl->allocArena);
		for (u64 tIndex = 0; tIndex < crl->numRegisteredTypes; tIndex++)
		{
			CrlRegisteredType_t* registeredType = &crl->registeredTypes[tIndex];
			FreeVarArray(&registeredType->versions);
		}
		FreeMem(crl->allocArena, crl->registeredTypes, sizeof(CrlRegisteredType_t) * crl->numRegisteredTypesAlloc);
	}
	if (crl->contextEntries != nullptr)
	{
		NotNull(crl->allocArena);
		FreeMem(crl->allocArena, crl->contextEntries, sizeof(CrlContextEntry_t) * crl->numContextEntries);
	}
	if (crl->outputPntr != nullptr)
	{
		MemArena_t* outputArena = ((crl->serializedOutputArena != nullptr) ? crl->serializedOutputArena : crl->allocArena);
		NotNull(outputArena);
		FreeMem(outputArena, crl->outputPntr, crl->outputSize);
	}
	FreeVarArray(&crl->taskStack);
	ClearPointer(crl);
}

void CreateCrlEngine(CrlEngine_t* crl, bool deserializing, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
{
	NotNull(crl);
	AssertIf(deserializing, processLog != nullptr);
	AssertIf(deserializing, stream != nullptr);
	ClearPointer(crl);
	crl->allocArena = memArena;
	crl->version = version;
	crl->log = processLog;
	crl->inputStream = stream;
	crl->isDeserializing = deserializing;
	crl->numRegisteredTypesAlloc = numTypes;
	if (numTypes > 0)
	{
		crl->registeredTypes = AllocArray(crl->allocArena, CrlRegisteredType_t, crl->numRegisteredTypesAlloc);
		NotNull(crl->registeredTypes);
	}
	crl->numContextEntries = numContextEntries;
	if (crl->numContextEntries > 0)
	{
		crl->contextEntries = AllocArray(crl->allocArena, CrlContextEntry_t, crl->numContextEntries);
		NotNull(crl->contextEntries);
		for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
		{
			CrlContextEntry_t* entry = &crl->contextEntries[cIndex];
			ClearPointer(entry);
			entry->index = cIndex;
		}
	}
	CreateVarArray(&crl->taskStack, crl->allocArena, sizeof(CrlTask_t));
}
//We need to know which arena is being used, so we don't share scratch arenas, but CrlEngine generally won't do anything with deserOutputArena directly. Each task can use it however it wants
void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* deserOutputArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
{
	NotNull2(processLog, stream);
	CreateCrlEngine(crl, true, version, memArena, numTypes, numContextEntries, processLog, stream);
	crl->deserOutputArena = deserOutputArena;
}
void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* serializedOutputArena, u64 numTypes, u64 numContextEntries)
{
	CreateCrlEngine(crl, false, version, memArena, numTypes, numContextEntries, nullptr, nullptr);
	crl->serializedOutputArena = serializedOutputArena;
}

// +--------------------------------------------------------------+
// |                         Registration                         |
// +--------------------------------------------------------------+
CrlRegisteredType_t* CrlGetType(CrlEngine_t* crl, u64 index)
{
	Assert(index < crl->numRegisteredTypes);
	return &crl->registeredTypes[index];
}
CrlRegisteredTypeVersion_t* CrlGetTypeVersion(CrlEngine_t* crl, u64 index, CrlVersion_t version, bool allowLowerVersions = true)
{
	CrlRegisteredType_t* type = CrlGetType(crl, index);
	CrlRegisteredTypeVersion_t* result = nullptr;
	VarArrayLoop(&type->versions, vIndex)
	{
		VarArrayLoopGet(CrlRegisteredTypeVersion_t, versionPntr, &type->versions, vIndex);
		if (IsCrlVersionEqual(versionPntr->version, version)) { return versionPntr; }
		else if (allowLowerVersions && IsCrlVersionLessThan(versionPntr->version, version)) { result = versionPntr; }
	}
	return result;
}

//The index must increase by 1 with each registration, OR stay the same and the version must then increase by some amount
CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, const char* debugName, u64 index, MyStr_t designation, CrlVersion_t version, u64 serializedSize, CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc)
{
	Assert(crl->numRegisteredTypes < crl->numRegisteredTypesAlloc);
	Assert(index >= crl->numRegisteredTypes || index == crl->numRegisteredTypes-1);
	NotNull2(serializeFunc, deserializeFunc);
	Assert(designation.length == CRL_TYPE_DESIGNATION_SIZE || designation.length == 0);
	Assert(serializedSize > 0);
	
	CrlRegisteredType_t* type = nullptr;
	if (index == crl->numRegisteredTypes-1)
	{
		CrlRegisteredType_t* existingType = &crl->registeredTypes[index];
		#if ASSERTIONS_ENABLED
		if (existingType->versions.length > 0)
		{
			CrlRegisteredTypeVersion_t* prevVersion = VarArrayGetHard(&existingType->versions, existingType->versions.length-1, CrlRegisteredTypeVersion_t);
			Assert(IsCrlVersionGreaterThan(version, prevVersion->version));
		}
		#endif
		type = existingType;
	}
	else
	{
		while (crl->numRegisteredTypes < index)
		{
			ClearPointer(&crl->registeredTypes[crl->numRegisteredTypes]);
			CreateVarArray(&crl->registeredTypes[crl->numRegisteredTypes].versions, crl->allocArena, sizeof(CrlRegisteredTypeVersion_t));
			crl->numRegisteredTypes++;
		}
		
		CrlRegisteredType_t* newType = &crl->registeredTypes[crl->numRegisteredTypes];
		crl->numRegisteredTypes++;
		ClearPointer(newType);
		CreateVarArray(&newType->versions, crl->allocArena, sizeof(CrlRegisteredTypeVersion_t), 1);
		type = newType;
	}
	
	CrlRegisteredTypeVersion_t* newVersion = VarArrayAdd(&type->versions, CrlRegisteredTypeVersion_t);
	NotNull(newVersion);
	ClearPointer(newVersion);
	newVersion->version = version;
	if (designation.length == CRL_TYPE_DESIGNATION_SIZE) { MyMemCopy(&newVersion->designation, designation.chars, CRL_TYPE_DESIGNATION_SIZE); }
	else { newVersion->designation[0] = '\0'; }
	newVersion->serializedSize = serializedSize;
	newVersion->Serialize = serializeFunc;
	newVersion->Deserialize = deserializeFunc;
	
	#if DEBUG_BUILD
	newVersion->debugName = NewStr(debugName);
	#endif
	
	return type;
}

// +--------------------------------------------------------------+
// |                           Context                            |
// +--------------------------------------------------------------+
void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false, bool keepForSecondPass = false)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	AssertIf(pntr != nullptr, size > 0);
	AssertIf(pntr == nullptr, keepForSecondPass == false);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (pntr != nullptr)
	{
		AssertIf(!allowOverwrite, !entry->isFilled);
		entry->size = size;
		entry->pntr = pntr;
		entry->keepForSecondPass = keepForSecondPass;
		entry->isFilled = true;
	}
	else
	{
		AssertIf(!allowOverwrite, entry->isFilled);
		entry->size = 0;
		entry->pntr = nullptr;
		entry->keepForSecondPass = false;
		entry->isFilled = false;
	}
}
void CrlPushContextValue_(CrlEngine_t* crl, u64 index, u64 valueSize, const void* valuePntr, bool allowOverwrite = false, bool keepForSecondPass = false)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	AssertIf(valuePntr != nullptr, valueSize > 0);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	Assert(valueSize <= CRL_CONTEXT_MAX_VALUE_SIZE);
	AssertIf(valuePntr == nullptr, keepForSecondPass == false);
	if (valuePntr != nullptr)
	{
		AssertIf(!allowOverwrite, !entry->isFilled);
		entry->size = valueSize;
		MyMemCopy(&entry->valueBytes[0], valuePntr, valueSize);
		entry->keepForSecondPass = keepForSecondPass;
		entry->isFilled = true;
	}
	else
	{
		AssertIf(!allowOverwrite, entry->isFilled);
		entry->size = 0;
		MyMemSet(&entry->valueBytes[0], 0x00, CRL_CONTEXT_MAX_VALUE_SIZE);
		entry->keepForSecondPass = false;
		entry->isFilled = false;
	}
}
#define CrlPushContext(crl, index, typedPntr, ...) CrlPushContext_((crl), (index), sizeof(*(typedPntr)), (void*)(typedPntr), ##__VA_ARGS__)
#define CrlPushContextI8(crl, index, value, ...)   do { i8   _valueI8   = (value); CrlPushContextValue_((crl), (index), sizeof(i8),   &_valueI8,   ##__VA_ARGS__); } while(0)
#define CrlPushContextI16(crl, index, value, ...)  do { i16  _valueI16  = (value); CrlPushContextValue_((crl), (index), sizeof(i16),  &_valueI16,  ##__VA_ARGS__); } while(0)
#define CrlPushContextI32(crl, index, value, ...)  do { i32  _valueI32  = (value); CrlPushContextValue_((crl), (index), sizeof(i32),  &_valueI32,  ##__VA_ARGS__); } while(0)
#define CrlPushContextI64(crl, index, value, ...)  do { i64  _valueI64  = (value); CrlPushContextValue_((crl), (index), sizeof(i64),  &_valueI64,  ##__VA_ARGS__); } while(0)
#define CrlPushContextU8(crl, index, value, ...)   do { u8   _valueU8   = (value); CrlPushContextValue_((crl), (index), sizeof(u8),   &_valueU8,   ##__VA_ARGS__); } while(0)
#define CrlPushContextU16(crl, index, value, ...)  do { u16  _valueU16  = (value); CrlPushContextValue_((crl), (index), sizeof(u16),  &_valueU16,  ##__VA_ARGS__); } while(0)
#define CrlPushContextU32(crl, index, value, ...)  do { u32  _valueU32  = (value); CrlPushContextValue_((crl), (index), sizeof(u32),  &_valueU32,  ##__VA_ARGS__); } while(0)
#define CrlPushContextU64(crl, index, value, ...)  do { u64  _valueU64  = (value); CrlPushContextValue_((crl), (index), sizeof(u64),  &_valueU64,  ##__VA_ARGS__); } while(0)
#define CrlPushContextR32(crl, index, value, ...)  do { r32  _valueR32  = (value); CrlPushContextValue_((crl), (index), sizeof(r32),  &_valueR32,  ##__VA_ARGS__); } while(0)
#define CrlPushContextR64(crl, index, value, ...)  do { r64  _valueR64  = (value); CrlPushContextValue_((crl), (index), sizeof(r64),  &_valueR64,  ##__VA_ARGS__); } while(0)
#define CrlPushContextBool(crl, index, value, ...) do { bool _valueBool = (value); CrlPushContextValue_((crl), (index), sizeof(bool), &_valueBool, ##__VA_ARGS__); } while(0)

void* CrlGetContext_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->pntr != nullptr)
	{
		Assert(entry->size == size);
		return entry->pntr;
	}
	else if (assertOnFailure)
	{
		AssertMsg(false, "Failed to get CrlEngine context entry!");
	}
	return nullptr;
}
CrlContextEntry_t* CrlGetContextRaw_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->isFilled)
	{
		Assert(entry->size == size);
		return entry;
	}
	else if (assertOnFailure)
	{
		AssertMsg(false, "Failed to get CrlEngine context entry!");
	}
	return nullptr;
}
#define CrlGetContextHard(crl, index, type) (type*)CrlGetContext_((crl), (index), sizeof(type), true)
#define CrlGetContextSoft(crl, index, type) (type*)CrlGetContext_((crl), (index), sizeof(type), false)
#define CrlGetContext(crl, index, type) CrlGetContextHard((crl), (index), type)

#define CrlGetContextI8(crl, index)   CrlGetContextRaw_((crl), (index), sizeof(i8), true)->valueI8
#define CrlGetContextI16(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(i16), true)->valueI16
#define CrlGetContextI32(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(i32), true)->valueI32
#define CrlGetContextI64(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(i64), true)->valueI64
#define CrlGetContextU8(crl, index)   CrlGetContextRaw_((crl), (index), sizeof(u8), true)->valueU8
#define CrlGetContextU16(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(u16), true)->valueU16
#define CrlGetContextU32(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(u32), true)->valueU32
#define CrlGetContextU64(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(u64), true)->valueU64
#define CrlGetContextR32(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(r32), true)->valueR32
#define CrlGetContextR64(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(r64), true)->valueR64
#define CrlGetContextBool(crl, index) CrlGetContextRaw_((crl), (index), sizeof(bool), true)->valueBool

// +--------------------------------------------------------------+
// |                            Tasks                             |
// +--------------------------------------------------------------+
//TODO: runtimeItemPntr and runtimeItemSize are not important for Deserialization.
//      Maybe we should just completely separate Deserializing vs. Serializing PushTask functions?
CrlTask_t* CrlPushTask(CrlEngine_t* crl, bool isArray, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
{
	NotNull(crl);
	if (isArray && arraySize == 0) { return nullptr; } //Pushing a task with no elements is pointless
	CrlRegisteredType_t* type = CrlGetType(crl, typeIndex);
	CrlRegisteredTypeVersion_t* typeVersion = CrlGetTypeVersion(crl, typeIndex, crl->version, true);
	CrlTask_t* newTask = VarArrayAdd(&crl->taskStack, CrlTask_t);
	NotNull(newTask);
	ClearPointer(newTask);
	newTask->isArray = isArray;
	newTask->type = type;
	newTask->typeVersion = typeVersion;
	newTask->runtimeItemPntr = runtimeItemPntr;
	newTask->runtimeItemSize = runtimeItemSize;
	newTask->arraySize = arraySize;
	newTask->isRuntimeVarArray = false;
	newTask->started = false;
	
	return newTask;
}
CrlTask_t* CrlPushArrayTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
{
	return CrlPushTask(crl, true, typeIndex, runtimeItemPntr, runtimeItemSize, arraySize);
}
CrlTask_t* CrlPushVarArrayTask(CrlEngine_t* crl, u64 typeIndex, const VarArray_t* runtimeVarArray, u64 arraySize = 0)
{
	CrlTask_t* task = CrlPushTask(crl, true, typeIndex, runtimeVarArray, runtimeVarArray->itemSize, (crl->isDeserializing ? arraySize : runtimeVarArray->length));
	if (task != nullptr) { task->isRuntimeVarArray = true; }
	return task;
}
CrlTask_t* CrlPushSingleTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize)
{
	return CrlPushTask(crl, false, typeIndex, runtimeItemPntr, runtimeItemSize);
}

// +--------------------------------------------------------------+
// |                             Run                              |
// +--------------------------------------------------------------+
bool CrlEngineRun(CrlEngine_t* crl, u64 firstTaskTypeIndex, const void* firstTaskRuntimeItemPntr, u64 firstTaskRuntimeItemSize)
{
	#define CrlEngine_ReadTyped(type, varPntr, debugName) do                           \
	{                                                                                  \
		u64 _numBytesRead = StreamReadInto(crl->inputStream, sizeof(type), (varPntr)); \
		if (_numBytesRead < sizeof(type))                                              \
		{                                                                              \
			LogPrintLine_E(crl->log,                                                   \
				"Expected %u more bytes for %s before end of file. Found %llu bytes",  \
				sizeof(type),                                                          \
				(debugName),                                                           \
				_numBytesRead                                                          \
			);                                                                         \
			LogExitFailure(crl->log, CrlError_IncompleteData);                         \
			FreeScratchArena(scratch);                                                 \
			return false;                                                              \
		}                                                                              \
	} while(0)
	
	MemArena_t* scratch = GetScratchArena(crl->allocArena, (crl->isDeserializing ? crl->deserOutputArena : crl->serializedOutputArena));
	
	for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
	{
		CrlContextEntry_t* entry = &crl->contextEntries[cIndex];
		if (entry->isFilled) { entry->keepForSecondPass = true; }
	}
	
	u8 numPasses = (crl->isDeserializing ? 1 : 2);
	for (u8 pass = 0; pass < numPasses; pass++)
	{
		crl->writeIndex = 0;
		CrlPushSingleTask(crl, firstTaskTypeIndex, firstTaskRuntimeItemPntr, firstTaskRuntimeItemSize);
		
		while (crl->taskStack.length > 0)
		{
			u64 nextTaskIndex = crl->taskStack.length-1;
			CrlTask_t* nextTask = VarArrayGetHard(&crl->taskStack, nextTaskIndex, CrlTask_t);
			if (!nextTask->started)
			{
				nextTask->startFileOffset = (crl->isDeserializing ? crl->inputStream->readIndex : crl->writeIndex);
				if (nextTask->isArray)
				{
					if (crl->isDeserializing)
					{
						CrlEngine_ReadTyped(u64, &nextTask->arraySize, "Array Length");
					}
					else
					{
						BinSer_WriteU64(crl->outputPntr, crl->outputSize, &crl->writeIndex, nextTask->arraySize);
					}
				}
				nextTask->started = true;
			}
			
			// +==============================+
			// |        Serialization         |
			// +==============================+
			if (!crl->isDeserializing)
			{
				const void* inputPntr = nextTask->runtimeItemPntr;
				if (nextTask->isArray)
				{
					if (nextTask->isRuntimeVarArray)
					{
						const VarArray_t* varArray = (const VarArray_t*)nextTask->runtimeItemPntr;
						inputPntr = VarArrayGet_(varArray, nextTask->progressIndex, nextTask->runtimeItemSize, true);
					}
					else
					{
						inputPntr = (void*)(((u8*)inputPntr) + (nextTask->runtimeItemSize * nextTask->progressIndex));
					}
				}
				
				if (nextTask->typeVersion->designation[0] != '\0')
				{
					BinSer_WriteBytes(crl->outputPntr, crl->outputSize, &crl->writeIndex, CRL_TYPE_DESIGNATION_SIZE, &nextTask->typeVersion->designation[0]);
				}
				
				u64 numTasksBeforeSerialize = crl->taskStack.length;
				if (!nextTask->typeVersion->Serialize(crl, inputPntr, nextTask->runtimeItemSize, nextTask->progressIndex, crl->outputPntr, crl->outputSize, &crl->writeIndex))
				{
					AssertIf(crl->log != nullptr, crl->log->errorCode != 0);
					FreeScratchArena(scratch);
					return false;
				}
				
				//To make it easy on the implementations, we reverse the items pushed onto the stack during
				//Serialize/Deserialize so they will get carried out in the order in which they were pushed
				if (numTasksBeforeSerialize < crl->taskStack.length)
				{
					VarArrayReverse(&crl->taskStack, numTasksBeforeSerialize, crl->taskStack.length);
				}
				
				//Re-acquire the nextTask pntr into the VarArray because Deserialize might have pushed new items into the array, causing a realloc
				nextTask = VarArrayGetHard(&crl->taskStack, nextTaskIndex, CrlTask_t);
				if (nextTask->isArray)
				{
					nextTask->progressIndex++;
					if (nextTask->progressIndex >= nextTask->arraySize)
					{
						VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
					}
				}
				else
				{
					VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
				}
			}
			// +==============================+
			// |       Deserialization        |
			// +==============================+
			else
			{
				PushMemMark(scratch);
				u64 numBytesRead = 0;
				
				{
					MyStr_t taskStackDebugStr = CrlGetDebugStackString(crl, scratch);
				}
				
				if (nextTask->typeVersion->designation[0] != '\0')
				{
					u8* designationBytes = nullptr;
					if (IsFlagSet(crl->inputStream->capabilities, StreamCapability_StaticRead))
					{
						designationBytes = (u8*)StreamRead(crl->inputStream, CRL_TYPE_DESIGNATION_SIZE, &numBytesRead);
					}
					else
					{
						designationBytes = (u8*)StreamReadInArena(crl->inputStream, CRL_TYPE_DESIGNATION_SIZE, scratch, &numBytesRead);
					}
					if (designationBytes == nullptr || numBytesRead < CRL_TYPE_DESIGNATION_SIZE)
					{
						if (designationBytes == nullptr) { LogPrintLine_E(crl->log, "Failed to read %u byte designation from stream (nullptr return)", CRL_TYPE_DESIGNATION_SIZE); }
						else { LogPrintLine_E(crl->log, "Failed to read %u byte designation from stream. Only %llu bytes read", CRL_TYPE_DESIGNATION_SIZE, numBytesRead); }
						LogExitFailure(crl->log, CrlError_IncompleteData);
						PopMemMark(scratch);
						FreeScratchArena(scratch);
						return false;
					}
					if (MyMemCompare(designationBytes, &nextTask->typeVersion->designation[0], CRL_TYPE_DESIGNATION_SIZE) != 0)
					{
						MyStr_t expectedDesignation = NewStr(CRL_TYPE_DESIGNATION_SIZE, (char*)&nextTask->typeVersion->designation[0]);
						MyStr_t readDesignation = NewStr(CRL_TYPE_DESIGNATION_SIZE, (char*)designationBytes);
						LogPrintLine_E(crl->log, "Invalid type designation found in file. Expected \"%.*s\", found \"%.*s\" at offset 0x%X", StrPrint(expectedDesignation), StrPrint(readDesignation), crl->inputStream->readIndex - CRL_TYPE_DESIGNATION_SIZE);
						MyStr_t taskStackDebugStr = CrlGetDebugStackString(crl, scratch);
						LogPrintLine_E(crl->log, "Task Stack: %.*s", StrPrint(taskStackDebugStr));
						LogExitFailure(crl->log, CrlError_InvalidDesignation);
						PopMemMark(scratch);
						FreeScratchArena(scratch);
						return false;
					}
				}
				
				u64 inputSize = nextTask->typeVersion->serializedSize;
				u8* inputBytes = nullptr;
				//TODO: Once Cereal engine is taking care of most of the reading, then we can re-enable this. For now, the implementations are reading things in a special way to handle simple versioning (structs growing in size)
				#if 0
				if (inputSize > 0)
				{
					if (IsFlagSet(crl->inputStream->capabilities, StreamCapability_StaticRead))
					{
						inputBytes = (u8*)StreamRead(crl->inputStream, inputSize, &numBytesRead);
					}
					else
					{
						inputBytes = (u8*)StreamReadInArena(crl->inputStream, inputSize, scratch, &numBytesRead);
					}
					if (inputBytes == nullptr || numBytesRead < inputSize)
					{
						if (inputBytes == nullptr) { LogPrintLine_E(crl->log, "Failed to read %llu byte struct from stream (nullptr return)", inputSize); }
						else { LogPrintLine_E(crl->log, "Failed to read %llu byte struct from stream. Only %llu bytes read", inputSize, numBytesRead); }
						LogExitFailure(crl->log, CrlError_IncompleteData);
						PopMemMark(scratch);
						FreeScratchArena(scratch);
						return false;
					}
				}
				#endif
				
				u64 numTasksBeforeDeserialize = crl->taskStack.length;
				if (!nextTask->typeVersion->Deserialize(crl, inputBytes, inputSize, nextTask->progressIndex))
				{
					Assert(crl->log->errorCode != 0);
					PopMemMark(scratch);
					FreeScratchArena(scratch);
					return false;
				}
				PopMemMark(scratch);
				
				//To make it easy on the implementations, we reverse the items pushed onto the stack during
				//Serialize/Deserialize so they will get carried out in the order in which they were pushed
				if (numTasksBeforeDeserialize < crl->taskStack.length)
				{
					VarArrayReverse(&crl->taskStack, numTasksBeforeDeserialize, crl->taskStack.length);
				}
				
				//Re-acquire the nextTask pntr into the VarArray because Deserialize might have pushed new items into the array, causing a realloc
				nextTask = VarArrayGetHard(&crl->taskStack, nextTaskIndex, CrlTask_t);
				if (nextTask->isArray)
				{
					nextTask->progressIndex++;
					if (nextTask->progressIndex >= nextTask->arraySize)
					{
						VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
					}
				}
				else
				{
					VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
				}
			}
		}
		
		if (!crl->isDeserializing)
		{
			if (pass == 0)
			{
				crl->outputSize = crl->writeIndex;
				if (crl->outputSize == 0)
				{
					if (crl->log != nullptr)
					{
						LogWriteLine_E(crl->log, "Serialization produced no data");
						LogExitFailure(crl->log, CrlError_EmptySerialization);
					}
					FreeScratchArena(scratch);
					return false;
				}
				MemArena_t* outputArena = ((crl->serializedOutputArena != nullptr) ? crl->serializedOutputArena : crl->allocArena);
				crl->outputPntr = AllocArray(outputArena, u8, crl->outputSize);
				NotNull(crl->outputPntr);
				#if DEBUG_BUILD
				MyMemSet(crl->outputPntr, 0xDD, crl->outputSize);
				#endif
				
				for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
				{
					CrlContextEntry_t* entry = &crl->contextEntries[cIndex];
					if (entry->isFilled && !entry->keepForSecondPass)
					{
						MyMemSet(&entry->valueBytes[0], 0x00, CRL_CONTEXT_MAX_VALUE_SIZE);
						entry->isFilled = false;
					}
				}
			}
			else if (pass == 1)
			{
				Assert(crl->writeIndex == crl->outputSize);
			}
		}
	}
	
	FreeScratchArena(scratch);
	if (crl->log != nullptr && !crl->log->hadErrors) { LogExitSuccess(crl->log); }
	return (crl->log != nullptr ? !crl->log->hadErrors : true);
}

// Ownership of this data is passed to the caller at this point
MyStr_t CrlEngineTakeSerializedData(CrlEngine_t* crl)
{
	NotNull(crl);
	Assert(!crl->isDeserializing);
	Assert(crl->outputPntr != nullptr);
	MyStr_t result = NewStr(crl->outputSize, (char*)crl->outputPntr);
	crl->outputPntr = nullptr;
	crl->outputSize = 0;
	return result;
}

#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

#endif //  _GY_CEREAL_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
CRL_TYPE_DESIGNATION_SIZE
CRL_CONTEXT_MAX_VALUE_SIZE
CrlError_None
CrlError_IncompleteData
CrlError_InvalidDesignation
CrlError_EmptySerialization
CrlError_NumErrors
CrlVersion_Zero
CrlVersion_Zero_Const
CrlVersion_Max
CrlVersion_Max_Const
@Types
CrlVersion_t
CrlError_t
CrlSerialize_f
CrlDeserialize_f
CrlRegisteredTypeVersion_t
CrlRegisteredType_t
CrlContextEntry_t
CrlTask_t
CrlEngine_t
@Functions
const char* GetCrlErrorStr(CrlError_t enumValue)
bool CRL_SERIALIZE_DEF(CrlEngine_t* crl, const void* inputPntr, void* outputPntr, u64 outputSize, u64* writeIndexPntr)
bool CRL_DESERIALIZE_DEF(CrlEngine_t* crl, const void* inputPntr, u64 inputSize, u64 arrayIndex)
CrlVersion_t NewCrlVersion(u8 major, u8 minor)
bool IsCrlVersionGreaterThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
bool IsCrlVersionLessThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
bool IsCrlVersionEqual(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
MyStr_t CrlGetDebugStackString(CrlEngine_t* crl, MemArena_t* memArena)
void FreeCrlEngine(CrlEngine_t* crl)
void CreateCrlEngine(CrlEngine_t* crl, bool deserializing, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* deserOutputArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* serializedOutputArena, u64 numTypes, u64 numContextEntries)
CrlRegisteredType_t* CrlGetType(CrlEngine_t* crl, u64 index)
CrlRegisteredTypeVersion_t* CrlGetTypeVersion(CrlEngine_t* crl, u64 index, CrlVersion_t version, bool allowLowerVersions = true)
CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, const char* debugName, u64 index, MyStr_t designation, CrlVersion_t version, u64 serializedSize, CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc)
void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextValue_(CrlEngine_t* crl, u64 index, u64 valueSize, const void* valuePntr, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContext(CrlEngine_t* crl, u64 index, T* typedPntr, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI8(CrlEngine_t* crl, u64 index, i8 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI16(CrlEngine_t* crl, u64 index, i16 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI32(CrlEngine_t* crl, u64 index, i32 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI64(CrlEngine_t* crl, u64 index, i64 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU8(CrlEngine_t* crl, u64 index, u8 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU16(CrlEngine_t* crl, u64 index, u16 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU32(CrlEngine_t* crl, u64 index, u32 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU64(CrlEngine_t* crl, u64 index, u64 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextR32(CrlEngine_t* crl, u64 index, r32 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextR64(CrlEngine_t* crl, u64 index, r64 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextBool(CrlEngine_t* crl, u64 index, bool value, bool allowOverwrite = false, bool keepForSecondPass = false)
void* CrlGetContext_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
CrlContextEntry_t* CrlGetContextRaw_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
T* CrlGetContextHard(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContextSoft(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContext(CrlEngine_t* crl, u64 index, Type T)
i8 CrlGetContextI8(CrlEngine_t* crl, u64 index)
i16 CrlGetContextI16(CrlEngine_t* crl, u64 index)
i32 CrlGetContextI32(CrlEngine_t* crl, u64 index)
i64 CrlGetContextI64(CrlEngine_t* crl, u64 index)
u8 CrlGetContextU8(CrlEngine_t* crl, u64 index)
u16 CrlGetContextU16(CrlEngine_t* crl, u64 index)
u32 CrlGetContextU32(CrlEngine_t* crl, u64 index)
u64 CrlGetContextU64(CrlEngine_t* crl, u64 index)
r32 CrlGetContextR32(CrlEngine_t* crl, u64 index)
r64 CrlGetContextR64(CrlEngine_t* crl, u64 index)
bool CrlGetContextBool(CrlEngine_t* crl, u64 index)
CrlTask_t* CrlPushTask(CrlEngine_t* crl, bool isArray, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
CrlTask_t* CrlPushArrayTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
CrlTask_t* CrlPushVarArrayTask(CrlEngine_t* crl, u64 typeIndex, const VarArray_t* runtimeVarArray, u64 arraySize = 0)
CrlTask_t* CrlPushSingleTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize)
bool CrlEngineRun(CrlEngine_t* crl, u64 firstTaskTypeIndex, const void* firstTaskRuntimeItemPntr, u64 firstTaskRuntimeItemSize)
MyStr_t CrlEngineTakeSerializedData(CrlEngine_t* crl)
*/