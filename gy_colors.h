/*
File:   gy_colors.h
Author: Taylor Robbins
Date:   09\26\2021
Description:
	** Holds the Color_t structure and other similar structures and all the functions to manipulate them
*/

#ifndef _GY_COLORS_H
#define _GY_COLORS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

union Color_t
{
	u32 value;
	struct { u8 b, g, r, a; };
	struct { u8 blue, green, red, alpha; };
	struct { u8 b, g, r, a; } argb; //TODO: The "ARGB" ordering used by windows? Or PNGs? or BMPs? or GIFs?
	struct { u8 r, g, b, a; } rgba; //TODO: The RGBA ordering is used by a few other things
};
union Colorf_t
{
	v4 values;
	struct { r32 b, g, r, a; };
	struct { r32 blue, green, red, alpha; };
};
union ColorHSV_t
{
	v4 values;
	struct { r32 h, s, v, a; };
	struct { r32 hue, saturation, value, alpha; };
};
union ColorXYZ_t
{
	v4 values;
	struct { r32 x, y, z, a; };
};
union ColorLCH_t
{
	v4 values;
	struct { r32 l, c, h, a; };
	struct { r32 luminence, chroma, hue, alpha; };
};
union ColorLAB_t
{
	v4 values;
	struct { r32 l, a, b, alpha; };
};

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Color_t NewColor(u32 value, bool rgbaOrder = false) //default is argb order
{
	Color_t result;
	result.value = rgbaOrder ? (((value&0x00FF0000) >> 16) | ((value&0x0000FF00) >> 0) | ((value&0x000000FF) << 16) | ((value&0xFF000000) >> 0)) : value;
	return result;
}
Color_t NewColor(u8 r, u8 g, u8 b, u8 a, bool rgbaOrder = false) //default is argb order
{
	Color_t result;
	if (rgbaOrder) { result.rgba.r = r; result.rgba.g = g; result.rgba.b = b; result.rgba.a = a; }
	else           { result.argb.r = r; result.argb.g = g; result.argb.b = b; result.argb.a = a; }
	return result;
}
Color_t NewColor(u8 r, u8 g, u8 b)
{
	Color_t result;
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = 255;
	return result;
}

inline Colorf_t NewColorf(r32 r, r32 g, r32 b)
{
	Colorf_t result = {};
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = 1.0f;
	return result;
}
inline Colorf_t NewColorf(r32 r, r32 g, r32 b, r32 a)
{
	Colorf_t result = {};
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = a;
	return result;
}

inline ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value)
{
	ColorHSV_t result = {};
	result.hue = hue;
	result.saturation = saturation;
	result.value = value;
	result.alpha = 1.0f;
	return result;
}
inline ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value, r32 alpha)
{
	ColorHSV_t result = {};
	result.hue = hue;
	result.saturation = saturation;
	result.value = value;
	result.alpha = alpha;
	return result;
}

// +--------------------------------------------------------------+
// |               Casting and Conversion Functions               |
// +--------------------------------------------------------------+
Color_t ToColor(Colorf_t colorf)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(colorf.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(colorf.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(colorf.b * 255.0f));
	result.a = ClampI32toU8(RoundR32i(colorf.a * 255.0f));
	return result;
}
Color_t ToColor(v3 vector3)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(vector3.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(vector3.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(vector3.b * 255.0f));
	result.a = 255;
	return result;
}
Color_t ToColor(v4 vector4)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(vector4.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(vector4.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(vector4.b * 255.0f));
	result.a = ClampI32toU8(RoundR32i(vector4.a * 255.0f));
	return result;
}
inline Color_t ColorTransparent(r32 alpha)
{
	return NewColor(255, 255, 255, ClampI32toU8(RoundR32i(255 * alpha)));
}
inline Color_t ColorTransparent(Color_t color, r32 alpha)
{
	return NewColor(color.r, color.g, color.b, ClampI32toU8(RoundR32i(255 * alpha)));
}

