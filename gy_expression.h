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
#define EXPRESSIONS_MAX_PART_CHILDREN     8 //aka max function argument count
#define EXPRESSIONS_MAX_PARSE_STACK_SIZE  16
#define EXPRESSIONS_MAX_EVAL_STACK_SIZE   16
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
	ExpOp_Ternary, //TODO: Implement handling of this properly!
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

enum ExpStepOrder_t
{
	ExpStepOrder_None = 0,
	ExpStepOrder_Prefix,
	ExpStepOrder_Natural,
	ExpStepOrder_Postfix,
	ExpStepOrder_NumOrders,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetExpStepOrderStr(ExpStepOrder_t enumValue);
#else
const char* GetExpStepOrderStr(ExpStepOrder_t enumValue)
{
	switch (enumValue)
	{
		case ExpStepOrder_None:    return "None";
		case ExpStepOrder_Prefix:  return "Prefix";
		case ExpStepOrder_Natural: return "Natural";
		case ExpStepOrder_Postfix: return "Postfix";
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
		MyStr_t valueStr; //needs to be unescaped before being used as a literal value
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
	u64 tokenIndex;
	ExpPartType_t type;
	ExpValueType_t evalType;
	u64 childCount;
	ExpPart_t* child[EXPRESSIONS_MAX_PART_CHILDREN];
	// ExpPartType_Constant
	ExpValue_t constantValue;
	// ExpPartType_Operator
	ExpOp_t opType;
	// ExpPartType_Variable
	u64 variableIndex;
	// ExpPartType_Function
	u64 functionIndex;
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
	bool isOptional; //TODO: Implement handling of this!
	ExpValue_t defaultValue;
};

struct ExpFuncDef_t
{
	ExpValueType_t returnType;
	MyStr_t name;
	u64 numArguments;
	ExpFuncArg_t arguments[EXPRESSIONS_MAX_PART_CHILDREN];
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

#define EXP_STEP_CALLBACK(functionName) void functionName(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpressionContext_t* context, void* userPntr)
typedef EXP_STEP_CALLBACK(ExpStepCallback_f);

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

inline bool IsExpValueTypeInteger(ExpValueType_t type)
{
	return (
		type == ExpValueType_I8 || type == ExpValueType_I16 || type == ExpValueType_I32 || type == ExpValueType_I64 ||
		type == ExpValueType_U8 || type == ExpValueType_U16 || type == ExpValueType_U32 || type == ExpValueType_U64
	);
}
inline bool IsExpValueTypeFloat(ExpValueType_t type)
{
	return (type == ExpValueType_R32 || type == ExpValueType_R64);
}
inline bool IsExpValueTypeSigned(ExpValueType_t type)
{
	return (
		type == ExpValueType_R32 || type == ExpValueType_R64 ||
		type == ExpValueType_I8 || type == ExpValueType_I16 || type == ExpValueType_I32 || type == ExpValueType_I64
	);
}
inline bool IsExpValueTypeNumber(ExpValueType_t type)
{
	return (IsExpValueTypeInteger(type) || IsExpValueTypeFloat(type));
}
inline bool IsExpValueTypeConstantCompat(ExpValueType_t type)
{
	return (IsExpValueTypeNumber(type) || type == ExpValueType_String);
}
inline bool IsExpValueTypeBoolable(ExpValueType_t type)
{
	return (type == ExpValueType_Bool || IsExpValueTypeNumber(type) || type == ExpValueType_Pointer);
}
inline bool CanExpValueTypeConvertTo(ExpValueType_t type, ExpValueType_t outType)
{
	if (type == outType) { return true; }
	if (!IsExpValueTypeNumber(type)) { return false; } //only numbers have automatic conversion
	if (!IsExpValueTypeNumber(outType)) { return false; } //only numbers have automatic conversion
	if (IsExpValueTypeFloat(type) && !IsExpValueTypeFloat(outType)) { return false; } // disallow float -> integer conversion
	//TODO: Do we want to be strict about any particular number conversions here? Our type checking has no static analysis for bubbling up real constant values,
	// so it is going to call foul on anything that expects smaller than 64-bit types and has values being fed in through operators. And we have no way of "casting"
	// from one type to another in the expression, so we can't appease the type checker if it's too strict.
	return true;
}
inline u8 GetExpValueTypeByteSize(ExpValueType_t type)
{
	switch (type)
	{
		case ExpValueType_Void: return 0;
		case ExpValueType_Bool: return sizeof(bool);
		case ExpValueType_Pointer: return sizeof(void*);
		case ExpValueType_String: return sizeof(MyStr_t);
		case ExpValueType_R32: return 4;
		case ExpValueType_R64: return 8;
		case ExpValueType_I8: return 1;
		case ExpValueType_I16: return 2;
		case ExpValueType_I32: return 4;
		case ExpValueType_I64: return 8;
		case ExpValueType_U8: return 1;
		case ExpValueType_U16: return 2;
		case ExpValueType_U32: return 4;
		case ExpValueType_U64: return 8;
		default: return 0;
	}
}

MyStr_t ExpValueToStr(ExpValue_t value, MemArena_t* memArena, bool includeType = false)
{
	MyStr_t result = MyStr_Empty;
	if (includeType)
	{
		switch (value.type)
		{
			case ExpValueType_Void:    result = PrintInArenaStr(memArena, "Void"); break;
			case ExpValueType_Bool:    result = PrintInArenaStr(memArena, "Bool %s",                  value.valueBool ? "True" : "False"); break;
			case ExpValueType_Pointer: result = PrintInArenaStr(memArena, "Pointer[%llu] %p",         value.valuePntrTypeId, value.valuePntr); break;
			case ExpValueType_String:  result = PrintInArenaStr(memArena, "String \"%.*s\"", StrPrint(value.valueStr)); break;
			case ExpValueType_R32:     result = PrintInArenaStr(memArena, "R32 %g",                   value.valueR32); break;
			case ExpValueType_R64:     result = PrintInArenaStr(memArena, "R64 %lg",                  value.valueR64); break;
			case ExpValueType_I8:      result = PrintInArenaStr(memArena, "I8 %d",               (int)value.valueI8);  break;
			case ExpValueType_I16:     result = PrintInArenaStr(memArena, "I16 %d",              (int)value.valueI16); break;
			case ExpValueType_I32:     result = PrintInArenaStr(memArena, "I32 %d",              (int)value.valueI32); break;
			case ExpValueType_I64:     result = PrintInArenaStr(memArena, "I64 %lld",                 value.valueI64); break;
			case ExpValueType_U8:      result = PrintInArenaStr(memArena, "U8 %u",      (unsigned int)value.valueU8);  break;
			case ExpValueType_U16:     result = PrintInArenaStr(memArena, "U16 %u",     (unsigned int)value.valueU16); break;
			case ExpValueType_U32:     result = PrintInArenaStr(memArena, "U32 %u",     (unsigned int)value.valueU32); break;
			case ExpValueType_U64:     result = PrintInArenaStr(memArena, "U64 %llu",                 value.valueU64); break;
			default:                   result = PrintInArenaStr(memArena, "UnknownType %u", value.type); break;
		}
	}
	else
	{
		switch (value.type)
		{
			case ExpValueType_Void:    result = PrintInArenaStr(memArena, "Void"); break;
			case ExpValueType_Bool:    result = PrintInArenaStr(memArena, "%s",                  value.valueBool ? "True" : "False"); break;
			case ExpValueType_Pointer: result = PrintInArenaStr(memArena, "%p",                 value.valuePntr); break;
			case ExpValueType_String:  result = PrintInArenaStr(memArena, "\"%.*s\"",   StrPrint(value.valueStr)); break;
			case ExpValueType_R32:     result = PrintInArenaStr(memArena, "%g",                  value.valueR32); break;
			case ExpValueType_R64:     result = PrintInArenaStr(memArena, "%lg",                 value.valueR64); break;
			case ExpValueType_I8:      result = PrintInArenaStr(memArena, "%d",             (int)value.valueI8);  break;
			case ExpValueType_I16:     result = PrintInArenaStr(memArena, "%d",             (int)value.valueI16); break;
			case ExpValueType_I32:     result = PrintInArenaStr(memArena, "%d",             (int)value.valueI32); break;
			case ExpValueType_I64:     result = PrintInArenaStr(memArena, "%lld",                value.valueI64); break;
			case ExpValueType_U8:      result = PrintInArenaStr(memArena, "%u",    (unsigned int)value.valueU8);  break;
			case ExpValueType_U16:     result = PrintInArenaStr(memArena, "%u",    (unsigned int)value.valueU16); break;
			case ExpValueType_U32:     result = PrintInArenaStr(memArena, "%u",    (unsigned int)value.valueU32); break;
			case ExpValueType_U64:     result = PrintInArenaStr(memArena, "%llu",                value.valueU64); break;
			default:                   result = PrintInArenaStr(memArena, "UnknownType"); break;
		}
	}
	return result;
}

// +--------------------------------------------------------------+
// |                       Value Conversion                       |
// +--------------------------------------------------------------+
ExpValueType_t GetExpResultTypeForMathOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isSubtractOp, Result_t* reasonOut = nullptr)
{
	if (leftOperandType == rightOperandType)
	{
		if (IsExpValueTypeNumber(leftOperandType)) { return leftOperandType; }
		else { SetOptionalOutPntr(reasonOut, Result_InvalidLeftOperand); return ExpValueType_None; }
	}
	else if (IsExpValueTypeNumber(leftOperandType) && IsExpValueTypeNumber(rightOperandType))
	{
		//TODO: Should we be smarter about this somehow? Right now, any operator will result in a rather large type during type-check,
		// because we can't be sure about the value that is stored in each operand and whether we will underflow/overflow if the operator is carried out
		if (IsExpValueTypeFloat(leftOperandType) || IsExpValueTypeFloat(rightOperandType)) { return ExpValueType_R64; }
		else if (IsExpValueTypeSigned(leftOperandType) || IsExpValueTypeSigned(rightOperandType) || isSubtractOp) { return ExpValueType_I64; }
		else { return ExpValueType_U64; }
	}
	else
	{
		//TODO: Are there any mismatching types that we accept for basic math operators?
		SetOptionalOutPntr(reasonOut, Result_InvalidRightOperand); return ExpValueType_None;
	}
}
ExpValueType_t GetExpIntegerTypeForBitwiseOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isAndOp, Result_t* reasonOut = nullptr)
{
	if (!IsExpValueTypeInteger(leftOperandType)) { SetOptionalOutPntr(reasonOut, Result_InvalidLeftOperand); return ExpValueType_None; }
	if (!IsExpValueTypeInteger(rightOperandType)) { SetOptionalOutPntr(reasonOut, Result_InvalidRightOperand); return ExpValueType_None; }
	
	if (leftOperandType == rightOperandType) { return leftOperandType; }
	else
	{
		// NOTE: Signed integers sign bit isn't treated specially, but the result is signed if one side is signed and it is >= the other in byte size
		bool resultIsSigned = false;
		if (IsExpValueTypeSigned(leftOperandType) && GetExpValueTypeByteSize(leftOperandType) >= GetExpValueTypeByteSize(rightOperandType)) { resultIsSigned = true; }
		else if (IsExpValueTypeSigned(rightOperandType) && GetExpValueTypeByteSize(rightOperandType) >= GetExpValueTypeByteSize(leftOperandType)) { resultIsSigned = true; }
		
		// NOTE: The & operator actually chooses the smaller type as the result because the bits involved in the larger type are guaranteed discarded
		u8 resultByteSize = isAndOp ?
			(u8)MinU32(GetExpValueTypeByteSize(leftOperandType), GetExpValueTypeByteSize(rightOperandType)) :
			(u8)MaxU32(GetExpValueTypeByteSize(leftOperandType), GetExpValueTypeByteSize(rightOperandType));
		switch (resultByteSize)
		{
			case 1: return (resultIsSigned ? ExpValueType_I8  : ExpValueType_U8);
			case 2: return (resultIsSigned ? ExpValueType_I16 : ExpValueType_U16);
			case 4: return (resultIsSigned ? ExpValueType_I32 : ExpValueType_U32);
			case 8: return (resultIsSigned ? ExpValueType_I64 : ExpValueType_U64);
			default: Assert(false); SetOptionalOutPntr(reasonOut, Result_Unknown); return ExpValueType_None;
		}
	}
}
ExpValueType_t GetExpCommonTypeForComparisonOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, Result_t* reasonOut = nullptr)
{
	if (leftOperandType == rightOperandType)
	{
		return leftOperandType;
	}
	else if (IsExpValueTypeNumber(leftOperandType) && IsExpValueTypeNumber(rightOperandType))
	{
		//TODO: Should we be smarter about this somehow? Right now, any operator will result in a rather large type during type-check,
		// because we can't be sure about the value that is stored in each operand and whether we will underflow/overflow if the operator is carried out
		if (IsExpValueTypeFloat(leftOperandType) || IsExpValueTypeFloat(rightOperandType)) { return ExpValueType_R64; }
		else if (IsExpValueTypeSigned(leftOperandType) || IsExpValueTypeSigned(rightOperandType)) { return ExpValueType_I64; }
		else { return ExpValueType_U64; }
	}
	else
	{
		//TODO: Are there any mismatching types that we accept for comparison operators?
		SetOptionalOutPntr(reasonOut, Result_InvalidRightOperand); return ExpValueType_None;
	}
}

