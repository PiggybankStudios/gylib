/*
File:   gy_string_buffer.h
Author: Taylor Robbins
Date:   05\08\2023
Description:
	** A string buffer is a fixed size array that sits on the stack and facilitates common string
	** operations (like print, concat, replace, etc.) within a context where the string length
	** is guaranteed to always be below some max number of bytes.
StringBuffer_t is always guaranteed to be null-terminated so you can easily do ToStr on a pointer to a string buffer
*/

//TODO: We could probably do a similar thing without using templates. All we need to do is make a macro for creation that declares a stack array and also a structure that points to that array and knows it's size

#ifndef _GY_STRING_BUFFER_H
#define _GY_STRING_BUFFER_H

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define TEMP_STRING_LENGTH 256 //bytes

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
// In order to facilitate implementations without templatizing everything, we have
// this generic version of StringBuffer_t that can be auto-casted to as a pointer
struct StringBufferGeneric_t
{
	uxx length;
	uxx bufferSize;
	char chars[1];
};

template<int LENGTH>
struct StringBuffer_t
{
	uxx length;
	uxx bufferSize;
	union
	{
		char chars[LENGTH];
		u8 bytes[LENGTH];
	};
	
	operator StringBufferGeneric_t*()
	{
		return (StringBufferGeneric_t*)this;
	}
	operator MyStr_t()
	{
		return NewStr(this->length, &this->chars[0]);
	}
};

typedef StringBuffer_t<TEMP_STRING_LENGTH> TempString_t;

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	template<int LENGTH> void InitStringBuffer(StringBuffer_t<LENGTH>* stringBufferPntr);
	template<int LENGTH> void InitStringBuffer(StringBuffer_t<LENGTH>* stringBufferPntr, MyStr_t initialValue);
	template<int LENGTH> void InitStringBuffer(StringBuffer_t<LENGTH>* stringBufferPntr, const char* initialValueNullTerm);
	bool IsInitialized(const StringBufferGeneric_t* stringBuffer);
	bool IsNullTerminated(StringBufferGeneric_t* stringBuffer);
	void StringBufferSet(StringBufferGeneric_t* stringBuffer, MyStr_t str);
	void StringBufferSet(StringBufferGeneric_t* stringBuffer, const char* nullTermStr);
	bool StringBufferTrySet(StringBufferGeneric_t* stringBuffer, MyStr_t str, bool appendWhatYouCan = true);
	bool StringBufferTrySet(StringBufferGeneric_t* stringBuffer, const char* nullTermStr);
	void StringBufferClear(StringBufferGeneric_t* stringBuffer, bool zeroMemory = false);
	template<int LENGTH> StringBufferGeneric_t* ToGeneric(StringBuffer_t<LENGTH>* stringBuffer);
	template<int LENGTH> StringBuffer_t<LENGTH>* ToStringBuffer(StringBufferGeneric_t* stringBuffer);
	TempString_t* ToTempString(StringBufferGeneric_t* stringBuffer);
	MyStr_t ToMyStr(StringBufferGeneric_t* stringBuffer);
	char* ToStr(StringBufferGeneric_t* stringBuffer);
	const char* ToStr(const StringBufferGeneric_t* stringBuffer);
	MyStr_t StringBufferSubstring(StringBufferGeneric_t* stringBuffer, uxx startIndex, uxx endIndex);
	MyStr_t StringBufferSubstring(StringBufferGeneric_t* stringBuffer, uxx startIndex);
	MyStr_t StringBufferSubstringFromEnd(StringBufferGeneric_t* stringBuffer, uxx startIndexFromEnd);
	void StringBufferPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...);
	bool StringBufferTryPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...);
	void StringBufferAppendPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...);
	bool StringBufferTryAppendPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...);
	void StringBufferAppend(StringBufferGeneric_t* stringBuffer, MyStr_t str);
	void StringBufferAppend(StringBufferGeneric_t* stringBuffer, const char* nullTermStr);
	bool StringBufferTryAppend(StringBufferGeneric_t* stringBuffer, MyStr_t str, bool appendWhatYouCan = true);
	bool StringBufferTryAppend(StringBufferGeneric_t* stringBuffer, const char* nullTermStr, bool appendWhatYouCan = true);
	uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, MyStr_t targetStr, MyStr_t replacementStr, bool ignoreCase = false);
	uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, const char* targetNullTermStr, MyStr_t replacementStr, bool ignoreCase = false);
	uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, MyStr_t targetStr, const char* replacementNullTermStr, bool ignoreCase = false);
	uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, const char* targetNullTermStr, const char* replacementNullTermStr, bool ignoreCase = false);
