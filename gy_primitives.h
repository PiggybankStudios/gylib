/*
File:   gy_primitives.h
Author: Taylor Robbins
Date:   10\09\2021
Description:
	** Contains a bunch of 3D primitive structures and related functions
*/

#ifndef _GY_PRIMITIVES_H
#define _GY_PRIMITIVES_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct Frustum_t
{
	union
	{
		v3 origin;
		struct { r32 x, y, z; };
	};
	union
	{
		v3 direction;
		struct { r32 dX, dY, dZ; };
	};
	union
	{
		v3 upVector;
		struct { r32 upX, upY, upZ; };
	};
	union
	{
		v2 fov;
		struct { r32 fovX, fovY; };
	};
	r32 zNear;
	r32 zFar;
};

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Frustum_t NewFrustum(v3 origin, v3 direction, v3 upVector, v2 fov, r32 zNear, r32 zFar)
{
	Frustum_t result;
	result.origin = origin;
	result.direction = Vec3Normalize(direction);
	result.upVector = Vec3Normalize(upVector);
	result.fov = fov;
	result.zNear = zNear;
	result.zFar = zFar;
	return result;
}


#endif //  _GY_PRIMITIVES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
Frustum_t
@Functions
Frustum_t NewFrustum(v3 origin, v3 direction, v3 upVector, v2 fov, r32 zNear, r32 zFar)
*/
