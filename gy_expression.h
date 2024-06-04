/*
File:   gy_expression.h
Author: Taylor Robbins
Date:   04\17\2024
Description:
	** Expressions are a simple system that lives somewhere between a full programming
	** language and a basic algebraic calculator. In declaration mode it supports
	** declaring functions with typed returns and parameters and declaring typed variables.
	** In evaluation mode it supports calling functions with typed parameters and
	** getting the result, with each parameter being it's own sub-expression that
	** could be calling another function or referencing a variable, or a mathematical
	** expression of same type values, and it supports assigning a value to a variable
	** with the right-hand side being an expression 
*/

#ifndef _GY_EXPRESSION_H
#define _GY_EXPRESSION_H

// +--------------------------------------------------------------+
// |                     Defines and Typedefs                     |
// +--------------------------------------------------------------+
#define EXPRESSIONS_MAX_NODE_CHILDREN     8 //aka max function argument count
#define EXPRESSIONS_MAX_PARSE_STACK_SIZE  16
#define EXPRESSIONS_MAX_NUM_PARTS         128

#define EXPRESSION_FUNC_DEFINITION(functionName) struct ExpValue_t functionName(MemArena_t* memArena, u64 numArgs, const struct ExpValue_t* args)
typedef EXPRESSION_FUNC_DEFINITION(ExpressionFunc_f);

// +--------------------------------------------------------------+
// |                         Enumerations                         |
// +--------------------------------------------------------------+
enum ExpValueType_t
{
	ExpValueType_None = 0,
	ExpValueType_Void,
	ExpValueType_Bool,
	ExpValueType_Pointer,
	ExpValueType_String,
	ExpValueType_R32,
	ExpValueType_R64,
	ExpValueType_I8,
	ExpValueType_I16,
	ExpValueType_I32,
	ExpValueType_I64,
	ExpValueType_U8,
	ExpValueType_U16,
	ExpValueType_U32,
	ExpValueType_U64,
	ExpValueType_NumTypes,
};

#ifdef GYLIB_HEADER_ONLY
const char* GetExpValueTypeStr(ExpValueType_t enumValue);
#else
const char* GetExpValueTypeStr(ExpValueType_t enumValue)
{
	switch (enumValue)
	{
		case ExpValueType_None:    return "None";
		case ExpValueType_Void:    return "Void";
		case ExpValueType_Bool:    return "Bool";
		case ExpValueType_Pointer: return "Pointer";
		case ExpValueType_String:  return "String";
		case ExpValueType_R32:     return "R32";
		case ExpValueType_R64:     return "R64";
		case ExpValueType_I8:      return "I8";
		case ExpValueType_I16:     return "I16";
		case ExpValueType_I32:     return "I32";
		case ExpValueType_I64:     return "I64";
		case ExpValueType_U8:      return "U8";
		case ExpValueType_U16:     return "U16";
		case ExpValueType_U32:     return "U32";
		case ExpValueType_U64:     return "U64";
		default: return "Unknown";
	}
}
#endif

enum ExpOp_t
{
	ExpOp_None = 0,
	ExpOp_Add,
	ExpOp_Subtract,
	ExpOp_Multiply,
	ExpOp_Divide,
	ExpOp_Modulo,
	ExpOp_Equals,
	ExpOp_NotEquals,
	ExpOp_Or,
	ExpOp_And,
	ExpOp_Not,
	ExpOp_BitwiseOr,
	ExpOp_BitwiseAnd,
	ExpOp_BitwiseXor,
	ExpOp_BitwiseNot,
	ExpOp_Ternary,
	ExpOp_Assignment,
	ExpOp_NumOps,
};

#ifdef GYLIB_HEADER_ONLY
const char* GetExpOpStr(ExpOp_t enumValue);
const char* GetExpOpSyntaxStr(ExpOp_t enumValue, bool secondPart = false);
#else
const char* GetExpOpStr(ExpOp_t enumValue)
{
	switch (enumValue)
	{
		case ExpOp_None:       return "None";
		case ExpOp_Add:        return "Add";
		case ExpOp_Subtract:   return "Subtract";
		case ExpOp_Multiply:   return "Multiply";
		case ExpOp_Divide:     return "Divide";
		case ExpOp_Modulo:     return "Modulo";
		case ExpOp_Equals:     return "Equals";
		case ExpOp_NotEquals:  return "NotEquals";
		case ExpOp_Or:         return "Or";
		case ExpOp_And:        return "And";
		case ExpOp_Not:        return "Not";
		case ExpOp_BitwiseOr:  return "BitwiseOr";
		case ExpOp_BitwiseAnd: return "BitwiseAnd";
		case ExpOp_BitwiseXor: return "BitwiseXor";
		case ExpOp_BitwiseNot: return "BitwiseNot";
		case ExpOp_Ternary:    return "Ternary";
		case ExpOp_Assignment: return "Assignment";
		default: return "Unknown";
	}
}
const char* GetExpOpSyntaxStr(ExpOp_t enumValue, bool secondPart = false)
{
	switch (enumValue)
	{
		case ExpOp_Add:        return "+";
		case ExpOp_Subtract:   return "-";
		case ExpOp_Multiply:   return "*";
		case ExpOp_Divide:     return "/";
		case ExpOp_Modulo:     return "%";
		case ExpOp_Equals:     return "==";
		case ExpOp_NotEquals:  return "!=";
		case ExpOp_Or:         return "||";
		case ExpOp_And:        return "&&";
		case ExpOp_Not:        return "!";
		case ExpOp_BitwiseOr:  return "|";
		case ExpOp_BitwiseAnd: return "&";
		case ExpOp_BitwiseXor: return "^";
		case ExpOp_BitwiseNot: return "~";
		case ExpOp_Ternary:    return (secondPart ? ":" : "?");
		case ExpOp_Assignment: return "=";
		default: return nullptr;
	}
}
#endif

