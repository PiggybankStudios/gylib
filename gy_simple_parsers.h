/*
File:   gy_simple_parsers.h
Author: Taylor Robbins
Date:   05\09\2023
LineParser Description:
	** This is a simple utility that helps a deserialization process break a string into lines
	** Handles stuff line /n vs /r/n and keeping track of the lineIndex
TextParser Description:
	** TODO:
XmlParser Description:
	** TODO:
*/

#ifndef _GY_SIMPLE_PARSERS_H
#define _GY_SIMPLE_PARSERS_H

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct LineParser_t
{
	u64 byteIndex;
	u64 lineIndex;
	MyStr_t fileContents;
};

enum ParsingTokenType_t
{
	ParsingTokenType_Unknown = 0,
	ParsingTokenType_FilePrefix,
	ParsingTokenType_KeyValuePair,
	ParsingTokenType_Comment,
	ParsingTokenType_NumTypes,
};
const char* GetParsingTokenTypeStr(ParsingTokenType_t type)
{
	switch (type)
	{
		case ParsingTokenType_Unknown:      return "Unknown";
		case ParsingTokenType_FilePrefix:   return "FilePrefix";
		case ParsingTokenType_KeyValuePair: return "KeyValuePair";
		case ParsingTokenType_Comment:      return "Comment";
		default: return "UNKNOWN";
	}
}

struct ParsingToken_t
{
	ParsingTokenType_t type;
	MyStr_t str;
	MyStr_t key;
	MyStr_t value;
};
struct TextParser_t
{
	LineParser_t lineParser;
	MyStr_t currentLine;
	u64 byteIndex;
};

