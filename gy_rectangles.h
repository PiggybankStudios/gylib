/*
File:   gy_rectangles.h
Author: Taylor Robbins
Date:   10\06\2021
Description:
	** Contains the definitions and related functions for rec, reci, aabb, obb2, and obb3
*/

#ifndef _GY_RECTANGLES_H
#define _GY_RECTANGLES_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
//TODO: Can we move the += and -= operator overloads to the bottom of the file??
//These are technically aabb's in 2D but we just call them "rectangles" for simplicity
struct Rectangle_t
{
	union
	{
		v2 topLeft;
		struct { r32 x, y; };
		struct { r32 left, top; };
	};
	union
	{
		v2 size;
		struct { r32 width, height; };
	};
	
	inline Rectangle_t& operator += (v2 vector)
	{
		this->topLeft += vector;
		return *this;
	}
	inline Rectangle_t& operator -= (v2 vector)
	{
		this->topLeft -= vector;
		return *this;
	}
};

struct Rectanglei_t
{
	union
	{
		v2i topLeft;
		struct { i32 x, y; };
		struct { i32 left, top; };
	};
	union
	{
		v2i size;
		struct { i32 width, height; };
	};
	
	inline Rectanglei_t& operator += (v2i vector)
	{
		this->topLeft += vector;
		return *this;
	}
	inline Rectanglei_t& operator -= (v2i vector)
	{
		this->topLeft -= vector;
		return *this;
	}
};

//Axis-Aligned Bounding Box
struct Box_t
{
	union
	{
		v3 topLeft;
		struct { r32 x, y, z; };
		struct { r32 left, top, front; };
	};
	union
	{
		v3 size;
		struct { r32 width, height, depth; };
	};
	
	inline Box_t& operator += (v3 vector)
	{
		this->topLeft += vector;
		return *this;
	}
	inline Box_t& operator -= (v3 vector)
	{
		this->topLeft -= vector;
		return *this;
	}
};

struct Boxi_t
{
	union
	{
		v3i topLeft;
		struct { i32 x, y, z; };
		struct { i32 left, top, front; };
	};
	union
	{
		v3i size;
		struct { i32 width, height, depth; };
	};
	
	inline Boxi_t& operator += (v3i vector)
	{
		this->topLeft += vector;
		return *this;
	}
	inline Boxi_t& operator -= (v3i vector)
	{
		this->topLeft -= vector;
		return *this;
	}
};

//Oriented Bounding Box
struct Obb2D_t
{
	union
	{
		v2 center;
		struct { r32 x, y; };
	};
	union
	{
		v2 size;
		struct { r32 width, height; };
	};
	r32 rotation;
	
	inline Obb2D_t& operator += (v2 vector)
	{
		this->center += vector;
		return *this;
	}
	inline Obb2D_t& operator -= (v2 vector)
	{
		this->center -= vector;
		return *this;
	}
};

#if 0
//TODO: Implement me once we have quaternions implemented
struct Obb3D_t
{
	union
	{
		v3 center;
		struct { r32 x, y, z; };
	};
	union
	{
		v3 size;
		struct { r32 width, height, depth; };
	};
	quat rotation;
	
	inline Obb3D_t& operator += (v3 vector)
	{
		this->center += vector;
		return *this;
	}
	inline Obb3D_t& operator -= (v3 vector)
	{
		this->center -= vector;
		return *this;
	}
};
#endif

