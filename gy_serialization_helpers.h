/*
File:   gy_serialization_helpers.h
Author: Taylor Robbins
Date:   07\23\2023
Description:
	** Holds functions and macros that help us serialize and deserialize things to binary data
	** Often these are helper functions to facilitate a 2-pass approach to serialization (one pass to size, allocate, then another pass to fill)
*/

#ifndef _GY_SERIALIZATION_HELPERS_H
#define _GY_SERIALIZATION_HELPERS_H

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define BinSer_WriteStructure(result, resultSize, byteIndexPntr, type) (type*)BinSer_WriteStructure_((result), (resultSize), (byteIndexPntr), sizeof(type))

#define BinSer_WriteValue(result, resultSize, byteIndexPntr, type, value) do                           \
{                                                                                                      \
	type localVariableForValue = (value);                                                              \
	BinSer_WriteValue_((result), (resultSize), (byteIndexPntr), sizeof(type), &localVariableForValue); \
} while(0)

#define BinSer_WriteU8(result, resultSize, byteIndexPntr, value)  BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u8,  (value))
#define BinSer_WriteU16(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u16, (value))
#define BinSer_WriteU32(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u32, (value))
#define BinSer_WriteU64(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u64, (value))
#define BinSer_WriteI8(result, resultSize, byteIndexPntr, value)  BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i8,  (value))
#define BinSer_WriteI16(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i16, (value))
#define BinSer_WriteI32(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i32, (value))
#define BinSer_WriteI64(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i64, (value))
#define BinSer_WriteBytes(result, resultSize, byteIndexPntr, numBytes, bytesPntr) BinSer_WriteValue_((result), (resultSize), (byteIndexPntr), (numBytes), (bytesPntr))

#define BinDeser_ReadStruct(dataPntr, dataSize, byteIndexPntr, type) (type*)BinDeser_ReadStruct_((dataPntr), (dataSize), (byteIndexPntr), sizeof(type))
#define BinDeser_ReadBytes(dataPntr, dataSize, byteIndexPntr, numBytes) (u8*)BinDeser_ReadStruct_((dataPntr), (dataSize), (byteIndexPntr), (numBytes))

#define BinDeser_ReadStructDynamicSize(dataPntr, dataSize, byteIndexPntr, structSizeOut, type, sizeMemberName, firstOptionalMemberName) (type*)BinDeser_ReadStructDynamicSize_((dataPntr), (dataSize), (byteIndexPntr), (structSizeOut), STRUCT_VAR_OFFSET(type, sizeMemberName), STRUCT_VAR_OFFSET(type, firstOptionalMemberName), sizeof(type))

#define BinDeser_IsMemberPresent(structSize, type, memberName) (structSize >= (STRUCT_VAR_OFFSET(type, memberName) + sizeof(((type*)0)->memberName)))

#define BinDeser_ReadU8(dataPntr, dataSize, byteIndexPntr, variableOut)  BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u8),  (void*)(variableOut))
#define BinDeser_ReadU16(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u16), (void*)(variableOut))
#define BinDeser_ReadU32(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u32), (void*)(variableOut))
#define BinDeser_ReadU64(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u64), (void*)(variableOut))
#define BinDeser_ReadI8(dataPntr, dataSize, byteIndexPntr, variableOut)  BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i8),  (void*)(variableOut))
#define BinDeser_ReadI16(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i16), (void*)(variableOut))
#define BinDeser_ReadI32(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i32), (void*)(variableOut))
#define BinDeser_ReadI64(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i64), (void*)(variableOut))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void* BinSer_WriteStructure_(void* result, u64 resultSize, u64* byteIndexPntr, u64 structureSize);
	void BinSer_WriteValue_(void* result, u64 resultSize, u64* byteIndexPntr, u64 valueSize, const void* valuePntr);
	const void* BinDeser_ReadStruct_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 structureSize);
	const void* BinDeser_ReadStructDynamicSize_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize);
	bool BinDeser_ReadVariable_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 valueSize, void* valueOutPntr);
#else

