/*
File:   gy_math.h
Author: Taylor Robbins
Date:   09\14\2021
Description:
	** Contains a bunch of functions that don't really belong to any other file but are useful things to have around
	** Usually this are common mathematical operations so we all this file gy_math
	
	NOTE: Include gy_lookup.h BEFORE this file to make some operations (like ReduceRatioU32) faster
*/

#ifndef _GY_MATH_H
#define _GY_MATH_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"

// +--------------------------------------------------------------+
// |                 Normalized Float Operations                  |
// +--------------------------------------------------------------+
r32 SubAnimAmountR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
{
	return ClampR32((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0f, 1.0f);
}
r64 SubAnimAmountR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
{
	return ClampR64((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0, 1.0);
}

r32 SubAnimAmountTwoWayR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
{
	r32 subAnimAmount = ClampR32((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0f, 1.0f);
	if (subAnimAmount < 0.5f) { return subAnimAmount*2; }
	else { return (1.0f-subAnimAmount)*2; }
}
r64 SubAnimAmountTwoWayR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
{
	r64 subAnimAmount = ClampR64((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0, 1.0);
	if (subAnimAmount < 0.5) { return subAnimAmount*2; }
	else { return (1.0-subAnimAmount)*2; }
}

// +--------------------------------------------------------------+
// |                   Angle Related Functions                    |
// +--------------------------------------------------------------+
r32 AngleFixR32(r32 angle)
{
	if (IsInfiniteR32(angle)) { return angle; }
	r32 result = angle;
	if (result >= TwoPi32) { result = ModR32(result, TwoPi32); }
	if (result < 0) { result = TwoPi32 - ModR32(-result, TwoPi32); }
	return result;
}
r64 AngleFixR64(r64 angle)
{
	if (IsInfiniteR64(angle)) { return angle; }
	r64 result = angle;
	if (result >= TwoPi64) { result = ModR64(result, TwoPi64); }
	if (result < 0) { result = TwoPi64 - ModR64(-result, TwoPi64); }
	return result;
}

r32 AngleDiffR32(r32 left, r32 right)
{
	r32 fixedLeft = AngleFixR32(left);
	r32 fixedRight = AngleFixR32(right);
	if (fixedLeft - fixedRight > Pi32) { fixedLeft -= TwoPi32; }
	if (fixedLeft - fixedRight < -Pi32) { fixedLeft += TwoPi32; }
	return left - right;
}
r64 AngleDiffR64(r64 left, r64 right)
{
	r64 fixedLeft = AngleFixR64(left);
	r64 fixedRight = AngleFixR64(right);
	if (fixedLeft - fixedRight > Pi32) { fixedLeft -= TwoPi32; }
	if (fixedLeft - fixedRight < -Pi32) { fixedLeft += TwoPi32; }
	return left - right;
}

r32 AngleOppositeR32(r32 angle)
{
	return AngleFixR32(angle + TwoPi32);
}
r64 AngleOppositeR64(r64 angle)
{
	return AngleFixR64(angle + TwoPi64);
}

r32 AngleFlipVerticalR32(r32 angle, bool normalize = true)
{
	r32 result = TwoPi32 - angle;
	if (normalize) { result = AngleFixR32(result); }
	return result;
}
r32 AngleFlipHorizontalR32(r32 angle, bool normalize = true)
{
	r32 result = Pi32 - angle;
	if (normalize) { result = AngleFixR32(result); }
	return result;
}
r64 AngleFlipVerticalR64(r64 angle, bool normalize = true)
{
	r64 result = TwoPi64 - angle;
	if (normalize) { result = AngleFixR64(result); }
	return result;
}
r64 AngleFlipHorizontalR64(r64 angle, bool normalize = true)
{
	r64 result = Pi64 - angle;
	if (normalize) { result = AngleFixR64(result); }
	return result;
}

r32 AngleLerpR32(r32 angleFrom, r32 angleTo, r32 amount)
{
	r32 from = AngleFixR32(angleFrom);
	r32 to = AngleFixR32(angleTo);
	if (to - from > Pi32) { to -= TwoPi32; }
	if (to - from < -Pi32) { to += TwoPi32; }
	return from + (to - from) * amount;
}
r64 AngleLerpR64(r64 angleFrom, r64 angleTo, r64 amount)
{
	r64 from = AngleFixR64(angleFrom);
	r64 to = AngleFixR64(angleTo);
	if (to - from > Pi32) { to -= TwoPi32; }
	if (to - from < -Pi32) { to += TwoPi32; }
	return from + (to - from) * amount;
}

// +--------------------------------------------------------------+
// |                  Triangle Related Functions                  |
// +--------------------------------------------------------------+
bool IsTriangleClockwise(v2 p0, v2 p1, v2 p2)
{
	return (Vec2Dot(p2 - p0, Vec2PerpRight(p1 - p0)) >= 0);
}
bool IsInsideTriangle(v2 p0, v2 p1, v2 p2, v2 test)
{
	v2 perp0 = Vec2PerpRight(p1 - p0);
	v2 perp1 = Vec2PerpRight(p2 - p1);
	v2 perp2 = Vec2PerpRight(p0 - p2);
	if (p0.x == p1.x && test.x == p0.x && test.y >= MinR32(p0.y, p1.y) && test.y <= MaxR32(p0.y, p1.y)) { return true; }
	if (p1.x == p2.x && test.x == p1.x && test.y >= MinR32(p1.y, p2.y) && test.y <= MaxR32(p1.y, p2.y)) { return true; }
	if (p2.x == p0.x && test.x == p2.x && test.y >= MinR32(p2.y, p0.y) && test.y <= MaxR32(p2.y, p0.y)) { return true; }
	if (p0.y == p1.y && test.y == p0.y && test.x >= MinR32(p0.x, p1.x) && test.x <= MaxR32(p0.x, p1.x)) { return true; }
	if (p1.y == p2.y && test.y == p1.y && test.x >= MinR32(p1.x, p2.x) && test.x <= MaxR32(p1.x, p2.x)) { return true; }
	if (p2.y == p0.y && test.y == p2.y && test.x >= MinR32(p2.x, p0.x) && test.x <= MaxR32(p2.x, p0.x)) { return true; }
	return (
		SignOfR32(Vec2Dot(p2 - p0, perp0)) == SignOfR32(Vec2Dot(test - p0, perp0)) &&
		SignOfR32(Vec2Dot(p0 - p1, perp1)) == SignOfR32(Vec2Dot(test - p1, perp1)) &&
		SignOfR32(Vec2Dot(p1 - p2, perp2)) == SignOfR32(Vec2Dot(test - p2, perp2))
	);
}

// +--------------------------------------------------------------+
// |                   Miscellaneous Functions                    |
// +--------------------------------------------------------------+
bool MinNoInfinitiesR32(r32 value1, r32 value2, r32 value3, r32* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR32(value1) && IsInfiniteR32(value2) && IsInfiniteR32(value3))
	{
		return false;
	}
	
	if (!IsInfiniteR32(value1) && (IsInfiniteR32(value2) || value1 <= value2) && (IsInfiniteR32(value3) || value1 <= value3))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR32(value2) && (IsInfiniteR32(value1) || value2 <= value1) && (IsInfiniteR32(value3) || value2 <= value3))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else if (!IsInfiniteR32(value3) && (IsInfiniteR32(value1) || value3 <= value1) && (IsInfiniteR32(value2) || value3 <= value2))
	{
		*outValue = value3;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 2;
		return true;
	}
	else
	{
		Assert(false);
		return false;
	}
}
bool MinNoInfinitiesR64(r64 value1, r64 value2, r64 value3, r64* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR64(value1) && IsInfiniteR64(value2) && IsInfiniteR64(value3))
	{
		return false;
	}
	
	if (!IsInfiniteR64(value1) && (IsInfiniteR64(value2) || value1 <= value2) && (IsInfiniteR64(value3) || value1 <= value3))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR64(value2) && (IsInfiniteR64(value1) || value2 <= value1) && (IsInfiniteR64(value3) || value2 <= value3))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else if (!IsInfiniteR64(value3) && (IsInfiniteR64(value1) || value3 <= value1) && (IsInfiniteR64(value2) || value3 <= value2))
	{
		*outValue = value3;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 2;
		return true;
	}
	else
	{
		Assert(false);
		return false;
	}
}