#else

// +--------------------------------------------------------------+
// |                        Initialization                        |
// +--------------------------------------------------------------+
template<int LENGTH>
void InitStringBuffer(StringBuffer_t<LENGTH>* stringBufferPntr)
{
	NotNull(stringBufferPntr);
	stringBufferPntr->bufferSize = LENGTH;
	stringBufferPntr->length = 0;
	stringBufferPntr->chars[0] = '\0';
}
template<int LENGTH>
void InitStringBuffer(StringBuffer_t<LENGTH>* stringBufferPntr, MyStr_t initialValue)
{
	NotNull(stringBufferPntr);
	Assert(initialValue.length < LENGTH); //not equal to because we need space for null-term char
	stringBufferPntr->bufferSize = LENGTH;
	stringBufferPntr->length = initialValue.length;
	MyMemCopy(&stringBufferPntr->chars[0], initialValue.chars, initialValue.length);
	stringBufferPntr->chars[initialValue.length] = '\0';
}
template<int LENGTH>
void InitStringBuffer(StringBuffer_t<LENGTH>* stringBufferPntr, const char* initialValueNullTerm)
{
	NotNull(stringBufferPntr);
	InitStringBuffer(stringBufferPntr, NewStr(initialValueNullTerm));
}

bool IsInitialized(const StringBufferGeneric_t* stringBuffer)
{
	NotNull(stringBuffer);
	return (stringBuffer->bufferSize > 0);
}
bool IsNullTerminated(StringBufferGeneric_t* stringBuffer)
{
	Assert(stringBuffer->length < stringBuffer->bufferSize);
	return (stringBuffer->chars[stringBuffer->length] == '\0');
}

void StringBufferSet(StringBufferGeneric_t* stringBuffer, MyStr_t str)
{
	Assert(IsInitialized(stringBuffer));
	Assert(str.length < stringBuffer->bufferSize); //not equal to because we need space for null-term char
	if (str.length > 0) { MyMemCopy(&stringBuffer->chars[0], str.chars, str.length); }
	stringBuffer->length = str.length;
	stringBuffer->chars[str.length] = '\0';
}
void StringBufferSet(StringBufferGeneric_t* stringBuffer, const char* nullTermStr)
{
	StringBufferSet(stringBuffer, NewStr(nullTermStr));
}

bool StringBufferTrySet(StringBufferGeneric_t* stringBuffer, MyStr_t str, bool appendWhatYouCan = true)
{
	Assert(IsInitialized(stringBuffer));
	if (str.length >= stringBuffer->bufferSize) //equal to is also a failure because we need space for null-term char
	{
		if (appendWhatYouCan && str.length > 0 && stringBuffer->length < stringBuffer->bufferSize-1)
		{
			MyMemCopy(&stringBuffer->chars[0], str.chars, stringBuffer->bufferSize-1);
			stringBuffer->length = stringBuffer->bufferSize-1;
			stringBuffer->chars[stringBuffer->length] = '\0';
		}
		return false;
	}
	if (str.length > 0) { MyMemCopy(&stringBuffer->chars[0], str.chars, str.length); }
	stringBuffer->length = str.length;
	stringBuffer->chars[str.length] = '\0';
	return true;
}
bool StringBufferTrySet(StringBufferGeneric_t* stringBuffer, const char* nullTermStr)
{
	return StringBufferTrySet(stringBuffer, NewStr(nullTermStr));
}

void StringBufferClear(StringBufferGeneric_t* stringBuffer, bool zeroMemory = false)
{
	StringBufferSet(stringBuffer, MyStr_Empty);
	if (zeroMemory) { MyMemSet(&stringBuffer->chars[0], 0x00, stringBuffer->bufferSize); }
}