typedef Rectangle_t   rec;
typedef Rectanglei_t  reci;
typedef Obb2D_t       obb2;
typedef Box_t        box;
typedef Boxi_t       boxi;
// typedef OBB3D_t       obb3; //TODO: Uncomment me!

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
rec NewRec(r32 left, r32 top, r32 width, r32 height)
{
	rec result;
	result.left = left;
	result.top = top;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRec(v2 topLeft, r32 width, r32 height)
{
	rec result;
	result.topLeft = topLeft;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRec(r32 left, r32 top, v2 size)
{
	rec result;
	result.left = left;
	result.top = top;
	result.size = size;
	return result;
}
rec NewRec(v2 topLeft, v2 size)
{
	rec result;
	result.topLeft = topLeft;
	result.size = size;
	return result;
}
rec NewRecCentered(r32 centerX, r32 centerY, r32 width, r32 height)
{
	rec result;
	result.x = centerX - width/2;
	result.y = centerY - height/2;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRecCentered(v2 center, r32 width, r32 height)
{
	rec result;
	result.x = center.x - width/2;
	result.y = center.y - height/2;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRecCentered(r32 centerX, r32 centerY, v2 size)
{
	rec result;
	result.x = centerX - size.width/2;
	result.y = centerY - size.height/2;
	result.size = size;
	return result;
}
rec NewRecCentered(v2 center, v2 size)
{
	rec result;
	result.x = center.x - size.width/2;
	result.y = center.y - size.height/2;
	result.size = size;
	return result;
}

reci NewReci(i32 left, i32 top, i32 width, i32 height)
{
	reci result;
	result.left = left;
	result.top = top;
	result.width = width;
	result.height = height;
	return result;
}
reci NewReci(v2i topLeft, i32 width, i32 height)
{
	reci result;
	result.topLeft = topLeft;
	result.width = width;
	result.height = height;
	return result;
}
reci NewReci(i32 left, i32 top, v2i size)
{
	reci result;
	result.left = left;
	result.top = top;
	result.size = size;
	return result;
}
reci NewReci(v2i topLeft, v2i size)
{
	reci result;
	result.topLeft = topLeft;
	result.size = size;
	return result;
}

box NewBox(r32 x, r32 y, r32 z, r32 width, r32 height, r32 depth)
{
	box result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBox(v3 topLeft, r32 width, r32 height, r32 depth)
{
	box result;
	result.topLeft = topLeft;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBox(r32 x, r32 y, r32 z, v3 size)
{
	box result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.size = size;
	return result;
}
box NewBox(v3 topLeft, v3 size)
{
	box result;
	result.topLeft = topLeft;
	result.size = size;
	return result;
}
box NewBoxCentered(r32 centerX, r32 centerY, r32 centerZ, r32 width, r32 height, r32 depth)
{
	box result;
	result.x = centerX - width/2;
	result.y = centerY - height/2;
	result.z = centerZ - depth/2;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBoxCentered(v3 center, r32 width, r32 height, r32 depth)
{
	box result;
	result.x = center.x - width/2;
	result.y = center.y - height/2;
	result.z = center.z - depth/2;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBoxCentered(r32 centerX, r32 centerY, r32 centerZ, v3 size)
{
	box result;
	result.x = centerX - size.width/2;
	result.y = centerY - size.height/2;
	result.z = centerZ - size.depth/2;
	result.size = size;
	return result;
}
box NewBoxCentered(v3 center, v3 size)
{
	box result;
	result.x = center.x - size.width/2;
	result.y = center.y - size.height/2;
	result.z = center.z - size.depth/2;
	result.size = size;
	return result;
}

boxi NewBoxi(i32 x, i32 y, i32 z, i32 width, i32 height, i32 depth)
{
	boxi result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
boxi NewBoxi(v3i topLeft, i32 width, i32 height, i32 depth)
{
	boxi result;
	result.topLeft = topLeft;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
boxi NewBoxi(i32 x, i32 y, i32 z, v3i size)
{
	boxi result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.size = size;
	return result;
}
boxi NewBoxi(v3i topLeft, v3i size)
{
	boxi result;
	result.topLeft = topLeft;
	result.size = size;
	return result;
}

obb2 NewObb2D(r32 centerX, r32 centerY, r32 width, r32 height, r32 rotation)
{
	obb2 result;
	result.x = centerX;
	result.y = centerY;
	result.width = width;
	result.height = height;
	result.rotation = rotation;
	return result;
}
obb2 NewObb2D(v2 center, r32 width, r32 height, r32 rotation)
{
	obb2 result;
	result.center = center;
	result.width = width;
	result.height = height;
	result.rotation = rotation;
	return result;
}
obb2 NewObb2D(r32 centerX, r32 centerY, v2 size, r32 rotation)
{
	obb2 result;
	result.x = centerX;
	result.y = centerY;
	result.size = size;
	result.rotation = rotation;
	return result;
}
obb2 NewObb2D(v2 center, v2 size, r32 rotation)
{
	obb2 result;
	result.center = center;
	result.size = size;
	result.rotation = rotation;
	return result;
}

//TODO: Add New functions for OBB3D_t

// +--------------------------------------------------------------+
// |                   Simple Value Definitions                   |
// +--------------------------------------------------------------+
#define Rec_Zero    NewRec(0, 0, 0, 0)
#define Rec_Default NewRec(0, 0, 1, 1)
#define Reci_Zero   NewReci(0, 0, 0, 0)
#define Box_Zero    NewBox(0, 0, 0, 0, 0, 0)
#define Boxi_Zero   NewBoxi(0, 0, 0, 0, 0, 0)
#define Obb2_Zero   NewObb2D(0, 0, 0, 0, 0)
// #define Obb3_Zero NewObb2D(0, 0, 0, 0, 0, 0, Quat_Identity) //TODO: Uncomment me!

// +--------------------------------------------------------------+
// |                 Simple Conversions and Casts                 |
// +--------------------------------------------------------------+
obb2 ToObb2D(rec rectangle)
{
	obb2 result;
	result.x = rectangle.x + rectangle.width/2;
	result.y = rectangle.y + rectangle.height/2;
	result.size = rectangle.size;
	return result;
}
box ToBox(boxi boundingBox)
{
	box result;
	result.x = (r32)boundingBox.x;
	result.y = (r32)boundingBox.y;
	result.z = (r32)boundingBox.z;
	result.width  = (r32)boundingBox.width;
	result.height = (r32)boundingBox.height;
	result.depth  = (r32)boundingBox.depth;
	return result;
}

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
// +==============================+
// |             Rec              |
// +==============================+
rec RecShift(rec rectangle, r32 amountX, r32 amountY)
{
	rec result;
	result.x = rectangle.x + amountX;
	result.y = rectangle.y + amountY;
	result.size = rectangle.size;
	return result;
}
rec RecShift(rec rectangle, v2 amount)
{
	rec result;
	result.x = rectangle.x + amount.x;
	result.y = rectangle.y + amount.y;
	result.size = rectangle.size;
	return result;
}
rec RecScale(rec rectangle, r32 scalar)
{
	rec result;
	result.x = rectangle.x * scalar;
	result.y = rectangle.y * scalar;
	result.width = rectangle.width * scalar;
	result.height = rectangle.height * scalar;
	return result;
}
rec RecScale2(rec rectangle, r32 scaleX, r32 scaleY)
{
	rec result;
	result.x = rectangle.x * scaleX;
	result.y = rectangle.y * scaleY;
	result.width = rectangle.width * scaleX;
	result.height = rectangle.height * scaleY;
	return result;
}
rec RecScale2(rec rectangle, v2 scale)
{
	rec result;
	result.x = rectangle.x * scale.x;
	result.y = rectangle.y * scale.y;
	result.width = rectangle.width * scale.x;
	result.height = rectangle.height * scale.y;
	return result;
}
bool RecBasicallyEqual(rec left, rec right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x,      right.x))      { return false; }
	if (!BasicallyEqualR32(left.y,      right.y))      { return false; }
	if (!BasicallyEqualR32(left.width,  right.width))  { return false; }
	if (!BasicallyEqualR32(left.height, right.height)) { return false; }
	return true;
}

// +==============================+
// |             Reci             |
// +==============================+
reci ReciShift(reci rectangle, i32 amountX, i32 amountY)
{
	reci result;
	result.x = rectangle.x + amountX;
	result.y = rectangle.y + amountY;
	result.size = rectangle.size;
	return result;
}
reci ReciShift(reci rectangle, v2i amount)
{
	reci result;
	result.x = rectangle.x + amount.x;
	result.y = rectangle.y + amount.y;
	result.size = rectangle.size;
	return result;
}
reci ReciScale(reci rectangle, i32 scalar)
{
	reci result;
	result.x = rectangle.x * scalar;
	result.y = rectangle.y * scalar;
	result.width = rectangle.width * scalar;
	result.height = rectangle.height * scalar;
	return result;
}
reci ReciScale2(reci rectangle, i32 scaleX, i32 scaleY)
{
	reci result;
	result.x = rectangle.x * scaleX;
	result.y = rectangle.y * scaleY;
	result.width = rectangle.width * scaleX;
	result.height = rectangle.height * scaleY;
	return result;
}
reci ReciScale2(reci rectangle, v2i scale)
{
	reci result;
	result.x = rectangle.x * scale.x;
	result.y = rectangle.y * scale.y;
	result.width = rectangle.width * scale.x;
	result.height = rectangle.height * scale.y;
	return result;
}
bool ReciEqual(reci left, reci right)
{
	if (left.x      != right.x)      { return false; }
	if (left.y      != right.y)      { return false; }
	if (left.width  != right.width)  { return false; }
	if (left.height != right.height) { return false; }
	return true;
}

// +==============================+
// |             Box              |
// +==============================+
box BoxShift(box boundingBox, r32 amountX, r32 amountY, r32 amountZ)
{
	box result;
	result.x = boundingBox.x + amountX;
	result.y = boundingBox.y + amountY;
	result.z = boundingBox.z + amountZ;
	result.size = boundingBox.size;
	return result;
}
box BoxShift(box boundingBox, v3 amount)
{
	box result;
	result.x = boundingBox.x + amount.x;
	result.y = boundingBox.y + amount.y;
	result.z = boundingBox.z + amount.z;
	result.size = boundingBox.size;
	return result;
}
box BoxScale(box boundingBox, r32 scalar)
{
	box result;
	result.x = boundingBox.x * scalar;
	result.y = boundingBox.y * scalar;
	result.z = boundingBox.z * scalar;
	result.width = boundingBox.width * scalar;
	result.height = boundingBox.height * scalar;
	result.depth = boundingBox.depth * scalar;
	return result;
}
box BoxScale3(box boundingBox, r32 scaleX, r32 scaleY, r32 scaleZ)
{
	box result;
	result.x = boundingBox.x * scaleX;
	result.y = boundingBox.y * scaleY;
	result.z = boundingBox.z * scaleZ;
	result.width = boundingBox.width * scaleX;
	result.height = boundingBox.height * scaleY;
	result.depth = boundingBox.depth * scaleZ;
	return result;
}
box BoxScale2(box boundingBox, v3 scale)
{
	box result;
	result.x = boundingBox.x * scale.x;
	result.y = boundingBox.y * scale.y;
	result.z = boundingBox.z * scale.z;
	result.width = boundingBox.width * scale.x;
	result.height = boundingBox.height * scale.y;
	result.depth = boundingBox.depth * scale.z;
	return result;
}
bool BoxBasicallyEqual(box left, box right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x,      right.x))      { return false; }
	if (!BasicallyEqualR32(left.y,      right.y))      { return false; }
	if (!BasicallyEqualR32(left.z,      right.z))      { return false; }
	if (!BasicallyEqualR32(left.width,  right.width))  { return false; }
	if (!BasicallyEqualR32(left.height, right.height)) { return false; }
	if (!BasicallyEqualR32(left.depth,  right.depth))  { return false; }
	return true;
}

// +==============================+
// |             Boxi             |
// +==============================+
boxi BoxiShift(boxi boundingBox, i32 amountX, i32 amountY, i32 amountZ)
{
	boxi result;
	result.x = boundingBox.x + amountX;
	result.y = boundingBox.y + amountY;
	result.z = boundingBox.z + amountZ;
	result.size = boundingBox.size;
	return result;
}
boxi BoxiShift(boxi boundingBox, v3i amount)
{
	boxi result;
	result.x = boundingBox.x + amount.x;
	result.y = boundingBox.y + amount.y;
	result.z = boundingBox.z + amount.z;
	result.size = boundingBox.size;
	return result;
}
boxi BoxiScale(boxi boundingBox, i32 scalar)
{
	boxi result;
	result.x = boundingBox.x * scalar;
	result.y = boundingBox.y * scalar;
	result.z = boundingBox.z * scalar;
	result.width = boundingBox.width * scalar;
	result.height = boundingBox.height * scalar;
	result.depth = boundingBox.depth * scalar;
	return result;
}
boxi BoxiScale3(boxi boundingBox, i32 scaleX, i32 scaleY, i32 scaleZ)
{
	boxi result;
	result.x = boundingBox.x * scaleX;
	result.y = boundingBox.y * scaleY;
	result.z = boundingBox.z * scaleZ;
	result.width = boundingBox.width * scaleX;
	result.height = boundingBox.height * scaleY;
	result.depth = boundingBox.depth * scaleZ;
	return result;
}
boxi BoxiScale2(boxi boundingBox, v3i scale)
{
	boxi result;
	result.x = boundingBox.x * scale.x;
	result.y = boundingBox.y * scale.y;
	result.z = boundingBox.z * scale.z;
	result.width = boundingBox.width * scale.x;
	result.height = boundingBox.height * scale.y;
	result.depth = boundingBox.depth * scale.z;
	return result;
}
bool BoxiEqual(boxi left, boxi right)
{
	if (left.x      != right.x)      { return false; }
	if (left.y      != right.y)      { return false; }
	if (left.z      != right.z)      { return false; }
	if (left.width  != right.width)  { return false; }
	if (left.height != right.height) { return false; }
	if (left.depth  != right.depth)  { return false; }
	return true;
}

// +==============================+
// |            Obb2D             |
// +==============================+
obb2 Obb2DShift(obb2 boundingBox, r32 amountX, r32 amountY)
{
	obb2 result;
	result.x = boundingBox.x + amountX;
	result.y = boundingBox.y + amountY;
	result.size = boundingBox.size;
	result.rotation = boundingBox.rotation;
	return result;
}
obb2 Obb2DShift(obb2 boundingBox, v2 amount)
{
	obb2 result;
	result.x = boundingBox.x + amount.x;
	result.y = boundingBox.y + amount.y;
	result.size = boundingBox.size;
	result.rotation = boundingBox.rotation;
	return result;
}
obb2 Obb2DScale(obb2 boundingBox, r32 scalar)
{
	obb2 result;
	result.x = boundingBox.x * scalar;
	result.y = boundingBox.y * scalar;
	result.width = boundingBox.width * scalar;
	result.height = boundingBox.height * scalar;
	result.rotation = boundingBox.rotation;
	return result;
}
bool Obb2DBasicallyEqual(obb2 left, obb2 right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x,        right.x))         { return false; }
	if (!BasicallyEqualR32(left.y,        right.y))         { return false; }
	if (!BasicallyEqualR32(left.width,    right.width))     { return false; }
	if (!BasicallyEqualR32(left.height,   right.height))    { return false; }
	if (!BasicallyEqualR32(left.rotation, right.rotation))  { return false; }
	return true;
}

// +--------------------------------------------------------------+
// |                Expand Retract Inflate Deflate                |
// +--------------------------------------------------------------+
//Expand/Retract: +------+ (top left position stays the same)
//                ||---->|
//                ||     |
//                |V     |
//                +------+
//Inflate/Deflate: +------+ (total inflation/deflation is 2 times given value)
//                 |<^^^^>|
//                 |<    >|
//                 |<vvvv>|
//                 +------+

// +==============================+
// |             Rec              |
// +==============================+
rec RecExpand(rec rectangle, r32 extraWidth, r32 extraHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height + extraHeight;
	return result;
}
rec RecExpand(rec rectangle, v2 extraSize)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraSize.width;
	result.height = rectangle.height + extraSize.height;
	return result;
}
rec RecExpandX(rec rectangle, r32 extraWidth)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height;
	return result;
}
rec RecExpandY(rec rectangle, r32 extraHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight;
	return result;
}
rec RecRetract(rec rectangle, r32 subWidth, r32 subHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height - subHeight;
	return result;
}
rec RecRetract(rec rectangle, v2 subSize)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subSize.width;
	result.height = rectangle.height - subSize.height;
	return result;
}
rec RecRetractX(rec rectangle, r32 subWidth)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height;
	return result;
}
rec RecRetractY(rec rectangle, r32 subHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight;
	return result;
}
rec RecInflate(rec rectangle, r32 extraWidth, r32 extraHeight)
{
	rec result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
rec RecInflate(rec rectangle, v2 extraSize)
{
	rec result;
	result.x = rectangle.x - extraSize.width;
	result.y = rectangle.y - extraSize.height;
	result.width = rectangle.width + extraSize.width*2;
	result.height = rectangle.height + extraSize.height*2;
	return result;
}
rec RecInflateX(rec rectangle, r32 extraWidth)
{
	rec result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height;
	return result;
}
rec RecInflateY(rec rectangle, r32 extraHeight)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
rec RecDeflate(rec rectangle, r32 subWidth, r32 subHeight)
{
	rec result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height - subHeight*2;
	return result;
}
rec RecDeflate(rec rectangle, v2 extraSize)
{
	rec result;
	result.x = rectangle.x + extraSize.width;
	result.y = rectangle.y + extraSize.height;
	result.width = rectangle.width - extraSize.width*2;
	result.height = rectangle.height - extraSize.height*2;
	return result;
}
rec RecDeflateX(rec rectangle, r32 subWidth)
{
	rec result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height;
	return result;
}
rec RecDeflateY(rec rectangle, r32 subHeight)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight*2;
	return result;
}