inline Colorf_t ToColorf(Color_t color)
{
	Colorf_t result = {};
	result.r = ((r32)color.r / 255.0f);
	result.g = ((r32)color.g / 255.0f);
	result.b = ((r32)color.b / 255.0f);
	result.a = ((r32)color.a / 255.0f);
	return result;
}
inline Colorf_t ToColorf(v3 vector3)
{
	Colorf_t result = {};
	result.r = vector3.r;
	result.g = vector3.g;
	result.b = vector3.b;
	result.a = 1.0f;
	return result;
}
inline Colorf_t ToColorf(v4 vector4)
{
	Colorf_t result = {};
	result.r = vector4.r;
	result.g = vector4.g;
	result.b = vector4.b;
	result.a = vector4.a;
	return result;
}

inline ColorHSV_t ToColorHsv(v3 vector3)
{
	ColorHSV_t result = {};
	result.hue = vector3.x;
	result.saturation = vector3.y;
	result.value = vector3.z;
	result.alpha = 1.0f;
	return result;
}
inline ColorHSV_t ToColorHsv(v4 vector4)
{
	ColorHSV_t result = {};
	result.hue = vector4.x;
	result.saturation = vector4.y;
	result.value = vector4.z;
	result.alpha = vector4.w;
	return result;
}

v3 ToVec3(Color_t color)
{
	v3 result;
	result.r = (r32)color.r / 255.0f;
	result.g = (r32)color.g / 255.0f;
	result.b = (r32)color.b / 255.0f;
	return result;
}
v3 ToVec3(Colorf_t color)
{
	v3 result;
	result.r = color.r;
	result.g = color.g;
	result.b = color.b;
	return result;
}

v4 ToVec4(Color_t color)
{
	v4 result;
	result.r = (r32)color.r / 255.0f;
	result.g = (r32)color.g / 255.0f;
	result.b = (r32)color.b / 255.0f;
	result.a = (r32)color.a / 255.0f;
	return result;
}
v4 ToVec4(Colorf_t color)
{
	v4 result;
	result.r = color.r;
	result.g = color.g;
	result.b = color.b;
	result.a = color.a;
	return result;
}

// +--------------------------------------------------------------+
// |                     Basic Manipulations                      |
// +--------------------------------------------------------------+
// +==============================+
// |            Color             |
// +==============================+
Color_t ColorLerp(Color_t start, Color_t end, r32 amount)
{
	Color_t result = {};
	result.r = ClampI32toU8(RoundR32i(LerpR32((r32)start.r, (r32)end.r, amount)));
	result.g = ClampI32toU8(RoundR32i(LerpR32((r32)start.g, (r32)end.g, amount)));
	result.b = ClampI32toU8(RoundR32i(LerpR32((r32)start.b, (r32)end.b, amount)));
	result.a = ClampI32toU8(RoundR32i(LerpR32((r32)start.a, (r32)end.a, amount)));
	return result;
}
//TODO: ColorMultiply
//TODO: ColorMultiplyAlpha
//TODO: ColorDarken
//TODO: ColorLighten
//TODO: ColorOpposite

// +==============================+
// |            Colorf            |
// +==============================+
Colorf_t ColorfLerp(Colorf_t start, Colorf_t end, r32 amount)
{
	Colorf_t result = {};
	result.r = LerpR32(start.r, end.r, amount);
	result.g = LerpR32(start.g, end.g, amount);
	result.b = LerpR32(start.b, end.b, amount);
	result.a = LerpR32(start.a, end.a, amount);
	return result;
}

// +--------------------------------------------------------------+
// |                   Color Space Conversions                    |
// +--------------------------------------------------------------+
//TODO: What does this do??
#if 0
r32 HueToRGB(r32 value1, r32 value2, r32 vH)
{
	if (vH < 0) { vH += 1; }
	if (vH > 1) { vH -= 1; }
	
	if (6*vH < 1)
	{
		return (value1 + (value2-value1) * 6 * vH);
	}
	else if (2*vH < 1)
	{
		return value2;
	}
	else if (3*vH < 2)
	{
		return (value1 + (value2-value1) * ((2.0f/3) - vH) * 6);
	}
	else
	{
		return value1;
	}
}
#endif