// +--------------------------------------------------------------+
// |                     Conversion Functions                     |
// +--------------------------------------------------------------+
template<int LENGTH>
StringBufferGeneric_t* ToGeneric(StringBuffer_t<LENGTH>* stringBuffer)
{
	return (StringBufferGeneric_t*)stringBuffer;
}
template<int LENGTH>
StringBuffer_t<LENGTH>* ToStringBuffer(StringBufferGeneric_t* stringBuffer)
{
	Assert(stringBuffer == nullptr || stringBuffer->bufferSize == LENGTH);
	return (StringBuffer_t<LENGTH>*)stringBuffer;
}
TempString_t* ToTempString(StringBufferGeneric_t* stringBuffer)
{
	Assert(stringBuffer == nullptr || stringBuffer->bufferSize == TEMP_STRING_LENGTH);
	return (TempString_t*)stringBuffer;
}

MyStr_t ToMyStr(StringBufferGeneric_t* stringBuffer)
{
	Assert(IsInitialized(stringBuffer));
	return NewStr(stringBuffer->length, &stringBuffer->chars[0]);
}

char* ToStr(StringBufferGeneric_t* stringBuffer)
{
	Assert(IsInitialized(stringBuffer));
	DebugAssert(stringBuffer->length < stringBuffer->bufferSize);
	DebugAssert(stringBuffer->chars[stringBuffer->length] == '\0');
	return &stringBuffer->chars[0];
}
const char* ToStr(const StringBufferGeneric_t* stringBuffer) //const variant
{
	return (const char*)ToStr((StringBufferGeneric_t*)stringBuffer);
}

// +--------------------------------------------------------------+
// |                        Get Functions                         |
// +--------------------------------------------------------------+
MyStr_t StringBufferSubstring(StringBufferGeneric_t* stringBuffer, uxx startIndex, uxx endIndex)
{
	Assert(IsInitialized(stringBuffer));
	Assert(startIndex <= stringBuffer->length);
	Assert(endIndex <= stringBuffer->length);
	Assert(startIndex <= endIndex);
	return NewStr(endIndex - startIndex, &stringBuffer->chars[startIndex]);
}
MyStr_t StringBufferSubstring(StringBufferGeneric_t* stringBuffer, uxx startIndex)
{
	Assert(IsInitialized(stringBuffer));
	Assert(startIndex <= stringBuffer->length);
	return NewStr(stringBuffer->length - startIndex, &stringBuffer->chars[startIndex]);
}
MyStr_t StringBufferSubstringFromEnd(StringBufferGeneric_t* stringBuffer, uxx startIndexFromEnd)
{
	Assert(IsInitialized(stringBuffer));
	Assert(startIndexFromEnd <= stringBuffer->length);
	return NewStr(startIndexFromEnd, &stringBuffer->chars[stringBuffer->length - startIndexFromEnd]);
}

//TODO: Add StringBufferFindSubstring
//TODO: Add StringBufferCountSubstring

// +--------------------------------------------------------------+
// |                    Manipulation Functions                    |
// +--------------------------------------------------------------+
void StringBufferPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
{
	Assert(IsInitialized(stringBuffer));
	va_list args;
	va_start(args, formatString);
	int printResult = MyVaListPrintf(&stringBuffer->chars[0], (int)stringBuffer->bufferSize, formatString, args);
	va_end(args);
	Assert(printResult >= 0);
	Assert((uxx)printResult < stringBuffer->bufferSize);
	stringBuffer->length = (uxx)printResult;
	stringBuffer->chars[stringBuffer->length] = '\0';
}
//returns false if the print fails or if it doesn't fit in the buffer (will still put the portion that fits into the buffer)
bool StringBufferTryPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
{
	Assert(IsInitialized(stringBuffer));
	va_list args;
	va_start(args, formatString);
	int printResult = MyVaListPrintf(&stringBuffer->chars[0], (int)stringBuffer->bufferSize, formatString, args);
	va_end(args);
	if (printResult < 0) { DebugAssert(IsNullTerminated(stringBuffer)); return false; }
	if ((uxx)printResult >= stringBuffer->bufferSize) { stringBuffer->length = stringBuffer->bufferSize-1; DebugAssert(IsNullTerminated(stringBuffer)); return false; }
	stringBuffer->length = (uxx)printResult;
	stringBuffer->chars[stringBuffer->length] = '\0';
	return true;
}

void StringBufferAppendPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
{
	Assert(IsInitialized(stringBuffer));
	va_list args;
	va_start(args, formatString);
	int printResult = MyVaListPrintf(&stringBuffer->chars[stringBuffer->length], (int)(stringBuffer->bufferSize - stringBuffer->length), formatString, args);
	va_end(args);
	Assert(printResult >= 0);
	Assert((uxx)printResult < stringBuffer->bufferSize - stringBuffer->length);
	stringBuffer->length += (uxx)printResult;
	stringBuffer->chars[stringBuffer->length] = '\0';
}
//returns false if the print fails or if it doesn't fit in the buffer (will still put the portion that fits into the buffer)
bool StringBufferTryAppendPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
{
	Assert(IsInitialized(stringBuffer));
	va_list args;
	va_start(args, formatString);
	int printResult = MyVaListPrintf(&stringBuffer->chars[stringBuffer->length], (int)(stringBuffer->bufferSize - stringBuffer->length), formatString, args);
	va_end(args);
	if (printResult < 0) { DebugAssert(IsNullTerminated(stringBuffer)); return false; }
	if ((uxx)printResult >= stringBuffer->bufferSize - stringBuffer->length) { stringBuffer->length = stringBuffer->bufferSize-1; DebugAssert(IsNullTerminated(stringBuffer)); return false; }
	stringBuffer->length += (uxx)printResult;
	stringBuffer->chars[stringBuffer->length] = '\0';
	return true;
}

void StringBufferAppend(StringBufferGeneric_t* stringBuffer, MyStr_t str)
{
	Assert(IsInitialized(stringBuffer));
	Assert(stringBuffer->length + str.length < stringBuffer->bufferSize);
	if (str.length == 0) { return; }
	MyMemCopy(&stringBuffer->chars[stringBuffer->length], str.chars, str.length);
	stringBuffer->length += str.length;
	stringBuffer->chars[stringBuffer->length] = '\0';
}
void StringBufferAppend(StringBufferGeneric_t* stringBuffer, const char* nullTermStr)
{
	return StringBufferAppend(stringBuffer, NewStr(nullTermStr));
}
bool StringBufferTryAppend(StringBufferGeneric_t* stringBuffer, MyStr_t str, bool appendWhatYouCan = true)
{
	Assert(IsInitialized(stringBuffer));
	if (str.length == 0) { return true; }
	if (stringBuffer->length + str.length >= stringBuffer->bufferSize)
	{
		if (appendWhatYouCan && stringBuffer->length < stringBuffer->bufferSize-1)
		{
			MyMemCopy(&stringBuffer->chars[stringBuffer->length], str.chars, (stringBuffer->bufferSize-1) - stringBuffer->length);
			stringBuffer->length = (stringBuffer->bufferSize-1);
			stringBuffer->chars[stringBuffer->length] = '\0';
		}
		return false;
	}
	MyMemCopy(&stringBuffer->chars[stringBuffer->length], str.chars, str.length);
	stringBuffer->length += str.length;
	stringBuffer->chars[stringBuffer->length] = '\0';
	return true;
}
bool StringBufferTryAppend(StringBufferGeneric_t* stringBuffer, const char* nullTermStr, bool appendWhatYouCan = true)
{
	return StringBufferTryAppend(stringBuffer, NewStr(nullTermStr), appendWhatYouCan);
}