ExpValue_t CastExpValue(ExpValue_t value, ExpValueType_t type)
{
	ExpValue_t result = value;
	if (value.type == type) { return result; }
	result.type = type;
	
	switch (value.type)
	{
		case ExpValueType_Bool:
		{
			switch (type)
			{
				case ExpValueType_R32: result.valueR32 = (r32)(value.valueBool ? 1.0f : 0.0f); break;
				case ExpValueType_R64: result.valueR64 = (r64)(value.valueBool ? 1.0  : 0.0);  break;
				case ExpValueType_U8:  result.valueU8  =  (u8)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_U16: result.valueU16 = (u16)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_U32: result.valueU32 = (u32)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_U64: result.valueU64 = (u64)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_I8:  result.valueI8  =  (i8)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_I16: result.valueI16 = (i16)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_I32: result.valueI32 = (i32)(value.valueBool ? 1    :   0);  break;
				case ExpValueType_I64: result.valueI64 = (i64)(value.valueBool ? 1    :   0);  break;
				//TODO: Can we support string conversion by doing "True" and "False"? Is that helpful?
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); break;
			}
		} break;
		
		case ExpValueType_Pointer:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (value.valuePntr != nullptr); break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); break;
			}
		} break;
		
		case ExpValueType_String:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = !IsEmptyStr(value.valueStr); break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); break;
			}
		} break;
		
		case ExpValueType_R32:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueR32 != 0.0f); break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueR32;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueR32;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueR32;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueR32;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueR32;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueR32;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueR32;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueR32;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueR32;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_R64:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueR64 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueR64;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueR64;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueR64;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueR64;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueR64;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueR64;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueR64;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueR64;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueR64;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		
		case ExpValueType_I8:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueI8 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueI8;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueI8;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueI8;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueI8;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueI8;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueI8;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueI8;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueI8;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueI8;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_I16:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueI16 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueI16;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueI16;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueI16;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueI16;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueI16;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueI16;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueI16;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueI16;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueI16;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_I32:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueI32 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueI32;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueI32;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueI32;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueI32;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueI32;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueI32;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueI32;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueI32;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueI32;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_I64:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueI64 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueI64;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueI64;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueI64;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueI64;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueI64;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueI64;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueI64;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueI64;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueI64;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		
		case ExpValueType_U8:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueU8 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueU8;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueU8;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueU8;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueU8;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueU8;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueU8;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueU8;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueU8;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueU8;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_U16:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueU16 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueU16;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueU16;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueU16;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueU16;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueU16;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueU16;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueU16;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueU16;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueU16;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_U32:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueU32 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueU32;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueU32;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueU32;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueU32;      break;
				case ExpValueType_U64:  result.valueU64  = (u64)result.valueU32;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueU32;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueU32;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueU32;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueU32;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		case ExpValueType_U64:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (result.valueU64 != 0.0f); break;
				case ExpValueType_R32:  result.valueR32  = (r32)result.valueU64;      break;
				case ExpValueType_R64:  result.valueR64  = (r64)result.valueU64;      break;
				case ExpValueType_U8:   result.valueU8   =  (u8)result.valueU64;      break;
				case ExpValueType_U16:  result.valueU16  = (u16)result.valueU64;      break;
				case ExpValueType_U32:  result.valueU32  = (u32)result.valueU64;      break;
				case ExpValueType_I8:   result.valueI8   =  (i8)result.valueU64;      break;
				case ExpValueType_I16:  result.valueI16  = (i16)result.valueU64;      break;
				case ExpValueType_I32:  result.valueI32  = (i32)result.valueU64;      break;
				case ExpValueType_I64:  result.valueI64  = (i64)result.valueU64;      break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue");  break;
			}
		} break;
		
		default: AssertMsg(false, "Unsupported conversion in CastExpValue"); break;
	}
	
	return result;
}

