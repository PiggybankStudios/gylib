/*
File:   gy_test_expressions.h
Author: Taylor Robbins
Date:   06\02\2024
*/

#ifndef _GY_TEST_EXPRESSIONS_H
#define _GY_TEST_EXPRESSIONS_H

void GyTestCase_ExpTokenizer(const char* expressionStr, Result_t expectedResult, u64 expectedTokenCount, ExpTokenType_t* expectedTokenTypes, const char** expectedTokenStrs)
{
	ExpTokenizer_t tokenizer = NewExpTokenizer(NewStr(expressionStr));
	ExpToken_t token;
	Result_t error = Result_None;
	u64 tIndex = 0;
	while (ExpTokenizerGetNext(&tokenizer, &token, &error))
	{
		Assert(tIndex < expectedTokenCount);
		Assert(token.type == expectedTokenTypes[tIndex]);
		Assert(StrEquals(token.str, expectedTokenStrs[tIndex]));
		tIndex++;
	}
	Assert(tIndex == expectedTokenCount);
	Assert(error == expectedResult);
}

void GyTestCase_UnescapeExpressionStr(const char* escapedStr, const char* unescapedStr)
{
	char stackArray[256];
	MemArena_t stackArena;
	InitMemArena_Buffer(&stackArena, sizeof(stackArray), &stackArray[0], true);
	MyStr_t result = UnescapeExpressionStr(&stackArena, NewStr(escapedStr));
	Assert(StrEquals(result, unescapedStr));
}

void GyTestCase_ExpNumberConversion(const char* numberStr, ExpValueType_t expectedType, i64 expectedValueInt, r64 expectedValueFloat = 0.0)
{
	ExpValue_t expValue = ConvertExpNumberToken(NewStr(numberStr));
	Assert(expValue.type == expectedType);
	AssertIf(expectedType == ExpValueType_R32, expValue.valueR32 == (r32)expectedValueFloat);
	AssertIf(expectedType == ExpValueType_R64, expValue.valueR64 ==      expectedValueFloat);
	AssertIf(expectedType == ExpValueType_I8,  expValue.valueI8  ==  (i8)expectedValueInt);
	AssertIf(expectedType == ExpValueType_I16, expValue.valueI16 == (i16)expectedValueInt);
	AssertIf(expectedType == ExpValueType_I32, expValue.valueI32 == (i32)expectedValueInt);
	AssertIf(expectedType == ExpValueType_I64, expValue.valueI64 ==      expectedValueInt);
	AssertIf(expectedType == ExpValueType_U8,  expValue.valueU8  ==  (u8)expectedValueInt);
	AssertIf(expectedType == ExpValueType_U16, expValue.valueU16 == (u16)expectedValueInt);
	AssertIf(expectedType == ExpValueType_U32, expValue.valueU32 == (u32)expectedValueInt);
	AssertIf(expectedType == ExpValueType_U64, expValue.valueU64 == (u64)expectedValueInt);
}

void GyTestCase_PrintExpPartHelper(const ExpPart_t* expPart)
{
	switch (expPart->type)
	{
		case ExpPartType_Constant:
		{
			GyLibPrint_I("\tPart[%llu] %s: ", expPart->index, GetExpValueTypeStr(expPart->constantValue.type));
			switch (expPart->constantValue.type)
			{
				case ExpValueType_R32: GyLibPrintLine_I("%g",                 expPart->constantValue.valueR32); break;
				case ExpValueType_R64: GyLibPrintLine_I("%lg",                expPart->constantValue.valueR64); break;
				case ExpValueType_I8:  GyLibPrintLine_I("%d",            (int)expPart->constantValue.valueI8);  break;
				case ExpValueType_I16: GyLibPrintLine_I("%d",            (int)expPart->constantValue.valueI16); break;
				case ExpValueType_I32: GyLibPrintLine_I("%d",            (int)expPart->constantValue.valueI32); break;
				case ExpValueType_I64: GyLibPrintLine_I("%lld",               expPart->constantValue.valueI64); break;
				case ExpValueType_U8:  GyLibPrintLine_I("%u",   (unsigned int)expPart->constantValue.valueU8);  break;
				case ExpValueType_U16: GyLibPrintLine_I("%u",   (unsigned int)expPart->constantValue.valueU16); break;
				case ExpValueType_U32: GyLibPrintLine_I("%u",   (unsigned int)expPart->constantValue.valueU32); break;
				case ExpValueType_U64: GyLibPrintLine_I("%llu", expPart->constantValue.valueU64);               break;
				default: GyLibPrintLine_I("UnknownType: %u", expPart->constantValue.type); break;
			}
		} break;
		
		case ExpPartType_Operator:
		{
			GyLibPrintLine_I("\tPart[%llu] %s (%s):", expPart->index, GetExpOpStr(expPart->opType), GetExpOpSyntaxStr(expPart->opType));
			u8 numOperands = GetExpOperandCount(expPart->opType);
			for (u8 oIndex = 0; oIndex < numOperands; oIndex++)
			{
				ExpPart_t* operand = expPart->child[oIndex];
				if (operand != nullptr)
				{
					GyLibPrintLine_D("\t\tOperand[%u] Part[%llu] (%s)", (u32)oIndex, operand->index, GetExpPartTypeStr(operand->type));
				}
				else
				{
					GyLibPrintLine_E("\t\tOperand[%u] Empty", (u32)oIndex);
				}
			}
		} break;
		
		default:
		{
			GyLibPrintLine_E("\tPart[%llu] UnknownType %d", expPart->index, expPart->type);
		} break;
	}
}

