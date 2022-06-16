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

struct RayVsRectangle2DResult_t
{
	bool intersects;
	r32 enterTime;
	r32 exitTime;
	v2 enterPos;
	v2 exitPos;
	Dir2_t enterSide;
	Dir2_t exitSide;
};
bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeEnterTimes = false)
{
	NotNull(result);
	ClearPointer(result);
	
	if (ray.direction.x == 0 && ray.direction.y == 0) // Empty Ray
	{
		if (IsInsideRec(rectangle, ray.origin))
		{
			result->intersects = true;
			result->enterTime = 0;
			result->enterPos = ray.origin;
			result->enterSide = Dir2_None;
			result->exitTime = 0;
			result->exitPos = ray.origin;
			result->exitSide = Dir2_None;
		}
	}
	else if (ray.direction.x == 0) //Vertical Ray
	{
		if (ray.origin.x >= rectangle.x && ray.origin.x < rectangle.x + rectangle.width)
		{
			r32 enterDist = rectangle.y - ray.origin.y;
			r32 exitDist = (rectangle.y + rectangle.height) - ray.origin.y;
			r32 enterTime = (enterDist / ray.direction.y);
			r32 exitTime = (exitDist / ray.direction.y);
			if (enterTime > exitTime)
			{
				SWAP_VARIABLES(r32, enterDist, exitDist);
				SWAP_VARIABLES(r32, enterTime, exitTime);
			}
			
			if (exitTime >= 0)
			{
				if (!giveNegativeEnterTimes) { enterTime = MaxR32(0, enterTime); }
				result->intersects = true;
				result->enterTime = enterTime;
				result->exitTime = exitTime;
				result->enterPos = ray.origin + (ray.direction * enterTime);
				result->exitPos = ray.origin + (ray.direction * exitTime);
				result->enterSide = (ray.direction.y > 0) ? Dir2_Up : Dir2_Down;
				result->exitSide = (ray.direction.y > 0) ? Dir2_Down : Dir2_Up;
			}
		}
	}
	else if (ray.direction.y == 0) //Horizontal Ray
	{
		if (ray.origin.y >= rectangle.y && ray.origin.y < rectangle.y + rectangle.height)
		{
			r32 enterDist = rectangle.x - ray.origin.x;
			r32 exitDist = (rectangle.x + rectangle.width) - ray.origin.x;
			r32 enterTime = (enterDist / ray.direction.x);
			r32 exitTime = (exitDist / ray.direction.x);
			if (enterTime > exitTime)
			{
				SWAP_VARIABLES(r32, enterDist, exitDist);
				SWAP_VARIABLES(r32, enterTime, exitTime);
			}
			
			if (exitTime >= 0)
			{
				if (!giveNegativeEnterTimes) { enterTime = MaxR32(0, enterTime); }
				result->intersects = true;
				result->enterTime = enterTime;
				result->exitTime = exitTime;
				result->enterPos = ray.origin + (ray.direction * enterTime);
				result->exitPos = ray.origin + (ray.direction * exitTime);
				result->enterSide = (ray.direction.x > 0) ? Dir2_Left : Dir2_Right;
				result->exitSide = (ray.direction.x > 0) ? Dir2_Right : Dir2_Left;
			}
		}
	}
	else //Diagonal Ray
	{
		r32 horiEnterDist = rectangle.x - ray.origin.x;
		r32 horiExitDist = (rectangle.x + rectangle.width) - ray.origin.x;
		r32 horiEnterTime = (horiEnterDist / ray.direction.x);
		r32 horiExitTime = (horiExitDist / ray.direction.x);
		if (horiEnterTime > horiExitTime)
		{
			SWAP_VARIABLES(r32, horiEnterDist, horiExitDist);
			SWAP_VARIABLES(r32, horiEnterTime, horiExitTime);
		}
		
		r32 vertEnterDist = rectangle.y - ray.origin.y;
		r32 vertExitDist = (rectangle.y + rectangle.height) - ray.origin.y;
		r32 vertEnterTime = (vertEnterDist / ray.direction.y);
		r32 vertExitTime = (vertExitDist / ray.direction.y);
		if (vertEnterTime > vertExitTime)
		{
			SWAP_VARIABLES(r32, vertEnterDist, vertExitDist);
			SWAP_VARIABLES(r32, vertEnterTime, vertExitTime);
		}
		
		//Make sure the regions overlap and the overlap doesn't end before the ray starts (negative time values)
		if (horiExitTime >= 0 && vertExitTime >= 0 && horiExitTime >= vertEnterTime && vertExitTime >= horiEnterTime)
		{
			if (horiEnterTime >= vertEnterTime) //we enter last on horizontal-axis
			{
				if (!giveNegativeEnterTimes) { horiEnterTime = MaxR32(0, horiEnterTime); }
				result->intersects = true;
				result->enterTime = horiEnterTime;
				result->enterPos = ray.origin + (ray.direction * horiEnterTime);
				result->enterSide = (ray.direction.x > 0) ? Dir2_Left : Dir2_Right;
			}
			else //we enter last on vertical-axis
			{
				if (!giveNegativeEnterTimes) { vertEnterTime = MaxR32(0, vertEnterTime); }
				result->intersects = true;
				result->enterTime = vertEnterTime;
				result->enterPos = ray.origin + (ray.direction * vertEnterTime);
				result->enterSide = (ray.direction.y > 0) ? Dir2_Up : Dir2_Down;
			}
			
			if (horiExitTime <= vertExitTime) //we exit first on horizontal-axis
			{
				result->exitTime = horiExitTime;
				result->exitPos = ray.origin + (ray.direction * horiExitTime);
				result->exitSide = (ray.direction.x > 0) ? Dir2_Right : Dir2_Left;
			}
			else //we exit first on vertical-axis
			{
				result->exitTime = vertExitTime;
				result->exitPos = ray.origin + (ray.direction * vertExitTime);
				result->exitSide = (ray.direction.y > 0) ? Dir2_Down : Dir2_Up;
			}
		}
	}
	
	return result->intersects;
}

