/*
File:   gy_collision.h
Author: Taylor Robbins
Date:   10\09\2021
Description:
	** Contains a bunch of functions that help us calculate intersections and collision information between various primitive shapes in 2D and 3D
	
	NOTE: Depending on what files are included before this various functions may or may not be available
*/

#ifndef _GY_COLLISION_H
#define _GY_COLLISION_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

#if defined(_GY_PRIMITIVES_H) && defined(_GY_RECTANGLES_H)
// TODO: bool FrustumVsAABB(Frustum_t frustum, AABB3_t aabb)
#endif

// TODO: bool SeperatingAxisCheck(u32 numNormals, const v2* normals, u32 numVerts1, const v2* verts1, u32 numVerts2, const v2* verts2, r32* depthOut = nullptr, v2* sepAxisOut = nullptr, r32* sepOffsetOut = nullptr)

#endif //  _GY_COLLISION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines

@Types

@Functions

*/