enum XmlParsingError_t
{
	XmlParsingError_None = 0,
	XmlParsingError_UnexpectedEol,
	XmlParsingError_ExpectedClosingAngleBracket,
	XmlParsingError_UnexpectedClosingToken,
	XmlParsingError_ClosingTokenMismatch,
	XmlParsingError_InvalidCharacterOutsideToken,
	XmlParsingError_InvalidUtf8,
	XmlParsingError_NoTypeFoundForToken,
	XmlParsingError_InvalidCharInIdentifier,
	XmlParsingError_NoEqualsForProperty,
	XmlParsingError_InvalidPropertyFirstChar,
	XmlParsingError_MissingPropertyValue,
	XmlParsingError_MissingClosingTokens,
	XmlParsingError_NumErrors,
};
const char* GetXmlParsingErrorStr(XmlParsingError_t error)
{
	switch (error)
	{
		case XmlParsingError_None:                         return "None";
		case XmlParsingError_UnexpectedEol:                return "UnexpectedEol";
		case XmlParsingError_ExpectedClosingAngleBracket:  return "ExpectedClosingAngleBracket";
		case XmlParsingError_UnexpectedClosingToken:       return "UnexpectedClosingToken";
		case XmlParsingError_ClosingTokenMismatch:         return "ClosingTokenMismatch";
		case XmlParsingError_InvalidCharacterOutsideToken: return "InvalidCharacterOutsideToken";
		case XmlParsingError_InvalidUtf8:                  return "InvalidUtf8";
		case XmlParsingError_NoTypeFoundForToken:          return "NoTypeFoundForToken";
		case XmlParsingError_InvalidCharInIdentifier:      return "InvalidCharInIdentifier";
		case XmlParsingError_NoEqualsForProperty:          return "NoEqualsForProperty";
		case XmlParsingError_InvalidPropertyFirstChar:     return "InvalidPropertyFirstChar";
		case XmlParsingError_MissingPropertyValue:         return "MissingPropertyValue";
		case XmlParsingError_MissingClosingTokens:         return "MissingClosingTokens";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                     LineParser Functions                     |
// +--------------------------------------------------------------+
LineParser_t NewLineParser(MyStr_t fileContents)
{
	NotNullStr(&fileContents);
	LineParser_t result = {};
	result.byteIndex = 0;
	result.lineIndex = 0;
	result.fileContents = fileContents;
	return result;
}

bool LineParserGetLine(LineParser_t* parser, MyStr_t* lineOut)
{
	NotNull(parser);
	NotNullStr(&parser->fileContents);
	if (parser->byteIndex >= parser->fileContents.length) { return false; }
	parser->lineIndex++;
	
	u64 endOfLineByteSize = 0;
	u64 startIndex = parser->byteIndex;
	while (parser->byteIndex < parser->fileContents.length)
	{
		char nextChar = parser->fileContents.pntr[parser->byteIndex];
		char nextNextChar = parser->fileContents.pntr[parser->byteIndex+1];
		if (nextChar != nextNextChar &&
			(nextChar     == '\n' || nextChar     == '\r') &&
			(nextNextChar == '\n' || nextNextChar == '\r'))
		{
			endOfLineByteSize = 2;
			break;
		}
		else if (nextChar == '\n' || nextChar == '\r')
		{
			endOfLineByteSize = 1;
			break;
		}
		else
		{
			parser->byteIndex++;
		}
	}
	
	MyStr_t line = NewStr(parser->byteIndex - startIndex, &parser->fileContents.pntr[startIndex]);
	parser->byteIndex += endOfLineByteSize;
	if (lineOut != nullptr) { *lineOut = line; }
	return true;
}

// +--------------------------------------------------------------+
// |                     TextParser Functions                     |
// +--------------------------------------------------------------+
TextParser_t NewTextParser(MyStr_t fileContents)
{
	TextParser_t result = {};
	result.lineParser = NewLineParser(fileContents);
	result.byteIndex = 0;
	result.currentLine = MyStr_Empty;
	return result;
}

bool TextParserGetToken(TextParser_t* parser, ParsingToken_t* tokenOut)
{
	NotNull(parser);
	NotNull(tokenOut);
	
	while (parser->lineParser.byteIndex < parser->lineParser.fileContents.length)
	{
		if (parser->byteIndex >= parser->currentLine.length)
		{
			bool gotLine = LineParserGetLine(&parser->lineParser, &parser->currentLine);
			if (!gotLine)
			{
				return false;
			}
			parser->byteIndex = 0;
		}
		
		MyStr_t line = StrSubstring(&parser->currentLine, parser->byteIndex);
		u64 numTrimmedWhitespaceChars = TrimLeadingWhitespace(&line);
		if (line.length == 0)
		{
			parser->byteIndex = parser->currentLine.length;
			continue;
		}
		
		//TODO: This doesn't handle if a // shows up inside something like a string where it shouldn't be treated as a comment
		u64 commentStartIndex = 0;
		bool lineContainsComment = FindSubstring(line, "//", &commentStartIndex);
		
		if (lineContainsComment && commentStartIndex == 0)
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_Comment;
			tokenOut->str = line;
			tokenOut->value = StrSubstring(&tokenOut->str, 2, tokenOut->str.length);
			parser->byteIndex = parser->currentLine.length;
			return true;
		}
		else if (lineContainsComment)
		{
			line = StrSubstring(&line, 0, commentStartIndex);
		}
		
		u64 colonIndex = 0;
		bool lineContainsColon = FindSubstring(line, ":", &colonIndex);
		
		if (lineContainsColon)
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_KeyValuePair;
			tokenOut->str = line;
			tokenOut->key = StrSubstring(&line, 0, colonIndex);
			tokenOut->value = StrSubstring(&line, colonIndex+1, line.length);
			TrimWhitespace(&tokenOut->key);
			TrimWhitespace(&tokenOut->value);
			parser->byteIndex += numTrimmedWhitespaceChars + line.length;
			return true;
		}
		
		if (StrStartsWith(line, "#"))
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_FilePrefix;
			tokenOut->str = line;
			tokenOut->value = StrSubstring(&line, 1);
			parser->byteIndex += numTrimmedWhitespaceChars + line.length;
			return true;
		}
		
		ClearPointer(tokenOut);
		tokenOut->type = ParsingTokenType_Unknown;
		tokenOut->str = line;
		parser->byteIndex += numTrimmedWhitespaceChars + line.length;
		return true;
	}
	
	return false;
}

// +--------------------------------------------------------------+
// |                     XmlParser Functions                      |
// +--------------------------------------------------------------+

#endif //  _GY_SIMPLE_PARSERS_H
