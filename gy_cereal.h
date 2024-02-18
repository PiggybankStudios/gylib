/*
File:   gy_cereal.h
Author: Taylor Robbins
Date:   02\17\2024
Description: 
	** The "Cereal Engine" is a system by which we serialize and deserialize various
	** types to binary file formats in a backwards compatible way (using simple versioning),
	** and with compositional structure (types can exist in multiple file formats)
*/

#ifndef _GY_CEREAL_H
#define _GY_CEREAL_H

#if GYLIB_SCRATCH_ARENA_AVAILABLE
//NOTE: Crl stands for Cereal

#define CRL_TYPE_DESIGNATION_SIZE 4 //chars

struct CrlVersion_t
{
	u8 major;
	u8 minor;
};

struct CrlEngine_t;
#define CRL_SERIALIZE_DEF(functionName) bool functionName(CrlEngine_t* crl, const void* inputPntr, void* outputPntr, u64 outputSize, u64* writeIndexPntr)
typedef CRL_SERIALIZE_DEF(CrlSerialize_f);
#define CRL_DESERIALIZE_DEF(functionName) bool functionName(CrlEngine_t* crl, const void* inputPntr, u64 inputSize)
typedef CRL_DESERIALIZE_DEF(CrlDeserialize_f);

struct CrlRegisteredTypeVersion_t
{
	CrlVersion_t version;
	char designation[CRL_TYPE_DESIGNATION_SIZE];
	u64 serializedSize;
	CrlSerialize_f* Serialize;
	CrlDeserialize_f* Deserialize;
};

struct CrlRegisteredType_t
{
	u64 index;
	VarArray_t versions; //CrlRegisteredTypeVersion_t
};

struct CrlContextEntry_t
{
	u64 index;
	u64 size;
	void* pntr;
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
			entry->index = cIndex;
			entry->size = 0;
			entry->pntr = nullptr;
		}
	}
	CreateVarArray(&crl->taskStack, crl->allocArena, sizeof(CrlTask_t));
}
void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
{
	NotNull2(processLog, stream);
	CreateCrlEngine(crl, true, version, memArena, numTypes, numContextEntries, processLog, stream);
}
void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries)
{
	CreateCrlEngine(crl, false, version, memArena, numTypes, numContextEntries, nullptr, nullptr);
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
CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, u64 index, MyStr_t designation, CrlVersion_t version, u64 serializedSize, CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc)
{
	Assert(crl->numRegisteredTypes < crl->numRegisteredTypesAlloc);
	Assert(index == crl->numRegisteredTypes || index == crl->numRegisteredTypes-1);
	NotNull2(serializeFunc, deserializeFunc);
	Assert(designation.length == CRL_TYPE_DESIGNATION_SIZE);
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
	MyMemCopy(&newVersion->designation, designation.chars, CRL_TYPE_DESIGNATION_SIZE);
	newVersion->serializedSize = serializedSize;
	newVersion->Serialize = serializeFunc;
	newVersion->Deserialize = deserializeFunc;
	
	return type;
}

// +--------------------------------------------------------------+
// |                           Context                            |
// +--------------------------------------------------------------+
void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	AssertIf(pntr != nullptr, size > 0);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (pntr != nullptr)
	{
		AssertIf(!allowOverwrite, entry->pntr == nullptr);
		entry->size = size;
		entry->pntr = pntr;
	}
	else
	{
		AssertIf(!allowOverwrite, entry->pntr != nullptr);
		entry->size = 0;
		entry->pntr = nullptr;
	}
}
#define CrlPushContext(crl, index, typedPntr, ...) CrlPushContext_((crl), (index), sizeof(*(typedPntr)), (typedPntr), ##__VA_ARGS__)

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
#define CrlGetContextHard(crl, index, type) (type*)CrlGetContext_((crl), (index), sizeof(type), true)
#define CrlGetContextSoft(crl, index, type) (type*)CrlGetContext_((crl), (index), sizeof(type), false)
#define CrlGetContext(crl, index, type) CrlGetContextHard((crl), (index), type)

