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

//TODO: The following expressions don't quite act like we want?
//   false ? "first" : false ? "second" : "third"
//   false ? false ? "second" : "third" : "first"
// Maybe the ternary operator needs to be higher precedence than itself?

//TODO: These expressions are failing:
//		"(bool)0 + 1 || false"
//		"(bool)0 + (bool)1 || false"
//		"false + true || false"
//		"(false + true) || false"
//		"((bool)0 + (bool)1) || false"
//		"((bool)0 || (bool)1) || false"

//TODO: Add support for named constants like pi?
//TODO: Add a deep copy function so we can duplicate and Expression_t or move it somewhere else in memory
//TODO: Add support for multiple contexts being combined together without needing to merge the variable and function def lists into a single space in memory. Maybe make them a linked list?

//TODO: Add support for functions overloads with different types but same number of arguments
//TODO: Implement BitwiseNot operator
//TODO: Add proper support for optional arguments on functions
//TODO: Add PIGGEN support for registering a function as callback by expressions
//TODO: We need to return more error information from Evaluate, Parse, and Tokenize so that the error reporting can tell you where in the expression the problem occurred
//TODO: Should/Could we add support for composite types like v2, v3, v2i, v3i, rec, reci?
//TODO: Add comment syntax to the language (strip them out during tokenization)
//TODO: Add support for semicolons and producing multiple expressions from a single string
//TODO: Bitwise operators on bools should be allowed? Also make sure that |= and &= work when assigning a bool
//TODO: Does the negation operator work on variables? What about a subtraction operator that is right next to a number literal?
//TODO: Ternary operators should be higher precedence than themselves? So we can chain them together without parenthesis?
//TODO: Figure out exactly how we are going to manage TypeId value and enforcement!
//TODO: Function overrides with same argument counts and different typed arguments do not currently work!
//TODO: Add a way to register "enum" types with the ExpContext so that enum values can be parsed without the need for quotes and with parse-time type checking and boiling down to integer values
//TODO: Now that we have type casting, we can be more strict in the type checking phase?
//TODO: Can we define restrictions on number arguments to be within a particular range? Also maybe strings not empty and pointers not null?
//TODO: Should we allow strings defined with ' ' characters? So we can use them inside string literals without escaping?
//TODO: Should we allow suffixes to large numbers, like 1kB or 8MB meaning 1024 and 8388608? Or maybe we should just register functions like "u64 kilo(u64 num_kilobytes)"
//TODO: Can we make the variable and function name lookups faster by building a hash table during context creation?
//TODO: Update the Autocomplete Dictionary at the bottom of the file, and the GYLIB_HEADER_ONLY section
//TODO: Should we make an identifier automatically call a argumentless function if isConsoleInput and no () is typed?

//TODO: Add nullptr constant support
//TODO: Add support for typing numbers in hexadecimal
//TODO: Add ++ and -- operators?
//TODO: Should we short-circuit && and || operators when left-hand side is false/true?
//TODO: Should we add a "dump" function to the base language? Or maybe we just leave that up to the context?

#ifndef _GY_EXPRESSION_H
#define _GY_EXPRESSION_H

// +--------------------------------------------------------------+
// |                     Defines and Typedefs                     |
// +--------------------------------------------------------------+
#define EXPRESSIONS_MAX_PART_CHILDREN     8
#define EXPRESSIONS_MAX_FUNC_ARGS         EXPRESSIONS_MAX_PART_CHILDREN
#define EXPRESSIONS_MAX_PARSE_STACK_SIZE  16
#define EXPRESSIONS_MAX_EVAL_STACK_SIZE   16
#define EXPRESSIONS_MAX_NUM_PARTS         128

#define EXPRESSION_FUNC_DEFINITION(functionName) struct ExpValue_t functionName(struct Expression_t* expression, struct ExpContext_t* context, u64 numArgs, const struct ExpValue_t* args)
typedef EXPRESSION_FUNC_DEFINITION(ExpressionFunc_f);
#define EXPRESSION_VAR_GETTER(type, functionName) type functionName(struct Expression_t* expression, struct ExpContext_t* context, struct ExpVariableDef_t* variableDef)
typedef EXPRESSION_VAR_GETTER(bool,    ExpGetterBool_f);
typedef EXPRESSION_VAR_GETTER(void*,   ExpGetterPntr_f);
typedef EXPRESSION_VAR_GETTER(MyStr_t, ExpGetterStr_f);
typedef EXPRESSION_VAR_GETTER(r32,     ExpGetterR32_f);
typedef EXPRESSION_VAR_GETTER(r64,     ExpGetterR64_f);
typedef EXPRESSION_VAR_GETTER(i8,      ExpGetterI8_f);
typedef EXPRESSION_VAR_GETTER(i16,     ExpGetterI16_f);
typedef EXPRESSION_VAR_GETTER(i32,     ExpGetterI32_f);
typedef EXPRESSION_VAR_GETTER(i64,     ExpGetterI64_f);
typedef EXPRESSION_VAR_GETTER(u8,      ExpGetterU8_f);
typedef EXPRESSION_VAR_GETTER(u16,     ExpGetterU16_f);
typedef EXPRESSION_VAR_GETTER(u32,     ExpGetterU32_f);
typedef EXPRESSION_VAR_GETTER(u64,     ExpGetterU64_f);
#define EXPRESSION_VAR_SETTER(type, functionName) void functionName(struct Expression_t* expression, struct ExpContext_t* context, struct ExpVariableDef_t* variableDef, type newValue)
typedef EXPRESSION_VAR_SETTER(bool,    ExpSetterBool_f);
typedef EXPRESSION_VAR_SETTER(void*,   ExpSetterPntr_f);
typedef EXPRESSION_VAR_SETTER(MyStr_t, ExpSetterStr_f);
typedef EXPRESSION_VAR_SETTER(r32,     ExpSetterR32_f);
typedef EXPRESSION_VAR_SETTER(r64,     ExpSetterR64_f);
typedef EXPRESSION_VAR_SETTER(i8,      ExpSetterI8_f);
typedef EXPRESSION_VAR_SETTER(i16,     ExpSetterI16_f);
typedef EXPRESSION_VAR_SETTER(i32,     ExpSetterI32_f);
typedef EXPRESSION_VAR_SETTER(i64,     ExpSetterI64_f);
typedef EXPRESSION_VAR_SETTER(u8,      ExpSetterU8_f);
typedef EXPRESSION_VAR_SETTER(u16,     ExpSetterU16_f);
typedef EXPRESSION_VAR_SETTER(u32,     ExpSetterU32_f);
typedef EXPRESSION_VAR_SETTER(u64,     ExpSetterU64_f);

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
	//TODO: Should we add v2, v3, v2i, v3i here?
	ExpValueType_NumTypes,
};

#ifdef GYLIB_HEADER_ONLY
const char* GetExpValueTypeStr(ExpValueType_t enumValue);
const char* GetExpValueTypeStrLower(ExpValueType_t enumValue);
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
const char* GetExpValueTypeStrLower(ExpValueType_t enumValue)
{
	switch (enumValue)
	{
		case ExpValueType_None:    return "none";
		case ExpValueType_Void:    return "void";
		case ExpValueType_Bool:    return "bool";
		case ExpValueType_Pointer: return "pointer";
		case ExpValueType_String:  return "string";
		case ExpValueType_R32:     return "r32";
		case ExpValueType_R64:     return "r64";
		case ExpValueType_I8:      return "i8";
		case ExpValueType_I16:     return "i16";
		case ExpValueType_I32:     return "i32";
		case ExpValueType_I64:     return "i64";
		case ExpValueType_U8:      return "u8";
		case ExpValueType_U16:     return "u16";
		case ExpValueType_U32:     return "u32";
		case ExpValueType_U64:     return "u64";
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
	ExpOp_GreaterThan,
	ExpOp_GreaterThanOrEqual,
	ExpOp_LessThan,
	ExpOp_LessThanOrEqual,
	ExpOp_Or,
	ExpOp_And,
	ExpOp_Not,
	ExpOp_BitwiseOr,
	ExpOp_BitwiseAnd,
	ExpOp_BitwiseXor,
	ExpOp_BitwiseNot,
	ExpOp_Ternary,
	ExpOp_Assignment,
	ExpOp_AssignmentAdd,
	ExpOp_AssignmentSubtract,
	ExpOp_AssignmentMultiply,
	ExpOp_AssignmentDivide,
	ExpOp_AssignmentBitwiseOr,
	ExpOp_AssignmentBitwiseAnd,
	ExpOp_AssignmentBitwiseXor,
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
		case ExpOp_None:                 return "None";
		case ExpOp_Add:                  return "Add";
		case ExpOp_Subtract:             return "Subtract";
		case ExpOp_Multiply:             return "Multiply";
		case ExpOp_Divide:               return "Divide";
		case ExpOp_Modulo:               return "Modulo";
		case ExpOp_Equals:               return "Equals";
		case ExpOp_NotEquals:            return "NotEquals";
		case ExpOp_GreaterThan:          return "GreaterThan";
		case ExpOp_GreaterThanOrEqual:   return "GreaterThanOrEqual";
		case ExpOp_LessThan:             return "LessThan";
		case ExpOp_LessThanOrEqual:      return "LessThanOrEqual";
		case ExpOp_Or:                   return "Or";
		case ExpOp_And:                  return "And";
		case ExpOp_Not:                  return "Not";
		case ExpOp_BitwiseOr:            return "BitwiseOr";
		case ExpOp_BitwiseAnd:           return "BitwiseAnd";
		case ExpOp_BitwiseXor:           return "BitwiseXor";
		case ExpOp_BitwiseNot:           return "BitwiseNot";
		case ExpOp_Ternary:              return "Ternary";
		case ExpOp_Assignment:           return "Assignment";
		case ExpOp_AssignmentAdd:        return "AssignmentAdd";
		case ExpOp_AssignmentSubtract:   return "AssignmentSubtract";
		case ExpOp_AssignmentMultiply:   return "AssignmentMultiply";
		case ExpOp_AssignmentDivide:     return "AssignmentDivide";
		case ExpOp_AssignmentBitwiseOr:  return "AssignmentBitwiseOr";
		case ExpOp_AssignmentBitwiseAnd: return "AssignmentBitwiseAnd";
		case ExpOp_AssignmentBitwiseXor: return "AssignmentBitwiseXor";
		default: return "Unknown";
	}
}
const char* GetExpOpSyntaxStr(ExpOp_t enumValue, bool secondPart = false)
{
	switch (enumValue)
	{
		case ExpOp_Add:                  return "+";
		case ExpOp_Subtract:             return "-";
		case ExpOp_Multiply:             return "*";
		case ExpOp_Divide:               return "/";
		case ExpOp_Modulo:               return "%";
		case ExpOp_Equals:               return "==";
		case ExpOp_NotEquals:            return "!=";
		case ExpOp_GreaterThan:          return ">";
		case ExpOp_GreaterThanOrEqual:   return ">=";
		case ExpOp_LessThan:             return "<";
		case ExpOp_LessThanOrEqual:      return "<=";
		case ExpOp_Or:                   return "||";
		case ExpOp_And:                  return "&&";
		case ExpOp_Not:                  return "!";
		case ExpOp_BitwiseOr:            return "|";
		case ExpOp_BitwiseAnd:           return "&";
		case ExpOp_BitwiseXor:           return "^";
		case ExpOp_BitwiseNot:           return "~";
		case ExpOp_Ternary:              return (secondPart ? ":" : "?");
		case ExpOp_Assignment:           return "=";
		case ExpOp_AssignmentAdd:        return "+=";
		case ExpOp_AssignmentSubtract:   return "-=";
		case ExpOp_AssignmentMultiply:   return "*=";
		case ExpOp_AssignmentDivide:     return "/=";
		case ExpOp_AssignmentBitwiseOr:  return "|=";
		case ExpOp_AssignmentBitwiseAnd: return "&=";
		case ExpOp_AssignmentBitwiseXor: return "^=";
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
	ExpPartType_TypeCast,
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
		case ExpPartType_TypeCast:         return "TypeCast";
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
	//locked children are ones that can't be stolen by later higher precedence operators, because a syntactically element already occurred to distance them from later operators
	//left-hand children are always locked in (because the operator came after). The first branch of a ternary operator gets locked in when we find a ':' character
	bool childLocked[EXPRESSIONS_MAX_PART_CHILDREN];
	// ExpPartType_Constant
	ExpValue_t constantValue;
	// ExpPartType_Operator
	ExpOp_t opType;
	// ExpPartType_Variable
	u64 variableIndex;
	// ExpPartType_Function
	u64 functionIndex;
	// ExpPartType_TypeCast
	ExpValueType_t castType;
};

struct ExpPartStack_t
{
	u64 length;
	ExpPart_t* parts[EXPRESSIONS_MAX_PARSE_STACK_SIZE];
};

struct ExpVariableDef_t
{
	bool isReadOnly;
	ExpValueType_t type;
	u64 pntrTypeId; //Only used to ValueType_Pointer
	MyStr_t name;
	MyStr_t documentation;
	void* pntr;
	void* getter;
	void* setter;
};

struct ExpConstantDef_t
{
	ExpValue_t value;
	MyStr_t name;
	MyStr_t documentation;
};

struct ExpFuncArg_t
{
	ExpValueType_t type;
	MyStr_t name;
	MyStr_t documentation;
	bool isOptional; //TODO: Implement handling of this!
	ExpValue_t defaultValue;
};

struct ExpFuncDef_t
{
	ExpValueType_t returnType;
	MyStr_t name;
	MyStr_t documentation;
	u64 numArguments;
	ExpFuncArg_t arguments[EXPRESSIONS_MAX_FUNC_ARGS];
	ExpressionFunc_f* pntr;
};

struct ExpContext_t
{
	MemArena_t* allocArena;
	bool allocateStrings;
	VarArray_t constantDefs; //ExpConstantDef_t
	VarArray_t variableDefs; //ExpVariableDef_t
	VarArray_t functionDefs; //ExpFuncDef_t
	// Turns on some functionality that makes debug console experience better, at the expense of conflicting with functionality that would be used for other expressions that are usually used to produce a value
	bool isConsoleInput;
	MemArena_t* scratch; //Optional scratch arena pointer, meant to be used as a place to put strings that only need to last as long as the evaluation of a particular expression
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

#define EXP_STEP_CALLBACK(functionName) void functionName(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpContext_t* context, void* userPntr)
typedef EXP_STEP_CALLBACK(ExpStepCallback_f);

struct ExpTypeCheckState_t
{
	Result_t result;
	u64 errorPartIndex;
};

struct ExpEvaluateState_t
{
	u64 stackSize;
	ExpValue_t stack[EXPRESSIONS_MAX_EVAL_STACK_SIZE];
	Result_t result;
};

struct ExpAutocompleteInfo_t
{
	const ExpContext_t* context;
	MyStr_t expressionStr;
	u64 cursorIndex;
	
	i64 parensBeginIndex;
	i64 parensEndIndex;
	
	bool isBetweenTokens; //=|
	bool isInsideToken;   // |== Only one of these should be true at a time
	bool isNextToToken;   //=|
	bool insideFuncArgs;
	bool funcDefFound;
	bool isAtBeginning;
	bool isAtEnd;
	
	//if not isAtBeginning
	u64 prevTokenIndex;
	u64 prevTokenStartIndex;
	u64 prevTokenEndIndex;
	ExpTokenType_t prevTokenType;
	
	//if not isAtEnd
	u64 nextTokenIndex;
	u64 nextTokenStartIndex;
	u64 nextTokenEndIndex;
	ExpTokenType_t nextTokenType;
	
	//if isInsideToken or isNextToToken (aka not isBetweenTokens)
	u64 currentTokenIndex;
	u64 currentTokenStartIndex;
	u64 currentTokenEndIndex;
	u64 currentTokenCursorIndex;
	ExpTokenType_t currentTokenType;
	MyStr_t currentTokenStr;
	
	//if insideFuncArgs
	u64 currentFuncNameStartIndex;
	u64 currentFuncNameEndIndex;
	MyStr_t currentFuncNameStr;
	u64 currentFuncArgCount; //this may not match the ExpFuncDef exactly, it's just based on how many commas the user has typed in this scope
	u64 currentFuncArgIndex;
	
	//if funcDefFound
	u64 currentFuncDefIndex;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define EXP_GET_ARG_BOOL(index, argName) Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_Bool);    bool    argName = args[index].valueBool
// #define EXP_GET_ARG_PNTR(index, argName) Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_Pointer); void*   argName = args[index].valuePntr //TODO: Implement this with a typeId check somehow?
#define EXP_GET_ARG_STR(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_String);  MyStr_t argName = args[index].valueStr
#define EXP_GET_ARG_R32(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_R32);     r32     argName = args[index].valueR32
#define EXP_GET_ARG_R64(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_R64);     r64     argName = args[index].valueR64
#define EXP_GET_ARG_I8(index, argName)   Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_I8);      i8      argName = args[index].valueI8
#define EXP_GET_ARG_I16(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_I16);     i16     argName = args[index].valueI16
#define EXP_GET_ARG_I32(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_I32);     i32     argName = args[index].valueI32
#define EXP_GET_ARG_I64(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_I64);     i64     argName = args[index].valueI32
#define EXP_GET_ARG_U8(index, argName)   Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_U8);      u8      argName = args[index].valueU8
#define EXP_GET_ARG_U16(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_U16);     u16     argName = args[index].valueU16
#define EXP_GET_ARG_U32(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_U32);     u32     argName = args[index].valueU32
#define EXP_GET_ARG_U64(index, argName)  Assert(numArgs >= ((index)+1) && args != nullptr && args[index].type == ExpValueType_U64);     u64     argName = args[index].valueU64

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeExpValue(MemArena_t* allocArena, ExpValue_t* value);
	void FreeExpression(Expression_t* expression);
	void FreeExpFuncDef(ExpFuncDef_t* funcDef, MemArena_t* allocArena);
	void FreeExpContext(ExpContext_t* context);
	void CopyExpFuncDef(ExpFuncDef_t* dest, ExpFuncDef_t* source, MemArena_t* memArena = nullptr);
	ExpValue_t NewExpValueVoid();
	ExpValue_t NewExpValueBool(bool value);
	ExpValue_t NewExpValueStr(MyStr_t value);
	ExpValue_t NewExpValueStr(const char* valueNt);
	ExpValue_t NewExpValuePntr(u64 typeId, void* value);
	ExpValue_t NewExpValueR32(r32 value);
	ExpValue_t NewExpValueR64(r64 value);
	ExpValue_t NewExpValueI8(i8 value);
	ExpValue_t NewExpValueI16(i16 value);
	ExpValue_t NewExpValueI32(i32 value);
	ExpValue_t NewExpValueI64(i64 value);
	ExpValue_t NewExpValueU8(u8 value);
	ExpValue_t NewExpValueU16(u16 value);
	ExpValue_t NewExpValueU32(u32 value);
	ExpValue_t NewExpValueU64(u64 value);
	u8 GetExpOperandCount(ExpOp_t opType);
	u8 GetExpOpPrecedence(ExpOp_t opType);
	bool IsExpPartReadyToBeOperand(const ExpPart_t* expPart);
	bool CanExpPartProduceLeftHandOperand(const ExpPart_t* expPart);
	inline bool IsExpValueTypeInteger(ExpValueType_t type);
	inline bool IsExpValueTypeFloat(ExpValueType_t type);
	inline bool IsExpValueTypeSigned(ExpValueType_t type);
	inline bool IsExpValueTypeNumber(ExpValueType_t type);
	inline bool IsExpValueTypeConstantCompat(ExpValueType_t type);
	inline bool IsExpValueTypeBoolable(ExpValueType_t type);
	inline bool CanExpValueTypeConvertTo(ExpValueType_t type, ExpValueType_t outType);
	inline bool CanCastExpValueTo(ExpValueType_t valueType, ExpValueType_t type);
	inline u8 GetExpValueTypeByteSize(ExpValueType_t type);
	MyStr_t ExpValueToStr(ExpValue_t value, MemArena_t* memArena, bool includeType = false);
	void InitExpContext(MemArena_t* memArena, ExpContext_t* contextOut, bool allocateStrings = true);
	ExpConstantDef_t* FindExpConstantDef(ExpContext_t* context, MyStr_t constantName, u64* indexOut = nullptr);
	const ExpConstantDef_t* FindExpConstantDef(const ExpContext_t* context, MyStr_t constantName, u64* indexOut = nullptr);
	ExpVariableDef_t* FindExpVariableDef(ExpContext_t* context, MyStr_t variableName, u64* indexOut = nullptr);
	const ExpVariableDef_t* FindExpVariableDef(const ExpContext_t* context, MyStr_t variableName, u64* indexOut = nullptr);
	ExpFuncDef_t* FindExpFuncDef(ExpContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr);
	const ExpFuncDef_t* FindExpFuncDef(const ExpContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr);
	ExpConstantDef_t* AddExpConstantDef(ExpContext_t* context, MyStr_t constantName, ExpValue_t value, MyStr_t documentation = MyStr_Empty_Const);
	ExpConstantDef_t* AddExpConstantDef(ExpContext_t* context, const char* constantNameNt, ExpValue_t value, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDef_(ExpContext_t* context, bool writeable, MyStr_t variableName, ExpValueType_t type, u64 pntrSize, const void* pntr, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefFuncs_(ExpContext_t* context, MyStr_t variableName, ExpValueType_t type, void* getter, void* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, bool writeable, MyStr_t     variableName,   const bool* pntr,    MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, bool writeable, const char* variableNameNt, const bool* pntr,    const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, MyStr_t     variableName,   ExpGetterBool_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, const char* variableNameNt, ExpGetterBool_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, MyStr_t     variableName,   ExpGetterBool_f* getter, ExpSetterBool_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, const char* variableNameNt, ExpGetterBool_f* getter, ExpSetterBool_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, bool writeable, MyStr_t     variableName,   const void** pntr,   MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, bool writeable, const char* variableNameNt, const void** pntr,   const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, MyStr_t     variableName,   ExpGetterPntr_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, const char* variableNameNt, ExpGetterPntr_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, MyStr_t     variableName,   ExpGetterPntr_f* getter, ExpSetterPntr_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, const char* variableNameNt, ExpGetterPntr_f* getter, ExpSetterPntr_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const MyStr_t* pntr, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, bool writeable, const char* variableNameNt, const MyStr_t* pntr, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterStr_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, const char* variableNameNt, ExpGetterStr_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterStr_f* getter, ExpSetterStr_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, const char* variableNameNt, ExpGetterStr_f* getter, ExpSetterStr_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const r32* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, bool writeable, const char* variableNameNt, const r32* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR32_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, const char* variableNameNt, ExpGetterR32_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR32_f* getter, ExpSetterR32_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, const char* variableNameNt, ExpGetterR32_f* getter, ExpSetterR32_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const r64* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, bool writeable, const char* variableNameNt, const r64* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR64_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, const char* variableNameNt, ExpGetterR64_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR64_f* getter, ExpSetterR64_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, const char* variableNameNt, ExpGetterR64_f* getter, ExpSetterR64_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, bool writeable, MyStr_t     variableName,   const i8*  pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, bool writeable, const char* variableNameNt, const i8*  pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterI8_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, const char* variableNameNt, ExpGetterI8_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterI8_f* getter, ExpSetterI8_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, const char* variableNameNt, ExpGetterI8_f* getter, ExpSetterI8_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i16* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, bool writeable, const char* variableNameNt, const i16* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI16_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, const char* variableNameNt, ExpGetterI16_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI16_f* getter, ExpSetterI16_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, const char* variableNameNt, ExpGetterI16_f* getter, ExpSetterI16_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i32* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, bool writeable, const char* variableNameNt, const i32* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI32_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, const char* variableNameNt, ExpGetterI32_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI32_f* getter, ExpSetterI32_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, const char* variableNameNt, ExpGetterI32_f* getter, ExpSetterI32_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i64* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, bool writeable, const char* variableNameNt, const i64* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI64_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, const char* variableNameNt, ExpGetterI64_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI64_f* getter, ExpSetterI64_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, const char* variableNameNt, ExpGetterI64_f* getter, ExpSetterI64_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, bool writeable, MyStr_t     variableName,   const u8*  pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, bool writeable, const char* variableNameNt, const u8*  pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterU8_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, const char* variableNameNt, ExpGetterU8_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterU8_f* getter, ExpSetterU8_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, const char* variableNameNt, ExpGetterU8_f* getter, ExpSetterU8_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u16* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, bool writeable, const char* variableNameNt, const u16* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU16_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, const char* variableNameNt, ExpGetterU16_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU16_f* getter, ExpSetterU16_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, const char* variableNameNt, ExpGetterU16_f* getter, ExpSetterU16_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u32* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, bool writeable, const char* variableNameNt, const u32* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU32_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, const char* variableNameNt, ExpGetterU32_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU32_f* getter, ExpSetterU32_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, const char* variableNameNt, ExpGetterU32_f* getter, ExpSetterU32_f* setter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u64* pntr,     MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, bool writeable, const char* variableNameNt, const u64* pntr,     const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU64_f* getter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, const char* variableNameNt, ExpGetterU64_f* getter, const char* documentationNt = "");
	ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU64_f* getter, ExpSetterU64_f* setter, MyStr_t documentation = MyStr_Empty_Const);
	ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, const char* variableNameNt, ExpGetterU64_f* getter, ExpSetterU64_f* setter, const char* documentationNt = "");
	ExpFuncDef_t* AddExpFuncDef(ExpContext_t* context, ExpValueType_t returnType, MyStr_t functionName, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const);
	ExpFuncDef_t* AddExpFuncDef(ExpContext_t* context, ExpValueType_t returnType, const char* functionNameNt, ExpressionFunc_f* functionPntr, const char* documentationNt = "");
	ExpFuncArg_t* AddExpFuncArg(ExpContext_t* context, ExpFuncDef_t* funcDef, ExpValueType_t argumentType, MyStr_t argumentName, MyStr_t documentation = MyStr_Empty_Const);
	ExpFuncArg_t* AddExpFuncArg(ExpContext_t* context, ExpFuncDef_t* funcDef, ExpValueType_t argumentType, const char* argumentNameNt, const char* documentationNt = "");
	ExpValueType_t GetExpResultTypeForMathOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isSubtractOp, Result_t* reasonOut = nullptr);
	ExpValueType_t GetExpResultTypeForTernaryOp(ExpValueType_t trueOperandType, ExpValueType_t falseOperandType, Result_t* reasonOut = nullptr);
	ExpValueType_t GetExpIntegerTypeForBitwiseOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isAndOp, Result_t* reasonOut = nullptr);
	ExpValueType_t GetExpCommonTypeForComparisonOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isStraightEqualityOp, Result_t* reasonOut = nullptr);
	ExpValue_t CastExpValue(ExpValue_t value, ExpValueType_t type);
	void WriteExpVariable_(ExpVariableDef_t* variableDef, u64 valueSize, const void* valuePntr);
	void WriteExpVariable(ExpVariableDef_t* variableDef, ExpValue_t value);
	void WriteExpVariableBool(ExpVariableDef_t* variableDef, bool value);
	void WriteExpVariablePointer(ExpVariableDef_t* variableDef, void* value);
	void WriteExpVariableString(ExpVariableDef_t* variableDef, MyStr_t value);
	void WriteExpVariableR32(ExpVariableDef_t* variableDef, r32 value);
	void WriteExpVariableR64(ExpVariableDef_t* variableDef, r64 value);
	void WriteExpVariableI8(ExpVariableDef_t* variableDef, i8 value);
	void WriteExpVariableI16(ExpVariableDef_t* variableDef, i16 value);
	void WriteExpVariableI32(ExpVariableDef_t* variableDef, i32 value);
	void WriteExpVariableI64(ExpVariableDef_t* variableDef, i64 value);
	void WriteExpVariableU8(ExpVariableDef_t* variableDef, u8 value);
	void WriteExpVariableU16(ExpVariableDef_t* variableDef, u16 value);
	void WriteExpVariableU32(ExpVariableDef_t* variableDef, u32 value);
	void WriteExpVariableU64(ExpVariableDef_t* variableDef, u64 value);
	ExpValue_t ReadExpVariable_(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	bool ReadExpVariableBool(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	void* ReadExpVariablePointer(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	r32 ReadExpVariableR32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	r64 ReadExpVariableR64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	i8 ReadExpVariableI8(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	i16 ReadExpVariableI16(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	i32 ReadExpVariableI32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	i64 ReadExpVariableI64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	u8 ReadExpVariableU8(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	u16 ReadExpVariableU16(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	u32 ReadExpVariableU32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	u64 ReadExpVariableU64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr);
	ExpToken_t NewExpToken(ExpTokenType_t type, MyStr_t tokenStr);
	ExpTokenizer_t NewExpTokenizer(MyStr_t expressionStr);
	bool ExpTokenizerGetNext(ExpTokenizer_t* tokenizer, ExpToken_t* tokenOut, Result_t* errorOut = nullptr, bool allowErrors = false);
	Result_t TryTokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, ExpToken_t** tokensOut, u64* numTokensOut, bool allowErrors = false);
	ExpToken_t* TokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, u64* numTokensOut, Result_t* resultOut = nullptr, bool allowErrors = false);
	MyStr_t UnescapeExpressionStr(MemArena_t* memArena, MyStr_t string);
	MyStr_t EscapeExpressionStr(MemArena_t* memArena, MyStr_t string);
	void PushExpPart(ExpPartStack_t* stack, ExpPart_t* partPntr);
	ExpPart_t* PopExpPart(ExpPartStack_t* stack);
	ExpPart_t* PeekExpPart(ExpPartStack_t* stack);
	void PushAndConnectExpPart(ExpPartStack_t* stack, ExpPart_t* partPntr);
	ExpValue_t ConvertExpNumberToken(MyStr_t numberStr);
	ExpPart_t* SplitExpPartTreeWithPrecedenceAtLeast(ExpPart_t* leftPart, ExpOp_t opType, ExpPart_t** remainingPortionOut);
	bool FindExpClosingParensToken(u64 numTokens, const ExpToken_t* tokens, u64 startIndex, u64* indexOut);
	ExpPart_t* AddExpPart(Expression_t* expression, u64 tokenIndex, ExpPartType_t type);
	ExpPart_t* AddExpConstantString(Expression_t* expression, u64 tokenIndex, MyStr_t value);
	ExpPart_t* AddExpVariable(Expression_t* expression, u64 tokenIndex, u64 variableIndex);
	ExpPart_t* AddExpOperator(Expression_t* expression, u64 tokenIndex, ExpOp_t opType, ExpPart_t* firstChild = nullptr, ExpPart_t* secondChild = nullptr, ExpPart_t* thirdChild = nullptr);
	ExpPart_t* AddExpFunction(Expression_t* expression, u64 tokenIndex, u64 functionIndex);
	ExpPart_t* AddExpParenthesisGroup(Expression_t* expression, u64 tokenIndex, ExpPart_t* childRoot);
	Result_t TryCreateExpressionFromTokens_Helper(Expression_t* expression, const ExpContext_t* context, u64 numTokens, const ExpToken_t* tokens, ExpPart_t** rootOut, ExpPart_t* functionPart = nullptr);
	Result_t TryCreateExpressionFromTokens(const ExpContext_t* context, u64 numTokens, const ExpToken_t* tokens, Expression_t* expressionOut, MemArena_t* memArena = nullptr);
	Result_t TryCreateExpFuncDefFromTokens(u64 numTokens, const ExpToken_t* tokens, ExpFuncDef_t* funcDefOut, MemArena_t* memArena = nullptr);
	ExpFuncDef_t CreateExpFuncDefFromTokens(u64 numTokens, const ExpToken_t* tokens, MemArena_t* memArena = nullptr);
	u64 StepThroughExpression_Helper(Expression_t* expression, ExpPart_t* part, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpContext_t* context, void* userPntr, u64 startIndex, u64 depth);
	void StepThroughExpression(Expression_t* expression, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpContext_t* context = nullptr, void* userPntr = nullptr);
	Result_t ExpressionTypeCheckWalk(Expression_t* expression, const ExpContext_t* context = nullptr, u64* errorPartIndex = nullptr);
	ExpValue_t PerformMathOpOnExpValues(ExpValue_t leftOperand, ExpOp_t opType, ExpValue_t rightOperand);
	Result_t EvaluateExpression(Expression_t* expression, ExpContext_t* context = nullptr, ExpValue_t* valueOut = nullptr);
	Result_t TryAddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const);
	MyStr_t TryAddExpFuncDefByStrErrorStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const);
	void AddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const);
	Result_t ValidateExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpContext_t* context = nullptr);
	Result_t TryRunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpContext_t* context = nullptr);
	MyStr_t TryRunExpressionErrorStr(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpContext_t* context = nullptr);
	ExpValue_t RunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpContext_t* context = nullptr);
	bool IsTokenHigherPriorityForAutocomplete(const ExpToken_t* newToken, const ExpToken_t* oldToken);
	void GetExpAutocompleteInfo(MyStr_t expressionStr, u64 cursorIndex, MemArena_t* scratchArena, ExpAutocompleteInfo_t* infoOut, const ExpContext_t* context = nullptr);
	void AddStdLibraryFuncsToExpContext(ExpContext_t* context, MemArena_t* scratchArena);
	void AddStdLibraryConstantsToExpContext(ExpContext_t* context);
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

