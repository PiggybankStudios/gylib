/*
File:   gy_vectors.h
Author: Taylor Robbins
Date:   09\14\2021
*/

#ifndef _GY_VECTORS_H
#define _GY_VECTORS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
union Vector2_t
{
	r32 values[2];
	struct { r32 x, y; };
	struct { r32 width, height; };
	struct { r32 horizontal, vertical; };
};
union Vector3_t
{
	r32 values[3];
	struct { r32 x, y, z; };
	struct { r32 width, height, depth; };
	struct { r32 r, g, b; };
	struct { r32 red, green, blue; };
};
union Vector4_t
{
	r32 values[4];
	struct { r32 x, y, z, w; };
	struct { r32 width, height, depth, time; };
	struct { r32 r, g, b, a; };
	struct { r32 red, green, blue, alpha; };
};

union Vector2i_t
{
	i32 values[2];
	struct { i32 x, y; };
	struct { i32 width, height; };
	struct { i32 horizontal, vertical; };
	struct { i32 columns, rows; };
};
union Vector3i_t
{
	i32 values[3];
	struct { i32 x, y, z; };
	struct { i32 width, height, depth; };
};
union Vector4i_t
{
	i32 values[4];
	struct { i32 x, y, z, w; };
	struct { i32 width, height, depth, time; };
};

typedef Vector2_t v2;
typedef Vector3_t v3;
typedef Vector4_t v4;

typedef Vector2i_t v2i;
typedef Vector3i_t v3i;
typedef Vector4i_t v4i;

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Vector2_t NewVec2(r32 x, r32 y)
{
	Vector2_t result;
	result.x = x;
	result.y = y;
	return result;
}
Vector3_t NewVec3(r32 x, r32 y, r32 z)
{
	Vector3_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}
Vector4_t NewVec4(r32 x, r32 y, r32 z, r32 w)
{
	Vector4_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

Vector2i_t NewVec2i(i32 x, i32 y)
{
	Vector2i_t result;
	result.x = x;
	result.y = y;
	return result;
}
Vector3i_t NewVec3i(i32 x, i32 y, i32 z)
{
	Vector3i_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}
Vector4i_t NewVec4i(i32 x, i32 y, i32 z, i32 w)
{
	Vector4i_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

// +--------------------------------------------------------------+
// |                   Simple Value Definitions                   |
// +--------------------------------------------------------------+
#define Vec2_Zero      NewVec2( 0.0f,  0.0f)
#define Vec2_One       NewVec2( 1.0f,  1.0f)
#define Vec2_Half      NewVec2( 0.5f,  0.5f)
#define Vec2_Left      NewVec2(-1.0f,  0.0f)
#define Vec2_Right     NewVec2( 1.0f,  0.0f)
#define Vec2_Up        NewVec2( 0.0f, -1.0f)
#define Vec2_Down      NewVec2( 0.0f,  1.0f)

#define Vec2i_Zero     NewVec2i( 0,  0)
#define Vec2i_One      NewVec2i( 1,  1)
#define Vec2i_Left     NewVec2i(-1,  0)
#define Vec2i_Right    NewVec2i( 1,  0)
#define Vec2i_Up       NewVec2i( 0, -1)
#define Vec2i_Down     NewVec2i( 0,  1)

#define Vec3_Zero      NewVec3( 0.0f,  0.0f,  0.0f)
#define Vec3_One       NewVec3( 1.0f,  1.0f,  1.0f)
#define Vec3_Half      NewVec3( 0.5f,  0.5f,  0.5f)
#define Vec3_Left      NewVec3(-1.0f,  0.0f,  0.0f)
#define Vec3_Right     NewVec3( 1.0f,  0.0f,  0.0f)
#define Vec3_Bottom    NewVec3( 0.0f, -1.0f,  0.0f)
#define Vec3_Top       NewVec3( 0.0f,  1.0f,  0.0f)
#define Vec3_Back      NewVec3( 0.0f,  0.0f, -1.0f)
#define Vec3_Front     NewVec3( 0.0f,  0.0f,  1.0f)
#define Vec3_Down      NewVec3( 0.0f, -1.0f,  0.0f)
#define Vec3_Up        NewVec3( 0.0f,  1.0f,  0.0f)
#define Vec3_Backward  NewVec3( 0.0f,  0.0f, -1.0f)
#define Vec3_Forward   NewVec3( 0.0f,  0.0f,  1.0f)

#define Vec3i_Zero     NewVec3i( 0,  0,  0)
#define Vec3i_One      NewVec3i( 1,  1,  1)
#define Vec3i_Left     NewVec3i(-1,  0,  0)
#define Vec3i_Right    NewVec3i( 1,  0,  0)
#define Vec3i_Bottom   NewVec3i( 0, -1,  0)
#define Vec3i_Top      NewVec3i( 0,  1,  0)
#define Vec3i_Back     NewVec3i( 0,  0, -1)
#define Vec3i_Front    NewVec3i( 0,  0,  1)
#define Vec3i_Down     NewVec3i( 0, -1,  0)
#define Vec3i_Up       NewVec3i( 0,  1,  0)
#define Vec3i_Backward NewVec3i( 0,  0, -1)
#define Vec3i_Forward  NewVec3i( 0,  0,  1)

#define Vec4_Zero      NewVec4( 0.0f,  0.0f,  0.0f, 0.0f)
#define Vec4_One       NewVec4( 1.0f,  1.0f,  1.0f, 1.0f)
#define Vec4_Half      NewVec4( 0.5f,  0.5f,  0.5f, 0.5f)
#define Vec4_Left      NewVec4(-1.0f,  0.0f,  0.0f, 0.0f)
#define Vec4_Right     NewVec4( 1.0f,  0.0f,  0.0f, 0.0f)
#define Vec4_Bottom    NewVec4( 0.0f, -1.0f,  0.0f, 0.0f)
#define Vec4_Top       NewVec4( 0.0f,  1.0f,  0.0f, 0.0f)
#define Vec4_Back      NewVec4( 0.0f,  0.0f, -1.0f, 0.0f)
#define Vec4_Front     NewVec4( 0.0f,  0.0f,  1.0f, 0.0f)
#define Vec4_Down      NewVec4( 0.0f, -1.0f,  0.0f, 0.0f)
#define Vec4_Up        NewVec4( 0.0f,  1.0f,  0.0f, 0.0f)
#define Vec4_Backward  NewVec4( 0.0f,  0.0f, -1.0f, 0.0f)
#define Vec4_Forward   NewVec4( 0.0f,  0.0f,  1.0f, 0.0f)

#define Vec4i_Zero     NewVec4( 0,  0,  0, 0)
#define Vec4i_One      NewVec4( 1,  1,  1, 0)
#define Vec4i_Left     NewVec4(-1,  0,  0, 0)
#define Vec4i_Right    NewVec4( 1,  0,  0, 0)
#define Vec4i_Bottom   NewVec4( 0, -1,  0, 0)
#define Vec4i_Top      NewVec4( 0,  1,  0, 0)
#define Vec4i_Back     NewVec4( 0,  0, -1, 0)
#define Vec4i_Front    NewVec4( 0,  0,  1, 0)
#define Vec4i_Down     NewVec4( 0, -1,  0, 0)
#define Vec4i_Up       NewVec4( 0,  1,  0, 0)
#define Vec4i_Backward NewVec4( 0,  0, -1, 0)
#define Vec4i_Forward  NewVec4( 0,  0,  1, 0)

#endif //  _GY_VECTORS_H
