/*
File:   gy_result.h
Author: Taylor Robbins
Date:   04\16\2024
Description:
	** Result_t is an enumeration that is used for many of our enumeration scenarios where:
	**  1. the full list of enum values is irrelevant (i.e. unapplicable values are not a problem)
	**  2. The integer values of enum entries are unimportant
	**  3. There are a small number of possibilities (or the possibilities are shared with many other enums)
	** With all this in mind, we can make a single enum that serves all these
	** disparate scenarios, so we don't have to create new enums all the time.
*/

#ifndef _GY_RESULT_H
#define _GY_RESULT_H

enum Result_t
{
	Result_None = 0,
	Result_Success,
	Result_Failure,
	Result_Unknown,
	Result_FunctionMissing,
	Result_NotAFunction,
	Result_Exception,
	
	Result_CantOpenFile,
	Result_EmptyFile,
	Result_MissingFilePrefix,
	Result_TokenBeforeFilePrefix,
	Result_InvalidFilePrefix,
	Result_MultipleFilePrefix,
	Result_MissingRequiredFields,
	Result_UnknownBoneName,
	Result_MissingOrCorruptBones,
	Result_MissingOrCorruptChildren,
	Result_UnknownParentBoneName,
	
	Result_NumValues,
};

const char* GetResultStr(Result_t enumValue)
{
	switch (enumValue)
	{
		case Result_None:                     return "None";
		case Result_Success:                  return "Success";
		case Result_Failure:                  return "Failure";
		case Result_Unknown:                  return "Unknown";
		case Result_FunctionMissing:          return "FunctionMissing";
		case Result_NotAFunction:             return "NotAFunction";
		case Result_Exception:                return "Exception";
		case Result_CantOpenFile:             return "CantOpenFile";
		case Result_EmptyFile:                return "EmptyFile";
		case Result_MissingFilePrefix:        return "MissingFilePrefix";
		case Result_TokenBeforeFilePrefix:    return "TokenBeforeFilePrefix";
		case Result_InvalidFilePrefix:        return "InvalidFilePrefix";
		case Result_MultipleFilePrefix:       return "MultipleFilePrefix";
		case Result_MissingRequiredFields:    return "MissingRequiredFields";
		case Result_UnknownBoneName:          return "UnknownBoneName";
		case Result_MissingOrCorruptBones:    return "MissingOrCorruptBones";
		case Result_MissingOrCorruptChildren: return "MissingOrCorruptChildren";
		case Result_UnknownParentBoneName:    return "UnknownParentBoneName";
		default: return "Unknown";
	}
}

#endif //  _GY_RESULT_H