// +--------------------------------------------------------------+
// |                            Tasks                             |
// +--------------------------------------------------------------+
//TODO: runtimeItemPntr and runtimeItemSize are not important for Deserialization.
//      Maybe we should just completely separate Deserializing vs. Serializing PushTask functions?
CrlTask_t* CrlPushTask(CrlEngine_t* crl, bool isArray, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
{
	NotNull(crl);
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
	task->isRuntimeVarArray = true;
	return task;
}
CrlTask_t* CrlPushSingleTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize)
{
	return CrlPushTask(crl, false, typeIndex, runtimeItemPntr, runtimeItemSize);
}

// +--------------------------------------------------------------+
// |                             Run                              |
// +--------------------------------------------------------------+
bool CrlEngineRun(CrlEngine_t* crl)
{
	MemArena_t* scratch = GetScratchArena(crl->allocArena);
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
			LogExitFailure(crl->log, 1); /*TODO: Make a real error code?*/             \
			FreeScratchArena(scratch);                                                 \
			return false;                                                              \
		}                                                                              \
	} while(0)
	
	u64 originalTaskStackSize = 0;
	CrlTask_t* originalTaskStack = nullptr;
	if (!crl->isDeserializing && crl->taskStack.length > 0)
	{
		originalTaskStackSize = crl->taskStack.length;
		originalTaskStack = AllocArray(scratch, CrlTask_t, originalTaskStackSize);
		NotNull(originalTaskStack);
		MyMemCopy(originalTaskStack, crl->taskStack.items, sizeof(CrlTask_t) * originalTaskStackSize);
	}
	
	u8 numPasses = (crl->isDeserializing ? 1 : 2);
	for (u8 pass = 0; pass < numPasses; pass++)
	{
		crl->writeIndex = 0;
		if (crl->isDeserializing && pass == 2 && originalTaskStackSize > 0)
		{
			CrlTask_t* newTasks = VarArrayAddRange(&crl->taskStack, 0, originalTaskStackSize, CrlTask_t);
			MyMemCopy(newTasks, originalTaskStack, originalTaskStackSize);
		}
		
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
				
				BinSer_WriteBytes(crl->outputPntr, crl->outputSize, &crl->writeIndex, CRL_TYPE_DESIGNATION_SIZE, &nextTask->typeVersion->designation[0]);
				
				u64 numTasksBeforeSerialize = crl->taskStack.length;
				if (!nextTask->typeVersion->Serialize(crl, inputPntr, crl->outputPntr, crl->outputSize, &crl->writeIndex))
				{
					Assert(crl->log->errorCode != 0);
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
						VarArrayPop(&crl->taskStack, CrlTask_t);
					}
				}
				else
				{
					VarArrayPop(&crl->taskStack, CrlTask_t);
				}
			}
			// +==============================+
			// |       Deserialization        |
			// +==============================+
			else
			{
				PushMemMark(scratch);
				
				u64 numBytesRead = 0;
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
					LogExitFailure(crl->log, 4); //TODO: Add an actual error code!
					PopMemMark(scratch);
					FreeScratchArena(scratch);
					return false;
				}
				if (MyMemCompare(designationBytes, &nextTask->typeVersion->designation[0], CRL_TYPE_DESIGNATION_SIZE) != 0)
				{
					MyStr_t expectedDesignation = NewStr(CRL_TYPE_DESIGNATION_SIZE, (char*)&nextTask->typeVersion->designation[0]);
					MyStr_t readDesignation = NewStr(CRL_TYPE_DESIGNATION_SIZE, (char*)designationBytes);
					LogPrintLine_E(crl->log, "Invalid type designation found in file. Expected \"%.*s\", found \"%.*s\"", StrPrint(expectedDesignation), StrPrint(readDesignation));
					LogExitFailure(crl->log, 5); //TODO: Add an actual error code!
					PopMemMark(scratch);
					FreeScratchArena(scratch);
					return false;
				}
				
				u8* inputBytes = nullptr;
				if (IsFlagSet(crl->inputStream->capabilities, StreamCapability_StaticRead))
				{
					inputBytes = (u8*)StreamRead(crl->inputStream, nextTask->typeVersion->serializedSize, &numBytesRead);
				}
				else
				{
					inputBytes = (u8*)StreamReadInArena(crl->inputStream, nextTask->typeVersion->serializedSize, scratch, &numBytesRead);
				}
				if (inputBytes == nullptr || numBytesRead < nextTask->typeVersion->serializedSize)
				{
					if (inputBytes == nullptr) { LogPrintLine_E(crl->log, "Failed to read %llu byte struct from stream (nullptr return)", nextTask->typeVersion->serializedSize); }
					else { LogPrintLine_E(crl->log, "Failed to read %llu byte struct from stream. Only %llu bytes read", nextTask->typeVersion->serializedSize, numBytesRead); }
					LogExitFailure(crl->log, 3); //TODO: Add an actual error code!
					PopMemMark(scratch);
					FreeScratchArena(scratch);
					return false;
				}
				
				u64 numTasksBeforeDeserialize = crl->taskStack.length;
				if (!nextTask->typeVersion->Deserialize(crl, inputBytes, nextTask->typeVersion->serializedSize))
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
						VarArrayPop(&crl->taskStack, CrlTask_t);
					}
				}
				else
				{
					VarArrayPop(&crl->taskStack, CrlTask_t);
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
					LogWriteLine_E(crl->log, "Serialization produced no data");
					LogExitFailure(crl->log, 2); //TODO: Add an actual error code
					FreeScratchArena(scratch);
					return false;
				}
				crl->outputPntr = AllocArray(crl->allocArena, u8, crl->outputSize);
				NotNull(crl->outputPntr);
				#if DEBUG_BUILD
				MyMemSet(crl->outputPntr, 0xDD, crl->outputSize);
				#endif
			}
			else if (pass == 1)
			{
				Assert(crl->writeIndex == crl->outputSize);
			}
		}
	}
	
	FreeScratchArena(scratch);
	if (!crl->log->hadErrors) { LogExitSuccess(crl->log); }
	return !crl->log->hadErrors;
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
CrlVersion_Zero
CrlVersion_Zero_Const
CrlVersion_Max
CrlVersion_Max_Const
@Types
CrlVersion_t
CrlSerialize_f
CrlDeserialize_f
CrlRegisteredTypeVersion_t
CrlRegisteredType_t
CrlContextEntry_t
CrlTask_t
CrlEngine_t
@Functions
bool CRL_SERIALIZE_DEF(CrlEngine_t* crl, const void* inputPntr, void* outputPntr, u64 outputSize, u64* writeIndexPntr)
bool CRL_DESERIALIZE_DEF(CrlEngine_t* crl, const void* inputPntr, u64 inputSize)
CrlVersion_t NewCrlVersion(u8 major, u8 minor)
bool IsCrlVersionGreaterThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
bool IsCrlVersionLessThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
bool IsCrlVersionEqual(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
void FreeCrlEngine(CrlEngine_t* crl)
void CreateCrlEngine(CrlEngine_t* crl, bool deserializing, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries)
CrlRegisteredType_t* CrlGetType(CrlEngine_t* crl, u64 index)
CrlRegisteredTypeVersion_t* CrlGetTypeVersion(CrlEngine_t* crl, u64 index, CrlVersion_t version, bool allowLowerVersions = true)
CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, u64 index, MyStr_t designation, CrlVersion_t version, u64 serializedSize, CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc)
void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false)
void CrlPushContext(CrlEngine_t* crl, u64 index, T* typedPntr, bool allowOverwrite = false)
void* CrlGetContext_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
T* CrlGetContextHard(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContextSoft(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContext(CrlEngine_t* crl, u64 index, Type T)
CrlTask_t* CrlPushTask(CrlEngine_t* crl, bool isArray, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
CrlTask_t* CrlPushArrayTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize = 0)
CrlTask_t* CrlPushVarArrayTask(CrlEngine_t* crl, u64 typeIndex, const VarArray_t* runtimeVarArray, u64 arraySize = 0)
CrlTask_t* CrlPushSingleTask(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize)
bool CrlEngineRun(CrlEngine_t* crl)
MyStr_t CrlEngineTakeSerializedData(CrlEngine_t* crl)
*/