void FreeExpFuncDef(ExpFuncDef_t* funcDef, MemArena_t* allocArena)
{
	NotNull2(funcDef, allocArena);
	FreeString(allocArena, &funcDef->name);
	FreeString(allocArena, &funcDef->documentation);
	for (u64 aIndex = 0; aIndex < funcDef->numArguments; aIndex++)
	{
		FreeString(allocArena, &funcDef->arguments[aIndex].name);
		FreeString(allocArena, &funcDef->arguments[aIndex].documentation);
	}
	ClearPointer(funcDef);
}

void FreeExpContext(ExpContext_t* context)
{
	NotNull(context);
	if (context->constantDefs.length > 0 && context->allocateStrings)
	{
		NotNull(context->allocArena);
		VarArrayLoop(&context->constantDefs, cIndex)
		{
			VarArrayLoopGet(ExpConstantDef_t, constantDef, &context->constantDefs, cIndex);
			FreeExpValue(context->allocArena, &constantDef->value);
			FreeString(context->allocArena, &constantDef->name);
			FreeString(context->allocArena, &constantDef->documentation);
		}
	}
	FreeVarArray(&context->constantDefs);
	if (context->variableDefs.length > 0 && context->allocateStrings)
	{
		NotNull(context->allocArena);
		VarArrayLoop(&context->variableDefs, vIndex)
		{
			VarArrayLoopGet(ExpVariableDef_t, varDef, &context->variableDefs, vIndex);
			FreeString(context->allocArena, &varDef->name);
			FreeString(context->allocArena, &varDef->documentation);
		}
	}
	FreeVarArray(&context->variableDefs);
	if (context->functionDefs.length > 0 && context->allocateStrings)
	{
		NotNull(context->allocArena);
		VarArrayLoop(&context->functionDefs, fIndex)
		{
			VarArrayLoopGet(ExpFuncDef_t, funcDef, &context->functionDefs, fIndex);
			FreeExpFuncDef(funcDef, context->allocArena);
		}
	}
	FreeVarArray(&context->functionDefs);
	ClearPointer(context);
}

// +--------------------------------------------------------------+
// |                        Copy Functions                        |
// +--------------------------------------------------------------+
void CopyExpFuncDef(ExpFuncDef_t* dest, ExpFuncDef_t* source, MemArena_t* memArena = nullptr)
{
	NotNull2(dest, source);
	MyMemCopy(dest, source, sizeof(ExpFuncDef_t));
	if (memArena != nullptr)
	{
		dest->name = AllocString(memArena, &dest->name);
		dest->documentation = AllocString(memArena, &dest->documentation);
		for (u64 aIndex = 0; aIndex < dest->numArguments; aIndex++)
		{
			dest->arguments[aIndex].name = AllocString(memArena, &dest->arguments[aIndex].name);
			dest->arguments[aIndex].documentation = AllocString(memArena, &dest->arguments[aIndex].documentation);
		}
	}
}