// +--------------------------------------------------------------+
// |                 Binary Serialization Headers                 |
// +--------------------------------------------------------------+
void* BinSer_WriteStructure_(void* result, u64 resultSize, u64* byteIndexPntr, u64 structureSize)
{
	NotNull(byteIndexPntr);
	AssertIf(result != nullptr, (*byteIndexPntr) + structureSize <= resultSize);
	void* structPntr = (result != nullptr) ? (((u8*)result) + (*byteIndexPntr)) : nullptr;
	*byteIndexPntr = (*byteIndexPntr) + structureSize;
	return structPntr;
}

void BinSer_WriteValue_(void* result, u64 resultSize, u64* byteIndexPntr, u64 valueSize, const void* valuePntr)
{
	NotNull2(byteIndexPntr, valuePntr);
	if (result != nullptr)
	{
		Assert((*byteIndexPntr) + valueSize <= resultSize);
		void* valueWritePntr = ((u8*)result) + (*byteIndexPntr);
		MyMemCopy(valueWritePntr, valuePntr, valueSize);
	}
	*byteIndexPntr = (*byteIndexPntr) + valueSize;
}

// +--------------------------------------------------------------+
// |                Binary Deserialization Helpers                |
// +--------------------------------------------------------------+
const void* BinDeser_ReadStruct_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 structureSize)
{
	NotNull2(dataPntr, byteIndexPntr);
	if ((*byteIndexPntr) + structureSize > dataSize) { return nullptr; }
	const void* result = ((u8*)dataPntr) + (*byteIndexPntr);
	*byteIndexPntr = (*byteIndexPntr) + structureSize;
	return result;
}

const void* BinDeser_ReadStructDynamicSize_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize)
{
	NotNull2(dataPntr, byteIndexPntr);
	Assert (sizeMemberOffset + sizeof(u64) <= minStructSize);
	if ((*byteIndexPntr) + minStructSize > dataSize) { return nullptr; }
	const u64* sizePntr = (const u64*)(((u8*)dataPntr) + (*byteIndexPntr) + sizeMemberOffset);
	if (*sizePntr < minStructSize) { return nullptr; }
	if (*sizePntr > maxStructSize) { return nullptr; }
	if (structSizeOut != nullptr) { *structSizeOut = *sizePntr; }
	const void* result = ((u8*)dataPntr) + (*byteIndexPntr);
	*byteIndexPntr = (*byteIndexPntr) + (*sizePntr);
	return result;
}

bool BinDeser_ReadVariable_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 valueSize, void* valueOutPntr)
{
	NotNull3(dataPntr, byteIndexPntr, valueOutPntr);
	if ((*byteIndexPntr) + valueSize > dataSize) { return false; }
	const void* valueReadPntr = ((u8*)dataPntr) + (*byteIndexPntr);
	MyMemCopy(valueOutPntr, valueReadPntr, valueSize);
	*byteIndexPntr = (*byteIndexPntr) + valueSize;
	return true;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_SERIALIZATION_HELPERS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Functions
void* BinSer_WriteStructure_(void* result, u64 resultSize, u64* byteIndexPntr, u64 structureSize)
#define BinSer_WriteStructure(result, resultSize, byteIndexPntr, type)
void BinSer_WriteValue_(void* result, u64 resultSize, u64* byteIndexPntr, u64 valueSize, const void* valuePntr)
#define BinSer_WriteValue(result, resultSize, byteIndexPntr, type, value)
#define BinSer_WriteU8(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteU16(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteU32(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteU64(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI8(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI16(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI32(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI64(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteBytes(result, resultSize, byteIndexPntr, numBytes, bytesPntr)
const void* BinDeser_ReadStruct_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 structureSize)
#define BinDeser_ReadStruct(dataPntr, dataSize, byteIndexPntr, type)
#define BinDeser_ReadBytes(dataPntr, dataSize, byteIndexPntr, numBytes)
const void* BinDeser_ReadStructDynamicSize_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize)
#define BinDeser_ReadStructDynamicSize(dataPntr, dataSize, byteIndexPntr, structSizeOut, type, sizeMemberName, firstOptionalMemberName)
#define BinDeser_IsMemberPresent(structSize, type, memberName)
bool BinDeser_ReadVariable_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 valueSize, void* valueOutPntr)
#define BinDeser_ReadU8(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadU16(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadU32(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadU64(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI8(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI16(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI32(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI64(dataPntr, dataSize, byteIndexPntr, variableOut)
*/