// +==============================+
// |             Reci             |
// +==============================+
reci ReciExpand(reci rectangle, i32 extraWidth, i32 extraHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height + extraHeight;
	return result;
}
reci ReciExpand(reci rectangle, v2i extraSize)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraSize.width;
	result.height = rectangle.height + extraSize.height;
	return result;
}
reci ReciExpandX(reci rectangle, i32 extraWidth)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height;
	return result;
}
reci ReciExpandY(reci rectangle, i32 extraHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight;
	return result;
}
reci ReciRetract(reci rectangle, i32 subWidth, i32 subHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height - subHeight;
	return result;
}
reci ReciRetract(reci rectangle, v2i subSize)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subSize.width;
	result.height = rectangle.height - subSize.height;
	return result;
}
reci ReciRetractX(reci rectangle, i32 subWidth)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height;
	return result;
}
reci ReciRetractY(reci rectangle, i32 subHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight;
	return result;
}
reci ReciInflate(reci rectangle, i32 extraWidth, i32 extraHeight)
{
	reci result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
reci ReciInflate(reci rectangle, v2i extraSize)
{
	reci result;
	result.x = rectangle.x - extraSize.width;
	result.y = rectangle.y - extraSize.height;
	result.width = rectangle.width + extraSize.width*2;
	result.height = rectangle.height + extraSize.height*2;
	return result;
}
reci ReciInflateX(reci rectangle, i32 extraWidth)
{
	reci result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height;
	return result;
}
reci ReciInflateY(reci rectangle, i32 extraHeight)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
reci ReciDeflate(reci rectangle, i32 subWidth, i32 subHeight)
{
	reci result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height - subHeight*2;
	return result;
}
reci ReciDeflate(reci rectangle, v2i extraSize)
{
	reci result;
	result.x = rectangle.x + extraSize.width;
	result.y = rectangle.y + extraSize.height;
	result.width = rectangle.width - extraSize.width*2;
	result.height = rectangle.height - extraSize.height*2;
	return result;
}
reci ReciDeflateX(reci rectangle, i32 subWidth)
{
	reci result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height;
	return result;
}
reci ReciDeflateY(reci rectangle, i32 subHeight)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight*2;
	return result;
}