//Returns the number of replacements performed
uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, MyStr_t targetStr, MyStr_t replacementStr, bool ignoreCase = false)
{
	Assert(IsInitialized(stringBuffer));
	if (targetStr.length == 0) { return 0; } //no work to do if there is no target
	uxx result = 0;
	
	for (uxx cIndex = 0; cIndex + targetStr.length <= stringBuffer->length; cIndex++)
	{
		if ((!ignoreCase && MyMemCompare(&stringBuffer->chars[cIndex], targetStr.chars, targetStr.length) == 0) ||
			(ignoreCase && StrCompareIgnoreCase(&stringBuffer->chars[cIndex], targetStr.chars, targetStr.length) == 0))
		{
			if (replacementStr.length > targetStr.length) //if replacing with larger string
			{
				Assert(stringBuffer->length + (replacementStr.length - targetStr.length) < stringBuffer->bufferSize); //make sure we have space
				//Move all characters after this point up
				uxx offset = (replacementStr.length - targetStr.length);
				for (uxx cIndex2 = stringBuffer->length-1 + offset; cIndex2 - offset >= cIndex; cIndex2--)
				{
					stringBuffer->chars[cIndex2] = stringBuffer->chars[cIndex2 - offset];
				}
				stringBuffer->length += offset;
			}
			else if (replacementStr.length < targetStr.length) //if replacing with a smaller string
			{
				//Move all characters after this point down
				uxx offset = targetStr.length - replacementStr.length;
				for (uxx cIndex2 = cIndex; cIndex2 + offset < stringBuffer->length; cIndex2++)
				{
					stringBuffer->chars[cIndex2] = stringBuffer->chars[cIndex2 + offset];
				}
				stringBuffer->length -= offset;
			}
			if (replacementStr.length > 0)
			{
				MyMemCopy(&stringBuffer->chars[cIndex], replacementStr.chars, replacementStr.length);
			}
			if (replacementStr.length > 1) { cIndex += replacementStr.length-1; }
			
			result++;
		}
	}
	
	DebugAssert(stringBuffer->length < stringBuffer->bufferSize);
	stringBuffer->chars[stringBuffer->length] = '\0';
	
	return result;
}
uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, const char* targetNullTermStr, MyStr_t replacementStr, bool ignoreCase = false)
{
	return StringBufferReplace(stringBuffer, NewStr(targetNullTermStr), replacementStr, ignoreCase);
}
uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, MyStr_t targetStr, const char* replacementNullTermStr, bool ignoreCase = false)
{
	return StringBufferReplace(stringBuffer, targetStr, NewStr(replacementNullTermStr), ignoreCase);
}
uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, const char* targetNullTermStr, const char* replacementNullTermStr, bool ignoreCase = false)
{
	return StringBufferReplace(stringBuffer, NewStr(targetNullTermStr), NewStr(replacementNullTermStr), ignoreCase);
}

//TODO: Add StringBufferTryReplace

// +--------------------------------------------------------------+
// |                      Advanced Functions                      |
// +--------------------------------------------------------------+
//TODO: Add StringBufferUnescapeQuotedString
//TODO: Add StringBufferEscapeQuotedString
//TODO: Add StringBufferPathNormalize?
//TODO: Add StringBufferPathAppend

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_STRING_BUFFER_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
TEMP_STRING_LENGTH
@Types
StringBufferGeneric_t
StringBuffer_t
TempString_t
@Functions
void InitStringBuffer(StringBuffer_t* stringBufferPntr, MyStr_t initialValue = "")
bool IsInitialized(const StringBufferGeneric_t* stringBuffer)
bool IsNullTerminated(StringBufferGeneric_t* stringBuffer)
void StringBufferSet(StringBufferGeneric_t* stringBuffer, MyStr_t str)
bool StringBufferTrySet(StringBufferGeneric_t* stringBuffer, MyStr_t str, bool appendWhatYouCan = true)
void StringBufferClear(StringBufferGeneric_t* stringBuffer, bool zeroMemory = false)
StringBufferGeneric_t* ToGeneric(StringBuffer_t* stringBuffer)
StringBuffer_t* ToStringBuffer(StringBufferGeneric_t* stringBuffer)
TempString_t* ToTempString(StringBufferGeneric_t* stringBuffer)
MyStr_t ToMyStr(StringBufferGeneric_t* stringBuffer)
char* ToStr(StringBufferGeneric_t* stringBuffer)
MyStr_t StringBufferSubstring(StringBufferGeneric_t* stringBuffer, uxx startIndex, uxx endIndex = optional)
MyStr_t StringBufferSubstringFromEnd(StringBufferGeneric_t* stringBuffer, uxx startIndexFromEnd)
void StringBufferPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
bool StringBufferTryPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
void StringBufferAppendPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
bool StringBufferTryAppendPrint(StringBufferGeneric_t* stringBuffer, const char* formatString, ...)
void StringBufferAppend(StringBufferGeneric_t* stringBuffer, MyStr_t str)
bool StringBufferTryAppend(StringBufferGeneric_t* stringBuffer, MyStr_t str, bool appendWhatYouCan = true)
uxx StringBufferReplace(StringBufferGeneric_t* stringBuffer, MyStr_t targetStr, MyStr_t replacementStr, bool ignoreCase = false)
*/
