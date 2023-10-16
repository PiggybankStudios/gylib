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

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
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

struct RayVsBoxResult_t
{
	bool intersects;
	r32 enterTime;
	r32 exitTime;
	v3 enterPos;
	v3 exitPos;
	Dir3_t enterSide;
	Dir3_t exitSide;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeTimes = false);
	bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeTimes = false);
	bool RayVsBox(Ray3_t ray, box boundingBox, RayVsBoxResult_t* result, bool giveNegativeTimes = false, bool inclusive = true);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
#if defined(_GY_PRIMITIVES_H) && defined(_GY_RECTANGLES_H)
// TODO: bool FrustumVsAABB(Frustum_t frustum, AABB3_t aabb)
#endif

// TODO: bool SeperatingAxisCheck(u32 numNormals, const v2* normals, u32 numVerts1, const v2* verts1, u32 numVerts2, const v2* verts2, r32* depthOut = nullptr, v2* sepAxisOut = nullptr, r32* sepOffsetOut = nullptr)

bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeTimes = false)
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
				if (!giveNegativeTimes) { enterTime = MaxR32(0, enterTime); }
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
				if (!giveNegativeTimes) { enterTime = MaxR32(0, enterTime); }
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
				if (!giveNegativeTimes) { horiEnterTime = MaxR32(0, horiEnterTime); }
				result->intersects = true;
				result->enterTime = horiEnterTime;
				result->enterPos = ray.origin + (ray.direction * horiEnterTime);
				result->enterSide = (ray.direction.x > 0) ? Dir2_Left : Dir2_Right;
			}
			else //we enter last on vertical-axis
			{
				if (!giveNegativeTimes) { vertEnterTime = MaxR32(0, vertEnterTime); }
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

bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeTimes = false)
{
	NotNull(result);
	ClearPointer(result);
	
	Ray2_t alignedRay;
	alignedRay.origin = Vec2Rotate(ray.origin - boundingBox.center, -boundingBox.rotation);
	alignedRay.direction = Vec2Rotate(ray.direction, -boundingBox.rotation);
	rec alignedRec = NewRecCentered(Vec2_Zero, boundingBox.size);
	
	RayVsRectangle2DResult_t recResult;
	if (RayVsRectangle2D(alignedRay, alignedRec, &recResult, giveNegativeTimes))
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

bool RayVsBox(Ray3_t ray, box boundingBox, RayVsBoxResult_t* result, bool giveNegativeTimes = false, bool inclusive = true)
{
	NotNull(result);
	ClearPointer(result);
	result->intersects = false;
	
	const r32 dumbInfinite = 10000; //TODO: Get rid of the need for this
	if (ray.direction == Vec3_Zero)
	{
		if (IsInsideBox(boundingBox, ray.origin))
		{
			result->intersects = true;
			result->enterTime = 0;
			result->exitTime = dumbInfinite;
			result->enterPos = ray.origin;
			result->exitPos = ray.origin;
			result->enterSide = Dir3_None;
			result->exitSide = Dir3_None;
			return true;
		}
	}
	
	//don't start inside on an axis and not moving on that axis. Will never intersect
	if (ray.direction.x == 0 && (ray.origin.x < boundingBox.x || ray.origin.x > boundingBox.x + boundingBox.width  || (!inclusive && ray.origin.x == boundingBox.x + boundingBox.width)))  { return false; }
	if (ray.direction.y == 0 && (ray.origin.y < boundingBox.y || ray.origin.y > boundingBox.y + boundingBox.height || (!inclusive && ray.origin.y == boundingBox.y + boundingBox.height))) { return false; }
	if (ray.direction.z == 0 && (ray.origin.z < boundingBox.z || ray.origin.z > boundingBox.z + boundingBox.depth  || (!inclusive && ray.origin.z == boundingBox.z + boundingBox.depth)))  { return false; }
	
	v3 positiveRayDir = NewVec3(AbsR32(ray.direction.x), AbsR32(ray.direction.y), AbsR32(ray.direction.z));
	r32 startX = (ray.direction.x >= 0) ? (boundingBox.x - ray.origin.x)                        : ((boundingBox.x + boundingBox.width) - ray.origin.x);
	r32 endX   = (ray.direction.x >= 0) ? ((boundingBox.x + boundingBox.width) - ray.origin.x)  : (boundingBox.x - ray.origin.x);
	r32 startY = (ray.direction.y >= 0) ? (boundingBox.y - ray.origin.y)                        : ((boundingBox.y + boundingBox.height) - ray.origin.y);
	r32 endY   = (ray.direction.y >= 0) ? ((boundingBox.y + boundingBox.height) - ray.origin.y) : (boundingBox.y - ray.origin.y);
	r32 startZ = (ray.direction.z >= 0) ? (boundingBox.z - ray.origin.z)                        : ((boundingBox.z + boundingBox.depth) - ray.origin.z);
	r32 endZ   = (ray.direction.z >= 0) ? ((boundingBox.z + boundingBox.depth) - ray.origin.z)  : (boundingBox.z - ray.origin.z);
	
	r32 startTimeX = (ray.direction.x != 0) ? (startX / ray.direction.x) : -dumbInfinite;
	r32 endTimeX   = (ray.direction.x != 0) ? (endX   / ray.direction.x) :  dumbInfinite;
	r32 startTimeY = (ray.direction.y != 0) ? (startY / ray.direction.y) : -dumbInfinite;
	r32 endTimeY   = (ray.direction.y != 0) ? (endY   / ray.direction.y) :  dumbInfinite;
	r32 startTimeZ = (ray.direction.z != 0) ? (startZ / ray.direction.z) : -dumbInfinite;
	r32 endTimeZ   = (ray.direction.z != 0) ? (endZ   / ray.direction.z) :  dumbInfinite;
	
	//our last start time is greater than our first end time. No intersection
	if (ray.direction.x != 0 && ((ray.direction.y != 0 && endTimeX <= startTimeY) || (ray.direction.z != 0 && endTimeX <= startTimeZ)))
	{
		return false;
	}
	if (ray.direction.y != 0 && ((ray.direction.x != 0 && endTimeY <= startTimeX) || (ray.direction.z != 0 && endTimeY <= startTimeZ)))
	{
		return false;
	}
	if (ray.direction.z != 0 && ((ray.direction.x != 0 && endTimeZ <= startTimeX) || (ray.direction.y != 0 && endTimeZ <= startTimeY)))
	{
		return false;
	}
	
	r32 startTime = 0.0f;
	Dir3_t startSide = Dir3_None;
	if (startTimeX != dumbInfinite && (startTimeY == dumbInfinite || startTimeX >= startTimeY) && (startTimeZ == dumbInfinite || startTimeX >= startTimeZ))
	{
		startTime = startTimeX;
		startSide = (ray.direction.x > 0) ? Dir3_Left : Dir3_Right;
	}
	else if (startTimeY != dumbInfinite && (startTimeZ == dumbInfinite || startTimeY >= startTimeZ))
	{
		startTime = startTimeY;
		startSide = (ray.direction.y > 0) ? Dir3_Down : Dir3_Up;
	}
	else if (startTimeZ != dumbInfinite)
	{
		startTime = startTimeZ;
		startSide = (ray.direction.z > 0) ? Dir3_Backward : Dir3_Forward;
	}
	else { Assert(false); return false; }
	if (!giveNegativeTimes && startTime < 0) { startTime = 0; }
	
	r32 endTime = 0.0f;
	Dir3_t endSide = Dir3_None;
	if (endTimeX != dumbInfinite && (endTimeY == dumbInfinite || endTimeX <= endTimeY) && (endTimeZ == dumbInfinite || endTimeX <= endTimeZ))
	{
		endTime = endTimeX;
		endSide = (ray.direction.x > 0) ? Dir3_Right : Dir3_Left;
	}
	else if (endTimeY != dumbInfinite && (endTimeZ == dumbInfinite || endTimeY <= endTimeZ))
	{
		endTime = endTimeY;
		endSide = (ray.direction.y > 0) ? Dir3_Up : Dir3_Down;
	}
	else if (endTimeZ != dumbInfinite)
	{
		endTime = endTimeZ;
		endSide = (ray.direction.z > 0) ? Dir3_Forward : Dir3_Backward;
	}
	else { Assert(false); return false; }
	if (endTime < 0) { return false; } //intersection happened in opposite direction (negative time)
	
	result->intersects = true;
	result->enterTime = startTime;
	result->exitTime = endTime;
	result->enterPos = ray.origin + (ray.direction * startTime);
	result->exitPos = ray.origin + (ray.direction * endTime);
	result->enterSide = startSide;
	result->exitSide = endSide;
	return true;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_COLLISION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
RayVsRectangle2DResult_t
RayVsObb2DResult_t
RayVsBoxResult_t
@Functions
bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeTimes = false)
bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeTimes = false)
bool RayVsBox(Ray3_t ray, box boundingBox, RayVsBoxResult_t* result, bool giveNegativeTimes = false)
*/