// +==============================+
// |             Box              |
// +==============================+
box BoxExpand(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
box BoxExpand(box boundingBox, v3 extraSize)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width + extraSize.width;
	result.height = boundingBox.height + extraSize.height;
	result.depth = boundingBox.depth + extraSize.depth;
	return result;
}
box BoxExpandX(box boundingBox, r32 extraWidth)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxExpandY(box boundingBox, r32 extraHeight)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth;
	return result;
}
box BoxExpandZ(box boundingBox, r32 extraDepth)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
box BoxRetract(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
box BoxRetract(box boundingBox, v3 subSize)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width - subSize.width;
	result.height = boundingBox.height - subSize.height;
	result.depth = boundingBox.depth - subSize.depth;
	return result;
}
box BoxRetractX(box boundingBox, r32 subWidth)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxRetractY(box boundingBox, r32 subHeight)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth;
	return result;
}
box BoxRetractZ(box boundingBox, r32 subDepth)
{
	box result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
box BoxInflate(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth)
{
	box result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
box BoxInflate(box boundingBox, v3 extraSize)
{
	box result;
	result.x = boundingBox.x - extraSize.width;
	result.y = boundingBox.y - extraSize.height;
	result.z = boundingBox.z - extraSize.depth;
	result.width = boundingBox.width + extraSize.width*2;
	result.height = boundingBox.height + extraSize.height*2;
	result.depth = boundingBox.depth + extraSize.depth*2;
	return result;
}
box BoxInflateX(box boundingBox, r32 extraWidth)
{
	box result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInflateY(box boundingBox, r32 extraHeight)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInflateZ(box boundingBox, r32 extraDepth)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
box BoxDeflate(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth)
{
	box result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}
box BoxDeflate(box boundingBox, v3 extraSize)
{
	box result;
	result.x = boundingBox.x + extraSize.width;
	result.y = boundingBox.y + extraSize.height;
	result.z = boundingBox.z + extraSize.depth;
	result.width = boundingBox.width - extraSize.width*2;
	result.height = boundingBox.height - extraSize.height*2;
	result.depth = boundingBox.depth - extraSize.depth*2;
	return result;
}
box BoxDeflateX(box boundingBox, r32 subWidth)
{
	box result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxDeflateY(box boundingBox, r32 subHeight)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
box BoxDeflateZ(box boundingBox, r32 subDepth)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}

// +==============================+
// |             Boxi             |
// +==============================+
boxi BoxiExpand(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
boxi BoxiExpand(boxi boundingBox, v3i extraSize)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width + extraSize.width;
	result.height = boundingBox.height + extraSize.height;
	result.depth = boundingBox.depth + extraSize.depth;
	return result;
}
boxi BoxiExpandX(boxi boundingBox, i32 extraWidth)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiExpandY(boxi boundingBox, i32 extraHeight)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiExpandZ(boxi boundingBox, i32 extraDepth)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
boxi BoxiRetract(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
boxi BoxiRetract(boxi boundingBox, v3i subSize)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width - subSize.width;
	result.height = boundingBox.height - subSize.height;
	result.depth = boundingBox.depth - subSize.depth;
	return result;
}
boxi BoxiRetractX(boxi boundingBox, i32 subWidth)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiRetractY(boxi boundingBox, i32 subHeight)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiRetractZ(boxi boundingBox, i32 subDepth)
{
	boxi result;
	result.topLeft = boundingBox.topLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
boxi BoxiInflate(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth)
{
	boxi result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
boxi BoxiInflate(boxi boundingBox, v3i extraSize)
{
	boxi result;
	result.x = boundingBox.x - extraSize.width;
	result.y = boundingBox.y - extraSize.height;
	result.z = boundingBox.z - extraSize.depth;
	result.width = boundingBox.width + extraSize.width*2;
	result.height = boundingBox.height + extraSize.height*2;
	result.depth = boundingBox.depth + extraSize.depth*2;
	return result;
}
boxi BoxiInflateX(boxi boundingBox, i32 extraWidth)
{
	boxi result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInflateY(boxi boundingBox, i32 extraHeight)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInflateZ(boxi boundingBox, i32 extraDepth)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
boxi BoxiDeflate(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth)
{
	boxi result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}
boxi BoxiDeflate(boxi boundingBox, v3i extraSize)
{
	boxi result;
	result.x = boundingBox.x + extraSize.width;
	result.y = boundingBox.y + extraSize.height;
	result.z = boundingBox.z + extraSize.depth;
	result.width = boundingBox.width - extraSize.width*2;
	result.height = boundingBox.height - extraSize.height*2;
	result.depth = boundingBox.depth - extraSize.depth*2;
	return result;
}
boxi BoxiDeflateX(boxi boundingBox, i32 subWidth)
{
	boxi result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiDeflateY(boxi boundingBox, i32 subHeight)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiDeflateZ(boxi boundingBox, i32 subDepth)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}

// +==============================+
// |            Obb2D             |
// +==============================+
//TODO: Implement these functions properly by moving the center with correct respect for rotation
#if 0
obb2 Obb2DExpand(obb2 rectangle, r32 extraWidth, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height + extraHeight;
	return result;
}
obb2 Obb2DExpand(obb2 rectangle, v2 extraSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraSize.width;
	result.height = rectangle.height + extraSize.height;
	return result;
}
obb2 Obb2DExpandX(obb2 rectangle, r32 extraWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DExpandY(obb2 rectangle, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight;
	return result;
}
obb2 Obb2DRetract(obb2 rectangle, r32 subWidth, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height - subHeight;
	return result;
}
obb2 Obb2DRetract(obb2 rectangle, v2 subSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subSize.width;
	result.height = rectangle.height - subSize.height;
	return result;
}
obb2 Obb2DRetractX(obb2 rectangle, r32 subWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DRetractY(obb2 rectangle, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight;
	return result;
}
#endif
obb2 Obb2DInflate(obb2 rectangle, r32 extraWidth, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
obb2 Obb2DInflate(obb2 rectangle, v2 extraSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraSize.width*2;
	result.height = rectangle.height + extraSize.height*2;
	return result;
}
obb2 Obb2DInflateX(obb2 rectangle, r32 extraWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DInflateY(obb2 rectangle, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
obb2 Obb2DDeflate(obb2 rectangle, r32 subWidth, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height - subHeight*2;
	return result;
}
obb2 Obb2DDeflate(obb2 rectangle, v2 extraSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - extraSize.width*2;
	result.height = rectangle.height - extraSize.height*2;
	return result;
}
obb2 Obb2DDeflateX(obb2 rectangle, r32 subWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DDeflateY(obb2 rectangle, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight*2;
	return result;
}

//TODO: Add Obb3D functions

// +--------------------------------------------------------------+
// |                    Other Simple Functions                    |
// +--------------------------------------------------------------+
// +==============================+
// |             Rec              |
// +==============================+
rec RecSquarify(rec rectangle, bool makeLarger = true, bool center = true)
{
	rec result;
	r32 newSize = (makeLarger ? MaxR32(rectangle.width, rectangle.height) : MinR32(rectangle.width, rectangle.height));
	result.x = rectangle.x - (center ? (newSize - rectangle.width)/2 : 0);
	result.y = rectangle.y - (center ? (newSize - rectangle.height)/2 : 0);
	result.width = newSize;
	result.height = newSize;
	return result;
}
rec RecInvert(rec rectangle)
{
	rec result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y + rectangle.height;
	result.width = -rectangle.width;
	result.height = -rectangle.height;
	return result;
}
rec RecInvertX(rec rectangle)
{
	rec result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y;
	result.width = -rectangle.width;
	result.height = rectangle.height;
	return result;
}
rec RecInvertY(rec rectangle)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y + rectangle.height;
	result.width = rectangle.width;
	result.height = -rectangle.height;
	return result;
}
rec RecUninvert(rec rectangle)
{
	rec result;
	result.x = MinR32(rectangle.x, rectangle.x + rectangle.width);
	result.y = MinR32(rectangle.y, rectangle.y + rectangle.height);
	result.width = AbsR32(rectangle.width);
	result.height = AbsR32(rectangle.height);
	return result;
}

// +==============================+
// |             Reci             |
// +==============================+
reci ReciSquarify(reci rectangle, bool makeLarger = true)
{
	reci result;
	i32 newSize = (makeLarger ? MaxI32(rectangle.width, rectangle.height) : MinI32(rectangle.width, rectangle.height));
	result.x = rectangle.x;
	result.y = rectangle.y;
	result.width = newSize;
	result.height = newSize;
	return result;
}
reci ReciInvert(reci rectangle)
{
	reci result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y + rectangle.height;
	result.width = -rectangle.width;
	result.height = -rectangle.height;
	return result;
}
reci ReciInvertX(reci rectangle)
{
	reci result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y;
	result.width = -rectangle.width;
	result.height = rectangle.height;
	return result;
}
reci ReciInvertY(reci rectangle)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y + rectangle.height;
	result.width = rectangle.width;
	result.height = -rectangle.height;
	return result;
}
reci ReciUninvert(reci rectangle)
{
	reci result;
	result.x = MinI32(rectangle.x, rectangle.x + rectangle.width);
	result.y = MinI32(rectangle.y, rectangle.y + rectangle.height);
	result.width = AbsI32(rectangle.width);
	result.height = AbsI32(rectangle.height);
	return result;
}

// +==============================+
// |             Box              |
// +==============================+
box BoxCubify(box boundingBox, bool makeLarger = true, bool center = true)
{
	box result;
	r32 newSize = (makeLarger ? MaxR32(boundingBox.width, boundingBox.height, boundingBox.depth) : MinR32(boundingBox.width, boundingBox.height, boundingBox.depth));
	result.x = boundingBox.x - (center ? (newSize - boundingBox.width)/2 : 0);
	result.y = boundingBox.y - (center ? (newSize - boundingBox.height)/2 : 0);
	result.z = boundingBox.z - (center ? (newSize - boundingBox.depth)/2 : 0);
	result.width = newSize;
	result.height = newSize;
	result.depth = newSize;
	return result;
}
box BoxInvert(box boundingBox)
{
	box result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = -boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
box BoxInvertX(box boundingBox)
{
	box result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = -boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInvertY(box boundingBox)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInvertZ(box boundingBox)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
box BoxUninvert(box boundingBox)
{
	box result;
	result.x = MinR32(boundingBox.x, boundingBox.x + boundingBox.width);
	result.y = MinR32(boundingBox.y, boundingBox.y + boundingBox.height);
	result.z = MinR32(boundingBox.z, boundingBox.z + boundingBox.depth);
	result.width = AbsR32(boundingBox.width);
	result.height = AbsR32(boundingBox.height);
	result.depth = AbsR32(boundingBox.depth);
	return result;
}

// +==============================+
// |             Boxi             |
// +==============================+
boxi BoxiCubify(boxi boundingBox, bool makeLarger = true)
{
	boxi result;
	i32 newSize = (makeLarger ? MaxI32(boundingBox.width, boundingBox.height, boundingBox.depth) : MinI32(boundingBox.width, boundingBox.height, boundingBox.depth));
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = newSize;
	result.height = newSize;
	result.depth = newSize;
	return result;
}
boxi BoxiInvert(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = -boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
boxi BoxiInvertX(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = -boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInvertY(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInvertZ(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
boxi BoxiUninvert(boxi boundingBox)
{
	boxi result;
	result.x = MinI32(boundingBox.x, boundingBox.x + boundingBox.width);
	result.y = MinI32(boundingBox.y, boundingBox.y + boundingBox.height);
	result.z = MinI32(boundingBox.z, boundingBox.z + boundingBox.depth);
	result.width = AbsI32(boundingBox.width);
	result.height = AbsI32(boundingBox.height);
	result.depth = AbsI32(boundingBox.depth);
	return result;
}

// +==============================+
// |            Obb2D             |
// +==============================+
obb2 Obb2DSquarify(obb2 rectangle, bool makeLarger = true)
{
	obb2 result;
	r32 newSize = (makeLarger ? MaxR32(rectangle.width, rectangle.height) : MinR32(rectangle.width, rectangle.height));
	result.center = rectangle.center;
	result.width = newSize;
	result.height = newSize;
	return result;
}
obb2 Obb2DInvert(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = -rectangle.width;
	result.height = -rectangle.height;
	return result;
}
obb2 Obb2DInvertX(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = -rectangle.width;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DInvertY(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = -rectangle.height;
	return result;
}
obb2 Obb2DUninvert(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = AbsR32(rectangle.width);
	result.height = AbsR32(rectangle.height);
	return result;
}

//TODO: Add Obb3D functions

// +--------------------------------------------------------------+
// |                 Other Complicated Functions                  |
// +--------------------------------------------------------------+
v2 GetObb2DRelativePos(obb2 box, v2 point)
{
	v2 result;
	v2 rotVec = NewVec2(CosR32(box.rotation), SinR32(box.rotation));
	result.x = Vec2Dot(point - box.center, rotVec);
	result.y = Vec2Dot(point - box.center, Vec2PerpRight(rotVec));
	return result;
}
v2 GetObb2DWorldPoint(obb2 box, v2 relativeOffset)
{
	v2 result = box.center;
	v2 rotVec = NewVec2(CosR32(box.rotation), SinR32(box.rotation));
	result += (-box.width/2 + relativeOffset.x) * rotVec;
	result += (-box.height/2 + relativeOffset.y) * Vec2PerpRight(rotVec);
	return result;
}

// +--------------------------------------------------------------+
// |                      Operator Overloads                      |
// +--------------------------------------------------------------+
inline rec operator + (rec rectangle, v2 vector) { return RecShift(rectangle, vector); }
inline rec operator - (rec rectangle, v2 vector) { return RecShift(rectangle, -vector); }
inline rec operator * (rec rectangle, r32 scale) { return RecScale(rectangle, scale); }
inline rec operator / (rec rectangle, r32 scale) { return RecScale(rectangle, 1/scale); }

inline reci operator + (reci rectangle, v2i vector) { return ReciShift(rectangle, vector); }
inline reci operator - (reci rectangle, v2i vector) { return ReciShift(rectangle, -vector); }
inline reci operator * (reci rectangle, i32 scale)  { return ReciScale(rectangle, scale); }
inline reci operator / (reci rectangle, i32 scale)  { return ReciScale(rectangle, 1/scale); }
inline bool operator == (reci left, reci right) { return  (left.x == right.x && left.y == right.y && left.width == right.width && left.height == right.height); }
inline bool operator != (reci left, reci right) { return !(left.x == right.x && left.y == right.y && left.width == right.width && left.height == right.height); }

inline box operator + (box boundingBox, v3 vector) { return BoxShift(boundingBox, vector); }
inline box operator - (box boundingBox, v3 vector) { return BoxShift(boundingBox, -vector); }
inline box operator * (box boundingBox, r32 scale) { return BoxScale(boundingBox, scale); }
inline box operator / (box boundingBox, r32 scale) { return BoxScale(boundingBox, 1/scale); }

inline boxi operator + (boxi boundingBox, v3i vector) { return BoxiShift(boundingBox, vector); }
inline boxi operator - (boxi boundingBox, v3i vector) { return BoxiShift(boundingBox, -vector); }
inline boxi operator * (boxi boundingBox, i32 scale)  { return BoxiScale(boundingBox, scale); }
inline boxi operator / (boxi boundingBox, i32 scale)  { return BoxiScale(boundingBox, 1/scale); }
inline bool operator == (boxi left, boxi right) { return  (left.x == right.x && left.y == right.y && left.z == right.z && left.width == right.width && left.height == right.height && left.depth == right.depth); }
inline bool operator != (boxi left, boxi right) { return !(left.x == right.x && left.y == right.y && left.z == right.z && left.width == right.width && left.height == right.height && left.depth == right.depth); }

inline obb2 operator + (obb2 box, v2 vector) { return Obb2DShift(box, vector); }
inline obb2 operator - (obb2 box, v2 vector) { return Obb2DShift(box, -vector); }
inline obb2 operator * (obb2 box, r32 scale) { return Obb2DScale(box, scale); }
inline obb2 operator / (obb2 box, r32 scale) { return Obb2DScale(box, 1/scale); }

//TODO: Add operator overloads for obb3

#endif //  _GY_RECTANGLES_H