enum ExpPartType_t
{
	ExpPartType_None = 0,
	ExpPartType_Constant,
	ExpPartType_Variable,
	ExpPartType_Operator,
	ExpPartType_Function,
	ExpPartType_ParenthesisGroup,
	ExpPartType_NumTypes,
};

#ifdef GYLIB_HEADER_ONLY
const char* GetExpPartTypeStr(ExpPartType_t enumValue);
#else
const char* GetExpPartTypeStr(ExpPartType_t enumValue)
{
	switch (enumValue)
	{
		case ExpPartType_None:             return "None";
		case ExpPartType_Constant:         return "Constant";
		case ExpPartType_Variable:         return "Variable";
		case ExpPartType_Operator:         return "Operator";
		case ExpPartType_Function:         return "Function";
		case ExpPartType_ParenthesisGroup: return "ParenthesisGroup";
		default: return "Unknown";
	}
}
#endif

enum ExpTokenType_t
{
	ExpTokenType_None = 0,
	ExpTokenType_Operator,
	ExpTokenType_Parenthesis,
	ExpTokenType_Comma,
	ExpTokenType_Number,
	ExpTokenType_String,
	ExpTokenType_Identifier,
	ExpTokenType_NumTypes,
};

#ifdef GYLIB_HEADER_ONLY
const char* GetExpTokenTypeStr(ExpTokenType_t enumValue);
#else
const char* GetExpTokenTypeStr(ExpTokenType_t enumValue)
{
	switch (enumValue)
	{
		case ExpTokenType_None:        return "None";
		case ExpTokenType_Operator:    return "Operator";
		case ExpTokenType_Parenthesis: return "Parenthesis";
		case ExpTokenType_Comma:       return "Comma";
		case ExpTokenType_Number:      return "Number";
		case ExpTokenType_String:      return "String";
		case ExpTokenType_Identifier:  return "Identifier";
		default: return "Unknown";
	}
}
#endif

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct ExpValue_t
{
	ExpValueType_t type;
	union
	{
		bool valueBool;
		struct
		{
			u64 valuePntrTypeId; //types for pointers are handled by the application, acceptance of any TypeId is at the discretion of the function that consumes them
			void* valuePntr;
		};
		MyStr_t valueStr;
		r32 valueR32;
		r64 valueR64;
		i8  valueI8;
		i16 valueI16;
		i32 valueI32;
		i64 valueI64;
		u8  valueU8;
		u16 valueU16;
		u32 valueU32;
		u64 valueU64;
	};
};

struct ExpPart_t
{
	u64 index;
	ExpPartType_t type;
	ExpValue_t constantValue;
	ExpOp_t opType;
	u64 functionIndex;
	u64 variableIndex;
	ExpPart_t* child[EXPRESSIONS_MAX_NODE_CHILDREN];
};

struct ExpPartStack_t
{
	u64 length;
	ExpPart_t* parts[EXPRESSIONS_MAX_PARSE_STACK_SIZE];
};

struct ExpVariableDef_t
{
	ExpValueType_t type;
	u64 pntrTypeId; //Only used to ValueType_Pointer
	MyStr_t name;
	void* pntr;
};

struct ExpFuncArg_t
{
	ExpValueType_t type;
	MyStr_t name;
	bool isOptional;
	ExpValue_t defaultValue;
};

struct ExpFuncDef_t
{
	ExpValueType_t returnType;
	MyStr_t name;
	u64 numArguments;
	ExpFuncArg_t arguments[EXPRESSIONS_MAX_NODE_CHILDREN];
	ExpressionFunc_f* pntr;
};

struct ExpressionContext_t
{
	MemArena_t* allocArena;
	VarArray_t variableDefs; //ExpVariableDef_t
	VarArray_t functionDefs; //ExpFuncDef_t
};

struct Expression_t
{
	MemArena_t* allocArena; //can be null, if not null, then strings referenced by parts are allocated in this arena
	u64 numParts;
	ExpPart_t parts[EXPRESSIONS_MAX_NUM_PARTS];
	ExpPart_t* rootPart;
};

struct ExpToken_t
{
	ExpTokenType_t type;
	MyStr_t str;
};