//TODO: Add CopyExpVariableDef
//TODO: Add CopyExpContext
//TODO: Add CopyExpression

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
ExpValue_t NewExpValueVoid()
{
	ExpValue_t result = {};
	result.type = ExpValueType_Void;
	return result;
}
ExpValue_t NewExpValueBool(bool value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_Bool;
	result.valueBool = value;
	return result;
}
ExpValue_t NewExpValueStr(MyStr_t value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_String;
	result.valueStr = value;
	return result;
}
ExpValue_t NewExpValueStr(const char* valueNt)
{
	ExpValue_t result = {};
	result.type = ExpValueType_String;
	result.valueStr = NewStr(valueNt);
	return result;
}
ExpValue_t NewExpValuePntr(u64 typeId, void* value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_Pointer;
	result.valuePntrTypeId = typeId;
	result.valuePntr = value;
	return result;
}
ExpValue_t NewExpValueR32(r32 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_R32;
	result.valueR32 = value;
	return result;
}
ExpValue_t NewExpValueR64(r64 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_R64;
	result.valueR64 = value;
	return result;
}
ExpValue_t NewExpValueI8(i8 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_I8;
	result.valueI8 = value;
	return result;
}
ExpValue_t NewExpValueI16(i16 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_I16;
	result.valueI16 = value;
	return result;
}
ExpValue_t NewExpValueI32(i32 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_I32;
	result.valueI32 = value;
	return result;
}
ExpValue_t NewExpValueI64(i64 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_I64;
	result.valueI64 = value;
	return result;
}
ExpValue_t NewExpValueU8(u8 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_U8;
	result.valueU8 = value;
	return result;
}
ExpValue_t NewExpValueU16(u16 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_U16;
	result.valueU16 = value;
	return result;
}
ExpValue_t NewExpValueU32(u32 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_U32;
	result.valueU32 = value;
	return result;
}
ExpValue_t NewExpValueU64(u64 value)
{
	ExpValue_t result = {};
	result.type = ExpValueType_U64;
	result.valueU64 = value;
	return result;
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
u8 GetExpOperandCount(ExpOp_t opType)
{
	switch (opType)
	{
		case ExpOp_Add:                  return 2;
		case ExpOp_Subtract:             return 2;
		case ExpOp_Multiply:             return 2;
		case ExpOp_Divide:               return 2;
		case ExpOp_Modulo:               return 2;
		case ExpOp_Equals:               return 2;
		case ExpOp_NotEquals:            return 2;
		case ExpOp_GreaterThan:          return 2;
		case ExpOp_GreaterThanOrEqual:   return 2;
		case ExpOp_LessThan:             return 2;
		case ExpOp_LessThanOrEqual:      return 2;
		case ExpOp_Or:                   return 2;
		case ExpOp_And:                  return 2;
		case ExpOp_Not:                  return 1;
		case ExpOp_BitwiseOr:            return 2;
		case ExpOp_BitwiseAnd:           return 2;
		case ExpOp_BitwiseXor:           return 2;
		case ExpOp_BitwiseNot:           return 1;
		case ExpOp_Ternary:              return 3;
		case ExpOp_Assignment:           return 2;
		case ExpOp_AssignmentAdd:        return 2;
		case ExpOp_AssignmentSubtract:   return 2;
		case ExpOp_AssignmentMultiply:   return 2;
		case ExpOp_AssignmentDivide:     return 2;
		case ExpOp_AssignmentBitwiseOr:  return 2;
		case ExpOp_AssignmentBitwiseAnd: return 2;
		case ExpOp_AssignmentBitwiseXor: return 2;
		default: AssertMsg(false, "Unhandled ExpOp value in GetExpOperandCount"); return 0;
	}
}

u8 GetExpOpPrecedence(ExpOp_t opType)
{
	// A higher precedence number means the operations happens sooner (if you imagine taking an expression and collapsing it one operator at a time until we are left with a single value)
	// Since we parse left->right, higher precedence operators will "steal" operands from lower precedence operators to their left. See SplitExpPartTreeWithPrecedenceAtLeast
	switch (opType)
	{
		case ExpOp_Add:                  return 5;
		case ExpOp_Subtract:             return 5;
		case ExpOp_Multiply:             return 6;
		case ExpOp_Divide:               return 6;
		case ExpOp_Modulo:               return 7;
		case ExpOp_Equals:               return 4; //TODO: Should this be lower priority thant comparisons?
		case ExpOp_GreaterThan:          return 4;
		case ExpOp_GreaterThanOrEqual:   return 4;
		case ExpOp_LessThan:             return 4;
		case ExpOp_LessThanOrEqual:      return 4;
		case ExpOp_NotEquals:            return 4; //TODO: Should this be lower priority thant comparisons?
		case ExpOp_Or:                   return 3;
		case ExpOp_And:                  return 3; //TODO: Should this be higher priority than Or? Do people actually expect that behavior?
		case ExpOp_Not:                  return 3;
		case ExpOp_BitwiseOr:            return 8;
		case ExpOp_BitwiseAnd:           return 8;
		case ExpOp_BitwiseXor:           return 8;
		case ExpOp_BitwiseNot:           return 8;
		case ExpOp_Ternary:              return 2;
		case ExpOp_Assignment:           return 1;
		case ExpOp_AssignmentAdd:        return 1;
		case ExpOp_AssignmentSubtract:   return 1;
		case ExpOp_AssignmentMultiply:   return 1;
		case ExpOp_AssignmentDivide:     return 1;
		case ExpOp_AssignmentBitwiseOr:  return 1;
		case ExpOp_AssignmentBitwiseAnd: return 1;
		case ExpOp_AssignmentBitwiseXor: return 1;
		default: return 0;
	}
}

// Basically are all operands filled in, or is the part any other type besides operator
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

bool CanExpPartProduceLeftHandOperand(const ExpPart_t* expPart)
{
	NotNull(expPart);
	if (expPart->type == ExpPartType_Operator)
	{
		u8 numOperands = GetExpOperandCount(expPart->opType);
		for (u8 oIndex = 0; oIndex < numOperands; oIndex++)
		{
			if (expPart->child[oIndex] != nullptr && !expPart->childLocked[oIndex]) { return true; }
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
	return (type == ExpValueType_Bool || IsExpValueTypeNumber(type) || type == ExpValueType_Pointer || type == ExpValueType_String);
}
inline bool CanExpValueTypeConvertTo(ExpValueType_t type, ExpValueType_t outType)
{
	if (type == outType) { return true; }
	if (!IsExpValueTypeNumber(type)) { return false; } //only numbers have automatic conversion
	if (!IsExpValueTypeNumber(outType)) { return false; } //only numbers have automatic conversion
	if (IsExpValueTypeFloat(type) && !IsExpValueTypeFloat(outType)) { return false; } // disallow float -> integer conversion
	return true;
}
inline bool CanCastExpValueTo(ExpValueType_t valueType, ExpValueType_t type)
{
	if (valueType == type) { return true; }
	if (IsExpValueTypeNumber(valueType) && IsExpValueTypeNumber(type)) { return true; }
	if (valueType == ExpValueType_Bool && IsExpValueTypeNumber(type)) { return true; }
	if (type == ExpValueType_Bool && (IsExpValueTypeNumber(valueType) || valueType == ExpValueType_Pointer || valueType == ExpValueType_String)) { return true; }
	//TODO: We could be strict about float -> integer conversion?
	//TODO: We could be strict about larger number types being cast down to smaller ones?
	//TODO: Do we want to be strict about any particular number conversions here? Our type checking has no static analysis for bubbling up real constant values,
	// so if we are strict it is going to call foul on anything that expects smaller than 64-bit types and has values being fed in through operators.
	// And we have no way of "casting" from one type to another in the expression, so we can't appease the type checker if it's too strict.
	return false;
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
			case ExpValueType_R32:     result = PrintInArenaStr(memArena, "R32 %f",                   value.valueR32); break;
			case ExpValueType_R64:     result = PrintInArenaStr(memArena, "R64 %f",                   value.valueR64); break;
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
			case ExpValueType_R32:     result = PrintInArenaStr(memArena, "%f",                  value.valueR32); break;
			case ExpValueType_R64:     result = PrintInArenaStr(memArena, "%f",                  value.valueR64); break;
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
// |                      Context Functions                       |
// +--------------------------------------------------------------+
void InitExpContext(MemArena_t* memArena, ExpContext_t* contextOut, bool allocateStrings = true)
{
	NotNull2(memArena, contextOut);
	ClearPointer(contextOut);
	contextOut->allocArena = memArena;
	contextOut->allocateStrings = allocateStrings;
	CreateVarArray(&contextOut->constantDefs, memArena, sizeof(ExpConstantDef_t));
	CreateVarArray(&contextOut->variableDefs, memArena, sizeof(ExpVariableDef_t));
	CreateVarArray(&contextOut->functionDefs, memArena, sizeof(ExpFuncDef_t));
}

ExpConstantDef_t* FindExpConstantDef(ExpContext_t* context, MyStr_t constantName, u64* indexOut = nullptr)
{
	VarArrayLoop(&context->constantDefs, cIndex)
	{
		VarArrayLoopGet(ExpConstantDef_t, constantDef, &context->constantDefs, cIndex);
		if (StrEquals(constantDef->name, constantName))
		{
			SetOptionalOutPntr(indexOut, cIndex);
			return constantDef;
		}
	}
	return nullptr;
}
const ExpConstantDef_t* FindExpConstantDef(const ExpContext_t* context, MyStr_t constantName, u64* indexOut = nullptr) //const variant
{
	return (const ExpConstantDef_t*)FindExpConstantDef((ExpContext_t*)context, constantName, indexOut);
}
ExpVariableDef_t* FindExpVariableDef(ExpContext_t* context, MyStr_t variableName, u64* indexOut = nullptr)
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
const ExpVariableDef_t* FindExpVariableDef(const ExpContext_t* context, MyStr_t variableName, u64* indexOut = nullptr) //const variant
{
	return (const ExpVariableDef_t*)FindExpVariableDef((ExpContext_t*)context, variableName, indexOut);
}
ExpFuncDef_t* FindExpFuncDef(ExpContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr)
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
const ExpFuncDef_t* FindExpFuncDef(const ExpContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr) //const variant
{
	return (const ExpFuncDef_t*)FindExpFuncDef((ExpContext_t*)context, functionName, numArguments, indexOut);
}

//TODO: This should probably make sure that there are no naming conflicts!
ExpConstantDef_t* AddExpConstantDef(ExpContext_t* context, MyStr_t constantName, ExpValue_t value, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull(context);
	NotNullStr(&constantName);
	Assert(value.type != ExpValueType_None && value.type != ExpValueType_Void);
	ExpConstantDef_t* newDef = VarArrayAdd(&context->constantDefs, ExpConstantDef_t);
	NotNull(newDef);
	ClearPointer(newDef);
	newDef->value = value;
	if (context->allocateStrings && value.type == ExpValueType_String) { newDef->value.valueStr = AllocString(context->allocArena, &newDef->value.valueStr); }
	newDef->name = context->allocateStrings ? AllocString(context->allocArena, &constantName) : constantName;
	if (!IsEmptyStr(documentation)) { newDef->documentation = context->allocateStrings ? AllocString(context->allocArena, &documentation) : documentation; }
	return newDef;
}
ExpConstantDef_t* AddExpConstantDef(ExpContext_t* context, const char* constantNameNt, ExpValue_t value, const char* documentationNt = "")
{
	return AddExpConstantDef(context, NewStr(constantNameNt), value, NewStr(documentationNt));
}

//TODO: These should probably make sure that there are no naming conflicts!
ExpVariableDef_t* AddExpVariableDef_(ExpContext_t* context, bool writeable, MyStr_t variableName, ExpValueType_t type, u64 pntrSize, const void* pntr, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull2(context, pntr);
	NotNullStr(&variableName);
	Assert(type != ExpValueType_None && type != ExpValueType_Void);
	ExpVariableDef_t* newDef = VarArrayAdd(&context->variableDefs, ExpVariableDef_t);
	NotNull(newDef);
	ClearPointer(newDef);
	newDef->isReadOnly = !writeable;
	newDef->type = type;
	newDef->name = context->allocateStrings ? AllocString(context->allocArena, &variableName) : variableName;
	if (!IsEmptyStr(documentation)) { newDef->documentation = context->allocateStrings ? AllocString(context->allocArena, &documentation) : documentation; }
	Assert(pntrSize == GetExpValueTypeByteSize(type));
	newDef->pntr = (void*)pntr;
	return newDef;
}
ExpVariableDef_t* AddExpVariableDefFuncs_(ExpContext_t* context, MyStr_t variableName, ExpValueType_t type, void* getter, void* setter, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull2(context, getter);
	NotNullStr(&variableName);
	Assert(type != ExpValueType_None && type != ExpValueType_Void);
	ExpVariableDef_t* newDef = VarArrayAdd(&context->variableDefs, ExpVariableDef_t);
	NotNull(newDef);
	ClearPointer(newDef);
	newDef->isReadOnly = (setter != nullptr);
	newDef->type = type;
	newDef->name = context->allocateStrings ? AllocString(context->allocArena, &variableName) : variableName;
	if (!IsEmptyStr(documentation)) { newDef->documentation = context->allocateStrings ? AllocString(context->allocArena, &documentation) : documentation; }
	newDef->getter = getter;
	newDef->setter = setter;
	return newDef;
}

ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, bool writeable, MyStr_t     variableName,   const bool* pntr,    MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_Bool,    sizeof(bool),    (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, bool writeable, const char* variableNameNt, const bool* pntr,    const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_Bool,    sizeof(bool),    (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, MyStr_t     variableName,   ExpGetterBool_f* getter, MyStr_t documentation = MyStr_Empty_Const)                          { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_Bool,    (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, const char* variableNameNt, ExpGetterBool_f* getter, const char* documentationNt = "")                                   { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_Bool,    (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, MyStr_t     variableName,   ExpGetterBool_f* getter, ExpSetterBool_f* setter, MyStr_t documentation = MyStr_Empty_Const) { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_Bool,    (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, const char* variableNameNt, ExpGetterBool_f* getter, ExpSetterBool_f* setter, const char* documentationNt = "")          { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_Bool,    (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, bool writeable, MyStr_t     variableName,   const void** pntr,   MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_Pointer, sizeof(void*),   (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, bool writeable, const char* variableNameNt, const void** pntr,   const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_Pointer, sizeof(void*),   (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, MyStr_t     variableName,   ExpGetterPntr_f* getter, MyStr_t documentation = MyStr_Empty_Const)                          { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_Pointer, (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, const char* variableNameNt, ExpGetterPntr_f* getter, const char* documentationNt = "")                                   { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_Pointer, (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, MyStr_t     variableName,   ExpGetterPntr_f* getter, ExpSetterPntr_f* setter, MyStr_t documentation = MyStr_Empty_Const) { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_Pointer, (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, const char* variableNameNt, ExpGetterPntr_f* getter, ExpSetterPntr_f* setter, const char* documentationNt = "")          { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_Pointer, (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const MyStr_t* pntr, MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_String,  sizeof(MyStr_t), (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, bool writeable, const char* variableNameNt, const MyStr_t* pntr, const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_String,  sizeof(MyStr_t), (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterStr_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_String,  (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, const char* variableNameNt, ExpGetterStr_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_String,  (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterStr_f* getter, ExpSetterStr_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_String,  (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, const char* variableNameNt, ExpGetterStr_f* getter, ExpSetterStr_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_String,  (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const r32* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_R32,     sizeof(r32),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, bool writeable, const char* variableNameNt, const r32* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_R32,     sizeof(r32),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR32_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_R32,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, const char* variableNameNt, ExpGetterR32_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_R32,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR32_f* getter, ExpSetterR32_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_R32,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, const char* variableNameNt, ExpGetterR32_f* getter, ExpSetterR32_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_R32,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const r64* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_R64,     sizeof(r64),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, bool writeable, const char* variableNameNt, const r64* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_R64,     sizeof(r64),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR64_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_R64,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, const char* variableNameNt, ExpGetterR64_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_R64,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterR64_f* getter, ExpSetterR64_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_R64,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, const char* variableNameNt, ExpGetterR64_f* getter, ExpSetterR64_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_R64,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, bool writeable, MyStr_t     variableName,   const i8*  pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_I8,      sizeof(i8),      (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, bool writeable, const char* variableNameNt, const i8*  pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_I8,      sizeof(i8),      (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterI8_f* getter, MyStr_t documentation = MyStr_Empty_Const)                            { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I8,      (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, const char* variableNameNt, ExpGetterI8_f* getter, const char* documentationNt = "")                                     { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I8,      (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterI8_f* getter, ExpSetterI8_f* setter, MyStr_t documentation = MyStr_Empty_Const)     { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I8,      (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, const char* variableNameNt, ExpGetterI8_f* getter, ExpSetterI8_f* setter, const char* documentationNt = "")              { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I8,      (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i16* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_I16,     sizeof(i16),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, bool writeable, const char* variableNameNt, const i16* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_I16,     sizeof(i16),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI16_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I16,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, const char* variableNameNt, ExpGetterI16_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I16,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI16_f* getter, ExpSetterI16_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I16,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, const char* variableNameNt, ExpGetterI16_f* getter, ExpSetterI16_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I16,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i32* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_I32,     sizeof(i32),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, bool writeable, const char* variableNameNt, const i32* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_I32,     sizeof(i32),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI32_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I32,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, const char* variableNameNt, ExpGetterI32_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I32,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI32_f* getter, ExpSetterI32_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I32,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, const char* variableNameNt, ExpGetterI32_f* getter, ExpSetterI32_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I32,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i64* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_I64,     sizeof(i64),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, bool writeable, const char* variableNameNt, const i64* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_I64,     sizeof(i64),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI64_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I64,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, const char* variableNameNt, ExpGetterI64_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I64,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterI64_f* getter, ExpSetterI64_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_I64,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, const char* variableNameNt, ExpGetterI64_f* getter, ExpSetterI64_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_I64,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, bool writeable, MyStr_t     variableName,   const u8*  pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_U8,      sizeof(u8),      (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, bool writeable, const char* variableNameNt, const u8*  pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_U8,      sizeof(u8),      (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterU8_f* getter, MyStr_t documentation = MyStr_Empty_Const)                            { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U8,      (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, const char* variableNameNt, ExpGetterU8_f* getter, const char* documentationNt = "")                                     { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U8,      (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, MyStr_t     variableName,   ExpGetterU8_f* getter, ExpSetterU8_f* setter, MyStr_t documentation = MyStr_Empty_Const)     { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U8,      (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, const char* variableNameNt, ExpGetterU8_f* getter, ExpSetterU8_f* setter, const char* documentationNt = "")              { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U8,      (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u16* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_U16,     sizeof(u16),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, bool writeable, const char* variableNameNt, const u16* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_U16,     sizeof(u16),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU16_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U16,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, const char* variableNameNt, ExpGetterU16_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U16,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU16_f* getter, ExpSetterU16_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U16,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, const char* variableNameNt, ExpGetterU16_f* getter, ExpSetterU16_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U16,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u32* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_U32,     sizeof(u32),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, bool writeable, const char* variableNameNt, const u32* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_U32,     sizeof(u32),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU32_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U32,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, const char* variableNameNt, ExpGetterU32_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U32,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU32_f* getter, ExpSetterU32_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U32,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, const char* variableNameNt, ExpGetterU32_f* getter, ExpSetterU32_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U32,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u64* pntr,     MyStr_t documentation = MyStr_Empty_Const)              { return AddExpVariableDef_(context, writeable, variableName,           ExpValueType_U64,     sizeof(u64),     (const void*)pntr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, bool writeable, const char* variableNameNt, const u64* pntr,     const char* documentationNt = "")                       { return AddExpVariableDef_(context, writeable, NewStr(variableNameNt), ExpValueType_U64,     sizeof(u64),     (const void*)pntr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU64_f* getter, MyStr_t documentation = MyStr_Empty_Const)                           { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U64,     (void*)getter,             nullptr, documentation);           }
ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, const char* variableNameNt, ExpGetterU64_f* getter, const char* documentationNt = "")                                    { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U64,     (void*)getter,             nullptr, NewStr(documentationNt)); }
ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, MyStr_t     variableName,   ExpGetterU64_f* getter, ExpSetterU64_f* setter, MyStr_t documentation = MyStr_Empty_Const)   { return AddExpVariableDefFuncs_(context,       variableName,           ExpValueType_U64,     (void*)getter,       (void*)setter, documentation);           }
ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, const char* variableNameNt, ExpGetterU64_f* getter, ExpSetterU64_f* setter, const char* documentationNt = "")            { return AddExpVariableDefFuncs_(context,       NewStr(variableNameNt), ExpValueType_U64,     (void*)getter,       (void*)setter, NewStr(documentationNt)); }

//TODO: These should probably make sure that there are no naming conflicts!
ExpFuncDef_t* AddExpFuncDef(ExpContext_t* context, ExpValueType_t returnType, MyStr_t functionName, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull(context);
	NotNullStr(&functionName);
	Assert(returnType != ExpValueType_None);
	ExpFuncDef_t* newDef = VarArrayAdd(&context->functionDefs, ExpFuncDef_t);
	NotNull(newDef);
	ClearPointer(newDef);
	newDef->name = context->allocateStrings ? AllocString(context->allocArena, &functionName) : functionName;
	if (!IsEmptyStr(documentation)) { newDef->documentation = context->allocateStrings ? AllocString(context->allocArena, &documentation) : documentation; }
	newDef->returnType = returnType;
	newDef->numArguments = 0;
	newDef->pntr = functionPntr;
	return newDef;
}
ExpFuncDef_t* AddExpFuncDef(ExpContext_t* context, ExpValueType_t returnType, const char* functionNameNt, ExpressionFunc_f* functionPntr, const char* documentationNt = "")
{
	return AddExpFuncDef(context, returnType, NewStr(functionNameNt), functionPntr, NewStr(documentationNt));
}

//TODO: These should probably make sure that there are no naming conflicts!
ExpFuncArg_t* AddExpFuncArg(ExpContext_t* context, ExpFuncDef_t* funcDef, ExpValueType_t argumentType, MyStr_t argumentName, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull2(context, funcDef);
	NotNullStr(&argumentName);
	Assert(argumentType != ExpValueType_None && argumentType != ExpValueType_Void);
	Assert(funcDef->numArguments < EXPRESSIONS_MAX_FUNC_ARGS);
	ExpFuncArg_t* newArg = &funcDef->arguments[funcDef->numArguments++];
	NotNull(newArg);
	ClearPointer(newArg);
	newArg->type = argumentType;
	newArg->name = context->allocateStrings ? AllocString(context->allocArena, &argumentName) : argumentName;
	if (!IsEmptyStr(documentation)) { newArg->documentation = context->allocateStrings ? AllocString(context->allocArena, &documentation) : documentation; }
	newArg->isOptional = false; //TODO: Implement handling of this!
	return newArg;
}
ExpFuncArg_t* AddExpFuncArg(ExpContext_t* context, ExpFuncDef_t* funcDef, ExpValueType_t argumentType, const char* argumentNameNt, const char* documentationNt = "")
{
	return AddExpFuncArg(context, funcDef, argumentType, NewStr(argumentNameNt), NewStr(documentationNt));
}

// +--------------------------------------------------------------+
// |                       Value Conversion                       |
// +--------------------------------------------------------------+
ExpValueType_t GetExpResultTypeForMathOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isSubtractOp, Result_t* reasonOut = nullptr)
{
	/*if (leftOperandType == rightOperandType)
	{
		if (IsExpValueTypeNumber(leftOperandType)) { return leftOperandType; }
		else { SetOptionalOutPntr(reasonOut, Result_InvalidLeftOperand); return ExpValueType_None; }
	}
	else */if (IsExpValueTypeNumber(leftOperandType) && IsExpValueTypeNumber(rightOperandType))
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
		SetOptionalOutPntr(reasonOut, Result_InvalidRightOperand);
		return ExpValueType_None;
	}
}
ExpValueType_t GetExpResultTypeForTernaryOp(ExpValueType_t trueOperandType, ExpValueType_t falseOperandType, Result_t* reasonOut = nullptr)
{
	if (trueOperandType == falseOperandType) { return trueOperandType; }
	else if (IsExpValueTypeNumber(trueOperandType) && IsExpValueTypeNumber(falseOperandType))
	{
		if (IsExpValueTypeFloat(trueOperandType) || IsExpValueTypeFloat(falseOperandType)) { return ExpValueType_R64; }
		else
		{
			bool resultIsSigned = (IsExpValueTypeSigned(trueOperandType) || IsExpValueTypeSigned(falseOperandType));
			u8 resultByteSize = (u8)MaxU32(GetExpValueTypeByteSize(trueOperandType), GetExpValueTypeByteSize(falseOperandType));
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
	else
	{
		//TODO: Are there any mismatching types that we accept for ternary operators?
		SetOptionalOutPntr(reasonOut, Result_MismatchedOperandTypes);
		return ExpValueType_None;
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
ExpValueType_t GetExpCommonTypeForComparisonOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isStraightEqualityOp, Result_t* reasonOut = nullptr)
{
	if (leftOperandType == rightOperandType)
	{
		if (leftOperandType == ExpValueType_Void) { SetOptionalOutPntr(reasonOut, Result_InvalidLeftOperand); return ExpValueType_None; }
		if (leftOperandType == ExpValueType_String && !isStraightEqualityOp) { SetOptionalOutPntr(reasonOut, Result_InvalidLeftOperand); return ExpValueType_None; }
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
		SetOptionalOutPntr(reasonOut, Result_InvalidRightOperand);
		return ExpValueType_None;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
			}
		} break;
		
		case ExpValueType_Pointer:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = (value.valuePntr != nullptr); break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
			}
		} break;
		
		case ExpValueType_String:
		{
			switch (type)
			{
				case ExpValueType_Bool: result.valueBool = !IsEmptyStr(value.valueStr); break;
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
				default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
			}
		} break;
		
		default: AssertMsg(false, "Unsupported conversion in CastExpValue"); result.type = ExpValueType_None; break;
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
void WriteExpVariable(ExpVariableDef_t* variableDef, ExpValue_t value)
{
	Assert(variableDef != nullptr && variableDef->type == value.type);
	switch (variableDef->type)
	{
		// case ExpValueType_Void:    
		case ExpValueType_Bool:    WriteExpVariable_(variableDef, sizeof(value.valueBool), &value.valueBool); break;
		case ExpValueType_Pointer: WriteExpVariable_(variableDef, sizeof(value.valuePntr), &value.valuePntr); break;
		case ExpValueType_String:  WriteExpVariable_(variableDef, sizeof(value.valueStr),  &value.valueStr); break;
		case ExpValueType_R32:     WriteExpVariable_(variableDef, sizeof(value.valueR32),  &value.valueR32); break;
		case ExpValueType_R64:     WriteExpVariable_(variableDef, sizeof(value.valueR64),  &value.valueR64); break;
		case ExpValueType_I8:      WriteExpVariable_(variableDef, sizeof(value.valueI8),   &value.valueI8); break;
		case ExpValueType_I16:     WriteExpVariable_(variableDef, sizeof(value.valueI16),  &value.valueI16); break;
		case ExpValueType_I32:     WriteExpVariable_(variableDef, sizeof(value.valueI32),  &value.valueI32); break;
		case ExpValueType_I64:     WriteExpVariable_(variableDef, sizeof(value.valueI64),  &value.valueI64); break;
		case ExpValueType_U8:      WriteExpVariable_(variableDef, sizeof(value.valueU8),   &value.valueU8); break;
		case ExpValueType_U16:     WriteExpVariable_(variableDef, sizeof(value.valueU16),  &value.valueU16); break;
		case ExpValueType_U32:     WriteExpVariable_(variableDef, sizeof(value.valueU32),  &value.valueU32); break;
		case ExpValueType_U64:     WriteExpVariable_(variableDef, sizeof(value.valueU64),  &value.valueU64); break;
		default: AssertMsg(false, "Unhandled ExpValueType in WriteExpVariable"); break;
	}
}
void WriteExpVariableBool(ExpVariableDef_t* variableDef, bool value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_Bool);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariablePointer(ExpVariableDef_t* variableDef, void* value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_Pointer);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableString(ExpVariableDef_t* variableDef, MyStr_t value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_String);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableR32(ExpVariableDef_t* variableDef, r32 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_R32);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableR64(ExpVariableDef_t* variableDef, r64 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_R64);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI8(ExpVariableDef_t* variableDef, i8 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_I8);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI16(ExpVariableDef_t* variableDef, i16 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_I16);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI32(ExpVariableDef_t* variableDef, i32 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_I32);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableI64(ExpVariableDef_t* variableDef, i64 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_I64);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU8(ExpVariableDef_t* variableDef, u8 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_U8);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU16(ExpVariableDef_t* variableDef, u16 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_U16);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU32(ExpVariableDef_t* variableDef, u32 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_U32);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}
void WriteExpVariableU64(ExpVariableDef_t* variableDef, u64 value)
{
	Assert(variableDef != nullptr && variableDef->type == ExpValueType_U64);
	WriteExpVariable_(variableDef, sizeof(value), &value);
}

ExpValue_t ReadExpVariable_(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	NotNull2(expression, variableDef);
	ExpValue_t result = {};
	result.type = variableDef->type;
	if (variableDef->pntr != nullptr)
	{
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
	}
	else if (variableDef->getter != nullptr)
	{
		switch (variableDef->type)
		{
			// case ExpValueType_Void:    
			case ExpValueType_Bool:    result.valueBool = ((ExpGetterBool_f*)variableDef->getter)(expression, context, variableDef); break;
			case ExpValueType_Pointer: result.valuePntr = ((ExpGetterPntr_f*)variableDef->getter)(expression, context, variableDef); break;
			case ExpValueType_String:  result.valueStr  = ((ExpGetterStr_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_R32:     result.valueR32  = ((ExpGetterR32_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_R64:     result.valueR64  = ((ExpGetterR64_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_I8:      result.valueI8   = ((ExpGetterI8_f*)variableDef->getter)(expression,   context, variableDef); break;
			case ExpValueType_I16:     result.valueI16  = ((ExpGetterI16_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_I32:     result.valueI32  = ((ExpGetterI32_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_I64:     result.valueI64  = ((ExpGetterI64_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_U8:      result.valueU8   = ((ExpGetterU8_f*)variableDef->getter)(expression,   context, variableDef); break;
			case ExpValueType_U16:     result.valueU16  = ((ExpGetterU16_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_U32:     result.valueU32  = ((ExpGetterU32_f*)variableDef->getter)(expression,  context, variableDef); break;
			case ExpValueType_U64:     result.valueU64  = ((ExpGetterU64_f*)variableDef->getter)(expression,  context, variableDef); break;
			default: AssertMsg(false, "Unhandled ExpValueType in ReadExpVariable_"); break;
		}
	}
	return result;
}
bool ReadExpVariableBool(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_Bool);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueBool;
}
void* ReadExpVariablePointer(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_Pointer);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valuePntr;
}
r32 ReadExpVariableR32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_R32);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueR32;
}
r64 ReadExpVariableR64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_R64);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueR64;
}
i8 ReadExpVariableI8(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_I8);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueI8;
}
i16 ReadExpVariableI16(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_I16);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueI16;
}
i32 ReadExpVariableI32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_I32);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueI32;
}
i64 ReadExpVariableI64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_I64);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueI64;
}
u8 ReadExpVariableU8(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_U8);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueU8;
}
u16 ReadExpVariableU16(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_U16);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueU16;
}
u32 ReadExpVariableU32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_U32);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
	return resultValue.valueU32;
}
u64 ReadExpVariableU64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
{
	Assert(variableDef->type == ExpValueType_U64);
	ExpValue_t resultValue = ReadExpVariable_(expression, variableDef, context);
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

bool ExpTokenizerGetNext(ExpTokenizer_t* tokenizer, ExpToken_t* tokenOut, Result_t* errorOut = nullptr, bool allowErrors = false)
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
				if (!allowErrors) { return false; }
			}
			tokenizer->currentIndex += 1 + str.length + (foundClosingQuote ? 1 : 0);
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
		else if (c == ',')
		{
			MyStr_t charStr = NewStr(1, &tokenizer->expressionStr.chars[tokenizer->currentIndex]);
			tokenizer->currentIndex += charStr.length;
			tokenizer->prevToken = NewExpToken(ExpTokenType_Comma, charStr);
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
						else { break; }
					}
					SetOptionalOutPntr(errorOut, Result_InvalidIdentifier);
					tokenizer->currentIndex += numberStr.length;
					if (!allowErrors) { return false; }
				}
			}
			
			tokenizer->currentIndex += numberStr.length;
			tokenizer->prevToken = NewExpToken(ExpTokenType_Number, numberStr);
			SetOptionalOutPntr(tokenOut, tokenizer->prevToken);
			return true;
			
		}
		else if (c == '+' || c == '-' || c == '/' || c == '*' || c == '|' || c == '&' || c == '^' || c == '%' || c == '!' || c == '?' || c == ':' || c == '=' || c == '~' || c == '>' || c == '<')
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
				else if (c == '>' && nextChar == '=') { opStr.length = 2; }
				else if (c == '<' && nextChar == '=') { opStr.length = 2; }
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
			if (!allowErrors) { return false; }
		}
	}
	
	SetOptionalOutPntr(errorOut, Result_Success);
	return false;
}

Result_t TryTokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, ExpToken_t** tokensOut, u64* numTokensOut, bool allowErrors = false)
{
	NotNullStr(expressionStr);
	Result_t result = Result_None;
	u64 tIndex = 0;
	
	ExpTokenizer_t tokenizer = NewExpTokenizer(expressionStr);
	while (ExpTokenizerGetNext(&tokenizer, nullptr, &result, allowErrors)) { tIndex++; }
	if (result != Result_Success && !allowErrors) { return result; }
	
	SetOptionalOutPntr(numTokensOut, tIndex);
	if (memArena == nullptr || tokensOut == nullptr) { return result; }
	if (tIndex == 0) { return Result_Empty; }
	
	u64 numTokens = tIndex;
	ExpToken_t* tokens = AllocArray(memArena, ExpToken_t, numTokens);
	if (tokens == nullptr) { return Result_AllocFailure; }
	
	tokenizer = NewExpTokenizer(expressionStr);
	tIndex = 0;
	result = Result_None;
	ExpToken_t token;
	while (ExpTokenizerGetNext(&tokenizer, &token, &result, allowErrors))
	{
		Assert(tIndex < numTokens);
		tokens[tIndex] = token;
		tIndex++;
	}
	Assert(allowErrors || result == Result_Success);
	Assert(tIndex == numTokens);
	
	SetOptionalOutPntr(tokensOut, tokens);
	return result;
}
ExpToken_t* TokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, u64* numTokensOut, Result_t* resultOut = nullptr, bool allowErrors = false)
{
	ExpToken_t* tokens = nullptr;
	u64 numTokens = 0;
	Result_t result = TryTokenizeExpressionStr(expressionStr, memArena, &tokens, &numTokens, allowErrors);
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
		if (prevPart->type == ExpPartType_TypeCast && IsExpPartReadyToBeOperand(partPntr) && prevPart->child[0] == nullptr)
		{
			prevPart->child[0] = partPntr;
			prevPart->childLocked[0] = true;
			PopExpPart(stack);
			PushAndConnectExpPart(stack, prevPart);
			return;
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
		u8 rightOpIndex = numOperands-1;
		while (rightOpIndex > 1 && leftPart->child[rightOpIndex] == nullptr && !leftPart->childLocked[rightOpIndex]) { rightOpIndex--; }
		Assert(numOperands > 0);
		if (!leftPart->childLocked[rightOpIndex])
		{
			ExpPart_t* rightMostOperand = leftPart->child[rightOpIndex];
			leftPart->child[numOperands-1] = nullptr;
			ExpPart_t* result = SplitExpPartTreeWithPrecedenceAtLeast(rightMostOperand, opType, &leftPart->child[rightOpIndex]);
			SetOptionalOutPntr(remainingPortionOut, leftPart);
			return result;
		}
		else
		{
			SetOptionalOutPntr(remainingPortionOut, nullptr);
			return leftPart;
		}
	}
}

bool FindExpClosingParensToken(u64 numTokens, const ExpToken_t* tokens, u64 startIndex, u64* indexOut)
{
	AssertIf(numTokens > 0, tokens != nullptr);
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
	result->childLocked[0] = (firstChild != nullptr);
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
Result_t TryCreateExpressionFromTokens_Helper(Expression_t* expression, const ExpContext_t* context, u64 numTokens, const ExpToken_t* tokens, ExpPart_t** rootOut, ExpPart_t* functionPart = nullptr)
{
	NotNull2(expression, context);
	AssertIf(functionPart != nullptr, rootOut == nullptr);
	u64 functionArgIndex = 0;
	ExpPartStack_t stack = {};
	
	for (u64 tIndex = 0; tIndex < numTokens; tIndex++)
	{
		const ExpToken_t* token = &tokens[tIndex];
		
		// If we have a ternary operator at the top of the stack with 2/3 children filled out, then we should expect to find it's secondary operator token next
		// The only exceptions is a higher precedence operator being next and stealing our second (non-locked) child as it's left-hand operand
		ExpPart_t* prevPart = PeekExpPart(&stack);
		bool expectingTernaryOpSecondPart = false;
		if (prevPart != nullptr && prevPart->type == ExpPartType_Operator &&
			GetExpOperandCount(prevPart->opType) == 3 && !prevPart->childLocked[1] &&
			prevPart->child[0] != nullptr && prevPart->child[1] != nullptr && prevPart->child[2] == nullptr)
		{
			expectingTernaryOpSecondPart = true;
			const char* secondOpStr = GetExpOpSyntaxStr(prevPart->opType, true);
			if (token->type != ExpTokenType_Operator) { return Result_MissingOperator; }
			else if (StrEquals(token->str, secondOpStr))
			{
				prevPart->childLocked[1] = true;
				continue;
			}
		}
		
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
					
					if (!IsExpPartReadyToBeOperand(leftOperand) && !CanExpPartProduceLeftHandOperand(leftOperand))
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
						if (leftOperand == nullptr) { return Result_InvalidLeftOperand; }
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
				
				bool boolValue = false;
				if (TryParseBool(token->str, &boolValue, nullptr, true))
				{
					ExpPart_t* newBoolPart = AddExpPart(expression, tIndex, ExpPartType_Constant);
					newBoolPart->constantValue.type = ExpValueType_Bool;
					newBoolPart->constantValue.valueBool = boolValue;
					PushAndConnectExpPart(&stack, newBoolPart);
				}
				else
				{
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
						// ExpFuncDef_t* FindExpFuncDef(ExpressionContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr)
						const ExpFuncDef_t* funcDef = FindExpFuncDef(context, token->str, functionPartProto.childCount, &funcDefIndex);
						if (funcDef == nullptr)
						{
							const ExpFuncDef_t* anyArgCountFuncDef = FindExpFuncDef(context, token->str);
							return anyArgCountFuncDef != nullptr ?
								(functionPartProto.childCount > anyArgCountFuncDef->numArguments ? Result_TooManyArguments : Result_MissingArguments) :
								Result_UnknownFunction;
						}
						
						ExpPart_t* newFunctionPart = AddExpFunction(expression, tIndex, funcDefIndex);
						MyMemCopy(&newFunctionPart->child[0], &functionPartProto.child[0], EXPRESSIONS_MAX_FUNC_ARGS * sizeof(ExpPart_t*));
						newFunctionPart->childCount = functionPartProto.childCount;
						PushAndConnectExpPart(&stack, newFunctionPart);
						
						tIndex = endParenthesisIndex;
					}
					else
					{
						u64 constantDefIndex = 0;
						const ExpConstantDef_t* constantDef = FindExpConstantDef(context, token->str, &constantDefIndex);
						if (constantDef != nullptr)
						{
							if (constantDef->value.type == ExpValueType_String)
							{
								ExpPart_t* newConstantPart = AddExpConstantString(expression, tIndex, constantDef->value.valueStr);
								PushAndConnectExpPart(&stack, newConstantPart);
							}
							else
							{
								ExpPart_t* newConstantPart = AddExpPart(expression, tIndex, ExpPartType_Constant);
								newConstantPart->constantValue = constantDef->value;
								PushAndConnectExpPart(&stack, newConstantPart);
							}
						}
						else
						{
							u64 variableDefIndex = 0;
							const ExpVariableDef_t* variableDef = FindExpVariableDef(context, token->str, &variableDefIndex);
							if (variableDef == nullptr) { return Result_UnknownVariable; }
							
							ExpPart_t* newVariablePart = AddExpVariable(expression, tIndex, variableDefIndex);
							PushAndConnectExpPart(&stack, newVariablePart);
						}
					}
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
				
				bool isTypeCast = false;
				if (numTokensInParenthesis == 1 && tokens[tIndex+1].type == ExpTokenType_Identifier)
				{
					ExpValueType_t castType = ExpValueType_None;
					if (TryParseEnum(tokens[tIndex+1].str, &castType, ExpValueType_NumTypes, GetExpValueTypeStr))
					{
						ExpPart_t* newTypeCastPart = AddExpPart(expression, tIndex+1, ExpPartType_TypeCast);
						newTypeCastPart->castType = castType;
						PushAndConnectExpPart(&stack, newTypeCastPart);
						isTypeCast = true;
					}
				}
				
				if (!isTypeCast)
				{
					ExpPart_t* groupRootPart = nullptr;
					Result_t subResult = TryCreateExpressionFromTokens_Helper(expression, context, numTokensInParenthesis, &tokens[tIndex+1], &groupRootPart);
					if (subResult != Result_Success) { return subResult; }
					
					ExpPart_t* newParensPart = AddExpParenthesisGroup(expression, tIndex, groupRootPart);
					PushAndConnectExpPart(&stack, newParensPart);
				}
				
				tIndex = endParenthesisIndex;
			} break;
			
			// +==============================+
			// |      Handle Comma Token      |
			// +==============================+
			case ExpTokenType_Comma:
			{
				if (functionPart == nullptr) { return Result_InvalidOperator; }
				if (functionArgIndex >= EXPRESSIONS_MAX_FUNC_ARGS) { return Result_TooManyArguments; }
				
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
		if (functionArgIndex >= EXPRESSIONS_MAX_FUNC_ARGS) { return Result_TooManyArguments; }
		
		if (stack.length == 1)
		{
			if (!IsExpPartReadyToBeOperand(stack.parts[0])) { return Result_MissingRightOperand; }
			ExpPart_t* argument = PopExpPart(&stack);
			functionPart->child[functionArgIndex++] = argument;
			functionPart->childCount = functionArgIndex;
		}
		else if (functionArgIndex > 0) { return Result_EmptyArgument; }
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
Result_t TryCreateExpressionFromTokens(const ExpContext_t* context, u64 numTokens, const ExpToken_t* tokens, Expression_t* expressionOut, MemArena_t* memArena = nullptr)
{
	NotNull2(context, expressionOut);
	AssertIf(numTokens > 0, tokens != nullptr);
	
	ClearPointer(expressionOut);
	expressionOut->allocArena = memArena;
	expressionOut->numParts = 0;
	
	Result_t result = TryCreateExpressionFromTokens_Helper(expressionOut, context, numTokens, tokens, &expressionOut->rootPart);
	if (result != Result_Success) { FreeExpression(expressionOut); }
	
	return result;
}

// +--------------------------------------------------------------+
// |                      Definition Parsing                      |
// +--------------------------------------------------------------+
Result_t TryCreateExpFuncDefFromTokens(u64 numTokens, const ExpToken_t* tokens, ExpFuncDef_t* funcDefOut, MemArena_t* memArena = nullptr)
{
	AssertIf(numTokens > 0, tokens != nullptr);
	NotNull(funcDefOut);
	ClearPointer(funcDefOut);
	
	u64 tIndex = 0;
	if (tIndex >= numTokens) { return Result_Empty; }
	const ExpToken_t* returnTypeToken = &tokens[tIndex++];
	if (returnTypeToken->type != ExpTokenType_Identifier) { return Result_MissingType; }
	if (!TryParseEnum(returnTypeToken->str, &funcDefOut->returnType, ExpValueType_NumTypes, GetExpValueTypeStr)) { return Result_InvalidIdentifier; }
	
	if (tIndex >= numTokens) { return Result_MissingName; }
	const ExpToken_t* nameToken = &tokens[tIndex++];
	if (returnTypeToken->type != ExpTokenType_Identifier) { return Result_MissingName; }
	
	if (tIndex >= numTokens) { return Result_MissingOperator; }
	const ExpToken_t* openParensToken = &tokens[tIndex++];
	if (openParensToken->type != ExpTokenType_Parenthesis || !StrEquals(openParensToken->str, "(")) { return Result_MissingOperator; }
	
	funcDefOut->name = (memArena != nullptr) ? AllocString(memArena, &nameToken->str) : nameToken->str;
	
	bool expectingArg = false;
	while (tIndex < numTokens && tokens[tIndex].type != ExpTokenType_Parenthesis)
	{
		const ExpToken_t* token1 = &tokens[tIndex];
		const ExpToken_t* token2 = (tIndex+1 < numTokens) ? &tokens[tIndex+1] : nullptr;
		
		if (token1->type == ExpTokenType_Identifier && token2 != nullptr && token2->type == ExpTokenType_Identifier)
		{
			ExpValueType_t argType = ExpValueType_None;
			if (!TryParseEnum(token1->str, &argType, ExpValueType_NumTypes, GetExpValueTypeStr))
			{
				if (memArena != nullptr) { FreeExpFuncDef(funcDefOut, memArena); }
				return Result_InvalidIdentifier;
			}
			
			if (funcDefOut->numArguments >= EXPRESSIONS_MAX_FUNC_ARGS)
			{
				if (memArena != nullptr) { FreeExpFuncDef(funcDefOut, memArena); }
				return Result_TooManyArguments;
			}
			
			ExpFuncArg_t* argument = &funcDefOut->arguments[funcDefOut->numArguments++];
			argument->type = argType;
			argument->name = (memArena != nullptr) ? AllocString(memArena, &token2->str) : token2->str;
			
			if (tIndex+2 < numTokens && tokens[tIndex+2].type == ExpTokenType_Comma)
			{
				tIndex += 3;
				expectingArg = true;
			}
			else
			{
				tIndex += 2;
				break;
			}
		}
		else
		{
			if (memArena != nullptr) { FreeExpFuncDef(funcDefOut, memArena); }
			return Result_InvalidSyntax;
		}
	}
	if (tIndex >= numTokens || tokens[tIndex].type != ExpTokenType_Parenthesis || !StrEquals(tokens[tIndex].str, ")"))
	{
		if (memArena != nullptr) { FreeExpFuncDef(funcDefOut, memArena); }
		return Result_MismatchParenthesis;
	}
	tIndex++;
	
	if (tIndex < numTokens)
	{
		if (memArena != nullptr) { FreeExpFuncDef(funcDefOut, memArena); }
		return Result_UnknownExtension;
	}
	
	return Result_Success;
}

ExpFuncDef_t CreateExpFuncDefFromTokens(u64 numTokens, const ExpToken_t* tokens, MemArena_t* memArena = nullptr)
{
	ExpFuncDef_t result = {};
	Result_t error = TryCreateExpFuncDefFromTokens(numTokens, tokens, &result, memArena);
	Assert(error == Result_Success);
	return result;
}

// +--------------------------------------------------------------+
// |                          Evaluating                          |
// +--------------------------------------------------------------+
u64 StepThroughExpression_Helper(Expression_t* expression, ExpPart_t* part, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpContext_t* context, void* userPntr, u64 startIndex, u64 depth)
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
		
		case ExpPartType_TypeCast:
		{
			if (order == ExpStepOrder_Prefix || order == ExpStepOrder_Natural) { callback(expression, part, index++, depth, context, userPntr); }
			index += StepThroughExpression_Helper(expression, part->child[0], order, callback, context, userPntr, index, depth+1);
			if (order == ExpStepOrder_Postfix) { callback(expression, part, index++, depth, context, userPntr); }
		} break;
		
		default: AssertMsg(false, "Unhandled ExpPartType in StepThroughExpression_Helper"); break;
	}
	
	return index - startIndex;
}
void StepThroughExpression(Expression_t* expression, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpContext_t* context = nullptr, void* userPntr = nullptr)
{
	NotNull2(expression, expression->rootPart);
	u64 numStepsTotal = StepThroughExpression_Helper(expression, expression->rootPart, order, callback, context, userPntr, 0, 0);
	UNUSED(numStepsTotal);
}

// +==================================+
// | ExpressionTypeCheckWalk_Callback |
// +==================================+
// void ExpressionTypeCheckWalk_Callback(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpContext_t* context, void* userPntr)
EXP_STEP_CALLBACK(ExpressionTypeCheckWalk_Callback)
{
	UNUSED(callbackIndex);
	UNUSED(depth);
	NotNull3(expression, part, userPntr);
	ExpTypeCheckState_t* state = (ExpTypeCheckState_t*)userPntr;
	if (state->result != Result_None) { return; } //once we have an error, skip the rest of the callbacks
	
	switch (part->type)
	{
		case ExpPartType_Constant:
		{
			part->evalType = part->constantValue.type;
		} break;
		
		case ExpPartType_Variable:
		{
			if (context == nullptr) { state->result = Result_MissingContext; state->errorPartIndex = part->index; break; }
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
					if (part->evalType == ExpValueType_None)
					{
						state->result = mismatchReason;
						state->errorPartIndex = part->index;
					}
				} break;
				
				// +============================================================+
				// | TypeCheck Equals/NotEquals/GreaterThan/LessThan Operators  |
				// +============================================================+
				case ExpOp_Equals:
				case ExpOp_NotEquals:
				case ExpOp_GreaterThan:
				case ExpOp_GreaterThanOrEqual:
				case ExpOp_LessThan:
				case ExpOp_LessThanOrEqual:
				{
					NotNull2(part->child[0], part->child[1]);
					ExpValueType_t leftOperandType = part->child[0]->evalType;
					ExpValueType_t rightOperandType = part->child[1]->evalType;
					Assert(leftOperandType != ExpValueType_None && rightOperandType != ExpValueType_None);
					Result_t mismatchReason = Result_None;
					ExpValueType_t commonType = GetExpCommonTypeForComparisonOp(leftOperandType, rightOperandType, (part->opType == ExpOp_Equals || part->opType == ExpOp_NotEquals), &mismatchReason);
					if (commonType != ExpValueType_None)
					{
						part->evalType = ExpValueType_Bool;
					}
					else
					{
						state->result = mismatchReason;
						state->errorPartIndex = part->index;
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
					if (!IsExpValueTypeBoolable(leftOperandType)) { state->result = Result_InvalidLeftOperand; state->errorPartIndex = part->index; }
					else if (!IsExpValueTypeBoolable(rightOperandType)) { state->result = Result_InvalidRightOperand; state->errorPartIndex = part->index; }
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
						state->result = Result_InvalidRightOperand;
						state->errorPartIndex = part->index;
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
					if (part->evalType == ExpValueType_None)
					{
						state->result = mismatchReason;
						state->errorPartIndex = part->index;
					}
				} break;
				
				//TODO: ExpOp_BitwiseNot
				
				// +==============================+
				// |  TypeCheck Ternary Operator  |
				// +==============================+
				case ExpOp_Ternary:
				{
					NotNull3(part->child[0], part->child[1], part->child[2]);
					ExpValueType_t conditionType = part->child[0]->evalType;
					ExpValueType_t trueOperandType = part->child[1]->evalType;
					ExpValueType_t falseOperandType = part->child[2]->evalType;
					Assert(conditionType != ExpValueType_None && trueOperandType != ExpValueType_None && falseOperandType != ExpValueType_None);
					if (!IsExpValueTypeBoolable(conditionType))
					{
						state->result = Result_InvalidCondition;
						state->errorPartIndex = part->index;
					}
					else
					{
						// Ternary operators do not combine their true and false values BUT during this typecheck pass we don't know whether the condition is true or not, so we need to report some common type that both sides could be cast to
						Result_t mismatchReason = Result_None;
						part->evalType = GetExpResultTypeForTernaryOp(trueOperandType, falseOperandType, &mismatchReason);
						if (part->evalType == ExpValueType_None)
						{
							state->result = mismatchReason;
							state->errorPartIndex = part->index;
						}
					}
				} break;
				
				// +===============================+
				// | TypeCheck Assignment Operator |
				// +===============================+
				case ExpOp_Assignment:
				case ExpOp_AssignmentAdd:
				case ExpOp_AssignmentSubtract:
				case ExpOp_AssignmentMultiply:
				case ExpOp_AssignmentDivide:
				case ExpOp_AssignmentBitwiseOr:
				case ExpOp_AssignmentBitwiseAnd:
				case ExpOp_AssignmentBitwiseXor:
				{
					NotNull2(part->child[0], part->child[1]);
					ExpValueType_t rightHandType = part->child[1]->evalType;
					Assert(rightHandType != ExpValueType_None);
					if (part->child[0]->type != ExpPartType_Variable)
					{
						state->result = Result_InvalidLeftOperand;
						state->errorPartIndex = part->index;
					}
					else
					{
						Assert(context != nullptr && part->child[0]->variableIndex < context->variableDefs.length);
						ExpVariableDef_t* variableDef = VarArrayGet(&context->variableDefs, part->child[0]->variableIndex, ExpVariableDef_t);
						if (!CanCastExpValueTo(rightHandType, variableDef->type))
						{
							state->result = Result_InvalidRightOperand;
							state->errorPartIndex = part->index;
						}
						else if (part->opType != ExpOp_Assignment && !IsExpValueTypeNumber(rightHandType))
						{
							state->result = Result_InvalidRightOperand;
							state->errorPartIndex = part->index;
						}
						else if (part->opType == ExpOp_AssignmentBitwiseOr || part->opType == ExpOp_AssignmentBitwiseAnd || part->opType == ExpOp_AssignmentBitwiseXor && !IsExpValueTypeInteger(rightHandType))
						{
							state->result = Result_InvalidRightOperand;
							state->errorPartIndex = part->index;
						}
						else if (variableDef->isReadOnly)
						{
							state->result = Result_ReadOnly;
							state->errorPartIndex = part->index;
						}
						else
						{
							part->evalType = variableDef->type;
						}
					}
				} break;
				
				default: AssertMsg(false, "Unhandled ExpOp in ExpressionTypeCheckWalk_Callback"); break;
			}
		} break;
		
		// +==============================+
		// |   TypeCheck Function Part    |
		// +==============================+
		case ExpPartType_Function:
		{
			if (context == nullptr) { state->result = Result_MissingContext; state->errorPartIndex = part->index; break; }
			ExpFuncDef_t* functionDef = VarArrayGet(&context->functionDefs, part->functionIndex, ExpFuncDef_t);
			Assert(part->childCount == functionDef->numArguments);
			for (u64 aIndex = 0; aIndex < functionDef->numArguments; aIndex++)
			{
				ExpPart_t* argument = part->child[aIndex];
				ExpFuncArg_t* argDef = &functionDef->arguments[aIndex];
				NotNull(argument);
				Assert(argument->evalType != ExpValueType_None);
				if (!CanCastExpValueTo(argument->evalType, argDef->type))
				{
					state->result = Result_InvalidArgument;
					state->errorPartIndex = part->index;
					break;
				}
			}
			if (state->result != Result_None) { break; }
			part->evalType = functionDef->returnType;
		} break;
		
		// +==============================+
		// |  TypeCheck Parenthesis Part  |
		// +==============================+
		case ExpPartType_ParenthesisGroup:
		{
			NotNull(part->child[0]);
			Assert(part->child[0]->evalType != ExpValueType_None);
			part->evalType = part->child[0]->evalType;
		} break;
		
		// +==============================+
		// |   TypeCheck TypeCast Part    |
		// +==============================+
		case ExpPartType_TypeCast:
		{
			NotNull(part->child[0]);
			Assert(part->child[0]->evalType != ExpValueType_None);
			if (!CanCastExpValueTo(part->child[0]->evalType, part->castType))
			{
				state->result = Result_InvalidCast;
				state->errorPartIndex = part->index;
				break;
			}
			part->evalType = part->castType;
		} break;
		
		default: AssertMsg(false, "Unhandled ExpPartType in ExpressionTypeCheckWalk_Callback"); state->result = Result_Unknown; break;
	}
}
Result_t ExpressionTypeCheckWalk(Expression_t* expression, const ExpContext_t* context = nullptr, u64* errorPartIndex = nullptr)
{
	ExpTypeCheckState_t state = {};
	state.result = Result_None;
	StepThroughExpression(expression, ExpStepOrder_Postfix, ExpressionTypeCheckWalk_Callback, (ExpContext_t*)context, &state);
	if (state.result == Result_None) { state.result = Result_Success; }
	SetOptionalOutPntr(errorPartIndex, state.errorPartIndex);
	return state.result;
}


// +--------------------------------------------------------------+
// |                          Evaluation                          |
// +--------------------------------------------------------------+
ExpValue_t PerformMathOpOnExpValues(ExpValue_t leftOperand, ExpOp_t opType, ExpValue_t rightOperand)
{
	Assert(leftOperand.type == rightOperand.type);
	ExpValue_t result = {};
	result.type = leftOperand.type;
	
	if (opType == ExpOp_Add)
	{
		switch (result.type)
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
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_Subtract)
	{
		switch (result.type)
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
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_Multiply)
	{
		switch (result.type)
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
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_Divide)
	{
		switch (result.type)
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
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_Modulo)
	{
		switch (result.type)
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
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_BitwiseOr)
	{
		switch (result.type)
		{
			case ExpValueType_I8:  result.valueI8  = (leftOperand.valueI8  | rightOperand.valueI8);  break;
			case ExpValueType_I16: result.valueI16 = (leftOperand.valueI16 | rightOperand.valueI16); break;
			case ExpValueType_I32: result.valueI32 = (leftOperand.valueI32 | rightOperand.valueI32); break;
			case ExpValueType_I64: result.valueI64 = (leftOperand.valueI64 | rightOperand.valueI64); break;
			case ExpValueType_U8:  result.valueU8  = (leftOperand.valueU8  | rightOperand.valueU8);  break;
			case ExpValueType_U16: result.valueU16 = (leftOperand.valueU16 | rightOperand.valueU16); break;
			case ExpValueType_U32: result.valueU32 = (leftOperand.valueU32 | rightOperand.valueU32); break;
			case ExpValueType_U64: result.valueU64 = (leftOperand.valueU64 | rightOperand.valueU64); break;
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_BitwiseAnd)
	{
		switch (result.type)
		{
			case ExpValueType_I8:  result.valueI8  = (leftOperand.valueI8  & rightOperand.valueI8);  break;
			case ExpValueType_I16: result.valueI16 = (leftOperand.valueI16 & rightOperand.valueI16); break;
			case ExpValueType_I32: result.valueI32 = (leftOperand.valueI32 & rightOperand.valueI32); break;
			case ExpValueType_I64: result.valueI64 = (leftOperand.valueI64 & rightOperand.valueI64); break;
			case ExpValueType_U8:  result.valueU8  = (leftOperand.valueU8  & rightOperand.valueU8);  break;
			case ExpValueType_U16: result.valueU16 = (leftOperand.valueU16 & rightOperand.valueU16); break;
			case ExpValueType_U32: result.valueU32 = (leftOperand.valueU32 & rightOperand.valueU32); break;
			case ExpValueType_U64: result.valueU64 = (leftOperand.valueU64 & rightOperand.valueU64); break;
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else if (opType == ExpOp_BitwiseXor)
	{
		switch (result.type)
		{
			case ExpValueType_I8:  result.valueI8  = (leftOperand.valueI8  ^ rightOperand.valueI8);  break;
			case ExpValueType_I16: result.valueI16 = (leftOperand.valueI16 ^ rightOperand.valueI16); break;
			case ExpValueType_I32: result.valueI32 = (leftOperand.valueI32 ^ rightOperand.valueI32); break;
			case ExpValueType_I64: result.valueI64 = (leftOperand.valueI64 ^ rightOperand.valueI64); break;
			case ExpValueType_U8:  result.valueU8  = (leftOperand.valueU8  ^ rightOperand.valueU8);  break;
			case ExpValueType_U16: result.valueU16 = (leftOperand.valueU16 ^ rightOperand.valueU16); break;
			case ExpValueType_U32: result.valueU32 = (leftOperand.valueU32 ^ rightOperand.valueU32); break;
			case ExpValueType_U64: result.valueU64 = (leftOperand.valueU64 ^ rightOperand.valueU64); break;
			default: Assert(false); result.type = ExpValueType_None; break;
		}
	}
	else
	{
		AssertMsg(false, "PerformMapOpOnExpValues does not support the opType that was requested");
		result.type = ExpValueType_None;
	}
	
	return result;
}

// +==============================+
// | EvaluateExpression_Callback  |
// +==============================+
// void EvaluateExpression_Callback(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpContext_t* context, void* userPntr)
EXP_STEP_CALLBACK(EvaluateExpression_Callback)
{
	UNUSED(callbackIndex);
	UNUSED(depth);
	NotNull3(expression, part, userPntr);
	ExpEvaluateState_t* state = (ExpEvaluateState_t*)userPntr;
	if (state->result != Result_None) { return; }
	
	switch (part->type)
	{
		// +==============================+
		// |    Evaluate Constant Part    |
		// +==============================+
		case ExpPartType_Constant:
		{
			if (state->stackSize >= EXPRESSIONS_MAX_EVAL_STACK_SIZE) { state->result = Result_StackOverflow; return; }
			state->stack[state->stackSize++] = part->constantValue;
		} break;
		
		// +==============================+
		// |    Evaluate Variable Part    |
		// +==============================+
		case ExpPartType_Variable:
		{
			if (state->stackSize >= EXPRESSIONS_MAX_EVAL_STACK_SIZE) { state->result = Result_StackOverflow; return; }
			Assert(context != nullptr && part->variableIndex < context->variableDefs.length);
			ExpVariableDef_t* variableDef = VarArrayGet(&context->variableDefs, part->variableIndex, ExpVariableDef_t);
			if (context->isConsoleInput && expression->rootPart == part && expression->numParts == 1 && variableDef->type == ExpValueType_Bool && !variableDef->isReadOnly)
			{
				// When a boolean variable is the entire expression, we actually implicitely toggle the value (but only in debug console input contexts)
				bool currentValue = ReadExpVariableBool(expression, variableDef, context);
				WriteExpVariableBool(variableDef, !currentValue);
				state->stack[state->stackSize++] = NewExpValueBool(!currentValue);
			}
			else
			{
				state->stack[state->stackSize++] = ReadExpVariable_(expression, variableDef, context);
			}
		} break;
		
		// +==============================+
		// |    Evaluate Operator Part    |
		// +==============================+
		case ExpPartType_Operator:
		{
			u8 numOperands = GetExpOperandCount(part->opType);
			if (state->stackSize < numOperands) { state->result = Result_InvalidStack; return; }
			// I know "top", "next" and "final" are not normally how we name variables like this. But since we are popping off in opossite order, I couldn't easily name them "first" "second" third" without doing some shuffling as we decide to pop 2 or 3 operators in the if statements below
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
					
					ExpValue_t result = PerformMathOpOnExpValues(leftOperand, part->opType, rightOperand);
					
					state->stack[state->stackSize++] = result;
				} break;
				
				// +======================================+
				// | Evaluate Equals/NotEquals Operators  |
				// +======================================+
				case ExpOp_Equals:
				case ExpOp_NotEquals:
				{
					ExpValueType_t commonType = GetExpCommonTypeForComparisonOp(nextOperand.type, topOperand.type, true);
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
				
				// +==========================================+
				// | Evaluate GreaterThan/LessThan Operators  |
				// +==========================================+
				case ExpOp_GreaterThan:
				case ExpOp_GreaterThanOrEqual:
				case ExpOp_LessThan:
				case ExpOp_LessThanOrEqual:
				{
					ExpValueType_t commonType = GetExpCommonTypeForComparisonOp(nextOperand.type, topOperand.type, false);
					Assert(commonType != ExpValueType_None);
					ExpValue_t leftOperand = CastExpValue(nextOperand, commonType);
					ExpValue_t rightOperand = CastExpValue(topOperand, commonType);
					
					ExpValue_t result = {};
					result.type = ExpValueType_Bool;
					if (part->opType == ExpOp_GreaterThan)
					{
						switch (commonType)
						{
							case ExpValueType_Bool:    result.valueBool = (leftOperand.valueBool > rightOperand.valueBool); break;
							case ExpValueType_Pointer: result.valueBool = (leftOperand.valuePntr > rightOperand.valuePntr); break;
							case ExpValueType_R32:     result.valueBool = (leftOperand.valueR32  > rightOperand.valueR32);  break;
							case ExpValueType_R64:     result.valueBool = (leftOperand.valueR64  > rightOperand.valueR64);  break;
							case ExpValueType_I8:      result.valueBool = (leftOperand.valueI8   > rightOperand.valueI8);   break;
							case ExpValueType_I16:     result.valueBool = (leftOperand.valueI16  > rightOperand.valueI16);  break;
							case ExpValueType_I32:     result.valueBool = (leftOperand.valueI32  > rightOperand.valueI32);  break;
							case ExpValueType_I64:     result.valueBool = (leftOperand.valueI64  > rightOperand.valueI64);  break;
							case ExpValueType_U8:      result.valueBool = (leftOperand.valueU8   > rightOperand.valueU8);   break;
							case ExpValueType_U16:     result.valueBool = (leftOperand.valueU16  > rightOperand.valueU16);  break;
							case ExpValueType_U32:     result.valueBool = (leftOperand.valueU32  > rightOperand.valueU32);  break;
							case ExpValueType_U64:     result.valueBool = (leftOperand.valueU64  > rightOperand.valueU64);  break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_GreaterThanOrEqual)
					{
						switch (commonType)
						{
							case ExpValueType_Bool:    result.valueBool = (leftOperand.valueBool >= rightOperand.valueBool); break;
							case ExpValueType_Pointer: result.valueBool = (leftOperand.valuePntr >= rightOperand.valuePntr); break;
							case ExpValueType_R32:     result.valueBool = (leftOperand.valueR32  >= rightOperand.valueR32);  break;
							case ExpValueType_R64:     result.valueBool = (leftOperand.valueR64  >= rightOperand.valueR64);  break;
							case ExpValueType_I8:      result.valueBool = (leftOperand.valueI8   >= rightOperand.valueI8);   break;
							case ExpValueType_I16:     result.valueBool = (leftOperand.valueI16  >= rightOperand.valueI16);  break;
							case ExpValueType_I32:     result.valueBool = (leftOperand.valueI32  >= rightOperand.valueI32);  break;
							case ExpValueType_I64:     result.valueBool = (leftOperand.valueI64  >= rightOperand.valueI64);  break;
							case ExpValueType_U8:      result.valueBool = (leftOperand.valueU8   >= rightOperand.valueU8);   break;
							case ExpValueType_U16:     result.valueBool = (leftOperand.valueU16  >= rightOperand.valueU16);  break;
							case ExpValueType_U32:     result.valueBool = (leftOperand.valueU32  >= rightOperand.valueU32);  break;
							case ExpValueType_U64:     result.valueBool = (leftOperand.valueU64  >= rightOperand.valueU64);  break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_LessThan)
					{
						switch (commonType)
						{
							case ExpValueType_Bool:    result.valueBool = (leftOperand.valueBool < rightOperand.valueBool); break;
							case ExpValueType_Pointer: result.valueBool = (leftOperand.valuePntr < rightOperand.valuePntr); break;
							case ExpValueType_R32:     result.valueBool = (leftOperand.valueR32  < rightOperand.valueR32);  break;
							case ExpValueType_R64:     result.valueBool = (leftOperand.valueR64  < rightOperand.valueR64);  break;
							case ExpValueType_I8:      result.valueBool = (leftOperand.valueI8   < rightOperand.valueI8);   break;
							case ExpValueType_I16:     result.valueBool = (leftOperand.valueI16  < rightOperand.valueI16);  break;
							case ExpValueType_I32:     result.valueBool = (leftOperand.valueI32  < rightOperand.valueI32);  break;
							case ExpValueType_I64:     result.valueBool = (leftOperand.valueI64  < rightOperand.valueI64);  break;
							case ExpValueType_U8:      result.valueBool = (leftOperand.valueU8   < rightOperand.valueU8);   break;
							case ExpValueType_U16:     result.valueBool = (leftOperand.valueU16  < rightOperand.valueU16);  break;
							case ExpValueType_U32:     result.valueBool = (leftOperand.valueU32  < rightOperand.valueU32);  break;
							case ExpValueType_U64:     result.valueBool = (leftOperand.valueU64  < rightOperand.valueU64);  break;
							default: Assert(false); break;
						}
					}
					else if (part->opType == ExpOp_LessThanOrEqual)
					{
						switch (commonType)
						{
							case ExpValueType_Bool:    result.valueBool = (leftOperand.valueBool <= rightOperand.valueBool); break;
							case ExpValueType_Pointer: result.valueBool = (leftOperand.valuePntr <= rightOperand.valuePntr); break;
							case ExpValueType_R32:     result.valueBool = (leftOperand.valueR32  <= rightOperand.valueR32);  break;
							case ExpValueType_R64:     result.valueBool = (leftOperand.valueR64  <= rightOperand.valueR64);  break;
							case ExpValueType_I8:      result.valueBool = (leftOperand.valueI8   <= rightOperand.valueI8);   break;
							case ExpValueType_I16:     result.valueBool = (leftOperand.valueI16  <= rightOperand.valueI16);  break;
							case ExpValueType_I32:     result.valueBool = (leftOperand.valueI32  <= rightOperand.valueI32);  break;
							case ExpValueType_I64:     result.valueBool = (leftOperand.valueI64  <= rightOperand.valueI64);  break;
							case ExpValueType_U8:      result.valueBool = (leftOperand.valueU8   <= rightOperand.valueU8);   break;
							case ExpValueType_U16:     result.valueBool = (leftOperand.valueU16  <= rightOperand.valueU16);  break;
							case ExpValueType_U32:     result.valueBool = (leftOperand.valueU32  <= rightOperand.valueU32);  break;
							case ExpValueType_U64:     result.valueBool = (leftOperand.valueU64  <= rightOperand.valueU64);  break;
							default: Assert(false); break;
						}
					}
					
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
					Assert(IsExpValueTypeBoolable(topOperand.type));
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
					
					ExpValue_t result = PerformMathOpOnExpValues(leftOperand, part->opType, rightOperand);
					Assert(result.type == resultType);
					
					state->stack[state->stackSize++] = result;
				} break;
				
				//TODO: ExpOp_BitwiseNot
				
				// +==============================+
				// |  Evaluate Ternary Operator   |
				// +==============================+
				case ExpOp_Ternary:
				{
					Assert(IsExpValueTypeBoolable(finalOperand.type));
					ExpValue_t conditionValue = CastExpValue(finalOperand, ExpValueType_Bool);
					ExpValueType_t resultType = GetExpResultTypeForTernaryOp(nextOperand.type, topOperand.type);
					Assert(resultType != ExpValueType_None);
					ExpValue_t trueResult = CastExpValue(nextOperand, resultType);
					ExpValue_t falseResult = CastExpValue(topOperand, resultType);
					
					state->stack[state->stackSize++] = (conditionValue.valueBool ? trueResult : falseResult);
				} break;
				
				// +==============================+
				// | Evaluate Assignment Operator |
				// +==============================+
				case ExpOp_Assignment:
				case ExpOp_AssignmentAdd:
				case ExpOp_AssignmentSubtract:
				case ExpOp_AssignmentMultiply:
				case ExpOp_AssignmentDivide:
				case ExpOp_AssignmentBitwiseOr:
				case ExpOp_AssignmentBitwiseAnd:
				case ExpOp_AssignmentBitwiseXor:
				{
					Assert(part->child[0] != nullptr && part->child[0]->type == ExpPartType_Variable);
					Assert(context != nullptr && part->child[0]->variableIndex < context->variableDefs.length);
					ExpVariableDef_t* variableDef = VarArrayGet(&context->variableDefs, part->child[0]->variableIndex, ExpVariableDef_t);
					Assert(nextOperand.type == variableDef->type);
					Assert(!variableDef->isReadOnly);
					ExpValue_t currentValue = nextOperand;
					Assert(CanCastExpValueTo(topOperand.type, variableDef->type));
					ExpValue_t newValue = CastExpValue(topOperand, variableDef->type);
					
					if (part->opType == ExpOp_AssignmentAdd)             { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_Add,        newValue); }
					else if (part->opType == ExpOp_AssignmentSubtract)   { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_Subtract,   newValue); }
					else if (part->opType == ExpOp_AssignmentMultiply)   { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_Multiply,   newValue); }
					else if (part->opType == ExpOp_AssignmentDivide)     { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_Divide,     newValue); }
					else if (part->opType == ExpOp_AssignmentBitwiseOr)  { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_BitwiseOr,  newValue); }
					else if (part->opType == ExpOp_AssignmentBitwiseAnd) { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_BitwiseAnd, newValue); }
					else if (part->opType == ExpOp_AssignmentBitwiseXor) { newValue = PerformMathOpOnExpValues(currentValue, ExpOp_BitwiseXor, newValue); }
					Assert(newValue.type == variableDef->type);
					
					WriteExpVariable(variableDef, newValue);
					
					state->stack[state->stackSize++] = newValue;
				} break;
			}
		} break;
		
		// +==============================+
		// |    Evaluate Function Part    |
		// +==============================+
		case ExpPartType_Function:
		{
			Assert(context != nullptr && part->functionIndex < context->functionDefs.length);
			ExpFuncDef_t* funcDef = VarArrayGet(&context->functionDefs, part->functionIndex, ExpFuncDef_t);
			Assert(funcDef->numArguments == part->childCount);
			Assert(funcDef->pntr != nullptr);
			u64 numArgs = funcDef->numArguments;
			if (state->stackSize < numArgs) { state->result = Result_InvalidStack; return; }
			
			//We need an array here so we can pop the arguments off the stack and arrange them in the reverse order so we can pass them all as a pointer to the function
			ExpValue_t arguments[EXPRESSIONS_MAX_FUNC_ARGS];
			for (u64 aIndex = 0; aIndex < numArgs; aIndex++)
			{
				arguments[numArgs-1 - aIndex] = state->stack[state->stackSize-1];
				state->stackSize--;
			}
			for (u64 aIndex = 0; aIndex < numArgs; aIndex++)
			{
				Assert(CanCastExpValueTo(arguments[aIndex].type, funcDef->arguments[aIndex].type));
				arguments[aIndex] = CastExpValue(arguments[aIndex], funcDef->arguments[aIndex].type);
			}
			
			ExpValue_t funcResult = funcDef->pntr(expression, context, numArgs, &arguments[0]);
			
			state->stack[state->stackSize++] = funcResult;
		} break;
		
		case ExpPartType_ParenthesisGroup:
		{
			 //We don't need to do anything for parenthesis at evaluation time
		} break;
		
		// +==============================+
		// |    Evaluate TypeCast Part    |
		// +==============================+
		case ExpPartType_TypeCast:
		{
			if (state->stackSize < 1) { state->result = Result_InvalidStack; return; }
			ExpValue_t operand = state->stack[state->stackSize-1];
			state->stackSize--;
			
			Assert(CanCastExpValueTo(operand.type, part->castType));
			ExpValue_t castValue = CastExpValue(operand, part->castType);
			
			state->stack[state->stackSize++] = castValue;
		} break;
		
		default: AssertMsg(false, "Unhandled ExpPartType in EvaluateExpression_Callback"); break;
	}
}
Result_t EvaluateExpression(Expression_t* expression, ExpContext_t* context = nullptr, ExpValue_t* valueOut = nullptr)
{
	ExpEvaluateState_t state = {};
	state.stackSize = 0;
	state.result = Result_None;
	StepThroughExpression(expression, ExpStepOrder_Postfix, EvaluateExpression_Callback, (ExpContext_t*)context, &state);
	if (state.stackSize == 0) { return Result_Empty; }
	if (state.stackSize > 1) { return Result_InvalidStack; }
	if (state.result == Result_None) { state.result = Result_Success; }
	SetOptionalOutPntr(valueOut, state.stack[0]);
	return state.result;
}

// +--------------------------------------------------------------+
// |                     Ease of Use Wrappers                     |
// +--------------------------------------------------------------+
Result_t TryAddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull2(context, scratchArena);
	AssertIf(context->allocateStrings, context->allocArena != nullptr && context->allocArena != scratchArena);
	PushMemMark(scratchArena);
	
	u64 numTokens = 0;
	ExpToken_t* tokens = nullptr;
	Result_t tokenizeResult = TryTokenizeExpressionStr(funcDefString, scratchArena, &tokens, &numTokens);
	if (tokenizeResult != Result_Success)
	{
		PopMemMark(scratchArena);
		return tokenizeResult;
	}
	AssertIf(numTokens > 0, tokens != nullptr);
	
	ExpFuncDef_t funcDef = {};
	Result_t parseResult = TryCreateExpFuncDefFromTokens(numTokens, tokens, &funcDef, context->allocateStrings ? context->allocArena : nullptr);
	if (parseResult != Result_Success)
	{
		PopMemMark(scratchArena);
		return parseResult;
	}
	
	funcDef.pntr = functionPntr;
	//TODO: Should we somehow split up the documentation string into pieces, with documentation for each argument?
	funcDef.documentation = (context->allocateStrings && !IsEmptyStr(documentation)) ? AllocString(context->allocArena, &documentation) : documentation;
	
	ExpFuncDef_t* newFuncDefSpace = VarArrayAdd(&context->functionDefs, ExpFuncDef_t);
	NotNull(newFuncDefSpace);
	MyMemCopy(newFuncDefSpace, &funcDef, sizeof(ExpFuncDef_t));
	
	PopMemMark(scratchArena);
	return Result_Success;
}
Result_t TryAddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, const char* funcDefStringNt, ExpressionFunc_f* functionPntr, const char* documentationNt = "")
{
	return TryAddExpFuncDefByStr(context, scratchArena, NewStr(funcDefStringNt), functionPntr, NewStr(documentationNt));
}

MyStr_t TryAddExpFuncDefByStrErrorStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
{
	NotNull2(context, scratchArena);
	AssertIf(context->allocateStrings, context->allocArena != nullptr && context->allocArena != scratchArena);
	
	u64 numTokens = 0;
	ExpToken_t* tokens = nullptr;
	Result_t tokenizeResult = TryTokenizeExpressionStr(funcDefString, scratchArena, &tokens, &numTokens);
	if (tokenizeResult != Result_Success)
	{
		//TODO: Can we get the character range where the syntax error occurred?
		return PrintInArenaStr(scratchArena, "Invalid syntax: %s", GetResultStr(tokenizeResult));
	}
	AssertIf(numTokens > 0, tokens != nullptr);
	
	ExpFuncDef_t funcDef = {};
	Result_t parseResult = TryCreateExpFuncDefFromTokens(numTokens, tokens, &funcDef, context->allocateStrings ? context->allocArena : nullptr);
	if (parseResult != Result_Success)
	{
		//TODO: Can we get the character range where the parsing error occurred?
		return PrintInArenaStr(scratchArena, "Invalid definition: %s", GetResultStr(parseResult));
	}
	
	funcDef.pntr = functionPntr;
	//TODO: Should we somehow split up the documentation string into piece, with documentation for each argument?
	funcDef.documentation = (context->allocateStrings && !IsEmptyStr(documentation)) ? AllocString(context->allocArena, &documentation) : documentation;
	
	ExpFuncDef_t* newFuncDefSpace = VarArrayAdd(&context->functionDefs, ExpFuncDef_t);
	NotNull(newFuncDefSpace);
	MyMemCopy(newFuncDefSpace, &funcDef, sizeof(ExpFuncDef_t));
	
	return MyStr_Empty;
}
MyStr_t TryAddExpFuncDefByStrErrorStr(ExpContext_t* context, MemArena_t* scratchArena, const char* funcDefStringNt, ExpressionFunc_f* functionPntr, const char* documentationNt = "")
{
	return TryAddExpFuncDefByStrErrorStr(context, scratchArena, NewStr(funcDefStringNt), functionPntr, NewStr(documentationNt));
}

void AddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
{
	Result_t result = TryAddExpFuncDefByStr(context, scratchArena, funcDefString, functionPntr, documentation);
	Assert(result == Result_Success);
}
void AddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, const char* funcDefStringNt, ExpressionFunc_f* functionPntr, const char* documentationNt = "")
{
	AddExpFuncDefByStr(context, scratchArena, NewStr(funcDefStringNt), functionPntr, NewStr(documentationNt));
}

Result_t ValidateExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpContext_t* context = nullptr)
{
	PushMemMark(scratchArena);
	
	//TODO: Do we actually need to do this?
	ExpContext_t emptyContext = {};
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
	
	PopMemMark(scratchArena);
	return Result_Success;
}

Result_t TryRunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpContext_t* context = nullptr)
{
	PushMemMark(scratchArena);
	
	//TODO: Do we actually need to do this?
	ExpContext_t emptyContext = {};
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

MyStr_t TryRunExpressionErrorStr(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpContext_t* context = nullptr)
{
	ExpContext_t emptyContext = {};
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

ExpValue_t RunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpContext_t* context = nullptr)
{
	ExpValue_t value = {};
	Result_t result = TryRunExpression(expressionStr, scratchArena, &value, context);
	if (result != Result_Success) { value.type = ExpValueType_None; }
	return value;
}

// +--------------------------------------------------------------+
// |                    Autocomplete Functions                    |
// +--------------------------------------------------------------+
bool IsTokenHigherPriorityForAutocomplete(const ExpToken_t* newToken, const ExpToken_t* oldToken)
{
	NotNull2(newToken, oldToken);
	if (newToken->type == oldToken->type) { return false; }
	if (newToken->type == ExpTokenType_Identifier) { return true; }
	return false;
}

//TODO: Test that this function works!
void GetExpAutocompleteInfo(MyStr_t expressionStr, u64 cursorIndex, MemArena_t* scratchArena, ExpAutocompleteInfo_t* infoOut, const ExpContext_t* context = nullptr)
{
	NotNull2(scratchArena, infoOut);
	NotNullStr(&expressionStr);
	Assert(cursorIndex <= expressionStr.length);
	PushMemMark(scratchArena);
	
	ClearPointer(infoOut);
	infoOut->context = context;
	infoOut->expressionStr = expressionStr;
	infoOut->cursorIndex = cursorIndex;
	infoOut->parensBeginIndex = -1;
	infoOut->parensEndIndex = -1;
	
	u64 numTokens = 0;
	ExpToken_t* tokens = nullptr;
	Result_t tokenizeResult = TryTokenizeExpressionStr(expressionStr, scratchArena, &tokens, &numTokens, true);
	Assert(tokenizeResult != Result_AllocFailure);
	if (tokenizeResult == Result_Empty) { PopMemMark(scratchArena); return; }
	
	ExpToken_t* currentTokenPntr = nullptr;
	ExpToken_t* prevTokenPntr = nullptr;
	ExpToken_t* nextTokenPntr = nullptr;
	for (u64 tIndex = 0; tIndex < numTokens; tIndex++)
	{
		ExpToken_t* token = &tokens[tIndex];
		Assert(IsPntrInsideRange(token->str.chars, expressionStr.chars, expressionStr.length, true));
		u64 tokenStartIndex = (u64)(token->str.chars - expressionStr.chars);
		if (IsPntrInsideRange(expressionStr.chars + cursorIndex, token->str.chars, token->str.length, true))
		{
			if (currentTokenPntr == nullptr || IsTokenHigherPriorityForAutocomplete(token, currentTokenPntr))
			{
				currentTokenPntr = token;
				infoOut->currentTokenIndex = tIndex;
			}
		}
		if (tokenStartIndex < cursorIndex)
		{
			prevTokenPntr = token;
			infoOut->prevTokenIndex = tIndex;
		}
		if (tokenStartIndex >= cursorIndex && nextTokenPntr == nullptr)
		{
			nextTokenPntr = token;
			infoOut->nextTokenIndex = tIndex;
		}
	}
	AssertIf(prevTokenPntr != nullptr && nextTokenPntr != nullptr, infoOut->prevTokenIndex+1 == infoOut->nextTokenIndex);
	
	if (prevTokenPntr != nullptr)
	{
		infoOut->prevTokenStartIndex = (u64)(prevTokenPntr->str.chars - expressionStr.chars);
		infoOut->prevTokenEndIndex = infoOut->prevTokenStartIndex + prevTokenPntr->str.length;
		infoOut->prevTokenType = prevTokenPntr->type;
	}
	else { infoOut->isAtBeginning = true; }
	
	if (nextTokenPntr != nullptr)
	{
		infoOut->nextTokenStartIndex = (u64)(nextTokenPntr->str.chars - expressionStr.chars);
		infoOut->nextTokenEndIndex = infoOut->nextTokenStartIndex + nextTokenPntr->str.length;
		infoOut->nextTokenType = nextTokenPntr->type;
	}
	else { infoOut->isAtEnd = true; }
	
	if (currentTokenPntr != nullptr)
	{
		infoOut->currentTokenStartIndex = (u64)(currentTokenPntr->str.chars - expressionStr.chars);
		infoOut->currentTokenEndIndex = infoOut->currentTokenStartIndex + currentTokenPntr->str.length;
		infoOut->currentTokenCursorIndex = cursorIndex - infoOut->currentTokenStartIndex;
		infoOut->isInsideToken = (infoOut->currentTokenCursorIndex > 0 && infoOut->currentTokenCursorIndex < currentTokenPntr->str.length);
		infoOut->isNextToToken = !infoOut->isInsideToken;
		infoOut->currentTokenType = currentTokenPntr->type;
		infoOut->currentTokenStr = currentTokenPntr->str;
	}
	else { infoOut->isBetweenTokens = true; }
	
	if (prevTokenPntr != nullptr)
	{
		u64 numNextCommasAtLocalScope = 0;
		u64 nextCloseParensIndex = 0;
		ExpToken_t* nextCloseParens = nullptr;
		u64 parensLevel = 0;
		for (u64 tIndex = infoOut->nextTokenIndex; tIndex < numTokens; tIndex++)
		{
			ExpToken_t* token = &tokens[tIndex];
			if (token->type == ExpTokenType_Parenthesis)
			{
				if (StrEquals(token->str, ")"))
				{
					if (parensLevel == 0)
					{
						nextCloseParens = token;
						nextCloseParensIndex = tIndex;
						break;
					}
					else { parensLevel--; }
				}
				else if (StrEquals(token->str, "(")) { parensLevel++; }
			}
		}
		if (nextCloseParens != nullptr)
		{
			infoOut->parensEndIndex = (i64)(nextCloseParens->str.chars - expressionStr.chars);
		}
		
		u64 numPrevCommasAtLocalScope = 0;
		u64 prevOpenParensIndex = 0;
		ExpToken_t* prevOpenParens = nullptr;
		parensLevel = 0;
		for (u64 tIndex = infoOut->prevTokenIndex+1; tIndex > 0; tIndex--)
		{
			ExpToken_t* token = &tokens[tIndex-1];
			if (token->type == ExpTokenType_Parenthesis)
			{
				if (StrEquals(token->str, "("))
				{
					if (parensLevel == 0)
					{
						prevOpenParens = token;
						prevOpenParensIndex = tIndex-1;
						break;
					}
					else { parensLevel--; }
				}
				else if (StrEquals(token->str, ")")) { parensLevel++; }
			}
			else if (token->type == ExpTokenType_Comma && parensLevel == 0)
			{
				numPrevCommasAtLocalScope++;
			}
		}
		if (prevOpenParens != nullptr)
		{
			infoOut->parensBeginIndex = (i64)(prevOpenParens->str.chars - expressionStr.chars);
		}
		
		if (prevOpenParens != nullptr && prevOpenParensIndex > 0 && tokens[prevOpenParensIndex-1].type == ExpTokenType_Identifier)
		{
			ExpToken_t* funcNameToken = &tokens[prevOpenParensIndex-1];
			infoOut->insideFuncArgs = true;
			infoOut->currentFuncNameStartIndex = (u64)(funcNameToken->str.chars - expressionStr.chars);
			infoOut->currentFuncNameEndIndex = infoOut->currentFuncNameStartIndex + funcNameToken->str.length;
			infoOut->currentFuncNameStr = funcNameToken->str;
			
			infoOut->currentFuncArgIndex = numPrevCommasAtLocalScope;
			infoOut->currentFuncArgCount = numPrevCommasAtLocalScope + numNextCommasAtLocalScope;
			if (infoOut->currentFuncArgCount == 0 && nextCloseParens != nullptr && nextCloseParensIndex > prevOpenParensIndex+1)
			{
				// If there are any more tokens typed between the open and close parens, then we must be entering at least 1 argument
				// This should help us switch between showing no argument overload and 1+ argument overloads
				infoOut->currentFuncArgCount++;
			}
			
			//TODO: We should probably prioritize the FuncDef that has the least number of arguments while still being >= the number of arguments expected
			u64 matchingFuncDefIndex = 0;
			ExpFuncDef_t* matchingFuncDef = nullptr;
			VarArrayLoop(&context->functionDefs, fIndex)
			{
				VarArrayLoopGet(ExpFuncDef_t, functionDef, &context->functionDefs, fIndex);
				if (StrEquals(functionDef->name, funcNameToken->str))
				{
					if (functionDef->numArguments >= infoOut->currentFuncArgCount)
					{
						matchingFuncDef = functionDef;
						matchingFuncDefIndex = fIndex;
						break;
					}
					else if (matchingFuncDef == nullptr || matchingFuncDef->numArguments < functionDef->numArguments)
					{
						//If we don't find a function with that many arguments, at least fall back to finding the function with the right name with the most arguments
						matchingFuncDef = functionDef;
						matchingFuncDefIndex = fIndex;
					}
				}
			}
			if (matchingFuncDef != nullptr)
			{
				infoOut->funcDefFound = true;
				infoOut->currentFuncDefIndex = matchingFuncDefIndex;
			}
		}
	}
	
	PopMemMark(scratchArena);
}

// +--------------------------------------------------------------+
// |                  Standard Function Library                   |
// +--------------------------------------------------------------+
#pragma warning(push)
#pragma warning(disable:4100) //unreferenced formal parameter
EXPRESSION_FUNC_DEFINITION(Square_Glue)               { EXP_GET_ARG_R32(0, value); return NewExpValueR32(Square(value)); }
EXPRESSION_FUNC_DEFINITION(Cube_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(Cube(value)); }
EXPRESSION_FUNC_DEFINITION(Sin_Glue)                  { EXP_GET_ARG_R32(0, angle); return NewExpValueR32(SinR32(angle)); }
EXPRESSION_FUNC_DEFINITION(Cos_Glue)                  { EXP_GET_ARG_R32(0, angle); return NewExpValueR32(CosR32(angle)); }
EXPRESSION_FUNC_DEFINITION(Tan_Glue)                  { EXP_GET_ARG_R32(0, angle); return NewExpValueR32(TanR32(angle)); }
EXPRESSION_FUNC_DEFINITION(Asin_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(AsinR32(value)); }
EXPRESSION_FUNC_DEFINITION(Acos_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(AcosR32(value)); }
EXPRESSION_FUNC_DEFINITION(Atan2_Glue)                { EXP_GET_ARG_R32(0, y); EXP_GET_ARG_R32(1, x); return NewExpValueR32(AtanR32(y, x)); }
EXPRESSION_FUNC_DEFINITION(Atan_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(AtanJoinedR32(value)); }
EXPRESSION_FUNC_DEFINITION(ToRadians_Glue)            { EXP_GET_ARG_R32(0, degrees); return NewExpValueR32(ToRadians32(degrees)); }
EXPRESSION_FUNC_DEFINITION(ToDegrees_Glue)            { EXP_GET_ARG_R32(0, radians); return NewExpValueR32(ToDegrees32(radians)); }
EXPRESSION_FUNC_DEFINITION(Kilo_Glue)                 { EXP_GET_ARG_U64(0, numKilobytes); return NewExpValueU64(Kilobytes(numKilobytes)); }
EXPRESSION_FUNC_DEFINITION(Mega_Glue)                 { EXP_GET_ARG_U64(0, numMegabytes); return NewExpValueU64(Megabytes(numMegabytes)); }
EXPRESSION_FUNC_DEFINITION(Giga_Glue)                 { EXP_GET_ARG_U64(0, numGigabytes); return NewExpValueU64(Gigabytes(numGigabytes)); }
EXPRESSION_FUNC_DEFINITION(Tera_Glue)                 { EXP_GET_ARG_U64(0, numTerabytes); return NewExpValueU64(Terabytes(numTerabytes)); }
EXPRESSION_FUNC_DEFINITION(Min_Glue)                  { EXP_GET_ARG_I64(0, value1); EXP_GET_ARG_I64(1, value2); return NewExpValueI64(MinI64(value1, value2)); }
EXPRESSION_FUNC_DEFINITION(Max_Glue)                  { EXP_GET_ARG_I64(0, value1); EXP_GET_ARG_I64(1, value2); return NewExpValueI64(MaxI64(value1, value2)); }
EXPRESSION_FUNC_DEFINITION(Abs_Glue)                  { EXP_GET_ARG_R32(0, value); return NewExpValueR32(AbsR32(value)); }
EXPRESSION_FUNC_DEFINITION(Round_Glue)                { EXP_GET_ARG_R32(0, value); return NewExpValueR32(RoundR32(value)); }
EXPRESSION_FUNC_DEFINITION(Roundi_Glue)               { EXP_GET_ARG_R32(0, value); return NewExpValueI64(RoundR32i(value)); }
EXPRESSION_FUNC_DEFINITION(Floor_Glue)                { EXP_GET_ARG_R32(0, value); return NewExpValueR32(FloorR32(value)); }
EXPRESSION_FUNC_DEFINITION(Ceil_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(CeilR32(value)); }
EXPRESSION_FUNC_DEFINITION(Saw_Glue)                  { EXP_GET_ARG_R32(0, angle); return NewExpValueR32(SawR32(angle)); }
EXPRESSION_FUNC_DEFINITION(Pow_Glue)                  { EXP_GET_ARG_R32(0, value); EXP_GET_ARG_R32(1, power); return NewExpValueR32(PowR32(value, power)); }
EXPRESSION_FUNC_DEFINITION(Ln_Glue)                   { EXP_GET_ARG_R32(0, value); return NewExpValueR32(LnR32(value)); }
EXPRESSION_FUNC_DEFINITION(Log2_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(Log2R32(value)); }
#if !ORCA_COMPILATION
EXPRESSION_FUNC_DEFINITION(Log10_Glue)                { EXP_GET_ARG_R32(0, value); return NewExpValueR32(Log10R32(value)); }
#endif
EXPRESSION_FUNC_DEFINITION(Sqrt_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(SqrtR32(value)); }
EXPRESSION_FUNC_DEFINITION(Cbrt_Glue)                 { EXP_GET_ARG_R32(0, value); return NewExpValueR32(CbrtR32(value)); }
EXPRESSION_FUNC_DEFINITION(SignOf_Glue)               { EXP_GET_ARG_R32(0, value); return NewExpValueR32(SignOfR32(value)); }
EXPRESSION_FUNC_DEFINITION(Clamp_Glue)                { EXP_GET_ARG_R32(0, value); EXP_GET_ARG_R32(1, min); EXP_GET_ARG_R32(2, max); return NewExpValueR32(ClampR32(value, min, max)); }
EXPRESSION_FUNC_DEFINITION(Lerp_Glue)                 { EXP_GET_ARG_R32(0, val1); EXP_GET_ARG_R32(1, val2); EXP_GET_ARG_R32(2, amount); return NewExpValueR32(LerpR32(val1, val2, amount)); }
EXPRESSION_FUNC_DEFINITION(LerpClamp_Glue)            { EXP_GET_ARG_R32(0, val1); EXP_GET_ARG_R32(1, val2); EXP_GET_ARG_R32(2, amount); return NewExpValueR32(LerpClampR32(val1, val2, amount)); }
EXPRESSION_FUNC_DEFINITION(DecimalPart_Glue)          { EXP_GET_ARG_R32(0, value); return NewExpValueR32(DecimalPartR32(value)); }
EXPRESSION_FUNC_DEFINITION(BasicallyEqual_Glue)       { EXP_GET_ARG_R32(0, value1); EXP_GET_ARG_R32(1, value2); return NewExpValueBool(BasicallyEqualR32(value1, value2)); }
EXPRESSION_FUNC_DEFINITION(BasicallyGreaterThan_Glue) { EXP_GET_ARG_R32(0, value1); EXP_GET_ARG_R32(1, value2); return NewExpValueBool(BasicallyGreaterThanR32(value1, value2)); }
EXPRESSION_FUNC_DEFINITION(BasicallyLessThan_Glue)    { EXP_GET_ARG_R32(0, value1); EXP_GET_ARG_R32(1, value2); return NewExpValueBool(BasicallyLessThanR32(value1, value2)); }
EXPRESSION_FUNC_DEFINITION(BasicallyBetween_Glue)     { EXP_GET_ARG_R32(0, value); EXP_GET_ARG_R32(1, min); EXP_GET_ARG_R32(2, max); return NewExpValueBool(BasicallyBetweenR32(value, min, max)); }
EXPRESSION_FUNC_DEFINITION(IsInfinite_Glue)           { EXP_GET_ARG_R32(0, value); return NewExpValueR32(IsInfiniteR32(value)); }
EXPRESSION_FUNC_DEFINITION(RoundUpTo_Glue)            { EXP_GET_ARG_U64(0, value); EXP_GET_ARG_U64(0, chunkSize); return NewExpValueU64(RoundUpToU64(value, chunkSize)); }
EXPRESSION_FUNC_DEFINITION(SubAnimAmount_Glue)        { EXP_GET_ARG_R32(0, animAmount); EXP_GET_ARG_R32(1, subPieceStart); EXP_GET_ARG_R32(2, subPieceEnd); return NewExpValueR32(SubAnimAmountR32(animAmount, subPieceStart, subPieceEnd)); }
EXPRESSION_FUNC_DEFINITION(AngleFix_Glue)             { EXP_GET_ARG_R32(0, angle); return NewExpValueR32(AngleFixR32(angle)); }
EXPRESSION_FUNC_DEFINITION(AngleDiff_Glue)            { EXP_GET_ARG_R32(0, left); EXP_GET_ARG_R32(0, right); return NewExpValueR32(AngleDiffR32(left, right)); }
EXPRESSION_FUNC_DEFINITION(AngleOpposite_Glue)        { EXP_GET_ARG_R32(0, angle); return NewExpValueR32(AngleOppositeR32(angle)); }
EXPRESSION_FUNC_DEFINITION(AngleLerp_Glue)            { EXP_GET_ARG_R32(0, angleFrom); EXP_GET_ARG_R32(1, angleTo); EXP_GET_ARG_R32(2, amount); return NewExpValueR32(AngleLerpR32(angleFrom, angleTo, amount)); }
#pragma warning(pop)
void AddStdLibraryFuncsToExpContext(ExpContext_t* context, MemArena_t* scratchArena)
{
	AddExpFuncDefByStr(context, scratchArena, "r32 square(r32 value)",                                      Square_Glue,               "Returns value*value");
	AddExpFuncDefByStr(context, scratchArena, "r32 cube(r32 value)",                                        Cube_Glue,                 "Returns value*value*value");
	AddExpFuncDefByStr(context, scratchArena, "r32 sin(r32 angle)",                                         Sin_Glue,                  "Returns the sine of the angle (angle is in radians)");
	AddExpFuncDefByStr(context, scratchArena, "r32 cos(r32 angle)",                                         Cos_Glue,                  "Returns the cosine of the angle (angle is in radians)");
	AddExpFuncDefByStr(context, scratchArena, "r32 tan(r32 angle)",                                         Tan_Glue,                  "Returns the tangent of the angle (angle is in radians)");
	AddExpFuncDefByStr(context, scratchArena, "r32 asin(r32 value)",                                        Asin_Glue,                 "Returns the arcsine of the value, as radians");
	AddExpFuncDefByStr(context, scratchArena, "r32 acos(r32 value)",                                        Acos_Glue,                 "Returns the arccosine of the value, as radians");
	AddExpFuncDefByStr(context, scratchArena, "r32 atan2(r32 y, r32 x)",                                    Atan2_Glue,                "Returns the arctangent of y/x, as radians");
	AddExpFuncDefByStr(context, scratchArena, "r32 atan(r32 value)",                                        Atan_Glue,                 "Returns the arctangent of the value, as radians");
	AddExpFuncDefByStr(context, scratchArena, "r32 to_radians(r32 degrees)",                                ToRadians_Glue,            "Converts degrees -> radians");
	AddExpFuncDefByStr(context, scratchArena, "r32 to_degrees(r32 radians)",                                ToDegrees_Glue,            "Converts radians -> degrees");
	AddExpFuncDefByStr(context, scratchArena, "u64 kilo(u64 num_kilobytes)",                                Kilo_Glue,                 "Multiplies the input number by 1024 (aka 1kB)");
	AddExpFuncDefByStr(context, scratchArena, "u64 mega(u64 num_megabytes)",                                Mega_Glue,                 "Multiplies the input number by 1024*1024 (aka 1MB)");
	AddExpFuncDefByStr(context, scratchArena, "u64 giga(u64 num_gigabytes)",                                Giga_Glue,                 "Multiplies the input number by 1024*1024*1024 (aka 1GB)");
	AddExpFuncDefByStr(context, scratchArena, "u64 tera(u64 num_terabytes)",                                Tera_Glue,                 "Multiplies the input number by 1024*1024*1024*1024 (aka 1TB)");
	AddExpFuncDefByStr(context, scratchArena, "i64 min(i64 value1, i64 value2)",                            Min_Glue,                  "Returns the minimum of the two values");
	AddExpFuncDefByStr(context, scratchArena, "i64 max(i64 value1, i64 value2)",                            Max_Glue,                  "Returns the maximum of the two values");
	AddExpFuncDefByStr(context, scratchArena, "r32 abs(r32 value)",                                         Abs_Glue,                  "Returns the absolute value of the input");
	AddExpFuncDefByStr(context, scratchArena, "r32 round(r32 value)",                                       Round_Glue,                "Rounds the input to the nearest whole number");
	AddExpFuncDefByStr(context, scratchArena, "i64 roundi(r32 value)",                                      Roundi_Glue,               "Rounds the input to the nearest whole number (and returns the result as an i64)");
	AddExpFuncDefByStr(context, scratchArena, "r32 floor(r32 value)",                                       Floor_Glue,                "Returns the closest whole number that is <= the input");
	AddExpFuncDefByStr(context, scratchArena, "r32 ceil(r32 value)",                                        Ceil_Glue,                 "Returns the closest whole number that is >= the input");
	AddExpFuncDefByStr(context, scratchArena, "r32 saw(r32 angle)",                                         Saw_Glue,                  "Acts like sin(angle) but returns a sawtooth waveform instead");
	AddExpFuncDefByStr(context, scratchArena, "r32 pow(r32 value, r32 power)",                              Pow_Glue,                  "Returns a number raised to a power");
	AddExpFuncDefByStr(context, scratchArena, "r32 ln(r32 value)",                                          Ln_Glue,                   "Returns the natural log of the value");
	AddExpFuncDefByStr(context, scratchArena, "r32 log2(r32 value)",                                        Log2_Glue,                 "Returns the base-2 log of the value");
	#if !ORCA_COMPILATION
	AddExpFuncDefByStr(context, scratchArena, "r32 log10(r32 value)",                                       Log10_Glue,                "Returns the base-10 log of the value");
	#endif
	AddExpFuncDefByStr(context, scratchArena, "r32 sqrt(r32 value)",                                        Sqrt_Glue,                 "Returns the square root of the value");
	AddExpFuncDefByStr(context, scratchArena, "r32 cbrt(r32 value)",                                        Cbrt_Glue,                 "Returns the cube root of the value");
	AddExpFuncDefByStr(context, scratchArena, "r32 sign(r32 value)",                                        SignOf_Glue,               "Returns -1 or 1 based on whether the input is positive or negative (returns 0 for 0)");
	AddExpFuncDefByStr(context, scratchArena, "r32 clamp(r32 value, r32 min, r32 max)",                     Clamp_Glue,                "Returns the input number, but if outside range will return min or max");
	AddExpFuncDefByStr(context, scratchArena, "r32 lerp(r32 val1, r32 val2, r32 amount)",                   Lerp_Glue,                 "Returns some value between val1 and val2 with amount specifying a percentage in the range [0, 1] to interpolate between val1 and val2");
	AddExpFuncDefByStr(context, scratchArena, "r32 lerp_clamp(r32 val1, r32 val2, r32 amount)",             LerpClamp_Glue,            "Same as lerp but will clamp the value to be between val1 and val2, so inputs outside the range [0, 1] for amount won't produce unexpected values");
	AddExpFuncDefByStr(context, scratchArena, "r32 fract(r32 value)",                                       DecimalPart_Glue,          "Returns the decimal portion of the input number. Ex: 7.234 = 0.234");
	AddExpFuncDefByStr(context, scratchArena, "bool equalf(r32 value1, r32 value2)",                        BasicallyEqual_Glue,       "Compares two floating point numbers to see if they are == (with a small tolerance for rounding errors)");
	AddExpFuncDefByStr(context, scratchArena, "bool greaterf(r32 value1, r32 value2)",                      BasicallyGreaterThan_Glue, "Compares two floating point numbers to see if they are >= (with a small tolerance for rounding errors)");
	AddExpFuncDefByStr(context, scratchArena, "bool lessf(r32 value1, r32 value2)",                         BasicallyLessThan_Glue,    "Compares two floating point numbers to see if they are <= (with a small tolerance for rounding errors)");
	AddExpFuncDefByStr(context, scratchArena, "bool betweenf(r32 value, r32 min, r32 max)",                 BasicallyBetween_Glue,     "Checks that the value is within the range [min, max] (with a small tolerance for rounding errors)");
	AddExpFuncDefByStr(context, scratchArena, "bool is_infinite(r32 value)",                                IsInfinite_Glue,           "Returns true if the value is +infinity or -infinity or NaN");
	AddExpFuncDefByStr(context, scratchArena, "u64 round_up_to(u64 value, u64 chunkSize)",                  RoundUpTo_Glue,            "Divides some value by chunkSize and returns the division, with remainders causing a round up");
	AddExpFuncDefByStr(context, scratchArena, "r32 sub_anim_amount(r32 input, r32 sub_start, r32 sub_end)", SubAnimAmount_Glue,        "Given some input in the range [0, 1] and a sub-range within that input, this will produce a new [0, 1] value that is mapped to that sub-range");
	AddExpFuncDefByStr(context, scratchArena, "r32 angle_fix(r32 angle)",                                   AngleFix_Glue,             "Brings a float storing radians within the range of [0, 2*pi) while maintaining the angle value");
	AddExpFuncDefByStr(context, scratchArena, "r32 angle_diff(r32 left, r32 right)",                        AngleDiff_Glue,            "Finds the smallest signed difference between two angles, shortest path, taking into account how 0 == 2*pi");
	AddExpFuncDefByStr(context, scratchArena, "r32 angle_opposite(r32 angle)",                              AngleOpposite_Glue,        "Returns the opposite of a particular angle, always normalized to be between [0, 2*pi)");
	AddExpFuncDefByStr(context, scratchArena, "r32 angle_lerp(r32 start, r32 end, r32 amount)",             AngleLerp_Glue,            "Like lerp, but it will take the shortest path between two angles (even if it crosses 0/2*pi)");
	// AddExpFuncDefByStr(context, scratchArena, "u64 name(u64 arg)", Name_Glue,      "desc");
}

void AddStdLibraryConstantsToExpContext(ExpContext_t* context)
{
	AddExpConstantDef(context, "pi",             NewExpValueR32(Pi32));
	AddExpConstantDef(context, "pi32",           NewExpValueR32(Pi32));
	AddExpConstantDef(context, "pi64",           NewExpValueR64(Pi64));
	AddExpConstantDef(context, "quarter_pi",     NewExpValueR32(QuarterPi32));
	AddExpConstantDef(context, "third_pi",       NewExpValueR32(ThirdPi32));
	AddExpConstantDef(context, "half_pi",        NewExpValueR32(HalfPi32));
	AddExpConstantDef(context, "three_halfs_pi", NewExpValueR32(ThreeHalfsPi32));
	AddExpConstantDef(context, "two_pi",         NewExpValueR32(TwoPi32));
	AddExpConstantDef(context, "e32",            NewExpValueR32(e32));
	AddExpConstantDef(context, "e64",            NewExpValueR64(e64));
	AddExpConstantDef(context, "sqrt2",          NewExpValueR32(Sqrt2_32));
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_EXPRESSION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
EXPRESSIONS_MAX_PART_CHILDREN
EXPRESSIONS_MAX_FUNC_ARGS
EXPRESSIONS_MAX_PARSE_STACK_SIZE
EXPRESSIONS_MAX_EVAL_STACK_SIZE
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
ExpPartType_None
ExpPartType_Constant
ExpPartType_Variable
ExpPartType_Operator
ExpPartType_Function
ExpPartType_ParenthesisGroup
ExpPartType_TypeCast
ExpPartType_NumTypes
ExpTokenType_None
ExpTokenType_Operator
ExpTokenType_Parenthesis
ExpTokenType_Comma
ExpTokenType_Number
ExpTokenType_String
ExpTokenType_Identifier
ExpTokenType_NumTypes
ExpStepOrder_None
ExpStepOrder_Prefix
ExpStepOrder_Natural
ExpStepOrder_Postfix
ExpStepOrder_NumOrders
@Types
ExpressionFunc_f
ExpGetterBool_f
ExpGetterPntr_f
ExpGetterStr_f
ExpGetterR32_f
ExpGetterR64_f
ExpGetterI8_f
ExpGetterI16_f
ExpGetterI32_f
ExpGetterI64_f
ExpGetterU8_f
ExpGetterU16_f
ExpGetterU32_f
ExpGetterU64_f
ExpSetterBool_f
ExpSetterPntr_f
ExpSetterStr_f
ExpSetterR32_f
ExpSetterR64_f
ExpSetterI8_f
ExpSetterI16_f
ExpSetterI32_f
ExpSetterI64_f
ExpSetterU8_f
ExpSetterU16_f
ExpSetterU32_f
ExpSetterU64_f
ExpValueType_t
ExpPartType_t
ExpTokenType_t
ExpStepOrder_t
ExpValue_t
ExpPart_t
ExpPartStack_t
ExpVariableDef_t
ExpFuncArg_t
ExpFuncDef_t
ExpContext_t
Expression_t
ExpToken_t
ExpTokenizer_t
ExpStepCallback_f
ExpTypeCheckState_t
ExpEvaluateState_t
ExpAutocompleteInfo_t
@Functions
ExpValue_t EXPRESSION_FUNC_DEFINITION(Expression_t* expression, ExpContext_t* context, u64 numArgs, const ExpValue_t* args)
type EXPRESSION_VAR_GETTER(struct Expression_t* expression, struct ExpContext_t* context, struct ExpVariableDef_t* variableDef)
void EXPRESSION_VAR_SETTER(struct Expression_t* expression, struct ExpContext_t* context, struct ExpVariableDef_t* variableDef, type newValue)
const char* GetExpValueTypeStr(ExpValueType_t enumValue)
const char* GetExpValueTypeStrLower(ExpValueType_t enumValue)
const char* GetExpOpStr(ExpOp_t enumValue)
const char* GetExpOpSyntaxStr(ExpOp_t enumValue, bool secondPart = false)
const char* GetExpPartTypeStr(ExpPartType_t enumValue)
const char* GetExpTokenTypeStr(ExpTokenType_t enumValue)
const char* GetExpStepOrderStr(ExpStepOrder_t enumValue)
void EXP_STEP_CALLBACK(Expression_t* expression, ExpPart_t* part, u64 callbackIndex, u64 depth, ExpContext_t* context, void* userPntr)
#define EXP_GET_ARG_BOOL(index, argName)
#define EXP_GET_ARG_STR(index, argName)
#define EXP_GET_ARG_R32(index, argName)
#define EXP_GET_ARG_R64(index, argName)
#define EXP_GET_ARG_I8(index, argName)
#define EXP_GET_ARG_I16(index, argName)
#define EXP_GET_ARG_I32(index, argName)
#define EXP_GET_ARG_I64(index, argName)
#define EXP_GET_ARG_U8(index, argName)
#define EXP_GET_ARG_U16(index, argName)
#define EXP_GET_ARG_U32(index, argName)
#define EXP_GET_ARG_U64(index, argName)
void FreeExpValue(MemArena_t* allocArena, ExpValue_t* value)
void FreeExpression(Expression_t* expression)
void FreeExpFuncDef(ExpFuncDef_t* funcDef, MemArena_t* allocArena)
void FreeExpContext(ExpContext_t* context)
void CopyExpFuncDef(ExpFuncDef_t* dest, ExpFuncDef_t* source, MemArena_t* memArena = nullptr)
ExpValue_t NewExpValueVoid()
ExpValue_t NewExpValueBool(bool value)
ExpValue_t NewExpValueStr(MyStr_t value)
ExpValue_t NewExpValueStr(const char* valueNt)
ExpValue_t NewExpValuePntr(u64 typeId, void* value)
ExpValue_t NewExpValueR32(r32 value)
ExpValue_t NewExpValueR64(r64 value)
ExpValue_t NewExpValueI8(i8 value)
ExpValue_t NewExpValueI16(i16 value)
ExpValue_t NewExpValueI32(i32 value)
ExpValue_t NewExpValueI64(i64 value)
ExpValue_t NewExpValueU8(u8 value)
ExpValue_t NewExpValueU16(u16 value)
ExpValue_t NewExpValueU32(u32 value)
ExpValue_t NewExpValueU64(u64 value)
u8 GetExpOperandCount(ExpOp_t opType)
u8 GetExpOpPrecedence(ExpOp_t opType)
bool IsExpPartReadyToBeOperand(const ExpPart_t* expPart)
bool CanExpPartProduceLeftHandOperand(const ExpPart_t* expPart)
inline bool IsExpValueTypeInteger(ExpValueType_t type)
inline bool IsExpValueTypeFloat(ExpValueType_t type)
inline bool IsExpValueTypeSigned(ExpValueType_t type)
inline bool IsExpValueTypeNumber(ExpValueType_t type)
inline bool IsExpValueTypeConstantCompat(ExpValueType_t type)
inline bool IsExpValueTypeBoolable(ExpValueType_t type)
inline bool CanExpValueTypeConvertTo(ExpValueType_t type, ExpValueType_t outType)
inline bool CanCastExpValueTo(ExpValueType_t valueType, ExpValueType_t type)
inline u8 GetExpValueTypeByteSize(ExpValueType_t type)
MyStr_t ExpValueToStr(ExpValue_t value, MemArena_t* memArena, bool includeType = false)
void InitExpContext(MemArena_t* memArena, ExpContext_t* contextOut, bool allocateStrings = true)
ExpConstantDef_t* FindExpConstantDef(ExpContext_t* context, MyStr_t constantName, u64* indexOut = nullptr)
ExpVariableDef_t* FindExpVariableDef(ExpContext_t* context, MyStr_t variableName, u64* indexOut = nullptr)
ExpFuncDef_t* FindExpFuncDef(ExpContext_t* context, MyStr_t functionName, u64 numArguments = UINT64_MAX, u64* indexOut = nullptr)
ExpConstantDef_t* AddExpConstantDef(ExpContext_t* context, MyStr_t constantName, ExpValue_t value, MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDef_(ExpContext_t* context, bool writeable, MyStr_t variableName, ExpValueType_t type, u64 pntrSize, const void* pntr, MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefFuncs_(ExpContext_t* context, MyStr_t variableName, ExpValueType_t type, void* getter, void* setter, MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefBool(ExpContext_t* context, bool writeable, MyStr_t     variableName,   const bool* pntr,    MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefPntr(ExpContext_t* context, bool writeable, MyStr_t     variableName,   const void** pntr,   MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefStr(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const MyStr_t* pntr, MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefR32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const r32* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefR64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const r64* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefI8(ExpContext_t*   context, bool writeable, MyStr_t     variableName,   const i8*  pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefI16(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i16* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefI32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i32* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefI64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const i64* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefU8(ExpContext_t*   context, bool writeable, MyStr_t     variableName,   const u8*  pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefU16(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u16* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefU32(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u32* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpVariableDef_t* AddExpVariableDefU64(ExpContext_t*  context, bool writeable, MyStr_t     variableName,   const u64* pntr,     MyStr_t documentation = MyStr_Empty_Const)
ExpFuncDef_t* AddExpFuncDef(ExpContext_t* context, ExpValueType_t returnType, MyStr_t functionName, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
ExpFuncArg_t* AddExpFuncArg(ExpContext_t* context, ExpFuncDef_t* funcDef, ExpValueType_t argumentType, MyStr_t argumentName, MyStr_t documentation = MyStr_Empty_Const)
ExpValueType_t GetExpResultTypeForMathOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isSubtractOp, Result_t* reasonOut = nullptr)
ExpValueType_t GetExpResultTypeForTernaryOp(ExpValueType_t trueOperandType, ExpValueType_t falseOperandType, Result_t* reasonOut = nullptr)
ExpValueType_t GetExpIntegerTypeForBitwiseOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isAndOp, Result_t* reasonOut = nullptr)
ExpValueType_t GetExpCommonTypeForComparisonOp(ExpValueType_t leftOperandType, ExpValueType_t rightOperandType, bool isStraightEqualityOp, Result_t* reasonOut = nullptr)
ExpValue_t CastExpValue(ExpValue_t value, ExpValueType_t type)
void WriteExpVariable_(ExpVariableDef_t* variableDef, u64 valueSize, const void* valuePntr)
void WriteExpVariable(ExpVariableDef_t* variableDef, ExpValue_t value)
void WriteExpVariableBool(ExpVariableDef_t* variableDef, bool value)
void WriteExpVariablePointer(ExpVariableDef_t* variableDef, void* value)
void WriteExpVariableString(ExpVariableDef_t* variableDef, MyStr_t value)
void WriteExpVariableR32(ExpVariableDef_t* variableDef, r32 value)
void WriteExpVariableR64(ExpVariableDef_t* variableDef, r64 value)
void WriteExpVariableI8(ExpVariableDef_t* variableDef, i8 value)
void WriteExpVariableI16(ExpVariableDef_t* variableDef, i16 value)
void WriteExpVariableI32(ExpVariableDef_t* variableDef, i32 value)
void WriteExpVariableI64(ExpVariableDef_t* variableDef, i64 value)
void WriteExpVariableU8(ExpVariableDef_t* variableDef, u8 value)
void WriteExpVariableU16(ExpVariableDef_t* variableDef, u16 value)
void WriteExpVariableU32(ExpVariableDef_t* variableDef, u32 value)
void WriteExpVariableU64(ExpVariableDef_t* variableDef, u64 value)
ExpValue_t ReadExpVariable_(ExpVariableDef_t* variableDef)
bool ReadExpVariableBool(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
void* ReadExpVariablePointer(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
r32 ReadExpVariableR32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
r64 ReadExpVariableR64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
i8 ReadExpVariableI8(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
i16 ReadExpVariableI16(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
i32 ReadExpVariableI32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
i64 ReadExpVariableI64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
u8 ReadExpVariableU8(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
u16 ReadExpVariableU16(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
u32 ReadExpVariableU32(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
u64 ReadExpVariableU64(Expression_t* expression, ExpVariableDef_t* variableDef, ExpContext_t* context = nullptr)
ExpToken_t NewExpToken(ExpTokenType_t type, MyStr_t tokenStr)
ExpTokenizer_t NewExpTokenizer(MyStr_t expressionStr)
bool ExpTokenizerGetNext(ExpTokenizer_t* tokenizer, ExpToken_t* tokenOut, Result_t* errorOut = nullptr, bool allowErrors = false)
Result_t TryTokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, ExpToken_t** tokensOut, u64* numTokensOut, bool allowErrors = false)
ExpToken_t* TokenizeExpressionStr(MyStr_t expressionStr, MemArena_t* memArena, u64* numTokensOut, Result_t* resultOut = nullptr, bool allowErrors = false)
MyStr_t UnescapeExpressionStr(MemArena_t* memArena, MyStr_t string)
MyStr_t EscapeExpressionStr(MemArena_t* memArena, MyStr_t string)
void PushExpPart(ExpPartStack_t* stack, ExpPart_t* partPntr)
ExpPart_t* PopExpPart(ExpPartStack_t* stack)
ExpPart_t* PeekExpPart(ExpPartStack_t* stack)
void PushAndConnectExpPart(ExpPartStack_t* stack, ExpPart_t* partPntr)
ExpValue_t ConvertExpNumberToken(MyStr_t numberStr)
ExpPart_t* SplitExpPartTreeWithPrecedenceAtLeast(ExpPart_t* leftPart, ExpOp_t opType, ExpPart_t** remainingPortionOut)
bool FindExpClosingParensToken(u64 numTokens, const ExpToken_t* tokens, u64 startIndex, u64* indexOut)
ExpPart_t* AddExpPart(Expression_t* expression, u64 tokenIndex, ExpPartType_t type)
ExpPart_t* AddExpConstantString(Expression_t* expression, u64 tokenIndex, MyStr_t value)
ExpPart_t* AddExpVariable(Expression_t* expression, u64 tokenIndex, u64 variableIndex)
ExpPart_t* AddExpOperator(Expression_t* expression, u64 tokenIndex, ExpOp_t opType, ExpPart_t* firstChild = nullptr, ExpPart_t* secondChild = nullptr, ExpPart_t* thirdChild = nullptr)
ExpPart_t* AddExpFunction(Expression_t* expression, u64 tokenIndex, u64 functionIndex)
ExpPart_t* AddExpParenthesisGroup(Expression_t* expression, u64 tokenIndex, ExpPart_t* childRoot)
Result_t TryCreateExpressionFromTokens_Helper(Expression_t* expression, const ExpContext_t* context, u64 numTokens, const ExpToken_t* tokens, ExpPart_t** rootOut, ExpPart_t* functionPart = nullptr)
Result_t TryCreateExpressionFromTokens(const ExpContext_t* context, u64 numTokens, const ExpToken_t* tokens, Expression_t* expressionOut, MemArena_t* memArena = nullptr)
Result_t TryCreateExpFuncDefFromTokens(u64 numTokens, const ExpToken_t* tokens, ExpFuncDef_t* funcDefOut, MemArena_t* memArena = nullptr)
ExpFuncDef_t CreateExpFuncDefFromTokens(u64 numTokens, const ExpToken_t* tokens, MemArena_t* memArena = nullptr)
u64 StepThroughExpression_Helper(Expression_t* expression, ExpPart_t* part, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpContext_t* context, void* userPntr, u64 startIndex, u64 depth)
void StepThroughExpression(Expression_t* expression, ExpStepOrder_t order, ExpStepCallback_f* callback, ExpContext_t* context = nullptr, void* userPntr = nullptr)
Result_t ExpressionTypeCheckWalk(Expression_t* expression, const ExpContext_t* context = nullptr, u64* errorPartIndex = nullptr)
ExpValue_t PerformMathOpOnExpValues(ExpValue_t leftOperand, ExpOp_t opType, ExpValue_t rightOperand)
Result_t EvaluateExpression(Expression_t* expression, ExpContext_t* context = nullptr, ExpValue_t* valueOut = nullptr)
Result_t TryAddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
MyStr_t TryAddExpFuncDefByStrErrorStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
void AddExpFuncDefByStr(ExpContext_t* context, MemArena_t* scratchArena, MyStr_t funcDefString, ExpressionFunc_f* functionPntr, MyStr_t documentation = MyStr_Empty_Const)
Result_t ValidateExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpContext_t* context = nullptr)
Result_t TryRunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpContext_t* context = nullptr)
MyStr_t TryRunExpressionErrorStr(MyStr_t expressionStr, MemArena_t* scratchArena, ExpValue_t* valueOut = nullptr, ExpContext_t* context = nullptr)
ExpValue_t RunExpression(MyStr_t expressionStr, MemArena_t* scratchArena, ExpContext_t* context = nullptr)
bool IsTokenHigherPriorityForAutocomplete(const ExpToken_t* newToken, const ExpToken_t* oldToken)
void GetExpAutocompleteInfo(MyStr_t expressionStr, u64 cursorIndex, MemArena_t* scratchArena, ExpAutocompleteInfo_t* infoOut, const ExpContext_t* context = nullptr)
void AddStdLibraryFuncsToExpContext(ExpContext_t* context, MemArena_t* scratchArena)
void AddStdLibraryConstantsToExpContext(ExpContext_t* context)
*/
