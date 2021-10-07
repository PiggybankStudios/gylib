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
Color_t NewColor(Colorf_t colorf)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(colorf.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(colorf.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(colorf.b * 255.0f));
	result.a = ClampI32toU8(RoundR32i(colorf.a * 255.0f));
	return result;
}
Color_t NewColor(v3 vector3)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(vector3.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(vector3.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(vector3.b * 255.0f));
	result.a = 255;
	return result;
}
Color_t NewColor(v4 vector4)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(vector4.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(vector4.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(vector4.b * 255.0f));
	result.a = ClampI32toU8(RoundR32i(vector4.a * 255.0f));
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
inline Colorf_t NewColorf(Color_t color)
{
	Colorf_t result = {};
	result.r = ((r32)color.r / 255.0f);
	result.g = ((r32)color.g / 255.0f);
	result.b = ((r32)color.b / 255.0f);
	result.a = ((r32)color.a / 255.0f);
	return result;
}
inline Colorf_t NewColorf(v3 vector3)
{
	Colorf_t result = {};
	result.r = vector3.r;
	result.g = vector3.g;
	result.b = vector3.b;
	result.a = 1.0f;
	return result;
}
inline Colorf_t NewColorf(v4 vector4)
{
	Colorf_t result = {};
	result.r = vector4.r;
	result.g = vector4.g;
	result.b = vector4.b;
	result.a = vector4.a;
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
inline ColorHSV_t NewColorHsv(v3 vector3)
{
	ColorHSV_t result = {};
	result.hue = vector3.x;
	result.saturation = vector3.y;
	result.value = vector3.z;
	result.alpha = 1.0f;
	return result;
}
inline ColorHSV_t NewColorHsv(v4 vector4)
{
	ColorHSV_t result = {};
	result.hue = vector4.x;
	result.saturation = vector4.y;
	result.value = vector4.z;
	result.alpha = vector4.w;
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

// +--------------------------------------------------------------+
// |                       Other Functions                        |
// +--------------------------------------------------------------+
v4 ToVec4(Color_t color)
{
	v4 result;
	result.r = (r32)color.r / 255.0f;
	result.g = (r32)color.g / 255.0f;
	result.b = (r32)color.b / 255.0f;
	result.a = (r32)color.a / 255.0f;
	return result;
}

#endif //  _GY_COLORS_H