struct ExpTokenizer_t
{
	MyStr_t expressionStr;
	u64 currentIndex;
	ExpToken_t prevToken;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeExpValue(MemArena_t* allocArena, ExpValue_t* value);
#else

// +--------------------------------------------------------------+
// |                        Free Functions                        |
// +--------------------------------------------------------------+
void FreeExpValue(MemArena_t* allocArena, ExpValue_t* value)
{
	NotNull(value);
	if (allocArena != nullptr && value->type == ExpValueType_String) { FreeString(allocArena, &value->valueStr); }
	ClearPointer(value);
}

void FreeExpression(Expression_t* expression)
{
	NotNull(expression);
	for (u64 pIndex = 0; pIndex < expression->numParts; pIndex++)
	{
		ExpPart_t* part = &expression->parts[pIndex];
		if (part->type == ExpPartType_Constant) { FreeExpValue(expression->allocArena, &part->constantValue); }
	}
	ClearPointer(expression);
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
u8 GetExpOperandCount(ExpOp_t opType)
{
	switch (opType)
	{
		case ExpOp_Add:        return 2;
		case ExpOp_Subtract:   return 2;
		case ExpOp_Multiply:   return 2;
		case ExpOp_Divide:     return 2;
		case ExpOp_Modulo:     return 2;
		case ExpOp_Equals:     return 2;
		case ExpOp_NotEquals:  return 2;
		case ExpOp_Or:         return 2;
		case ExpOp_And:        return 2;
		case ExpOp_Not:        return 1;
		case ExpOp_BitwiseOr:  return 2;
		case ExpOp_BitwiseAnd: return 2;
		case ExpOp_BitwiseXor: return 2;
		case ExpOp_BitwiseNot: return 1;
		case ExpOp_Ternary:    return 3;
		case ExpOp_Assignment: return 2;
		default: AssertMsg(false, "Unhandled ExpOp value in GetExpOperandCount"); return 0;
	}
}

u8 GetExpOpPrecedence(ExpOp_t opType)
{
	switch (opType)
	{
		case ExpOp_Add:        return 5;
		case ExpOp_Subtract:   return 5;
		case ExpOp_Multiply:   return 6;
		case ExpOp_Divide:     return 6;
		case ExpOp_Modulo:     return 7;
		case ExpOp_Equals:     return 4;
		case ExpOp_NotEquals:  return 4;
		case ExpOp_Or:         return 3;
		case ExpOp_And:        return 3;
		case ExpOp_Not:        return 3;
		case ExpOp_BitwiseOr:  return 8;
		case ExpOp_BitwiseAnd: return 8;
		case ExpOp_BitwiseXor: return 8;
		case ExpOp_BitwiseNot: return 8;
		case ExpOp_Ternary:    return 2;
		case ExpOp_Assignment: return 1;
		default: return 0;
	}
}

bool IsExpPartReadyToBeOperand(const ExpPart_t* expPart)
{
	NotNull(expPart);
	if (expPart->type == ExpPartType_Operator)
	{
		u8 numOperands = GetExpOperandCount(expPart->opType);
		for (u8 oIndex = 0; oIndex < numOperands; oIndex++)
		{
			if (expPart->child[oIndex] == nullptr) { return false; }
		}
	}
	return true;
}

// +--------------------------------------------------------------+
// |                         Tokenization                         |
// +--------------------------------------------------------------+
ExpToken_t NewExpToken(ExpTokenType_t type, MyStr_t tokenStr)
{
	ExpToken_t result = {};
	result.type = type;
	result.str = tokenStr;
	return result;
}

ExpTokenizer_t NewExpTokenizer(MyStr_t expressionStr)
{
	ExpTokenizer_t result = {};
	result.expressionStr = expressionStr;
	result.currentIndex = 0;
	result.prevToken.type = ExpTokenType_None;
	return result;
}

bool ExpTokenizerGetNext(ExpTokenizer_t* tokenizer, ExpToken_t* tokenOut, Result_t* errorOut = nullptr)
{
	NotNull(tokenizer);
	if (tokenizer->currentIndex >= tokenizer->expressionStr.length) { SetOptionalOutPntr(errorOut, Result_Finished); return false; }
	
	//TODO: We probably want to check for and remove comments
	
	for (; tokenizer->currentIndex < tokenizer->expressionStr.length; tokenizer->currentIndex++)
	{
		char c = tokenizer->expressionStr.chars[tokenizer->currentIndex];
		char nextChar = (tokenizer->currentIndex+1 < tokenizer->expressionStr.length) ? tokenizer->expressionStr.chars[tokenizer->currentIndex+1] : '\0';
		if (c == ' ' || c == '\t') { continue; }
		bool treatNegativeAsSignage = (tokenizer->prevToken.type == ExpTokenType_None || (
			tokenizer->prevToken.type != ExpTokenType_Number &&
			tokenizer->prevToken.type != ExpTokenType_String &&
			tokenizer->prevToken.type != ExpTokenType_Identifier
		));
		
		if (c == '\"')
		{
			MyStr_t str = NewStr(0, &tokenizer->expressionStr.chars[tokenizer->currentIndex+1]);
			bool foundClosingQuote = false;
			bool prevCharWasBackslash = false;
			//TODO: Should we ensure the string has valid escape sequences during this walk? That way the "UnescapeExpressionStr" function later doesn't have to report error codes when it finds invalid escape sequences?
			for (u64 cIndex = tokenizer->currentIndex+1; cIndex < tokenizer->expressionStr.length; cIndex++)
			{
				char strChar = tokenizer->expressionStr.chars[cIndex];
				if (strChar == '\"' && !prevCharWasBackslash)
				{
					foundClosingQuote = true;
					break;
				}
				else if (strChar == '\\' && !prevCharWasBackslash)
				{
					prevCharWasBackslash = true;
					str.length++;
				}
				else
				{
					prevCharWasBackslash = false;
					str.length++;
				}
			}
			if (!foundClosingQuote)
			{
				tokenizer->currentIndex = tokenizer->expressionStr.length;
				SetOptionalOutPntr(errorOut, Result_MissingQuote);
				return false;
			}
			tokenizer->currentIndex += 1 + str.length + 1;
			tokenizer->prevToken = NewExpToken(ExpTokenType_String, str);
			SetOptionalOutPntr(tokenOut, tokenizer->prevToken);
			return true;
		}
		else if (c == '(' || c == ')')
		{
			MyStr_t charStr = NewStr(1, &tokenizer->expressionStr.chars[tokenizer->currentIndex]);
			tokenizer->currentIndex += charStr.length;
			tokenizer->prevToken = NewExpToken(ExpTokenType_Parenthesis, charStr);
			SetOptionalOutPntr(tokenOut, tokenizer->prevToken);
			return true;
		}
		else if (IsCharNumeric(c) || (((treatNegativeAsSignage && c == '-') || c == '.') && IsCharNumeric(nextChar)))
		{
			MyStr_t numberStr = NewStr(1, &tokenizer->expressionStr.chars[tokenizer->currentIndex]);
			bool foundPeriod = (c == '.');
			for (u64 cIndex = tokenizer->currentIndex+1; cIndex < tokenizer->expressionStr.length; cIndex++)
			{
				char nextNumChar = tokenizer->expressionStr.chars[cIndex];
				if (IsCharNumeric(nextNumChar) || (!foundPeriod && nextNumChar == '.'))
				{
					numberStr.length++;
					if (nextNumChar == '.') { foundPeriod = true; }
				}
				else { break; }
			}
			
			//After a number we expect to find a non-identifier character, because otherwise an identifer that starts with numbers might be mistreated as two valid tokens
			if (tokenizer->currentIndex + numberStr.length < tokenizer->expressionStr.length)
			{
				char followingChar = tokenizer->expressionStr.chars[tokenizer->currentIndex + numberStr.length];
				if (IsCharAlphabetic(followingChar) || followingChar == '_')
				{
					while (tokenizer->currentIndex + numberStr.length < tokenizer->expressionStr.length)
					{
						char nextIdentChar = tokenizer->expressionStr.chars[tokenizer->currentIndex + numberStr.length];
						if (IsCharAlphaNumeric(nextIdentChar) || nextIdentChar == '_') { numberStr.length++; }
					}
					SetOptionalOutPntr(errorOut, Result_InvalidIdentifier);
					tokenizer->currentIndex += numberStr.length;
					return false;
				}
			}
			
			tokenizer->currentIndex += numberStr.length;
			tokenizer->prevToken = NewExpToken(ExpTokenType_Number, numberStr);
			SetOptionalOutPntr(tokenOut, tokenizer->prevToken);
			return true;
			
		}
		else if (c == '+' || c == '-' || c == '/' || c == '*' || c == '|' || c == '&' || c == '^' || c == '%' || c == '!' || c == '?' || c == ':' || c == '=' || c == '~')
		{
			MyStr_t opStr = NewStr(1, &tokenizer->expressionStr.chars[tokenizer->currentIndex]);
			if (tokenizer->currentIndex+1 < tokenizer->expressionStr.length)
			{
				if (c == '&' && nextChar == '&') { opStr.length = 2; }
				else if (c == '|' && nextChar == '|') { opStr.length = 2; }
				else if (c == '+' && nextChar == '=') { opStr.length = 2; }
				else if (c == '-' && nextChar == '=') { opStr.length = 2; }
				else if (c == '/' && nextChar == '=') { opStr.length = 2; }
				else if (c == '*' && nextChar == '=') { opStr.length = 2; }
				else if (c == '|' && nextChar == '=') { opStr.length = 2; }
				else if (c == '&' && nextChar == '=') { opStr.length = 2; }
				else if (c == '^' && nextChar == '=') { opStr.length = 2; }
				else if (c == '=' && nextChar == '=') { opStr.length = 2; }
			}
			
			tokenizer->currentIndex += opStr.length;
			tokenizer->prevToken = NewExpToken(ExpTokenType_Operator, opStr);
			SetOptionalOutPntr(tokenOut, tokenizer->prevToken);
			return true;
		}
		else if (IsCharAlphabetic(c) || c == '_')
		{
			MyStr_t identifierStr = NewStr(1, &tokenizer->expressionStr.chars[tokenizer->currentIndex]);
			for (u64 cIndex = tokenizer->currentIndex+1; cIndex < tokenizer->expressionStr.length; cIndex++)
			{
				char nextIdentChar = tokenizer->expressionStr.chars[cIndex];
				if (!IsCharAlphaNumeric(nextIdentChar) && nextIdentChar != '_') { break; }
				identifierStr.length++;
			}
			
			tokenizer->currentIndex += identifierStr.length;
			tokenizer->prevToken = NewExpToken(ExpTokenType_Identifier, identifierStr);
			SetOptionalOutPntr(tokenOut, tokenizer->prevToken);
			return true;
		}
		else
		{
			SetOptionalOutPntr(errorOut, Result_InvalidChar);
			tokenizer->currentIndex++;
			return false;
		}
	}
	
	SetOptionalOutPntr(errorOut, Result_Finished);
	return false;
}

Result_t TryTokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, ExpToken_t** tokensOut, u64* numTokensOut)
{
	NotNullStr(expressionStr);
	Result_t result = Result_None;
	u64 tIndex = 0;
	
	ExpTokenizer_t tokenizer = NewExpTokenizer(expressionStr);
	while (ExpTokenizerGetNext(&tokenizer, nullptr, &result)) { tIndex++; }
	if (result != Result_Finished) { return result; }
	
	SetOptionalOutPntr(numTokensOut, tIndex);
	if (memArena == nullptr || tokensOut == nullptr) { return result; }
	
	u64 numTokens = tIndex;
	ExpToken_t* tokens = AllocArray(memArena, ExpToken_t, numTokens);
	if (tokens == nullptr) { return Result_AllocFailure; }
	
	tokenizer = NewExpTokenizer(expressionStr);
	tIndex = 0;
	result = Result_None;
	ExpToken_t token;
	while (ExpTokenizerGetNext(&tokenizer, &token, &result))
	{
		Assert(tIndex < numTokens);
		tokens[tIndex] = token;
		tIndex++;
	}
	Assert(result == Result_Finished); //TODO: Maybe this should be Result_Success instead? So we match the good result of TryCreateExpressionFromTokens below
	Assert(tIndex == numTokens);
	
	SetOptionalOutPntr(tokensOut, tokens);
	return result;
}
ExpToken_t* TokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, u64* numTokensOut, Result_t* resultOut = nullptr)
{
	ExpToken_t* tokens = nullptr;
	u64 numTokens = 0;
	Result_t result = TryTokenizeExpressionStr(expressionStr, memArena, &tokens, &numTokens);
	SetOptionalOutPntr(numTokensOut, numTokens);
	SetOptionalOutPntr(resultOut, result);
	return tokens;
}

