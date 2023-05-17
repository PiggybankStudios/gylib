/*
File:   gy_process_log.h
Author: Taylor Robbins
Date:   05\09\2023
*/

#ifndef _GY_PROCESS_LOG_H
#define _GY_PROCESS_LOG_H

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define DEFAULT_PROCESS_LOG_FIFO_SIZE Kilobytes(8)
#define DBG_FILEPATH_AND_FUNCNAME_SEP_CHAR '|'

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct ProcessLogLine_t
{
	u8 flags;
	u64 timestamp;
	u64 programTime;
	u64 preciseProgramTime;
	i64 threadNumber;
	u64 fileLineNumber;
	DbgLevel_t dbgLevel;
};

struct ProcessLog_t
{
	bool hadErrors;
	bool hadWarnings;
	bool debugBreakOnWarningsAndErrors;
	u32 errorCode;
	TryParseFailureReason_t parseFailureReason;
	XmlParsingError_t xmlParsingError;
	
	MemArena_t* allocArena;
	MemArena_t* tempArena;
	MyStr_t processName;
	MyStr_t filePath;
	StringFifo_t fifo;
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
typedef u64 GetPreciseProgramTime_f();
typedef u64 GetThreadNumber_f();

struct LogGlobals_t
{
	u64 timestamp;
	u64 programTime;
	GetPreciseProgramTime_f* getPreciseProgramTime;
	GetThreadNumber_f* getThreadNumber;
};

//The global variable "logGlobals" is how ProcessLogs (and any other log who wants to)
// can get implicit information to shove into the metadata of every logged line
LogGlobals_t* logGlobals = nullptr;

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeProcessLog(ProcessLog_t* log)
{
	NotNull_(log);
	if (log->fifo.allocArena == nullptr || log->fifo.allocArena->type != MemArenaType_MarkedStack)
	{
		DestroyStringFifo(&log->fifo);
	}
	if (!IsEmptyStr(log->filePath))
	{
		NotNull(log->allocArena);
		FreeString(log->allocArena, &log->filePath);
	}
	if (!IsEmptyStr(log->processName))
	{
		NotNull(log->allocArena);
		FreeString(log->allocArena, &log->processName);
	}
	ClearPointer(log);
}

//The fifoArena is where we do a 1 time allocation from NOW, the logArena is where we allocate things that get stored in ProcessLog_t, the tempArena is used for printing/formatting strings
void CreateProcessLog(ProcessLog_t* logOut, u64 fifoSize, MemArena_t* fifoArena, MemArena_t* logArena, MemArena_t* tempArena)
{
	NotNull(logOut);
	NotNull(logArena);
	NotNull(tempArena);
	Assert(tempArena->type == MemArenaType_MarkedStack);
	ClearPointer(logOut);
	logOut->allocArena = logArena;
	logOut->tempArena = tempArena;
	if (fifoSize > 0)
	{
		NotNull(fifoArena);
		CreateStringFifoInArena(&logOut->fifo, fifoArena, fifoSize);
	}
	logOut->hadErrors = false;
	logOut->hadWarnings = false;
	logOut->debugBreakOnWarningsAndErrors = false;
	logOut->errorCode = 0;
	logOut->filePath = NewStringInArenaNt(logOut->allocArena, "[UnspecifiedPath]");
	logOut->processName = NewStringInArenaNt(logOut->allocArena, "[UnspecifiedName]");
}

void CreateProcessLogStub(ProcessLog_t* logOut)
{
	NotNull(logOut);
	ClearPointer(logOut);
	logOut->allocArena = nullptr;
	logOut->tempArena = nullptr;
	logOut->hadErrors = false;
	logOut->hadWarnings = false;
	logOut->debugBreakOnWarningsAndErrors = false;
	logOut->errorCode = 0;
	logOut->filePath = MyStr_Empty;
	logOut->processName = MyStr_Empty;
}

//TODO: Create a CopyProcessLog function?

// +--------------------------------------------------------------+
// |                   FilePath and ProcessName                   |
// +--------------------------------------------------------------+
void SetProcessLogFilePath(ProcessLog_t* log, MyStr_t filePath)
{
	NotNull(log);
	NotNullStr(&filePath);
	if (log->allocArena != nullptr)
	{
		if (!IsEmptyStr(log->filePath))
		{
			FreeString(log->allocArena, &log->filePath);
		}
		log->filePath = AllocString(log->allocArena, &filePath);
	}
}
void SetProcessLogName(ProcessLog_t* log, MyStr_t processName)
{
	NotNull(log);
	NotNullStr(&processName);
	if (log->allocArena != nullptr)
	{
		if (!IsEmptyStr(log->processName))
		{
			if (DoesMemArenaSupportFreeing(log->allocArena)) { FreeString(log->allocArena, &log->processName); }
		}
		log->processName = AllocString(log->allocArena, &processName);
	}
}

// +--------------------------------------------------------------+
// |                       Output and Print                       |
// +--------------------------------------------------------------+
void LogOutput_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* message)
{
	NotNull_(log);
	UNUSED(addNewLine); //TODO: We should probably take this into account
	if (log->debugBreakOnWarningsAndErrors && (dbgLevel == DbgLevel_Warning || dbgLevel == DbgLevel_Error)) { MyDebugBreak(); }
	if (log->fifo.bufferSize > 0)
	{
		PushMemMark(log->tempArena);
		
		MyStr_t text = NewStr(message);
		MyStr_t filePathAndFuncName = PrintInArenaStr(log->tempArena, "%s%c%s", filePath, DBG_FILEPATH_AND_FUNCNAME_SEP_CHAR, funcName);
		ProcessLogLine_t metaInfo = {};
		metaInfo.flags = flags;
		metaInfo.fileLineNumber = lineNumber;
		metaInfo.dbgLevel = dbgLevel;
		metaInfo.timestamp = (logGlobals != nullptr) ? logGlobals->timestamp : 0;
		metaInfo.programTime = (logGlobals != nullptr) ? logGlobals->programTime : 0;
		metaInfo.preciseProgramTime = (logGlobals != nullptr && logGlobals->getPreciseProgramTime != nullptr) ? logGlobals->getPreciseProgramTime() : 0;
		metaInfo.threadNumber = (logGlobals != nullptr && logGlobals->getThreadNumber != nullptr) ? logGlobals->getThreadNumber() : 0;
		
		StringFifoLine_t* newLine = StringFifoPushLineExt(&log->fifo, text, sizeof(metaInfo), &metaInfo, filePathAndFuncName);
		DebugAssertAndUnused_(newLine != nullptr, newLine);
		
		PopMemMark(log->tempArena);
	}
}