u8 NumDecimalDigitsU32(u32 number)
{
	//max is 4,294,967,295 or 4294967295
	if (number <         10UL) { return 1; }
	if (number <        100UL) { return 2; }
	if (number <       1000UL) { return 3; } //thousand
	if (number <      10000UL) { return 4; }
	if (number <     100000UL) { return 5; }
	if (number <    1000000UL) { return 6; } //million
	if (number <   10000000UL) { return 7; }
	if (number <  100000000UL) { return 8; }
	if (number < 1000000000UL) { return 9; } //billion
	return 10;
}
u8 NumDecimalDigitsU64(u64 number)
{
	//max is 18,446,744,073,709,551,616 or 18446744073709551616
	if (number <                   10ULL) { return  1; }
	if (number <                  100ULL) { return  2; }
	if (number <                 1000ULL) { return  3; } //thousand
	if (number <                10000ULL) { return  4; }
	if (number <               100000ULL) { return  5; }
	if (number <              1000000ULL) { return  6; } //million
	if (number <             10000000ULL) { return  7; }
	if (number <            100000000ULL) { return  8; }
	if (number <           1000000000ULL) { return  9; } //billion
	if (number <          10000000000ULL) { return 10; }
	if (number <         100000000000ULL) { return 11; }
	if (number <        1000000000000ULL) { return 12; } //trillion
	if (number <       10000000000000ULL) { return 13; }
	if (number <      100000000000000ULL) { return 14; }
	if (number <     1000000000000000ULL) { return 15; } //quadrillion
	if (number <    10000000000000000ULL) { return 16; }
	if (number <   100000000000000000ULL) { return 17; }
	if (number <  1000000000000000000ULL) { return 18; } //quintillion
	if (number < 10000000000000000000ULL) { return 19; }
	return 20;
}