// +--------------------------------------------------------------+
// |                    String Escape Handling                    |
// +--------------------------------------------------------------+
MyStr_t UnescapeExpressionStr(MemArena_t* memArena, MyStr_t string)
{
	NotNull(memArena);
	NotNullStr(string);
	
	u64 numEscapeSequences = 0;
	for (u64 cIndex = 0; cIndex < string.length; cIndex++)
	{
		char c = string.chars[cIndex];
		char nextChar = ((cIndex+1) < string.length) ? string.chars[cIndex+1] : '\0';
		if (c == '\\')
		{
			if (nextChar == '\\') { numEscapeSequences++; cIndex++; }
			else if (nextChar == 'n') { numEscapeSequences++; cIndex++; }
			else if (nextChar == 'r') { numEscapeSequences++; cIndex++; }
			else if (nextChar == 't') { numEscapeSequences++; cIndex++; }
			else if (nextChar == '"') { numEscapeSequences++; cIndex++; }
			else if (nextChar == '\'') { numEscapeSequences++; cIndex++; }
			//TODO: If we don't understand the escape sequence, we will treat it as a regular character for now. Eventually we should check invalid sequences during tokenization above
		}
	}
	
	MyStr_t result;
	result.length = string.length - numEscapeSequences;
	result.chars = AllocArray(memArena, char, result.length+1);
	NotNull(result.chars);
	
	u64 outIndex = 0;
	for (u64 inIndex = 0; inIndex < string.length; inIndex++)
	{
		Assert(outIndex < result.length);
		char c = string.chars[inIndex];
		char nextChar = ((inIndex+1) < string.length) ? string.chars[inIndex+1] : '\0';
		if (c == '\\')
		{
			if (nextChar == '\\') { result.chars[outIndex++] = '\\'; inIndex++; }
			else if (nextChar == 'n') { result.chars[outIndex++] = '\n'; inIndex++; }
			else if (nextChar == 'r') { result.chars[outIndex++] = '\r'; inIndex++; }
			else if (nextChar == 't') { result.chars[outIndex++] = '\t'; inIndex++; }
			else if (nextChar == '"') { result.chars[outIndex++] = '"'; inIndex++; }
			else if (nextChar == '\'') { result.chars[outIndex++] = '\''; inIndex++; }
			else
			{
				//TODO: If we don't understand the escape sequence, we will treat it as a regular character for now. Eventually we should check invalid sequences during tokenization above
				result.chars[outIndex++] = c;
			}
		}
		else
		{
			result.chars[outIndex++] = c;
		}
	}
	Assert(outIndex == result.length);
	
	result.chars[result.length] = '\0';
	return result;
}