void GyTestCase_PrintParse(const char* expressionStr)
{
	char stackArray[256];
	MemArena_t stackArena;
	InitMemArena_Buffer(&stackArena, sizeof(stackArray), &stackArray[0], true);
	
	u64 numTokens = 0;
	ExpToken_t* tokens = nullptr;
	Result_t tokenizeResult = TryTokenizeExpressionStr(NewStr(expressionStr), &stackArena, &tokens, &numTokens);
	Assert(tokenizeResult == Result_Finished);
	AssertIf(numTokens > 0, tokens != nullptr);
	
	ExpressionContext_t context = {};
	
	Expression_t expression = {};
	Result_t parseResult = TryCreateExpressionFromTokens(&context, numTokens, tokens, &expression);
	if (parseResult != Result_Success)
	{
		GyLibPrintLine_E("Expression \"%s\" failed to parse: Result_%s", expressionStr, GetResultStr(parseResult));
		return;
	}
	
	GyLibPrintLine_N("Expression \"%s\" has %llu part%s:", expressionStr, expression.numParts, Plural(expression.numParts, "s"));
	if (expression.rootPart != nullptr) { GyLibPrintLine_O("\tRoot = Part[%llu] (%s)", expression.rootPart->index, GetExpPartTypeStr(expression.rootPart->type)); }
	else { GyLibWriteLine_E("\tRoot = Empty"); }
	for (u64 pIndex = 0; pIndex < expression.numParts; pIndex++)
	{
		GyTestCase_PrintExpPartHelper(&expression.parts[pIndex]);
	}
}