void ReduceRatioU32(u32* num1, u32* num2)
{
	NotNull2(num1, num2);
	u32 result1 = *num1;
	u32 result2 = *num2;
	
	while (result1 > 1 && result2 > 1)
	{
		bool foundDivisor = false;
		#if defined(_GY_LOOKUP_H)
		u32 pIndex = 0;
		for (pIndex = 0; pIndex < PrimeNumbersU32_Count && PrimeNumbersU32[pIndex] < result1 && PrimeNumbersU32[pIndex] < result2; pIndex++)
		{
			u32 divisor = PrimeNumbersU32[pIndex];
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		if (!foundDivisor && pIndex >= PrimeNumbersU32_Count)
		{
			//fallback to regular testing
			u32 lastPrimeTested = PrimeNumbersU32[PrimeNumbersU32_Count-1];
			for (u32 divisor = lastPrimeTested+1; divisor < result1 && divisor < result2; divisor++)
			{
				if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
				{
					result1 /= divisor;
					result2 /= divisor;
					foundDivisor = true;
					break;
				}
			}
		}
		#else //!defined(_GY_LOOKUP_H)
		for (u32 divisor = 2; divisor < result1 && divisor < result2; divisor++)
		{
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		#endif
		
		if (!foundDivisor) { break; }
	}
	
	*num1 = result1;
	*num2 = result2;
}
void ReduceRatioU64(u64* num1, u64* num2)
{
	NotNull2(num1, num2);
	u64 result1 = *num1;
	u64 result2 = *num2;
	
	while (result1 > 1 && result2 > 1)
	{
		bool foundDivisor = false;
		#if defined(_GY_LOOKUP_H)
		u32 pIndex = 0;
		for (pIndex = 0; pIndex < PrimeNumbersU32_Count && PrimeNumbersU32[pIndex] < result1 && PrimeNumbersU32[pIndex] < result2; pIndex++)
		{
			u64 divisor = PrimeNumbersU32[pIndex];
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		if (!foundDivisor && pIndex >= PrimeNumbersU32_Count)
		{
			//fallback to regular testing
			u32 lastPrimeTested = PrimeNumbersU32[PrimeNumbersU32_Count-1];
			for (u64 divisor = lastPrimeTested+1; divisor < result1 && divisor < result2; divisor++)
			{
				if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
				{
					result1 /= divisor;
					result2 /= divisor;
					foundDivisor = true;
					break;
				}
			}
		}
		#else //!defined(_GY_LOOKUP_H)
		for (u64 divisor = 2; divisor < result1 && divisor < result2; divisor++)
		{
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		#endif
		
		if (!foundDivisor) { break; }
	}
	
	*num1 = result1;
	*num2 = result2;
}

//TODO: Make a 3D version of this function?
v2 ClosestPointOnLine(v2 lineStart, v2 lineEnd, v2 point)
{
	r32 lineLength = Vec2Length(lineEnd - lineStart);
	v2 lineNorm = (lineEnd - lineStart) / lineLength;
	r32 dot = Vec2Dot(lineNorm, point - lineStart);
	if (dot <= 0) { return lineStart; }
	if (dot >= lineLength) { return lineEnd; }
	return lineStart + (lineNorm * dot);
}

r32 AngleFromVec2(v2 vector)
{
	return AngleFixR32(AtanR32(vector.y, vector.x));
}
r32 AngleFromVec2(r32 x, r32 y)
{
	return AngleFixR32(AtanR32(y, x));
}

#endif //  _GY_MATH_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
r32 SubAnimAmountR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
r64 SubAnimAmountR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
r32 SubAnimAmountTwoWayR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
r64 SubAnimAmountTwoWayR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
r32 AngleFixR32(r32 angle)
r64 AngleFixR64(r64 angle)
r32 AngleDiffR32(r32 left, r32 right)
r64 AngleDiffR64(r64 left, r64 right)
r32 AngleOppositeR32(r32 angle)
r64 AngleOppositeR64(r64 angle)
r32 AngleFlipVerticalR32(r32 angle, bool normalize = true)
r32 AngleFlipHorizontalR32(r32 angle, bool normalize = true)
r64 AngleFlipVerticalR64(r64 angle, bool normalize = true)
r64 AngleFlipHorizontalR64(r64 angle, bool normalize = true)
r32 AngleLerpR32(r32 angleFrom, r32 angleTo, r32 amount)
r64 AngleLerpR64(r64 angleFrom, r64 angleTo, r64 amount)
bool IsTriangleClockwise(v2 p0, v2 p1, v2 p2)
bool IsInsideTriangle(v2 p0, v2 p1, v2 p2, v2 test)
bool MinNoInfinitiesR32(r32 value1, r32 value2, r32 value3, r32* outValue, u8* whichIsMaxOut = nullptr)
bool MinNoInfinitiesR64(r64 value1, r64 value2, r64 value3, r64* outValue, u8* whichIsMaxOut = nullptr)
u8 NumDecimalDigitsU32(u32 number)
u8 NumDecimalDigitsU64(u64 number)
void ReduceRatioU32(u32* num1, u32* num2)
void ReduceRatioU64(u64* num1, u64* num2)
v2 ClosestPointOnLine(v2 lineStart, v2 lineEnd, v2 point)
r32 AngleFromVec2(v2 vector)
r32 AngleFromVec2(r32 x, r32 y)
*/