//TODO: Add EscapeExpressionStr

// +--------------------------------------------------------------+
// |                       Parsing Helpers                        |
// +--------------------------------------------------------------+
ExpVariableDef_t* FindExpVariableDef(ExpressionContext_t* context, MyStr_t variableName)
{
	VarArrayLoop(&context->variableDefs, vIndex)
	{
		VarArrayLoopGet(ExpVariableDef_t, variableDef, &context->variableDefs, vIndex);
		if (StrEquals(variableDef->name, variableName))
		{
			return variableDef;
		}
	}
	return nullptr;
}
ExpFuncDef_t* FindExpFuncDef(ExpressionContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX)
{
	VarArrayLoop(&context->functionDefs, vIndex)
	{
		VarArrayLoopGet(ExpFuncDef_t, functionDef, &context->functionDefs, vIndex);
		if (StrEquals(functionDef->name, functionName) && (numArguments == UINT64_MAX || functionDef->numArguments == numArguments))
		{
			return functionDef;
		}
	}
	return nullptr;
}

void PushExpPart(ExpPartStack_t* stack, ExpPart_t* partPntr)
{
	NotNull2(stack, partPntr);
	Assert(stack->length < EXPRESSIONS_MAX_PARSE_STACK_SIZE);
	stack->parts[stack->length] = partPntr;
	stack->length++;
}
ExpPart_t* PopExpPart(ExpPartStack_t* stack)
{
	NotNull(stack);
	Assert(stack->length > 0);
	ExpPart_t* result = stack->parts[stack->length-1];
	stack->length--;
	return result;
}
ExpPart_t* PeekExpPart(ExpPartStack_t* stack)
{
	NotNull(stack);
	if (stack->length == 0) { return nullptr; }
	return stack->parts[stack->length-1];
}