struct RayVsObb2DResult_t
{
	bool intersects;
	r32 enterTime;
	r32 exitTime;
	v2 enterPos;
	v2 exitPos;
	Dir2_t enterSide;
	Dir2_t exitSide;
	v2 enterSideNormal;
	v2 exitSideNormal;
};
bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeEnterTimes = false)
{
	NotNull(result);
	ClearPointer(result);
	
	Ray2_t alignedRay;
	alignedRay.origin = Vec2Rotate(ray.origin - boundingBox.center, -boundingBox.rotation);
	alignedRay.direction = Vec2Rotate(ray.direction, -boundingBox.rotation);
	rec alignedRec = NewRecCentered(Vec2_Zero, boundingBox.size);
	
	RayVsRectangle2DResult_t recResult;
	if (RayVsRectangle2D(alignedRay, alignedRec, &recResult, giveNegativeEnterTimes))
	{
		result->intersects = recResult.intersects;
		result->enterTime = recResult.enterTime;
		result->exitTime = recResult.exitTime;
		result->enterPos = boundingBox.center + Vec2Rotate(recResult.enterPos, boundingBox.rotation);
		result->exitPos = boundingBox.center + Vec2Rotate(recResult.exitPos, boundingBox.rotation);
		result->enterSide = recResult.enterSide;
		result->exitSide = recResult.exitSide;
		result->enterSideNormal = Vec2FromAngle(boundingBox.rotation + GetDir2AngleR32(recResult.enterSide));
		result->exitSideNormal = Vec2FromAngle(boundingBox.rotation + GetDir2AngleR32(recResult.exitSide));
		return true;
	}
	else
	{
		return false;
	}
}

#endif //  _GY_COLLISION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines

@Types

@Functions

*/