void WriteExpVariable_(ExpVariableDef_t* variableDef, u64 valueSize, const void* valuePntr)
{
	NotNull2(variableDef, valuePntr);
	NotNull(variableDef->pntr);
	switch (variableDef->type)
	{
		// case ExpValueType_Void:    
		case ExpValueType_Bool:    Assert(valueSize == sizeof(bool));    *((bool*)variableDef->pntr)    = *(bool*)valuePntr;    break;
		case ExpValueType_Pointer: Assert(valueSize == sizeof(void*));   *((void**)variableDef->pntr)   = *(void**)valuePntr;   break;
		case ExpValueType_String:  Assert(valueSize == sizeof(MyStr_t)); *((MyStr_t*)variableDef->pntr) = *(MyStr_t*)valuePntr; break;
		case ExpValueType_R32:     Assert(valueSize == sizeof(r32));     *((r32*)variableDef->pntr)     = *(r32*)valuePntr;     break;
		case ExpValueType_R64:     Assert(valueSize == sizeof(r64));     *((r64*)variableDef->pntr)     = *(r64*)valuePntr;     break;
		case ExpValueType_I8:      Assert(valueSize == sizeof(i8));      *((i8*)variableDef->pntr)      = *(i8*)valuePntr;      break;
		case ExpValueType_I16:     Assert(valueSize == sizeof(i16));     *((i16*)variableDef->pntr)     = *(i16*)valuePntr;     break;
		case ExpValueType_I32:     Assert(valueSize == sizeof(i32));     *((i32*)variableDef->pntr)     = *(i32*)valuePntr;     break;
		case ExpValueType_I64:     Assert(valueSize == sizeof(i64));     *((i64*)variableDef->pntr)     = *(i64*)valuePntr;     break;
		case ExpValueType_U8:      Assert(valueSize == sizeof(u8));      *((u8*)variableDef->pntr)      = *(u8*)valuePntr;      break;
		case ExpValueType_U16:     Assert(valueSize == sizeof(u16));     *((u16*)variableDef->pntr)     = *(u16*)valuePntr;     break;
		case ExpValueType_U32:     Assert(valueSize == sizeof(u32));     *((u32*)variableDef->pntr)     = *(u32*)valuePntr;     break;
		case ExpValueType_U64:     Assert(valueSize == sizeof(u64));     *((u64*)variableDef->pntr)     = *(u64*)valuePntr;     break;
		default: AssertMsg(false, "Unhandled ExpValueType in WriteExpVariable_"); break;
	}
}
void WriteExpVariableBool(ExpVariableDef_t* variableDef, bool value)
{
	Assert(variableDef->type == ExpValueType_Bool);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariablePointer(ExpVariableDef_t* variableDef, void* value)
{
	Assert(variableDef->type == ExpValueType_Pointer);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableString(ExpVariableDef_t* variableDef, MyStr_t value)
{
	Assert(variableDef->type == ExpValueType_String);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableR32(ExpVariableDef_t* variableDef, r32 value)
{
	Assert(variableDef->type == ExpValueType_R32);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableR64(ExpVariableDef_t* variableDef, r64 value)
{
	Assert(variableDef->type == ExpValueType_R64);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI8(ExpVariableDef_t* variableDef, i8 value)
{
	Assert(variableDef->type == ExpValueType_I8);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI16(ExpVariableDef_t* variableDef, i16 value)
{
	Assert(variableDef->type == ExpValueType_I16);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI32(ExpVariableDef_t* variableDef, i32 value)
{
	Assert(variableDef->type == ExpValueType_I32);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI64(ExpVariableDef_t* variableDef, i64 value)
{
	Assert(variableDef->type == ExpValueType_I64);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU8(ExpVariableDef_t* variableDef, u8 value)
{
	Assert(variableDef->type == ExpValueType_U8);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU16(ExpVariableDef_t* variableDef, u16 value)
{
	Assert(variableDef->type == ExpValueType_U16);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU32(ExpVariableDef_t* variableDef, u32 value)
{
	Assert(variableDef->type == ExpValueType_U32);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU64(ExpVariableDef_t* variableDef, u64 value)
{
	Assert(variableDef->type == ExpValueType_U64);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}

ExpValue_t ReadExpVariable_(ExpVariableDef_t* variableDef)
{
	ExpValue_t result = {};
	result.type = variableDef->type;
	switch (variableDef->type)
	{
		// case ExpValueType_Void:    
		case ExpValueType_Bool:    result.valueBool = *((bool*)variableDef->pntr);    break;
		case ExpValueType_Pointer: result.valuePntr = *((void**)variableDef->pntr);   break;
		case ExpValueType_String:  result.valueStr  = *((MyStr_t*)variableDef->pntr); break;
		case ExpValueType_R32:     result.valueR32  = *((r32*)variableDef->pntr);     break;
		case ExpValueType_R64:     result.valueR64  = *((r64*)variableDef->pntr);     break;
		case ExpValueType_I8:      result.valueI8   = *((i8*)variableDef->pntr);      break;
		case ExpValueType_I16:     result.valueI16  = *((i16*)variableDef->pntr);     break;
		case ExpValueType_I32:     result.valueI32  = *((i32*)variableDef->pntr);     break;
		case ExpValueType_I64:     result.valueI64  = *((i64*)variableDef->pntr);     break;
		case ExpValueType_U8:      result.valueU8   = *((u8*)variableDef->pntr);      break;
		case ExpValueType_U16:     result.valueU16  = *((u16*)variableDef->pntr);     break;
		case ExpValueType_U32:     result.valueU32  = *((u32*)variableDef->pntr);     break;
		case ExpValueType_U64:     result.valueU64  = *((u64*)variableDef->pntr);     break;
		default: AssertMsg(false, "Unhandled ExpValueType in ReadExpVariable_"); break;
	}
	return result;
}
bool ReadExpVariableBool(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_Bool);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueBool;
}
void* ReadExpVariablePointer(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_Pointer);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valuePntr;
}
r32 ReadExpVariableR32(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_R32);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueR32;
}
r64 ReadExpVariableR64(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_R64);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueR64;
}
i8 ReadExpVariableI8(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_I8);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueI8;
}
i16 ReadExpVariableI16(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_I16);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueI16;
}
i32 ReadExpVariableI32(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_I32);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueI32;
}
i64 ReadExpVariableI64(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_I64);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueI64;
}
u8 ReadExpVariableU8(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_U8);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueU8;
}
u16 ReadExpVariableU16(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_U16);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueU16;
}
u32 ReadExpVariableU32(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_U32);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueU32;
}
u64 ReadExpVariableU64(ExpVariableDef_t* variableDef)
{
	Assert(variableDef->type == ExpValueType_U64);
	ExpValue_t resultValue = ReadExpVariable_(variableDef);
	return resultValue.valueU64;
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
	if (tokenizer->currentIndex >= tokenizer->expressionStr.length) { SetOptionalOutPntr(errorOut, Result_Success); return false; }
	
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
				else if (c == '!' && nextChar == '=') { opStr.length = 2; }
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
	
	SetOptionalOutPntr(errorOut, Result_Success);
	return false;
}

Result_t TryTokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, ExpToken_t** tokensOut, u64* numTokensOut)
{
	NotNullStr(expressionStr);
	Result_t result = Result_None;
	u64 tIndex = 0;
	
	ExpTokenizer_t tokenizer = NewExpTokenizer(expressionStr);
	while (ExpTokenizerGetNext(&tokenizer, nullptr, &result)) { tIndex++; }
	if (result != Result_Success) { return result; }
	
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
	Assert(result == Result_Success); //TODO: Maybe this should be Result_Success instead? So we match the good result of TryCreateExpressionFromTokens below
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

//NOTE: This does not escape tab or single quote characters because those escape sequences are optional
MyStr_t EscapeExpressionStr(MemArena_t* memArena, MyStr_t string)
{
	NotNull(memArena);
	NotNullStr(&string);
	
	u64 numEscapeSequences = 0;
	for (u64 cIndex = 0; cIndex < string.length; cIndex++)
	{
		char c = string.chars[cIndex];
		if (c == '\\' || c == '\"' || c == '\n' || c == '\r') { numEscapeSequences++; }
	}
	
	MyStr_t result;
	result.length = string.length + numEscapeSequences;
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
			result.chars[outIndex++] = '\\';
			Assert(outIndex < result.length);
			result.chars[outIndex++] = '\\';
		}
		else if (c == '\"')
		{
			result.chars[outIndex++] = '\\';
			Assert(outIndex < result.length);
			result.chars[outIndex++] = '\"';
		}
		else if (c == '\n')
		{
			result.chars[outIndex++] = '\\';
			Assert(outIndex < result.length);
			result.chars[outIndex++] = 'n';
		}
		else if (c == '\r')
		{
			result.chars[outIndex++] = '\\';
			Assert(outIndex < result.length);
			result.chars[outIndex++] = 'r';
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

// +--------------------------------------------------------------+
// |                       Parsing Helpers                        |
// +--------------------------------------------------------------+
ExpVariableDef_t* FindExpVariableDef(ExpressionContext_t* context, MyStr_t variableName, u64* indexOut = nullptr)
{
	VarArrayLoop(&context->variableDefs, vIndex)
	{
		VarArrayLoopGet(ExpVariableDef_t, variableDef, &context->variableDefs, vIndex);
		if (StrEquals(variableDef->name, variableName))
		{
			SetOptionalOutPntr(indexOut, vIndex);
			return variableDef;
		}
	}
	return nullptr;
}
const ExpVariableDef_t* FindExpVariableDef(const ExpressionContext_t* context, MyStr_t variableName, u64* indexOut = nullptr) //const variant
{
	return (const ExpVariableDef_t*)FindExpVariableDef((ExpressionContext_t*)context, variableName, indexOut);
}
ExpFuncDef_t* FindExpFuncDef(ExpressionContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr)
{
	VarArrayLoop(&context->functionDefs, fIndex)
	{
		VarArrayLoopGet(ExpFuncDef_t, functionDef, &context->functionDefs, fIndex);
		if (StrEquals(functionDef->name, functionName) && (numArguments == UINT64_MAX || functionDef->numArguments == numArguments))
		{
			SetOptionalOutPntr(indexOut, fIndex);
			return functionDef;
		}
	}
	return nullptr;
}
const ExpFuncDef_t* FindExpFuncDef(const ExpressionContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr) //const variant
{
	return (const ExpFuncDef_t*)FindExpFuncDef((ExpressionContext_t*)context, functionName, numArguments, indexOut);
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

bool FindExpClosingParensToken(u64 numTokens, const ExpToken_t* tokens, u64 startIndex, u64* indexOut)
{
	AssertIf(numTokens > 0, tokens != nullptr);
	bool foundEndParenthesis = false;
	u64 endParenthesisIndex = 0;
	u64 parensLevel = 0;
	
	for (u64 tIndex = startIndex; tIndex < numTokens; tIndex++)
	{
		const ExpToken_t* token = &tokens[tIndex];
		if (token->type == ExpTokenType_Parenthesis)
		{
			if (token->str.length == 1 && token->str.chars[0] == '(')
			{
				parensLevel++;
			}
			else if (token->str.length == 1 && token->str.chars[0] == ')')
			{
				if (parensLevel == 0)
				{
					SetOptionalOutPntr(indexOut, tIndex);
					return true;
				}
				else
				{
					parensLevel--;
				}
			}
		}
	}
	
	return false;
}

// +--------------------------------------------------------------+
// |                      Add Part Functions                      |
// +--------------------------------------------------------------+
ExpPart_t* AddExpPart(Expression_t* expression, u64 tokenIndex, ExpPartType_t type)
{
	NotNull(expression);
	Assert(expression->numParts < EXPRESSIONS_MAX_NUM_PARTS);
	
	ExpPart_t* result = &expression->parts[expression->numParts];
	ClearPointer(result);
	result->index = expression->numParts;
	result->type = type;
	result->tokenIndex = tokenIndex;
	
	expression->numParts++;
	return result;
}
ExpPart_t* AddExpConstantString(Expression_t* expression, u64 tokenIndex, MyStr_t value)
{
	ExpPart_t* result = AddExpPart(expression, tokenIndex, ExpPartType_Constant);
	if (result == nullptr) { return result; }
	result->constantValue.type = ExpValueType_String;
	result->constantValue.valueStr = ((expression->allocArena != nullptr) ? AllocString(expression->allocArena, &value) : value);
	return result;
}
ExpPart_t* AddExpVariable(Expression_t* expression, u64 tokenIndex, u64 variableIndex)
{
	ExpPart_t* result = AddExpPart(expression, tokenIndex, ExpPartType_Variable);
	if (result == nullptr) { return result; }
	result->variableIndex = variableIndex;
	return result;
}
ExpPart_t* AddExpOperator(Expression_t* expression, u64 tokenIndex, ExpOp_t opType, ExpPart_t* firstChild = nullptr, ExpPart_t* secondChild = nullptr, ExpPart_t* thirdChild = nullptr)
{
	ExpPart_t* result = AddExpPart(expression, tokenIndex, ExpPartType_Operator);
	if (result == nullptr) { return result; }
	result->opType = opType;
	result->child[0] = firstChild;
	result->child[1] = secondChild;
	result->child[2] = thirdChild;
	return result;
}
ExpPart_t* AddExpFunction(Expression_t* expression, u64 tokenIndex, u64 functionIndex)
{
	ExpPart_t* result = AddExpPart(expression, tokenIndex, ExpPartType_Function);
	if (result == nullptr) { return result; }
	result->functionIndex = functionIndex;
	return result;
}
ExpPart_t* AddExpParenthesisGroup(Expression_t* expression, u64 tokenIndex, ExpPart_t* childRoot)
{
	ExpPart_t* result = AddExpPart(expression, tokenIndex, ExpPartType_ParenthesisGroup);
	if (result == nullptr) { return result; }
	result->childCount = 1;
	result->child[0] = childRoot;
	return result;
}

// +--------------------------------------------------------------+
// |                           Parsing                            |
// +--------------------------------------------------------------+
//If memArena is passed, then the strings referenced by ExpParts will be allocated in the arena, otherwise they will be pointing directly at wherever the tokens were pointing
//TODO: Somehow we should return information about where an syntax error occurred in the expression
Result_t TryCreateExpressionFromTokens_Helper(Expression_t* expression, const ExpressionContext_t* context, u64 numTokens, const ExpToken_t* tokens, ExpPart_t** rootOut, ExpPart_t* functionPart = nullptr)
{
	NotNull2(expression, context);
	AssertIf(functionPart != nullptr, rootOut == nullptr);
	u64 functionArgIndex = 0;
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
				if (numberValue.type == ExpValueType_None) { return Result_InvalidConstant; }
				
				ExpPart_t* newNumberPart = AddExpPart(expression, tIndex, ExpPartType_Constant);
				newNumberPart->constantValue = numberValue;
				PushAndConnectExpPart(&stack, newNumberPart);
			} break;
			
			// +==============================+
			// |     Handle String Token      |
			// +==============================+
			case ExpTokenType_String:
			{
				ExpPart_t* newStrPart = AddExpConstantString(expression, tIndex, token->str);
				PushAndConnectExpPart(&stack, newStrPart);
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
				if (opType == ExpOp_None) { return Result_InvalidOperator; }
				
				u8 numOperands = GetExpOperandCount(opType);
				if (numOperands >= 2) //operators with 2 or more values expect one value to be to the left-hand side of the operator syntax
				{
					if (stack.length == 0) { return Result_MissingLeftOperand; }
					ExpPart_t* leftOperand = PopExpPart(&stack);
					NotNull(leftOperand);
					
					if (!IsExpPartReadyToBeOperand(leftOperand))
					{
						//If the part to the left is not fully complete, then we have some kind of syntax error (like two binary operators in a row)
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
					
					ExpPart_t* newOpPart = AddExpOperator(expression, tIndex, opType, leftOperand);
					newOpPart->childCount = numOperands;
					PushAndConnectExpPart(&stack, newOpPart);
				}
				else
				{
					ExpPart_t* newOpPart = AddExpOperator(expression, tIndex, opType);
					newOpPart->childCount = numOperands;
					PushAndConnectExpPart(&stack, newOpPart);
				}
			} break;
			
			// +==============================+
			// |   Handle Identifier Token    |
			// +==============================+
			case ExpTokenType_Identifier:
			{
				//TODO: Maybe we should add support for named constants like "pi" where the identifier produces a constant part rather than a reference to something in the context
				
				const ExpToken_t* nextToken = (tIndex+1 < numTokens) ? &tokens[tIndex+1] : nullptr;
				if (nextToken != nullptr && nextToken->type == ExpTokenType_Parenthesis && nextToken->str.length == 1 && nextToken->str.chars[0] == '(')
				{
					//If the next token is an open parenthesis, then this is a function call
					u64 endParenthesisIndex = 0;
					if (!FindExpClosingParensToken(numTokens, tokens, tIndex+2, &endParenthesisIndex)) { return Result_MismatchParenthesis; }
					u64 numTokensInParenthesis = endParenthesisIndex - (tIndex+2);
					
					ExpPart_t functionPartProto = {};
					Result_t subResult = TryCreateExpressionFromTokens_Helper(expression, context, numTokensInParenthesis, &tokens[tIndex+2], nullptr, &functionPartProto);
					if (subResult != Result_Success) { return subResult; }
					
					u64 funcDefIndex = 0;
					const ExpFuncDef_t* funcDef = FindExpFuncDef(context, token->str, functionPartProto.childCount, &funcDefIndex);
					if (funcDef == nullptr) { return Result_UnknownFunction; }
					
					ExpPart_t* newFunctionPart = AddExpFunction(expression, tIndex, funcDefIndex);
					MyMemCopy(&newFunctionPart->child[0], &functionPartProto.child[0], EXPRESSIONS_MAX_PART_CHILDREN * sizeof(ExpPart_t*));
					newFunctionPart->childCount = functionPartProto.childCount;
					PushAndConnectExpPart(&stack, newFunctionPart);
					
					tIndex = endParenthesisIndex;
				}
				else
				{
					u64 variableDefIndex = 0;
					const ExpVariableDef_t* variableDef = FindExpVariableDef(context, token->str, &variableDefIndex);
					if (variableDef == nullptr) { return Result_UnknownVariable; }
					
					ExpPart_t* newVariablePart = AddExpVariable(expression, tIndex, variableDefIndex);
					PushAndConnectExpPart(&stack, newVariablePart);
				}
			} break;
			
			// +==============================+
			// |   Handle Parenthesis Token   |
			// +==============================+
			case ExpTokenType_Parenthesis:
			{
				//If we find a closing parens here, then it had no starting parens to match it
				if (token->str.length != 1 || token->str.chars[0] != '(') { return Result_MismatchParenthesis; }
				
				u64 endParenthesisIndex = 0;
				if (!FindExpClosingParensToken(numTokens, tokens, tIndex+1, &endParenthesisIndex)) { return Result_MismatchParenthesis; }
				u64 numTokensInParenthesis = endParenthesisIndex - (tIndex+1);
				
				ExpPart_t* groupRootPart = nullptr;
				Result_t subResult = TryCreateExpressionFromTokens_Helper(expression, context, numTokensInParenthesis, &tokens[tIndex+1], &groupRootPart);
				if (subResult != Result_Success) { return subResult; }
				
				ExpPart_t* newParensPart = AddExpParenthesisGroup(expression, tIndex, groupRootPart);
				PushAndConnectExpPart(&stack, newParensPart);
				
				tIndex = endParenthesisIndex;
			} break;
			
			// +==============================+
			// |      Handle Comma Token      |
			// +==============================+
			case ExpTokenType_Comma:
			{
				if (functionPart == nullptr) { return Result_InvalidOperator; }
				if (functionArgIndex >= EXPRESSIONS_MAX_PART_CHILDREN) { return Result_TooManyArguments; }
				
				if (stack.length == 1)
				{
					if (!IsExpPartReadyToBeOperand(stack.parts[0])) { return Result_MissingRightOperand; }
					ExpPart_t* argument = PopExpPart(&stack);
					functionPart->child[functionArgIndex++] = argument;
					functionPart->childCount = functionArgIndex;
				}
				else { return Result_EmptyArgument; }
			} break;
			
			default: AssertMsg(false, "Unhandled ExpTokenType in TryCreateExpressionFromTokens"); break;
		}
	}
	
	if (functionPart != nullptr)
	{
		if (functionArgIndex >= EXPRESSIONS_MAX_PART_CHILDREN) { return Result_TooManyArguments; }
		
		if (stack.length == 1)
		{
			if (!IsExpPartReadyToBeOperand(stack.parts[0])) { return Result_MissingRightOperand; }
			ExpPart_t* argument = PopExpPart(&stack);
			functionPart->child[functionArgIndex++] = argument;
			functionPart->childCount = functionArgIndex;
		}
		else { return Result_EmptyArgument; }
	}
	else if (stack.length == 1)
	{
		if (!IsExpPartReadyToBeOperand(stack.parts[0])) { return Result_MissingRightOperand; }
		SetOptionalOutPntr(rootOut, stack.parts[0]);
	}
	else
	{
		return Result_MissingOperator;
	}
	
	return Result_Success;
}

//If memArena is passed, then the strings referenced by ExpParts will be allocated in the arena, otherwise they will be pointing directly at wherever the tokens were pointing
//TODO: Somehow we should return information about where an syntax error occurred in the expression
Result_t TryCreateExpressionFromTokens(const ExpressionContext_t* context, u64 numTokens, const ExpToken_t* tokens, Expression_t* expressionOut, MemArena_t* memArena = nullptr)
{
	NotNull2(context, expressionOut);
	AssertIf(numTokens > 0, tokens != nullptr);
	
	Expression_t expression = {};
	expression.allocArena = memArena;
	expression.numParts = 0;
	
	Result_t result = TryCreateExpressionFromTokens_Helper(&expression, context, numTokens, tokens, &expression.rootPart);
	
	if (result == Result_Success)
	{
		MyMemCopy(expressionOut, &expression, sizeof(Expression_t));
	}
	else
	{
		FreeExpression(&expression);
	}
	
	return result;
}

// +--------------------------------------------------------------+
// |                          Evaluating                          |
// +--------------------------------------------------------------+
u64 StepThroughExpression_Helper(Expression_t* expression, ExpPart_t* part, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpressionContext_t* context, void* userPntr, u64 startIndex, u64 depth)
{
	NotNull3(expression, part, callback);
	Assert(order == ExpStepOrder_Prefix || order == ExpStepOrder_Natural || order == ExpStepOrder_Postfix);
	
	u64 index = startIndex;
	
	switch (part->type)
	{
		case ExpPartType_Constant:
		case ExpPartType_Variable:
		{
			callback(expression, part, index++, depth, context, userPntr);
		} break;
		
		case ExpPartType_Operator:
		{
			u8 numOperands = GetExpOperandCount(part->opType);
			Assert(numOperands >= 1);
			if (order == ExpStepOrder_Prefix || (numOperands == 1 && order == ExpStepOrder_Natural)) { callback(expression, part, index++, depth, context, userPntr); }
			for (u8 oIndex = 0; oIndex < numOperands; oIndex++)
			{
				ExpPart_t* operand = part->child[oIndex];
				index += StepThroughExpression_Helper(expression, operand, order, callback, context, userPntr, index, depth+1);
				//For Natural order in operators that are 2 operands or greater, the operator comes in between every operand (potentially multiple times)
				if (oIndex+1 < numOperands && order == ExpStepOrder_Natural) { callback(expression, part, index++, depth, context, userPntr); }
			}
			if (order == ExpStepOrder_Postfix) { callback(expression, part, index++, depth, context, userPntr); }
		} break;
		
		case ExpPartType_Function:
		{
			if (order == ExpStepOrder_Prefix || order == ExpStepOrder_Natural) { callback(expression, part, index++, depth, context, userPntr); }
			for (u64 aIndex = 0; aIndex < part->childCount; aIndex++)
			{
				ExpPart_t* argument = part->child[aIndex];
				index += StepThroughExpression_Helper(expression, argument, order, callback, context, userPntr, index, depth+1);
			}
			if (order == ExpStepOrder_Postfix) { callback(expression, part, index++, depth, context, userPntr); }
		} break;
		
		case ExpPartType_ParenthesisGroup:
		{
			if (order == ExpStepOrder_Prefix) { callback(expression, part, index++, depth, context, userPntr); }
			index += StepThroughExpression_Helper(expression, part->child[0], order, callback, context, userPntr, index, depth+1);
			if (order == ExpStepOrder_Postfix || order == ExpStepOrder_Natural) { callback(expression, part, index++, depth, context, userPntr); }
		} break;
		
		default: AssertMsg(false, "Unhandled ExpPartType in StepThroughExpression_Helper"); break;
	}
	
	return index - startIndex;
}
void StepThroughExpression(Expression_t* expression, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpressionContext_t* context = nullptr, void* userPntr = nullptr)
{
	NotNull2(expression, expression->rootPart);
	u64 numStepsTotal = StepThroughExpression_Helper(expression, expression->rootPart, order, callback, context, userPntr, 0, 0);
	UNUSED(numStepsTotal);
}

struct ExpTypeCheckResult_t
{
	Result_t result;
	u64 errorPartIndex;
};

// +==================================+
// | ExpressionTypeCheckWalk_Callback |
// +==================================+
// void ExpressionTypeCheckWalk_Callback(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpressionContext_t* context, void* userPntr)
EXP_STEP_CALLBACK(ExpressionTypeCheckWalk_Callback)
{
	NotNull3(expression, part, userPntr);
	ExpTypeCheckResult_t* resultPntr = (ExpTypeCheckResult_t*)userPntr;
	if (resultPntr->result != Result_None) { return; } //once we have an error, skip the rest of the callbacks
	
	switch (part->type)
	{
		case ExpPartType_Constant:
		{
			part->evalType = part->constantValue.type;
		} break;
		
		case ExpPartType_Variable:
		{
			if (context == nullptr) { resultPntr->result = Result_MissingContext; resultPntr->errorPartIndex = part->index; break; }
			ExpVariableDef_t* variableDef = VarArrayGet(&context->variableDefs, part->variableIndex, ExpVariableDef_t);
			part->evalType = variableDef->type;
		} break;
		
		case ExpPartType_Operator:
		{
			switch (part->opType)
			{
				// +==========================================================+
				// | TypeCheck Add/Subtract/Multiply/Divide/Modulo Operators  |
				// +==========================================================+
				case ExpOp_Add:
				case ExpOp_Subtract:
				case ExpOp_Multiply:
				case ExpOp_Divide:
				case ExpOp_Modulo:
				{
					NotNull2(part->child[0], part->child[1]);
					ExpValueType_t leftOperandType = part->child[0]->evalType;
					ExpValueType_t rightOperandType = part->child[1]->evalType;
					Assert(leftOperandType != ExpValueType_None && rightOperandType != ExpValueType_None);
					Result_t mismatchReason = Result_None;
					part->evalType = GetExpResultTypeForMathOp(leftOperandType, rightOperandType, (part->opType == ExpOp_Subtract), &mismatchReason);
					if (part->evalType == ExpPartType_None)
					{
						resultPntr->result = mismatchReason;
						resultPntr->errorPartIndex = part->index;
					}
				} break;
				
				// +======================================+
				// | TypeCheck Equals/NotEquals Operators |
				// +======================================+
				case ExpOp_Equals:
				case ExpOp_NotEquals:
				{
					NotNull2(part->child[0], part->child[1]);
					ExpValueType_t leftOperandType = part->child[0]->evalType;
					ExpValueType_t rightOperandType = part->child[1]->evalType;
					Assert(leftOperandType != ExpValueType_None && rightOperandType != ExpValueType_None);
					Result_t mismatchReason = Result_None;
					ExpValueType_t commonType = GetExpCommonTypeForComparisonOp(leftOperandType, rightOperandType, &mismatchReason);
					if (commonType != ExpPartType_None)
					{
						part->evalType = ExpValueType_Bool;
					}
					else
					{
						resultPntr->result = mismatchReason;
						resultPntr->errorPartIndex = part->index;
					}
				} break;
				
				// +==============================+
				// |  TypeCheck Or/And Operators  |
				// +==============================+
				case ExpOp_Or:
				case ExpOp_And:
				{
					NotNull2(part->child[0], part->child[1]);
					ExpValueType_t leftOperandType = part->child[0]->evalType;
					ExpValueType_t rightOperandType = part->child[1]->evalType;
					Assert(leftOperandType != ExpValueType_None && rightOperandType != ExpValueType_None);
					if (!IsExpValueTypeBoolable(leftOperandType)) { resultPntr->result = Result_InvalidLeftOperand; resultPntr->errorPartIndex = part->index; }
					else if (!IsExpValueTypeBoolable(rightOperandType)) { resultPntr->result = Result_InvalidRightOperand; resultPntr->errorPartIndex = part->index; }
					{
						part->evalType = ExpValueType_Bool;
					}
				} break;
				
				// +==============================+
				// |    TypeCheck Not Operator    |
				// +==============================+
				case ExpOp_Not:
				{
					NotNull(part->child[0]);
					ExpValueType_t operandType = part->child[0]->evalType;
					Assert(operandType != ExpValueType_None);
					if (operandType == ExpValueType_Bool)
					{
						part->evalType = ExpValueType_Bool;
					}
					else if (IsExpValueTypeBoolable(operandType))
					{
						//The ! operator is able to convert things to bool
						part->evalType = ExpValueType_Bool;
					}
					else
					{
						resultPntr->result = Result_InvalidRightOperand;
						resultPntr->errorPartIndex = part->index;
					}
				} break;
				
				// +========================================+
				// | TypeCheck Bitwise Or/And/Xor Operators |
				// +========================================+
				case ExpOp_BitwiseOr:
				case ExpOp_BitwiseAnd:
				case ExpOp_BitwiseXor:
				{
					NotNull2(part->child[0], part->child[1]);
					ExpValueType_t leftOperandType = part->child[0]->evalType;
					ExpValueType_t rightOperandType = part->child[1]->evalType;
					Assert(leftOperandType != ExpValueType_None && rightOperandType != ExpValueType_None);
					Result_t mismatchReason = Result_None;
					part->evalType = GetExpIntegerTypeForBitwiseOp(leftOperandType, rightOperandType, (part->opType == ExpOp_And), &mismatchReason);
					if (part->evalType == ExpPartType_None)
					{
						resultPntr->result = mismatchReason;
						resultPntr->errorPartIndex = part->index;
					}
				} break;
				
				//TODO: ExpOp_BitwiseNot
				//TODO: ExpOp_Ternary
				//TODO: ExpOp_Assignment
				
				default: AssertMsg(false, "Unhandled ExpOp in ExpressionTypeCheckWalk_Callback"); break;
			}
		} break;
		
		case ExpPartType_Function:
		{
			if (context == nullptr) { resultPntr->result = Result_MissingContext; resultPntr->errorPartIndex = part->index; break; }
			ExpFuncDef_t* functionDef = VarArrayGet(&context->functionDefs, part->functionIndex, ExpFuncDef_t);
			Assert(part->childCount == functionDef->numArguments);
			for (u64 aIndex = 0; aIndex < functionDef->numArguments; aIndex++)
			{
				ExpPart_t* argument = part->child[aIndex];
				ExpFuncArg_t* argDef = &functionDef->arguments[aIndex];
				NotNull(argument);
				Assert(argument->evalType != ExpValueType_None);
				if (!CanExpValueTypeConvertTo(argument->evalType, argDef->type))
				{
					resultPntr->result = Result_InvalidArgument;
					resultPntr->errorPartIndex = part->index;
					break;
				}
			}
			if (resultPntr->result != Result_None) { break; }
			part->evalType = functionDef->returnType;
		} break;
		
		case ExpPartType_ParenthesisGroup:
		{
			NotNull(part->child[0]);
			Assert(part->child[0]->evalType != ExpValueType_None);
			part->evalType = part->child[0]->evalType;
		} break;
		
		default: AssertMsg(false, "Unhandled ExpPartType in ExpressionTypeCheckWalk_Callback"); resultPntr->result = Result_Unknown; break;
	}
}
//TODO: This should probably tell which part the type error occurred on!
Result_t ExpressionTypeCheckWalk(Expression_t* expression, const ExpressionContext_t* context = nullptr, u64* errorPartIndex = nullptr)
{
	ExpTypeCheckResult_t result = {};
	result.result = Result_None;
	StepThroughExpression(expression, ExpStepOrder_Postfix, ExpressionTypeCheckWalk_Callback, (ExpressionContext_t*)context, &result);
	if (result.result == Result_None) { result.result = Result_Success; }
	SetOptionalOutPntr(errorPartIndex, result.errorPartIndex);
	return result.result;
}

struct ExpEvaluateState_t
{
	u64 stackSize;
	ExpValue_t stack[EXPRESSIONS_MAX_EVAL_STACK_SIZE];
	Result_t result;
};

// +==============================+
// | EvaluateExpression_Callback  |
// +==============================+
// void EvaluateExpression_Callback(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpressionContext_t* context, void* userPntr)
EXP_STEP_CALLBACK(EvaluateExpression_Callback)
{
	NotNull3(expression, part, userPntr);
	ExpEvaluateState_t* state = (ExpEvaluateState_t*)userPntr;
	if (state->result != Result_None) { return; }
	
	switch (part->type)
	{
		case ExpPartType_Constant:
		{
			if (state->stackSize >= EXPRESSIONS_MAX_EVAL_STACK_SIZE) { state->result = Result_StackOverflow; return; }
			state->stack[state->stackSize++] = part->constantValue;
		} break;
		
		case ExpPartType_Variable:
		{
			if (state->stackSize >= EXPRESSIONS_MAX_EVAL_STACK_SIZE) { state->result = Result_StackOverflow; return; }
			ExpVariableDef_t* variableDef = VarArrayGet(&context->variableDefs, part->variableIndex, ExpVariableDef_t);
			state->stack[state->stackSize++] = ReadExpVariable_(variableDef);
		} break;
		
		case ExpPartType_Operator:
		{
			u8 numOperands = GetExpOperandCount(part->opType);
			if (state->stackSize < numOperands) { state->result = Result_InvalidStack; return; }
			ExpValue_t topOperand = state->stack[state->stackSize-1]; state->stackSize--;
			ExpValue_t nextOperand = {};
			ExpValue_t finalOperand = {};
			if (numOperands >= 2) { nextOperand = state->stack[state->stackSize-1]; state->stackSize--; }
			if (numOperands >= 3) { finalOperand = state->stack[state->stackSize-1]; state->stackSize--; }
			switch (part->opType)
			{
				// +========================================================+
				// | Evaluate Add/Subtract/Multiply/Divide/Modulo Operators |
				// +========================================================+
				case ExpOp_Add:
				case ExpOp_Subtract:
				case ExpOp_Multiply:
				case ExpOp_Divide:
				case ExpOp_Modulo:
				{
					ExpValueType_t commonType = GetExpResultTypeForMathOp(nextOperand.type, topOperand.type, (part->opType == ExpOp_Subtract));
					Assert(commonType != ExpValueType_None);
					ExpValue_t leftOperand = CastExpValue(nextOperand, commonType);
					ExpValue_t rightOperand = CastExpValue(topOperand, commonType);
					
					ExpValue_t result = {}; result.type = commonType;
					if (part->opType == ExpOp_Add)
					{
						switch (commonType)
						{
							case ExpValueType_R32: result.valueR32 = leftOperand.valueR32 + rightOperand.valueR32; break;
							case ExpValueType_R64: result.valueR64 = leftOperand.valueR64 + rightOperand.valueR64; break;
							case ExpValueType_I8:  result.valueI8  = leftOperand.valueI8  + rightOperand.valueI8;  break;
							case ExpValueType_I16: result.valueI16 = leftOperand.valueI16 + rightOperand.valueI16; break;
							case ExpValueType_I32: result.valueI32 = leftOperand.valueI32 + rightOperand.valueI32; break;
							case ExpValueType_I64: result.valueI64 = leftOperand.valueI64 + rightOperand.valueI64; break;
							case ExpValueType_U8:  result.valueU8  = leftOperand.valueU8  + rightOperand.valueU8;  break;
							case ExpValueType_U16: result.valueU16 = leftOperand.valueU16 + rightOperand.valueU16; break;
							case ExpValueType_U32: result.valueU32 = leftOperand.valueU32 + rightOperand.valueU32; break;
							case ExpValueType_U64: result.valueU64 = leftOperand.valueU64 + rightOperand.valueU64; break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_Subtract)
					{
						switch (commonType)
						{
							case ExpValueType_R32: result.valueR32 = leftOperand.valueR32 - rightOperand.valueR32; break;
							case ExpValueType_R64: result.valueR64 = leftOperand.valueR64 - rightOperand.valueR64; break;
							case ExpValueType_I8:  result.valueI8  = leftOperand.valueI8  - rightOperand.valueI8;  break;
							case ExpValueType_I16: result.valueI16 = leftOperand.valueI16 - rightOperand.valueI16; break;
							case ExpValueType_I32: result.valueI32 = leftOperand.valueI32 - rightOperand.valueI32; break;
							case ExpValueType_I64: result.valueI64 = leftOperand.valueI64 - rightOperand.valueI64; break;
							case ExpValueType_U8:  result.valueU8  = leftOperand.valueU8  - rightOperand.valueU8;  break;
							case ExpValueType_U16: result.valueU16 = leftOperand.valueU16 - rightOperand.valueU16; break;
							case ExpValueType_U32: result.valueU32 = leftOperand.valueU32 - rightOperand.valueU32; break;
							case ExpValueType_U64: result.valueU64 = leftOperand.valueU64 - rightOperand.valueU64; break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_Multiply)
					{
						switch (commonType)
						{
							case ExpValueType_R32: result.valueR32 = leftOperand.valueR32 * rightOperand.valueR32; break;
							case ExpValueType_R64: result.valueR64 = leftOperand.valueR64 * rightOperand.valueR64; break;
							case ExpValueType_I8:  result.valueI8  = leftOperand.valueI8  * rightOperand.valueI8;  break;
							case ExpValueType_I16: result.valueI16 = leftOperand.valueI16 * rightOperand.valueI16; break;
							case ExpValueType_I32: result.valueI32 = leftOperand.valueI32 * rightOperand.valueI32; break;
							case ExpValueType_I64: result.valueI64 = leftOperand.valueI64 * rightOperand.valueI64; break;
							case ExpValueType_U8:  result.valueU8  = leftOperand.valueU8  * rightOperand.valueU8;  break;
							case ExpValueType_U16: result.valueU16 = leftOperand.valueU16 * rightOperand.valueU16; break;
							case ExpValueType_U32: result.valueU32 = leftOperand.valueU32 * rightOperand.valueU32; break;
							case ExpValueType_U64: result.valueU64 = leftOperand.valueU64 * rightOperand.valueU64; break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_Divide)
					{
						switch (commonType)
						{
							case ExpValueType_R32: result.valueR32 = leftOperand.valueR32 / rightOperand.valueR32; break;
							case ExpValueType_R64: result.valueR64 = leftOperand.valueR64 / rightOperand.valueR64; break;
							case ExpValueType_I8:  result.valueI8  = leftOperand.valueI8  / rightOperand.valueI8;  break;
							case ExpValueType_I16: result.valueI16 = leftOperand.valueI16 / rightOperand.valueI16; break;
							case ExpValueType_I32: result.valueI32 = leftOperand.valueI32 / rightOperand.valueI32; break;
							case ExpValueType_I64: result.valueI64 = leftOperand.valueI64 / rightOperand.valueI64; break;
							case ExpValueType_U8:  result.valueU8  = leftOperand.valueU8  / rightOperand.valueU8;  break;
							case ExpValueType_U16: result.valueU16 = leftOperand.valueU16 / rightOperand.valueU16; break;
							case ExpValueType_U32: result.valueU32 = leftOperand.valueU32 / rightOperand.valueU32; break;
							case ExpValueType_U64: result.valueU64 = leftOperand.valueU64 / rightOperand.valueU64; break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_Modulo)
					{
						switch (commonType)
						{
							case ExpValueType_R32: result.valueR32 = ModR32(leftOperand.valueR32, rightOperand.valueR32); break;
							case ExpValueType_R64: result.valueR64 = ModR64(leftOperand.valueR64, rightOperand.valueR64); break;
							case ExpValueType_I8:  result.valueI8  = leftOperand.valueI8  % rightOperand.valueI8;  break;
							case ExpValueType_I16: result.valueI16 = leftOperand.valueI16 % rightOperand.valueI16; break;
							case ExpValueType_I32: result.valueI32 = leftOperand.valueI32 % rightOperand.valueI32; break;
							case ExpValueType_I64: result.valueI64 = leftOperand.valueI64 % rightOperand.valueI64; break;
							case ExpValueType_U8:  result.valueU8  = leftOperand.valueU8  % rightOperand.valueU8;  break;
							case ExpValueType_U16: result.valueU16 = leftOperand.valueU16 % rightOperand.valueU16; break;
							case ExpValueType_U32: result.valueU32 = leftOperand.valueU32 % rightOperand.valueU32; break;
							case ExpValueType_U64: result.valueU64 = leftOperand.valueU64 % rightOperand.valueU64; break;
							default: Assert(false); break;
						}
					}
					
					state->stack[state->stackSize++] = result;
				} break;
				
				// +======================================+
				// | Evaluate Equals/NotEquals Operators  |
				// +======================================+
				case ExpOp_Equals:
				case ExpOp_NotEquals:
				{
					ExpValueType_t commonType = GetExpCommonTypeForComparisonOp(nextOperand.type, topOperand.type);
					Assert(commonType != ExpValueType_None);
					ExpValue_t leftOperand = CastExpValue(nextOperand, commonType);
					ExpValue_t rightOperand = CastExpValue(topOperand, commonType);
					
					ExpValue_t result = {};
					result.type = ExpValueType_Bool;
					switch (commonType)
					{
						case ExpValueType_Bool:    result.valueBool = (leftOperand.valueBool == rightOperand.valueBool); break;
						case ExpValueType_Pointer: result.valueBool = (leftOperand.valuePntr == rightOperand.valuePntr); break;
						case ExpValueType_String:  result.valueBool = StrEquals(leftOperand.valueStr, rightOperand.valueStr); break;
						case ExpValueType_R32:     result.valueBool = (leftOperand.valueR32  == rightOperand.valueR32);  break;
						case ExpValueType_R64:     result.valueBool = (leftOperand.valueR64  == rightOperand.valueR64);  break;
						case ExpValueType_I8:      result.valueBool = (leftOperand.valueI8   == rightOperand.valueI8);   break;
						case ExpValueType_I16:     result.valueBool = (leftOperand.valueI16  == rightOperand.valueI16);  break;
						case ExpValueType_I32:     result.valueBool = (leftOperand.valueI32  == rightOperand.valueI32);  break;
						case ExpValueType_I64:     result.valueBool = (leftOperand.valueI64  == rightOperand.valueI64);  break;
						case ExpValueType_U8:      result.valueBool = (leftOperand.valueU8   == rightOperand.valueU8);   break;
						case ExpValueType_U16:     result.valueBool = (leftOperand.valueU16  == rightOperand.valueU16);  break;
						case ExpValueType_U32:     result.valueBool = (leftOperand.valueU32  == rightOperand.valueU32);  break;
						case ExpValueType_U64:     result.valueBool = (leftOperand.valueU64  == rightOperand.valueU64);  break;
						default: Assert(false); break;
					}
					if (part->opType == ExpOp_NotEquals) { result.valueBool = !result.valueBool; }
					
					state->stack[state->stackSize++] = result;
				} break;
				
				// +==============================+
				// |  Evaluate Or/And Operators   |
				// +==============================+
				case ExpOp_Or:
				case ExpOp_And:
				{
					Assert(IsExpValueTypeBoolable(nextOperand.type) && IsExpValueTypeBoolable(topOperand.type));
					ExpValue_t leftOperand = CastExpValue(nextOperand, ExpValueType_Bool);
					ExpValue_t rightOperand = CastExpValue(topOperand, ExpValueType_Bool);
					ExpValue_t result = {};
					result.type = ExpValueType_Bool;
					if (part->opType == ExpOp_Or)  { result.valueBool = (leftOperand.valueBool || rightOperand.valueBool); }
					if (part->opType == ExpOp_And) { result.valueBool = (leftOperand.valueBool && rightOperand.valueBool); }
					state->stack[state->stackSize++] = result;
				} break;
				
				// +==============================+
				// |    Evaluate Not Operator     |
				// +==============================+
				case ExpOp_Not:
				{
					ExpValue_t result = CastExpValue(topOperand, ExpValueType_Bool);
					result.valueBool = !result.valueBool;
					state->stack[state->stackSize++] = result;
				} break;
				
				// +========================================+
				// | Evaluate Bitwise Or/And/Xor Operators  |
				// +========================================+
				case ExpOp_BitwiseOr:
				case ExpOp_BitwiseAnd:
				case ExpOp_BitwiseXor:
				{
					ExpValueType_t resultType = GetExpIntegerTypeForBitwiseOp(nextOperand.type, topOperand.type, (part->opType == ExpOp_And));
					Assert(resultType != ExpValueType_None);
					ExpValue_t leftOperand = CastExpValue(nextOperand, resultType);
					ExpValue_t rightOperand = CastExpValue(topOperand, resultType);
					
					ExpValue_t result = {};
					result.type = resultType;
					if (part->opType == ExpOp_BitwiseOr)
					{
						switch (resultType)
						{
							case ExpValueType_I8:  result.valueI8  = (leftOperand.valueI8  | rightOperand.valueI8);  break;
							case ExpValueType_I16: result.valueI16 = (leftOperand.valueI16 | rightOperand.valueI16); break;
							case ExpValueType_I32: result.valueI32 = (leftOperand.valueI32 | rightOperand.valueI32); break;
							case ExpValueType_I64: result.valueI64 = (leftOperand.valueI64 | rightOperand.valueI64); break;
							case ExpValueType_U8:  result.valueU8  = (leftOperand.valueU8  | rightOperand.valueU8);  break;
							case ExpValueType_U16: result.valueU16 = (leftOperand.valueU16 | rightOperand.valueU16); break;
							case ExpValueType_U32: result.valueU32 = (leftOperand.valueU32 | rightOperand.valueU32); break;
							case ExpValueType_U64: result.valueU64 = (leftOperand.valueU64 | rightOperand.valueU64); break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_BitwiseAnd)
					{
						switch (resultType)
						{
							case ExpValueType_I8:  result.valueI8  = (leftOperand.valueI8  & rightOperand.valueI8);  break;
							case ExpValueType_I16: result.valueI16 = (leftOperand.valueI16 & rightOperand.valueI16); break;
							case ExpValueType_I32: result.valueI32 = (leftOperand.valueI32 & rightOperand.valueI32); break;
							case ExpValueType_I64: result.valueI64 = (leftOperand.valueI64 & rightOperand.valueI64); break;
							case ExpValueType_U8:  result.valueU8  = (leftOperand.valueU8  & rightOperand.valueU8);  break;
							case ExpValueType_U16: result.valueU16 = (leftOperand.valueU16 & rightOperand.valueU16); break;
							case ExpValueType_U32: result.valueU32 = (leftOperand.valueU32 & rightOperand.valueU32); break;
							case ExpValueType_U64: result.valueU64 = (leftOperand.valueU64 & rightOperand.valueU64); break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_BitwiseXor)
					{
						switch (resultType)
						{
							case ExpValueType_I8:  result.valueI8  = (leftOperand.valueI8  ^ rightOperand.valueI8);  break;
							case ExpValueType_I16: result.valueI16 = (leftOperand.valueI16 ^ rightOperand.valueI16); break;
							case ExpValueType_I32: result.valueI32 = (leftOperand.valueI32 ^ rightOperand.valueI32); break;
							case ExpValueType_I64: result.valueI64 = (leftOperand.valueI64 ^ rightOperand.valueI64); break;
							case ExpValueType_U8:  result.valueU8  = (leftOperand.valueU8  ^ rightOperand.valueU8);  break;
							case ExpValueType_U16: result.valueU16 = (leftOperand.valueU16 ^ rightOperand.valueU16); break;
							case ExpValueType_U32: result.valueU32 = (leftOperand.valueU32 ^ rightOperand.valueU32); break;
							case ExpValueType_U64: result.valueU64 = (leftOperand.valueU64 ^ rightOperand.valueU64); break;
							default: Assert(false); break;
						}
					}
					
					state->stack[state->stackSize++] = result;
				}
				//TODO: ExpOp_BitwiseNot
				//TODO: ExpOp_Ternary
				//TODO: ExpOp_Assignment
			}
		} break;
		
		case ExpPartType_Function:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		case ExpPartType_ParenthesisGroup:
		{
			 //We don't need to do anything for parenthesis at evaluation time
		} break;
		
		default: AssertMsg(false, "Unhandled ExpPartType in EvaluateExpression_Callback"); break;
	}
}
Result_t EvaluateExpression(Expression_t* expression, ExpressionContext_t* context = nullptr, ExpValue_t* valueOut = nullptr)
{
	ExpEvaluateState_t state = {};
	state.stackSize = 0;
	state.result = Result_None;
	StepThroughExpression(expression, ExpStepOrder_Postfix, EvaluateExpression_Callback, (ExpressionContext_t*)context, &state);
	if (state.stackSize == 0) { return Result_EmptyExpression; }
	if (state.stackSize > 1) { return Result_InvalidStack; }
	if (state.result == Result_None) { state.result = Result_Success; }
	SetOptionalOutPntr(valueOut, state.stack[0]);
	return state.result;
}

// +--------------------------------------------------------------+
// |                     Ease of Use Wrappers                     |
// +--------------------------------------------------------------+
Result_t TryRunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpressionContext_t* context = nullptr)
{
	PushMemMark(scratchArena);
	
	ExpressionContext_t emptyContext = {};
	if (context == nullptr) { context = &emptyContext; }
	
	u64 numTokens = 0;
	ExpToken_t* tokens = nullptr;
	Result_t tokenizeResult = TryTokenizeExpressionStr(expressionStr, scratchArena, &tokens, &numTokens);
	if (tokenizeResult != Result_Success)
	{
		PopMemMark(scratchArena);
		return tokenizeResult;
	}
	AssertIf(numTokens > 0, tokens != nullptr);
	
	Expression_t expression = {};
	Result_t parseResult = TryCreateExpressionFromTokens(context, numTokens, tokens, &expression);
	if (parseResult != Result_Success)
	{
		PopMemMark(scratchArena);
		return parseResult;
	}
	
	Result_t typeCheckResult = ExpressionTypeCheckWalk(&expression, context);
	if (typeCheckResult != Result_Success)
	{
		PopMemMark(scratchArena);
		return typeCheckResult;
	}
	
	Result_t evaluateResult = EvaluateExpression(&expression, context, valueOut);
	if (evaluateResult != Result_Success)
	{
		PopMemMark(scratchArena);
		return evaluateResult;
	}
	
	PopMemMark(scratchArena);
	return Result_Success;
}

MyStr_t TryRunExpressionErrorStr(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpressionContext_t* context = nullptr)
{
	ExpressionContext_t emptyContext = {};
	if (context == nullptr) { context = &emptyContext; }
	
	u64 numTokens = 0;
	ExpToken_t* tokens = nullptr;
	Result_t tokenizeResult = TryTokenizeExpressionStr(expressionStr, scratchArena, &tokens, &numTokens);
	if (tokenizeResult != Result_Success)
	{
		//TODO: Can we get the character range where the syntax error occurred?
		return PrintInArenaStr(scratchArena, "Invalid syntax: %s", GetResultStr(tokenizeResult));
	}
	AssertIf(numTokens > 0, tokens != nullptr);
	
	Expression_t expression = {};
	Result_t parseResult = TryCreateExpressionFromTokens(context, numTokens, tokens, &expression);
	if (parseResult != Result_Success)
	{
		//TODO: Can we get the character range where the parsing error occurred?
		return PrintInArenaStr(scratchArena, "Invalid expression: %s", GetResultStr(parseResult));
	}
	
	u64 errorPartIndex = 0;
	Result_t typeCheckResult = ExpressionTypeCheckWalk(&expression, context, &errorPartIndex);
	if (typeCheckResult != Result_Success)
	{
		ExpToken_t* errorToken = &tokens[expression.parts[errorPartIndex].tokenIndex];
		return PrintInArenaStr(scratchArena, "Type check failure: %s on part[%llu] \"%.*s\"", GetResultStr(typeCheckResult), errorPartIndex, StrPrint(errorToken->str));
	}
	
	Result_t evaluateResult = EvaluateExpression(&expression, context, valueOut);
	if (evaluateResult != Result_Success)
	{
		return PrintInArenaStr(scratchArena, "Evaluation failed: %s", GetResultStr(evaluateResult));
	}
	
	return MyStr_Empty;
}

ExpValue_t RunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpressionContext_t* context = nullptr)
{
	ExpValue_t value = {};
	Result_t result = TryRunExpression(expressionStr, scratchArena, &value, context);
	if (result != Result_Success) { value.type = ExpValueType_None; }
	return value;
}

//TODO: Implement the rest of me!

#endif GYLIB_HEADER_ONLY

#endif //  _GY_EXPRESSION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
EXPRESSIONS_MAX_PART_CHILDREN
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