void PushAndConnectExpPart(ExpPartStack_t* stack, ExpPart_t* partPntr)
{
	NotNull2(stack, partPntr);
	
	//If an operator is on the end of the stack, waiting for operands, we should connect the new part as an operand
	if (stack->length > 0)
	{
		ExpPart_t* prevPart = PeekExpPart(stack);
		if (prevPart->type == ExpPartType_Operator && IsExpPartReadyToBeOperand(partPntr))
		{
			u8 numOperands = GetExpOperandCount(prevPart->opType);
			for (u8 oIndex = 0; oIndex < numOperands; oIndex++)
			{
				if (prevPart->child[oIndex] == nullptr)
				{
					prevPart->child[oIndex] = partPntr;
					//if operator is full of operands, connect it to previous parts if necassary
					if (numOperands == oIndex+1)
					{
						PopExpPart(stack);
						PushAndConnectExpPart(stack, prevPart);
					}
					// else operator is still being formed, just leave it on the end of the stack
					return;
				}
			}
		}
	}
	
	PushExpPart(stack, partPntr);
}

ExpValue_t ConvertExpNumberToken(MyStr_t numberStr)
{
	ExpValue_t result = {};
	
	bool foundPeriod = false;
	bool foundNegativeSign = false;
	u64 numSigDigits = 0;
	u64 numMiddleZeros = 0;
	u8 highestDigit = 0;
	for (u64 cIndex = 0; cIndex < numberStr.length; cIndex++)
	{
		char c = numberStr.chars[cIndex];
		if (c == '.') { foundPeriod = true; }
		else if (c == '-') { foundNegativeSign = true; }
		else if (IsCharNumeric(c))
		{
			if (c != '0')
			{
				if (numSigDigits == 0) { highestDigit = (u8)(c - '0'); }
				numSigDigits += numMiddleZeros + 1;
				numMiddleZeros = 0;
			}
			else if (numSigDigits > 0)
			{
				numMiddleZeros++;
			}
		}
	}
	
	if (foundPeriod)
	{
		//Must be a floating point value
		//r32 has 23-bits of mantissa whis is about 7 digits of decimal precision. If we have that amount or more we are going to start up-converting to r64
		if (numSigDigits >= 7) 
		{
			bool parseSuccess = TryParseR64(numberStr, &result.valueR64);
			if (!parseSuccess) { return result; }
			result.type = ExpValueType_R64;
		}
		else
		{
			bool parseSuccess = TryParseR32(numberStr, &result.valueR32);
			if (!parseSuccess) { return result; }
			result.type = ExpValueType_R32;
		}
	}
	else
	{
		//Must be an integer
		if (foundNegativeSign)
		{
			bool parseSuccess = TryParseI64(numberStr, &result.valueI64);
			if (!parseSuccess) { return result; }
			if (result.valueI64 >= INT8_MIN && result.valueI64 <= INT8_MAX)
			{
				result.valueI8 = (i8)result.valueI64;
				result.type = ExpValueType_I8;
			}
			else if (result.valueI64 >= INT16_MIN && result.valueI64 <= INT16_MAX)
			{
				result.valueI16 = (i16)result.valueI64;
				result.type = ExpValueType_I16;
			}
			else if (result.valueI64 >= INT32_MIN && result.valueI64 <= INT32_MAX)
			{
				result.valueI32 = (i32)result.valueI64;
				result.type = ExpValueType_I32;
			}
			else
			{
				result.type = ExpValueType_I64;
			}
		}
		else
		{
			bool parseSuccess = TryParseU64(numberStr, &result.valueU64);
			if (!parseSuccess) { return result; }
			if (result.valueU64 <= UINT8_MAX)
			{
				result.valueU8 = (u8)result.valueI64;
				result.type = ExpValueType_U8;
			}
			else if (result.valueU64 <= UINT16_MAX)
			{
				result.valueU16 = (u16)result.valueU64;
				result.type = ExpValueType_U16;
			}
			else if (result.valueU64 <= UINT32_MAX)
			{
				result.valueU32 = (u32)result.valueU64;
				result.type = ExpValueType_U32;
			}
			else
			{
				result.type = ExpValueType_U64;
			}
		}
	}

	return result;
}