void GyTest_Expressions()
{
	// +--------------------------------------------------------------+
	// |                       Tokenizer Tests                        |
	// +--------------------------------------------------------------+
	{
		GyTestCase_ExpTokenizer("", Result_Finished, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer(" \t     \t\t\t\t\t   \t\t ", Result_Finished, 0, nullptr, nullptr);
		
		ExpTokenType_t tokenTypes1[] = { ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Number };
		const char* tokenStrs1[]  = { "a", "+", "b", "*", "100" };
		GyTestCase_ExpTokenizer("a + b * 100", Result_Finished, ArrayCount(tokenTypes1), tokenTypes1, tokenStrs1);
		
		ExpTokenType_t tokenTypes2[] = { ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier };
		const char* tokenStrs2[]  = { "a", "+", "b", "-", "c", "*", "d", "/", "e", "%", "f", "^", "g" };
		GyTestCase_ExpTokenizer("a+b-c*d/e%f^g", Result_Finished, ArrayCount(tokenTypes2), tokenTypes2, tokenStrs2);
		
		ExpTokenType_t tokenTypes3[] = { ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Number };
		const char* tokenStrs3[]  = { "1", "+", "2", "-", "3", "*", "4", "/", "5", "%", "6", "^", "a", "-", "8" };
		GyTestCase_ExpTokenizer("1+2-3*4/5%6^a-8", Result_Finished, ArrayCount(tokenTypes3), tokenTypes3, tokenStrs3);
		
		ExpTokenType_t tokenTypes4[] = { ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier };
		const char* tokenStrs4[]  = { "a", "&&", "1", "||", "b", "==", "c", "&&", "!", "d", "&&", "~", "e", "^", "f", "|", "g", "&", "h" };
		GyTestCase_ExpTokenizer("a && 1 || b == c && !d && ~e ^ f | g & h", Result_Finished, ArrayCount(tokenTypes4), tokenTypes4, tokenStrs4);
		
		ExpTokenType_t tokenTypes5[] = { ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number, ExpTokenType_Operator, ExpTokenType_Number };
		const char* tokenStrs5[]  = { "-1", "+", ".0", "-", "-1.02", "+", "7.", "+", "1234567890.1234567890" };
		GyTestCase_ExpTokenizer("-1 + .0 --1.02 + 7. + 1234567890.1234567890", Result_Finished, ArrayCount(tokenTypes5), tokenTypes5, tokenStrs5);
		
		ExpTokenType_t tokenTypes6[] = { ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_Identifier };
		const char* tokenStrs6[]  = { "_var1", "+", "____", "-", "abcdefghijklmnopqrstuvwxyz_0123456789" };
		GyTestCase_ExpTokenizer("_var1 + ____ - abcdefghijklmnopqrstuvwxyz_0123456789", Result_Finished, ArrayCount(tokenTypes6), tokenTypes6, tokenStrs6);
		
		ExpTokenType_t tokenTypes7[] = { ExpTokenType_Identifier };
		const char* tokenStrs7[]  = { "foo" };
		GyTestCase_ExpTokenizer("foo.bar", Result_InvalidChar, ArrayCount(tokenTypes7), tokenTypes7, tokenStrs7);
		
		ExpTokenType_t tokenTypes8[] = { ExpTokenType_Identifier, ExpTokenType_Operator, ExpTokenType_String };
		const char* tokenStrs8[]  = { "var", "=", "str\\\"ing" };
		GyTestCase_ExpTokenizer("var = \"str\\\"ing\"", Result_Finished, ArrayCount(tokenTypes8), tokenTypes8, tokenStrs8);
		
		GyTestCase_ExpTokenizer("12var", Result_InvalidIdentifier, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("12.3f", Result_InvalidIdentifier, 0, nullptr, nullptr);
		
		GyTestCase_ExpTokenizer("@",  Result_InvalidChar, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("#",  Result_InvalidChar, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("$",  Result_InvalidChar, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("\\", Result_InvalidChar, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("`",  Result_InvalidChar, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("\n", Result_InvalidChar, 0, nullptr, nullptr);
		GyTestCase_ExpTokenizer("\r", Result_InvalidChar, 0, nullptr, nullptr);
	}
	
	GyTestCase_UnescapeExpressionStr("Regular String", "Regular String");
	GyTestCase_UnescapeExpressionStr("\\\\ \\\\\\\\ \\\" \\' \\n \\r \\t", "\\ \\\\ \" ' \n \r \t");
	
	GyTestCase_ExpNumberConversion("1", ExpValueType_U8, 1);
	GyTestCase_ExpNumberConversion("00000000001", ExpValueType_U8, 1);
	GyTestCase_ExpNumberConversion("-1", ExpValueType_I8, -1);
	GyTestCase_ExpNumberConversion("255", ExpValueType_U8, 255);
	GyTestCase_ExpNumberConversion("256", ExpValueType_U16, 256);
	GyTestCase_ExpNumberConversion("-128", ExpValueType_I8, -128);
	GyTestCase_ExpNumberConversion("-129", ExpValueType_I16, -129);
	GyTestCase_ExpNumberConversion("65535", ExpValueType_U16, 65535);
	GyTestCase_ExpNumberConversion("65536", ExpValueType_U32, 65536);
	GyTestCase_ExpNumberConversion("-32768", ExpValueType_I16, -32768);
	GyTestCase_ExpNumberConversion("-32769", ExpValueType_I32, -32769);
	GyTestCase_ExpNumberConversion("4294967295", ExpValueType_U32, 4294967295ULL);
	GyTestCase_ExpNumberConversion("4294967296", ExpValueType_U64, 4294967296ULL);
	GyTestCase_ExpNumberConversion("-2147483648", ExpValueType_I32, -2147483648LL);
	GyTestCase_ExpNumberConversion("-2147483649", ExpValueType_I64, -2147483649LL);
	GyTestCase_ExpNumberConversion("1.0", ExpValueType_R32, 0, 1.0f);
	GyTestCase_ExpNumberConversion("1.000000000", ExpValueType_R32, 0, 1.0f);
	GyTestCase_ExpNumberConversion("0.1", ExpValueType_R32, 0, 0.1f);
	GyTestCase_ExpNumberConversion("0.00001", ExpValueType_R32, 0, 0.00001f);
	GyTestCase_ExpNumberConversion("3.14159", ExpValueType_R32, 0, 3.14159f);
	GyTestCase_ExpNumberConversion("123456.0", ExpValueType_R32, 0, 123456.0f);
	GyTestCase_ExpNumberConversion("1234567.0", ExpValueType_R64, 0, 1234567.0);
	GyTestCase_ExpNumberConversion("100.001", ExpValueType_R32, 0, 100.001f);
	GyTestCase_ExpNumberConversion("1000.001", ExpValueType_R64, 0, 1000.001);
	GyTestCase_ExpNumberConversion(".1", ExpValueType_R32, 0, 0.1f);
	GyTestCase_ExpNumberConversion(".001", ExpValueType_R32, 0, 0.001f);
	GyTestCase_ExpNumberConversion("0000.1", ExpValueType_R32, 0, 0.1f);
	GyTestCase_ExpNumberConversion("0000.001", ExpValueType_R32, 0, 0.001f);
	
	GyTestCase_PrintParse("3 + 8");
	GyTestCase_PrintParse("3 + 8 * !1500");
}

#endif //  _GY_TEST_EXPRESSIONS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Functions
void GyTest_Expressions()
*/