void LogPrint_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* formatString, ...)
{
	NotNull_(log);
	if (log->debugBreakOnWarningsAndErrors && (dbgLevel == DbgLevel_Warning || dbgLevel == DbgLevel_Error)) { MyDebugBreak(); }
	if (log->fifo.bufferSize > 0)
	{
		if (log->tempArena != nullptr)
		{
			PushMemMark(log->tempArena);
			va_list args;
			va_start(args, formatString);
			int formattedStrLength = PrintVa_Measure((formatString), args);
			va_end(args);
			if (formattedStrLength >= 0)
			{
				char* formattedStr = AllocArray(log->tempArena, char, formattedStrLength+1); //Allocate
				if (formattedStr != nullptr)
				{
					va_start(args, formatString);
					PrintVa_Print(formatString, args, formattedStr, formattedStrLength);
					va_end(args);
					formattedStr[formattedStrLength] = '\0';
					
					LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, formattedStr);
				}
				else
				{
					//failed print, just send out the formatString
					LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, formatString);
				}
			}
			else
			{
				//failed print, just send out the formatString
				LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, formatString);
			}
			PopMemMark(log->tempArena);
		}
		else
		{
			LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, "[No TempMem For Print]");
		}
	}
}

void LogExit_(ProcessLog_t* log, bool success, u32 errorCode, const char* filePath, u32 lineNumber, const char* funcName)
{
	NotNull_(log);
	if (success)
	{
		Assert_(errorCode == 0);
		LogOutput_(log, 0x00, filePath, lineNumber, funcName, DbgLevel_Info, true, "Process succeeded");
	}
	else
	{
		Assert_(errorCode != 0);
		if (log->debugBreakOnWarningsAndErrors) { MyDebugBreak(); }
		log->hadErrors = true;
		log->errorCode = errorCode;
		LogPrint_(log, 0x00, filePath, lineNumber, funcName, DbgLevel_Error, true, "Exiting with error code %u(0x%08X)", errorCode, errorCode);
		
	}
}

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define LogWriteAt(log, level, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, message)
#define LogWriteLineAt(log, level, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  message)
#define LogPrintAt(log, level, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, formatString, ##__VA_ARGS__)
#define LogPrintLineAt(log, level, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  formatString, ##__VA_ARGS__)
#define LogWriteAtx(log, level, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, message)
#define LogWriteLineAtx(log, level, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  message)
#define LogPrintAtx(log, level, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, formatString, ##__VA_ARGS__)
#define LogPrintLineAtx(log, level, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  formatString, ##__VA_ARGS__)

#define LogWrite_D(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, false, message)
#define LogWriteLine_D(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  message)
#define LogPrint_D(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_D(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Dx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, false, message)
#define LogWriteLine_Dx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  message)
#define LogPrint_Dx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Dx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  formatString, ##__VA_ARGS__)

#define LogWrite_R(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, false, message)
#define LogWriteLine_R(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  message)
#define LogPrint_R(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_R(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Rx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, false, message)
#define LogWriteLine_Rx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  message)
#define LogPrint_Rx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Rx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  formatString, ##__VA_ARGS__)

#define LogWrite_I(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, false, message)
#define LogWriteLine_I(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, true,  message)
#define LogPrint_I(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_I(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Ix(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, false, message)
#define LogWriteLine_Ix(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, true,  message)
#define LogPrint_Ix(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Ix(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, true,  formatString, ##__VA_ARGS__)

#define LogWrite_N(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, false, message)
#define LogWriteLine_N(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  message)
#define LogPrint_N(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_N(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Nx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, false, message)
#define LogWriteLine_Nx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  message)
#define LogPrint_Nx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Nx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  formatString, ##__VA_ARGS__)

#define LogWrite_O(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, false, message)
#define LogWriteLine_O(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, true,  message)
#define LogPrint_O(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_O(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Ox(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, false, message)
#define LogWriteLine_Ox(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, true,  message)
#define LogPrint_Ox(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Ox(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, true,  formatString, ##__VA_ARGS__)

#define LogWrite_W(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, false, message)
#define LogWriteLine_W(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  message)
#define LogPrint_W(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_W(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Wx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, false, message)
#define LogWriteLine_Wx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  message)
#define LogPrint_Wx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Wx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  formatString, ##__VA_ARGS__)

#define LogWrite_E(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, false, message)
#define LogWriteLine_E(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, true,  message)
#define LogPrint_E(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_E(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Ex(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, false, message)
#define LogWriteLine_Ex(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, true,  message)
#define LogPrint_Ex(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Ex(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, true,  formatString, ##__VA_ARGS__)

#define LogExitSuccess(log)            LogExit_((log), true,  0,           __FILE__, __LINE__, __func__)
#define LogExitFailure(log, errorCode) LogExit_((log), false, (errorCode), __FILE__, __LINE__, __func__)

#endif //  _GY_PROCESS_LOG_H