ExpPart_t* SplitExpPartTreeWithPrecedenceAtLeast(ExpPart_t* leftPart, ExpOp_t opType, ExpPart_t** remainingPortionOut)
{
	//Anything besides an operator is treated as highest precedence (constants, function calls, etc.)
	if (leftPart->type != ExpPartType_Operator)
	{
		SetOptionalOutPntr(remainingPortionOut, nullptr);
		return leftPart;
	}
	
	u8 minPrecedence = GetExpOpPrecedence(opType);
	if (GetExpOpPrecedence(leftPart->opType) >= minPrecedence)
	{
		SetOptionalOutPntr(remainingPortionOut, nullptr);
		return leftPart;
	}
	else
	{
		u8 numOperands = GetExpOperandCount(leftPart->opType);
		Assert(numOperands > 0);
		ExpPart_t* rightMostOperand = leftPart->child[numOperands-1];
		leftPart->child[numOperands-1] = nullptr;
		ExpPart_t* result = SplitExpPartTreeWithPrecedenceAtLeast(rightMostOperand, opType, &leftPart->child[numOperands-1]);
		SetOptionalOutPntr(remainingPortionOut, leftPart);
		return result;
	}
}

// +--------------------------------------------------------------+
// |                      Add Part Functions                      |
// +--------------------------------------------------------------+
ExpPart_t* AddExpPart(Expression_t* expression, ExpPartType_t type)
{
	NotNull(expression);
	Assert(expression->numParts < EXPRESSIONS_MAX_NUM_PARTS);
	
	ExpPart_t* result = &expression->parts[expression->numParts];
	ClearPointer(result);
	result->index = expression->numParts;
	result->type = type;
	
	expression->numParts++;
	return result;
}
ExpPart_t* AddExpConstantString(Expression_t* expression, MyStr_t value)
{
	ExpPart_t* result = AddExpPart(expression, ExpPartType_Constant);
	if (result == nullptr) { return result; }
	result->constantValue.type = ExpValueType_String;
	result->constantValue.valueStr = ((expression->allocArena != nullptr) ? AllocString(expression->allocArena, &value) : value);
	return result;
}
ExpPart_t* AddExpVariable(Expression_t* expression, u64 variableIndex)
{
	ExpPart_t* result = AddExpPart(expression, ExpPartType_Variable);
	if (result == nullptr) { return result; }
	result->variableIndex = variableIndex;
	return result;
}
ExpPart_t* AddExpOperator(Expression_t* expression, ExpOp_t opType, ExpPart_t* firstChild = nullptr, ExpPart_t* secondChild = nullptr, ExpPart_t* thirdChild = nullptr)
{
	ExpPart_t* result = AddExpPart(expression, ExpPartType_Operator);
	if (result == nullptr) { return result; }
	result->opType = opType;
	result->child[0] = firstChild;
	result->child[1] = secondChild;
	result->child[2] = thirdChild;
	return result;
}
ExpPart_t* AddExpFunction(Expression_t* expression, u64 functionIndex)
{
	ExpPart_t* result = AddExpPart(expression, ExpPartType_Function);
	if (result == nullptr) { return result; }
	result->functionIndex = functionIndex;
	return result;
}

