/*
File:   gy_path.h
Author: Taylor Robbins
Date:   07\03\2024
Description:
	** Holds functions and types that help us deal with file system "paths" on various platforms
*/

#ifndef _GY_PATH_H
#define _GY_PATH_H

struct Path_t
{
	bool usesBackslashes;
	bool isMixedSlashes;
	bool hasTrailingSlash;
	MyStr_t str;
	MyStr_t dirPart;
	MyStr_t namePart;
	MyStr_t nameAndExtPart;
	MyStr_t extPart;
};

// +--------------------------------------------------------------+
// |                    Split String Functions                    |
// +--------------------------------------------------------------+
// if extensionOut is not passed then the extension will be included in the fileNameOut
// Output MyStr_t are not reallocated so they are not all null-terminated
// extensionOut will include the '.' character
// If the path is actually a directory only we may interpret the last folder name as fileName w/ extension.
// Only use full file paths in order to avoid this, or have trailing "/"
void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr)
{
	NotNullStr(&fullPath);
	if (fullPath.length == 0)
	{
		if (directoryOut != nullptr) { *directoryOut = MyStr_Empty; }
		if (fileNameOut != nullptr) { *fileNameOut = MyStr_Empty; }
		if (extensionOut != nullptr) { *extensionOut = MyStr_Empty; }
		return;
	}
	
	bool foundSlash = false;
	bool foundPeriod = false;
	u64 lastSlashIndex = 0; //index after char
	u64 lastPeriodIndex = fullPath.length; //index before char
	for (u64 cIndex = 0; cIndex < fullPath.length; cIndex++)
	{
		if (fullPath.pntr[cIndex] == '\\' || fullPath.pntr[cIndex] == '/')
		{
			foundSlash = true;
			lastSlashIndex = cIndex+1;
		}
		if (fullPath.pntr[cIndex] == '.')
		{
			foundPeriod = true;
			lastPeriodIndex = cIndex;
		}
	}
	if (foundPeriod && lastPeriodIndex < lastSlashIndex) //periods in the directory portion are relative directives like ".\..\" 
	{
		foundPeriod = false;
		lastPeriodIndex = fullPath.length;
	}
	Assert(lastPeriodIndex >= lastSlashIndex);
	
	SetOptionalOutPntr(directoryOut, NewStr(lastSlashIndex, &fullPath.pntr[0]));
	if (extensionOut != nullptr)
	{
		SetOptionalOutPntr(fileNameOut, NewStr(lastPeriodIndex - lastSlashIndex, &fullPath.pntr[lastSlashIndex]));
		SetOptionalOutPntr(extensionOut, NewStr(fullPath.length - lastPeriodIndex, &fullPath.pntr[lastPeriodIndex]));
	}
	else
	{
		SetOptionalOutPntr(fileNameOut, NewStr(fullPath.length - lastSlashIndex, &fullPath.pntr[lastSlashIndex]));
	}
}
MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true)
{
	MyStr_t result;
	MyStr_t extensionThrowAway;
	SplitFilePath(filePath, nullptr, &result, (includeExtension ? nullptr : &extensionThrowAway));
	NotNullStr(&result);
	return result;
}

MyStr_t GetDirectoryPart(MyStr_t filePath)
{
	MyStr_t result;
	SplitFilePath(filePath, &result, nullptr, nullptr);
	NotNullStr(&result);
	return result;
}

//NOTE: Because this is expecting a null-terminated output, we can provide the option for includeExtension=false
const char* GetFileNamePartNt(const char* filePath)
{
	NotNull(filePath);
	MyStr_t result;
	SplitFilePath(NewStr(filePath), nullptr, &result, nullptr);
	NotNullStr(&result);
	return result.pntr;
}

// +--------------------------------------------------------------+
// |                         Free and New                         |
// +--------------------------------------------------------------+
void FreePath(Path_t* path, MemArena_t* allocArena)
{
	NotNull2(path, allocArena);
	FreeString(allocArena, &path->str);
	ClearPointer(path);
}

void SanatizePathSlashes(Path_t* path, bool useBackslashes = false)
{
	StrReplaceInPlace(path->str, useBackslashes ? "/" : "\\", useBackslashes ? "\\" : "/");
	path->usesBackslashes = useBackslashes;
	path->isMixedSlashes = false;
}

Path_t NewPath(MyStr_t pathStr, MemArena_t* memArena = nullptr, bool sanatizeSlashes = false, bool useBackslashes = false)
{
	AssertIf(sanatizeSlashes, memArena != nullptr);
	AssertIf(useBackslashes, sanatizeSlashes == true);
	
	Path_t result = {};
	result.str = pathStr;
	if (memArena != nullptr)
	{
		result.str = AllocString(memArena, &pathStr);
		NotNullStr(&result.str);
		if (sanatizeSlashes)
		{
			SanatizePathSlashes(&result, useBackslashes);
		}
		else
		{
			result.isMixedSlashes = (FindSubstring(result.str, "/") && FindSubstring(result.str, "\\"));
		}
	}
	else
	{
		result.isMixedSlashes = (FindSubstring(result.str, "/") && FindSubstring(result.str, "\\"));
	}
	
	SplitFilePath(result.str, &result.dirPart, &result.namePart, &result.extPart);
	AssertIf(result.extPart.length > 0, result.extPart.chars == result.namePart.chars + result.namePart.length);
	result.nameAndExtPart = result.namePart;
	result.nameAndExtPart.length += result.extPart.length;
	
	result.hasTrailingSlash = (result.str.length >= 1 && (result.str.chars[result.str.length-1] == '\\' || result.str.chars[result.str.length-1] == '/'));
	
	return result;
}

// +--------------------------------------------------------------+
// |                         Path Combine                         |
// +--------------------------------------------------------------+
Path_t PathCombine(Path_t firstPart, Path_t secondPart, MemArena_t* memArena, bool collapseRelativeMovement = false)
{
	NotNull(memArena);
	if (!collapseRelativeMovement || (!FindSubstring(firstPart.str, "..") && !FindSubstring(secondPart.str, "..")))
	{
		MyStr_t newPath = PrintInArenaStr(memArena, "%.*s%s%.*s",
			StrPrint(firstPart.str),
			firstPart.hasTrailingSlash ? "" : (firstPart.usesBackslashes ? "\\" : "/"),
			StrPrint(secondPart.str)
		);
	}
	
	Unimplemented(); //TODO: Implement me!
	return {};
}

#endif //  _GY_PATH_H