//TODO: These HSV <-> RGB functions seem to have problems!
Color_t ColorRGBFromHSV(ColorHSV_t colorHsv)
{
	r32 redValue = 0;
	r32 greenValue = 0;
	r32 blueValue = 0;
	
	r32 cValue = colorHsv.v * colorHsv.s;
	r32 xValue = cValue * (1 - AbsR32((r32)((RoundR32i(colorHsv.h) / 60)%2) - 1));
	r32 nextX = cValue * (1 - AbsR32((r32)((RoundR32i(colorHsv.h) / 60 + 1)%2) - 1));
	r32 mValue = colorHsv.v - cValue;
	
	u8 hueSwitch = ClampI32toU8(RoundR32i(colorHsv.h) / 60);
	v3 color1 = {};
	v3 color2 = {};
	r32 lerpAmount = (RoundR32i(colorHsv.h)%60) / 60.0f;
	
	switch (hueSwitch)
	{
		case 0: //0-60 degrees
		{
			color1 = NewVec3(cValue, xValue,      0);
			color2 = NewVec3( nextX, cValue,      0);
		} break;
		case 1: //60-120 degrees
		{
			color1 = NewVec3(xValue, cValue,      0);
			color2 = NewVec3(     0, cValue,  nextX);
		} break;
		case 2: //120-180 degrees
		{
			color1 = NewVec3(     0, cValue, xValue);
			color2 = NewVec3(     0,  nextX, cValue);
		} break;
		case 3: //180-240 degrees
		{
			color1 = NewVec3(     0, xValue, cValue);
			color2 = NewVec3( nextX,      0, cValue);
		} break;
		case 4: //240-300 degrees
		{
			color1 = NewVec3(xValue,      0, cValue);
			color2 = NewVec3(cValue,      0,  nextX);
		} break;
		case 5: //300-360 degrees
		case 6:
		{
			color1 = NewVec3(cValue,      0, xValue);
			color2 = NewVec3(cValue,  nextX,      0);
		} break;
		
		default: return NewColor(0, 0, 0, 0);
	};
	
	Color_t result = NewColor(
		(u8)((LerpR32(color1.x, color2.x, lerpAmount) + mValue)*255),
		(u8)((LerpR32(color1.y, color2.y, lerpAmount) + mValue)*255),
		(u8)((LerpR32(color1.z, color2.z, lerpAmount) + mValue)*255), 255);
	
	return result;
}
ColorHSV_t ColorHSVFromRGB(Color_t color)
{
	ColorHSV_t result = {};
	
	r32 red   = ((r32)color.r / 255.0f);
	r32 green = ((r32)color.g / 255.0f);
	r32 blue  = ((r32)color.b / 255.0f);
	r32 minRgb = MinR32(red, green, blue);
	r32 maxRgb = MaxR32(red, green, blue);
	r32 minMaxDelta = maxRgb - minRgb;
	
	result.a = ((r32)color.a / 255.0f);
	result.v = maxRgb;
	if (minMaxDelta == 0)
	{
		result.h = 0;
		result.s = 0.0f;
	}
	else
	{
		result.s = minMaxDelta / maxRgb;
		
		r32 rDelta = (((maxRgb - red)   / 6) + (minMaxDelta / 2)) / minMaxDelta;
		r32 gDelta = (((maxRgb - green) / 6) + (minMaxDelta / 2)) / minMaxDelta;
		r32 bDelta = (((maxRgb - blue)  / 6) + (minMaxDelta / 2)) / minMaxDelta;
		
		r32 hueR32 = 0.0f;
		if (red == maxRgb) { hueR32 = bDelta - gDelta; }
		else if (green == maxRgb) { hueR32 = (1.0f/3.0f) + rDelta - bDelta; }
		else if (blue == maxRgb) { hueR32 = (2.0f/3.0f) + gDelta - rDelta; }
		
		if (hueR32 < 0) { hueR32 += 1; }
		if (hueR32 > 1) { hueR32 -= 1; }
		
		result.hue = (r32)(RoundR32i(hueR32*360) % 360);
	}
	
	return result;
}

// +--------------------------------------------------------------+
// |                       Other Functions                        |
// +--------------------------------------------------------------+
//TODO: ColorComplimentary
//TODO: ColorDesaturate

#endif //  _GY_COLORS_H