// +--------------------------------------------------------------+
// |                           Parsing                            |
// +--------------------------------------------------------------+
//If memArena is passed, then the strings referenced by ExpParts will be allocated in the arena, otherwise they will be pointing directly at wherever the tokens were pointing
//TODO: Somehow we should return information about where an syntax error occurred in the expression
Result_t TryCreateExpressionFromTokens(const ExpressionContext_t* context, u64 numTokens, const ExpToken_t* tokens, Expression_t* expressionOut, MemArena_t* memArena = nullptr)
{
	NotNull2(context, expressionOut);
	AssertIf(numTokens > 0, tokens != nullptr);
	
	Expression_t result = {};
	result.allocArena = memArena;
	result.numParts = 0;
	result.rootPart = nullptr;
	
	ExpPartStack_t stack = {};
	
	for (u64 tIndex = 0; tIndex < numTokens; tIndex++)
	{
		const ExpToken_t* token = &tokens[tIndex];
		switch (token->type)
		{
			// +==============================+
			// |     Handle Number Token      |
			// +==============================+
			case ExpTokenType_Number:
			{
				ExpValue_t numberValue = ConvertExpNumberToken(token->str);
				if (numberValue.type == ExpValueType_None) { FreeExpression(&result); return Result_InvalidConstant; }
				
				ExpPart_t* newNumberPart = AddExpPart(&result, ExpPartType_Constant);
				newNumberPart->constantValue = numberValue;
				PushAndConnectExpPart(&stack, newNumberPart);
			} break;
			
			// +==============================+
			// |     Handle String Token      |
			// +==============================+
			case ExpTokenType_String:
			{
				Unimplemented(); //TODO: Implement me!
			} break;
			
			// +==============================+
			// |    Handle Operator Token     |
			// +==============================+
			case ExpTokenType_Operator:
			{
				ExpOp_t opType = ExpOp_None;
				for (u64 oIndex = 0; oIndex < ExpOp_NumOps; oIndex++)
				{
					const char* opSyntaxStr = GetExpOpSyntaxStr((ExpOp_t)oIndex);
					if (opSyntaxStr != nullptr && StrEquals(token->str, opSyntaxStr))
					{
						opType = (ExpOp_t)oIndex;
					}
				}
				if (opType == ExpOp_None) { FreeExpression(&result); return Result_InvalidOperator; }
				
				u8 numOperands = GetExpOperandCount(opType);
				if (numOperands >= 2) //operators with 2 or more values expect one value to be to the left-hand side of the operator syntax
				{
					if (stack.length == 0) { FreeExpression(&result); return Result_MissingLeftOperand; }
					ExpPart_t* leftOperand = PopExpPart(&stack);
					NotNull(leftOperand);
					
					if (!IsExpPartReadyToBeOperand(leftOperand))
					{
						//If the part to the left is not fully complete, then we have some kind of syntax error (like two binary operators in a row)
						FreeExpression(&result);
						return Result_InvalidLeftOperand;
					}
					
					if (leftOperand->type == ExpPartType_Operator)
					{
						//NOTE: We need to apply operator precedence here.
						//      If the operator on the left is lower precedence, then we should grab some portion of it's "right-most" operand
						//      (either the entire thing, or keep descending right-most until we find an operator with equal or higher precedence, or a constant)
						ExpPart_t* remainingPortion = nullptr;
						leftOperand = SplitExpPartTreeWithPrecedenceAtLeast(leftOperand, opType, &remainingPortion);
						if (remainingPortion != nullptr) { PushExpPart(&stack, remainingPortion); }
					}
					
					ExpPart_t* newOpPart = AddExpOperator(&result, opType, leftOperand);
					PushAndConnectExpPart(&stack, newOpPart);
				}
				else
				{
					ExpPart_t* newOpPart = AddExpOperator(&result, opType);
					PushAndConnectExpPart(&stack, newOpPart);
				}
			} break;
			
			// +==============================+
			// |   Handle Identifier Token    |
			// +==============================+
			case ExpTokenType_Identifier:
			{
				Unimplemented(); //TODO: Implement me! We need to decide if the identifier is a function call or a variable and then find it in the context (or throw an error if it's an unknown)
				//TODO: Maybe we should add support for named constants like "pi" where the identifier produces a constant part rather than a reference to something in the context
			} break;
			
			// +==============================+
			// |   Handle Parenthesis Token   |
			// +==============================+
			case ExpTokenType_Parenthesis:
			{
				Unimplemented(); //TODO: Implement me! We need to find the sub-portion of the tokens that are inside the parenthesis and parse them separately. Then add the lot to the stack
			} break;
			
			// +==============================+
			// |      Handle Comma Token      |
			// +==============================+
			case ExpTokenType_Comma:
			{
				Unimplemented(); //TODO: Implement me! We need to make sure that everything in the stack at this point is ready to be passed as an operand (aka argument) to a function. Also need to do a simila check at the end when we are return multiple items from the stack
			} break;
			
			default: AssertMsg(false, "Unhandled ExpTokenType in TryCreateExpressionFromTokens"); break;
		}
	}
	
	//TODO: If we are being called inside the context of a function's parenthesis then multiple items on the stack are allowed. Somehow we need to return those items to the calling scope so they can be hooked up a function in that scopes stack
	if (stack.length == 1)
	{
		result.rootPart = stack.parts[0];
	}
	else
	{
		FreeExpression(&result);
		return Result_Failure;//TODO: Better error code for this!
	}
	
	MyMemCopy(expressionOut, &result, sizeof(Expression_t));
	return Result_Success;
}

//TODO: Implement the rest of me!

#endif GYLIB_HEADER_ONLY

#endif //  _GY_EXPRESSION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
EXPRESSIONS_MAX_NODE_CHILDREN
EXPRESSIONS_MAX_NUM_PARTS
ExpValueType_None
ExpValueType_Void
ExpValueType_Bool
ExpValueType_Pointer
ExpValueType_String
ExpValueType_R32
ExpValueType_R64
ExpValueType_I8
ExpValueType_I16
ExpValueType_I32
ExpValueType_I64
ExpValueType_U8
ExpValueType_U16
ExpValueType_U32
ExpValueType_U64
ExpValueType_NumTypes
ExpOp_None
ExpOp_Add
ExpOp_Subtract
ExpOp_Multiply
ExpOp_Divide
ExpOp_Equals
ExpOp_NotEquals
ExpOp_Or
ExpOp_And
ExpOp_Xor
ExpOp_Not
ExpOp_BitwiseOr
ExpOp_BitwiseAnd
ExpOp_BitwiseXor
ExpOp_BitwiseNot
ExpOp_Ternary
ExpOp_Assignment
ExpOp_NumOps
ExpPartType_None
ExpPartType_Constant
ExpPartType_Variable
ExpPartType_Operator
ExpPartType_Function
ExpPartType_NumTypes
ExpTokenType_None
ExpTokenType_Operator
ExpTokenType_Parenthesis
ExpTokenType_Quote
ExpTokenType_Comma
ExpTokenType_Number
ExpTokenType_String
ExpTokenType_Identifier
ExpTokenType_NumTypes
@Types
ExpValueType_t
ExpOp_t
ExpPartType_t
ExpTokenType_t
ExpValue_t
ExpPart_t
ExpVariableDef_t
ExpFuncArg_t
ExpFuncDef_t
ExpressionContext_t
Expression_t
@Functions
ExpValue_t EXPRESSION_FUNC_DEFINITION(MemArena_t* memArena, u64 numArgs, const ExpValue_t* args)
const char* GetExpValueTypeStr(ExpValueType_t enumValue)
const char* GetExpOpStr(ExpOp_t enumValue)
const char* GetExpPartTypeStr(ExpPartType_t enumValue)
const char* GetExpTokenTypeStr(ExpTokenType_t enumValue)
void FreeExpValue(MemArena_t* allocArena, ExpValue_t* value)
*/
