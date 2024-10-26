/*
File:   gy_voxel_rotation.h
Author: Taylor Robbins
Date:   10\25\2024
*/

#ifndef _GY_VOXEL_ROTATION_H
#define _GY_VOXEL_ROTATION_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"
#include "gy_directions.h"
#include "gy_quaternions.h"
#include "gy_rectangles.h"

enum VoxelRotation_t
{
	VoxelRotation_Forward0 = 0,
	VoxelRotation_Forward90,
	VoxelRotation_Forward180,
	VoxelRotation_Forward270,
	VoxelRotation_Right0,
	VoxelRotation_Right90,
	VoxelRotation_Right180,
	VoxelRotation_Right270,
	VoxelRotation_Up0,
	VoxelRotation_Up90,
	VoxelRotation_Up180,
	VoxelRotation_Up270,
	VoxelRotation_Backward0,
	VoxelRotation_Backward90,
	VoxelRotation_Backward180,
	VoxelRotation_Backward270,
	VoxelRotation_Left0,
	VoxelRotation_Left90,
	VoxelRotation_Left180,
	VoxelRotation_Left270,
	VoxelRotation_Down0,
	VoxelRotation_Down90,
	VoxelRotation_Down180,
	VoxelRotation_Down270,
	VoxelRotation_NumRotations = 24,
	VoxelRotation_Default = VoxelRotation_Forward0,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetVoxelRotationStr(VoxelRotation_t rotation);
#else
const char* GetVoxelRotationStr(VoxelRotation_t rotation)
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return "Forward0";
		case VoxelRotation_Forward90:   return "Forward90";
		case VoxelRotation_Forward180:  return "Forward180";
		case VoxelRotation_Forward270:  return "Forward270";
		case VoxelRotation_Right0:      return "Right0";
		case VoxelRotation_Right90:     return "Right90";
		case VoxelRotation_Right180:    return "Right180";
		case VoxelRotation_Right270:    return "Right270";
		case VoxelRotation_Up0:         return "Up0";
		case VoxelRotation_Up90:        return "Up90";
		case VoxelRotation_Up180:       return "Up180";
		case VoxelRotation_Up270:       return "Up270";
		case VoxelRotation_Backward0:   return "Backward0";
		case VoxelRotation_Backward90:  return "Backward90";
		case VoxelRotation_Backward180: return "Backward180";
		case VoxelRotation_Backward270: return "Backward270";
		case VoxelRotation_Left0:       return "Left0";
		case VoxelRotation_Left90:      return "Left90";
		case VoxelRotation_Left180:     return "Left180";
		case VoxelRotation_Left270:     return "Left270";
		case VoxelRotation_Down0:       return "Down0";
		case VoxelRotation_Down90:      return "Down90";
		case VoxelRotation_Down180:     return "Down180";
		case VoxelRotation_Down270:     return "Down270";
		default: return "Unknown";
	}
}
#endif

struct VoxelBasis_t
{
	Dir3_t rightDir;
	Dir3_t upDir;
	Dir3_t forwardDir;
	
	//Helpers:
	v3i righti;
	v3i upi;
	v3i forwardi;
	v3 right;
	v3 up;
	v3 forward;
	VoxelRotation_t rotation;
	quat rotationQuat;
	bool isNegativeSide;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define VoxelRotationMult(startRotation, rotationAmount) RotateVoxelRotation((startRotation), (rotationAmount))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Dir3_t GetVoxelRotationNormalDir(VoxelRotation_t rotation);
	Dir3_t GetVoxelRotationUDir(VoxelRotation_t rotation);
	Dir3_t GetVoxelRotationVDir(VoxelRotation_t rotation);
	u8 GetVoxelRotationClockwiseAmount(VoxelRotation_t rotation);
	Dir3_t RotateDirByVoxelRotation(Dir3_t direction, VoxelRotation_t rotation);
	v3 RotateVecByVoxelRotation(v3 vector, VoxelRotation_t rotation, bool useVec3HalfAsCenter = false);
	v3i RotateVeciByVoxelRotation(v3i vector, VoxelRotation_t rotation);
	v3i RotateSizeiByVoxelRotation(v3i size, VoxelRotation_t rotation);
	VoxelRotation_t GetVoxelRotationWhereFrontIsInDir(Dir3_t direction, u8 rotationClockwiseAmount = 0);
	VoxelRotation_t GetVoxelRotationWhereTopIsInDir(Dir3_t direction, u8 rotationClockwiseAmount = 0);
	VoxelRotation_t GetStandardVoxelRotationForSide(Dir3_t side);
	Dir3_t GetDirForVoxelRotationAndSide(VoxelRotation_t rotation, Dir3_t side, u8* clockwiseAmountOut = nullptr);
	VoxelRotation_t RotateVoxelRotationClockwise(VoxelRotation_t rotation);
	VoxelRotation_t RotateVoxelRotationClockwise(VoxelRotation_t rotation, u64 numRotations);
	VoxelRotation_t RotateVoxelRotationCounterClockwise(VoxelRotation_t rotation);
	VoxelRotation_t RotateVoxelRotationCounterClockwise(VoxelRotation_t rotation, u64 numRotations);
	VoxelRotation_t RotateVoxelRotationAroundAxis(VoxelRotation_t rotation, Dir3_t axis);
	quat GetVoxelRotationQuat(VoxelRotation_t rotation);
	VoxelRotation_t GetVoxelRotationForNormalAndUp(Dir3_t normalDir, Dir3_t upDir);
	VoxelRotation_t VoxelRotationOpposite(VoxelRotation_t rotation);
	VoxelRotation_t RotateVoxelRotation(VoxelRotation_t startRotation, VoxelRotation_t rotationAmount);
	VoxelBasis_t ToVoxelBasis(Dir3_t rightDir, Dir3_t upDir, Dir3_t forwardDir);
	VoxelBasis_t ToVoxelBasis(VoxelRotation_t rotation);
	bool IsBasisPositiveOnAxis(const VoxelBasis_t& basis, Axis_t axis);
	v3i GetBoxiMinInBasis(const VoxelBasis_t& basis, boxi boundingBox);
#else //GYLIB_HEADER_ONLY

Dir3_t GetVoxelRotationNormalDir(VoxelRotation_t rotation)
{
	if (rotation >= VoxelRotation_Forward0  && rotation <= VoxelRotation_Forward270)  { return Dir3_Forward;  }
	if (rotation >= VoxelRotation_Right0    && rotation <= VoxelRotation_Right270)    { return Dir3_Right;    }
	if (rotation >= VoxelRotation_Up0       && rotation <= VoxelRotation_Up270)       { return Dir3_Up;       }
	if (rotation >= VoxelRotation_Backward0 && rotation <= VoxelRotation_Backward270) { return Dir3_Backward; }
	if (rotation >= VoxelRotation_Left0     && rotation <= VoxelRotation_Left270)     { return Dir3_Left;     }
	if (rotation >= VoxelRotation_Down0     && rotation <= VoxelRotation_Down270)     { return Dir3_Down;     }
	return Dir3_None;
}
Dir3_t GetVoxelRotationUDir(VoxelRotation_t rotation) //aka tangent, or "right"
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return Dir3_Left;
		case VoxelRotation_Forward90:   return Dir3_Down;
		case VoxelRotation_Forward180:  return Dir3_Right;
		case VoxelRotation_Forward270:  return Dir3_Up;
		
		case VoxelRotation_Right0:      return Dir3_Forward;
		case VoxelRotation_Right90:     return Dir3_Down;
		case VoxelRotation_Right180:    return Dir3_Backward;
		case VoxelRotation_Right270:    return Dir3_Up;
		
		case VoxelRotation_Up0:         return Dir3_Right;
		case VoxelRotation_Up90:        return Dir3_Backward;
		case VoxelRotation_Up180:       return Dir3_Left;
		case VoxelRotation_Up270:       return Dir3_Forward;
		
		case VoxelRotation_Backward0:   return Dir3_Right;
		case VoxelRotation_Backward90:  return Dir3_Down;
		case VoxelRotation_Backward180: return Dir3_Left;
		case VoxelRotation_Backward270: return Dir3_Up;
		
		case VoxelRotation_Left0:       return Dir3_Backward;
		case VoxelRotation_Left90:      return Dir3_Down;
		case VoxelRotation_Left180:     return Dir3_Forward;
		case VoxelRotation_Left270:     return Dir3_Up;
		
		case VoxelRotation_Down0:       return Dir3_Right;
		case VoxelRotation_Down90:      return Dir3_Forward;
		case VoxelRotation_Down180:     return Dir3_Left;
		case VoxelRotation_Down270:     return Dir3_Backward;
		
		default: return Dir3_None;
	}
}
Dir3_t GetVoxelRotationVDir(VoxelRotation_t rotation) //aka tangent2, or "down"
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return Dir3_Down;
		case VoxelRotation_Forward90:   return Dir3_Right;
		case VoxelRotation_Forward180:  return Dir3_Up;
		case VoxelRotation_Forward270:  return Dir3_Left;
		
		case VoxelRotation_Right0:      return Dir3_Down;
		case VoxelRotation_Right90:     return Dir3_Backward;
		case VoxelRotation_Right180:    return Dir3_Up;
		case VoxelRotation_Right270:    return Dir3_Forward;
		
		case VoxelRotation_Up0:         return Dir3_Backward;
		case VoxelRotation_Up90:        return Dir3_Left;
		case VoxelRotation_Up180:       return Dir3_Forward;
		case VoxelRotation_Up270:       return Dir3_Right;
		
		case VoxelRotation_Backward0:   return Dir3_Down;
		case VoxelRotation_Backward90:  return Dir3_Left;
		case VoxelRotation_Backward180: return Dir3_Up;
		case VoxelRotation_Backward270: return Dir3_Right;
		
		case VoxelRotation_Left0:       return Dir3_Down;
		case VoxelRotation_Left90:      return Dir3_Forward;
		case VoxelRotation_Left180:     return Dir3_Up;
		case VoxelRotation_Left270:     return Dir3_Backward;
		
		case VoxelRotation_Down0:       return Dir3_Forward;
		case VoxelRotation_Down90:      return Dir3_Left;
		case VoxelRotation_Down180:     return Dir3_Backward;
		case VoxelRotation_Down270:     return Dir3_Right;
		
		default: return Dir3_None;
	}
}
//Returns num of 90 degree turns clockwise that's built into this rotation value (for example Forward0=0, Forward90=1, Forward180=2, Forward270=3)
u8 GetVoxelRotationClockwiseAmount(VoxelRotation_t rotation)
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return 0;
		case VoxelRotation_Forward90:   return 1;
		case VoxelRotation_Forward180:  return 2;
		case VoxelRotation_Forward270:  return 3;
		case VoxelRotation_Right0:      return 0;
		case VoxelRotation_Right90:     return 1;
		case VoxelRotation_Right180:    return 2;
		case VoxelRotation_Right270:    return 3;
		case VoxelRotation_Up0:         return 0;
		case VoxelRotation_Up90:        return 1;
		case VoxelRotation_Up180:       return 2;
		case VoxelRotation_Up270:       return 3;
		case VoxelRotation_Backward0:   return 0;
		case VoxelRotation_Backward90:  return 1;
		case VoxelRotation_Backward180: return 2;
		case VoxelRotation_Backward270: return 3;
		case VoxelRotation_Left0:       return 0;
		case VoxelRotation_Left90:      return 1;
		case VoxelRotation_Left180:     return 2;
		case VoxelRotation_Left270:     return 3;
		case VoxelRotation_Down0:       return 0;
		case VoxelRotation_Down90:      return 1;
		case VoxelRotation_Down180:     return 2;
		case VoxelRotation_Down270:     return 3;
		default: DebugAssert(false); return 0;
	}
}
Dir3_t RotateDirByVoxelRotation(Dir3_t direction, VoxelRotation_t rotation)
{
	switch (direction)
	{
		case Dir3_None:     return Dir3_None;
		case Dir3_Forward:  return GetVoxelRotationNormalDir(rotation);
		case Dir3_Backward: return Dir3Opposite(GetVoxelRotationNormalDir(rotation));
		case Dir3_Right:    return Dir3Opposite(GetVoxelRotationUDir(rotation));
		case Dir3_Left:     return GetVoxelRotationUDir(rotation);
		case Dir3_Up:       return Dir3Opposite(GetVoxelRotationVDir(rotation));
		case Dir3_Down:     return GetVoxelRotationVDir(rotation);
		default: DebugAssert(false); return direction;
	}
}
v3 RotateVecByVoxelRotation(v3 vector, VoxelRotation_t rotation, bool useVec3HalfAsCenter = false)
{
	v3 newPositiveAxisX = -ToVec3(GetVoxelRotationUDir(rotation));
	v3 newPositiveAxisY = -ToVec3(GetVoxelRotationVDir(rotation));
	v3 newPositiveAxisZ = ToVec3(GetVoxelRotationNormalDir(rotation));
	if (useVec3HalfAsCenter)
	{
		v3 relativeVector = vector - Vec3_Half;
		return Vec3_Half + newPositiveAxisX * relativeVector.x + newPositiveAxisY * relativeVector.y + newPositiveAxisZ * relativeVector.z;
	}
	else
	{
		return newPositiveAxisX * vector.x + newPositiveAxisY * vector.y + newPositiveAxisZ * vector.z;
	}
}
v3i RotateVeciByVoxelRotation(v3i vector, VoxelRotation_t rotation)
{
	v3i newPositiveAxisX = -ToVec3i(GetVoxelRotationUDir(rotation));
	v3i newPositiveAxisY = -ToVec3i(GetVoxelRotationVDir(rotation));
	v3i newPositiveAxisZ = ToVec3i(GetVoxelRotationNormalDir(rotation));
	return newPositiveAxisX * vector.x + newPositiveAxisY * vector.y + newPositiveAxisZ * vector.z;
}
v3i RotateSizeiByVoxelRotation(v3i size, VoxelRotation_t rotation)
{
	//TODO: I'm sure there's a faster way to do this but this is the easiest solution I could come up with
	Axis_t widthAxis  = GetDir3Axis(GetVoxelRotationUDir(rotation));
	Axis_t heightAxis = GetDir3Axis(GetVoxelRotationVDir(rotation));
	Axis_t depthAxis  = GetDir3Axis(GetVoxelRotationNormalDir(rotation));
	DebugAssert(widthAxis != heightAxis);
	DebugAssert(heightAxis != depthAxis);
	DebugAssert(depthAxis != widthAxis);
	v3i result;
	result.x = (widthAxis == Axis_X ? size.width : 0) + (heightAxis == Axis_X ? size.height : 0) + (depthAxis == Axis_X ? size.depth : 0);
	result.y = (widthAxis == Axis_Y ? size.width : 0) + (heightAxis == Axis_Y ? size.height : 0) + (depthAxis == Axis_Y ? size.depth : 0);
	result.z = (widthAxis == Axis_Z ? size.width : 0) + (heightAxis == Axis_Z ? size.height : 0) + (depthAxis == Axis_Z ? size.depth : 0);
	return result;
}

VoxelRotation_t GetVoxelRotationWhereFrontIsInDir(Dir3_t direction, u8 rotationClockwiseAmount = 0)
{
	switch (direction)
	{
		case Dir3_Forward:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Forward0;
				case 1: return VoxelRotation_Forward90;
				case 2: return VoxelRotation_Forward180;
				case 3: return VoxelRotation_Forward270;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Right:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Right0;
				case 1: return VoxelRotation_Right90;
				case 2: return VoxelRotation_Right180;
				case 3: return VoxelRotation_Right270;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Up:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Up0;
				case 1: return VoxelRotation_Up90;
				case 2: return VoxelRotation_Up180;
				case 3: return VoxelRotation_Up270;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Backward:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Backward0;
				case 1: return VoxelRotation_Backward90;
				case 2: return VoxelRotation_Backward180;
				case 3: return VoxelRotation_Backward270;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Left:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Left0;
				case 1: return VoxelRotation_Left90;
				case 2: return VoxelRotation_Left180;
				case 3: return VoxelRotation_Left270;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Down:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Down0;
				case 1: return VoxelRotation_Down90;
				case 2: return VoxelRotation_Down180;
				case 3: return VoxelRotation_Down270;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		default: DebugAssert(false); return VoxelRotation_Default;
	}
}
VoxelRotation_t GetVoxelRotationWhereTopIsInDir(Dir3_t direction, u8 rotationClockwiseAmount = 0)
{
	switch (direction)
	{
		case Dir3_Forward:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Up0;
				case 1: return VoxelRotation_Left270;
				case 2: return VoxelRotation_Down180;
				case 3: return VoxelRotation_Right90;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Right:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Up90;
				case 1: return VoxelRotation_Forward270;
				case 2: return VoxelRotation_Down90;
				case 3: return VoxelRotation_Backward90;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Up:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Forward0;
				case 1: return VoxelRotation_Right0;
				case 2: return VoxelRotation_Backward0;
				case 3: return VoxelRotation_Left0;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Backward:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Up180;
				case 1: return VoxelRotation_Right270;
				case 2: return VoxelRotation_Down0;
				case 3: return VoxelRotation_Left90;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Left:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Up270;
				case 1: return VoxelRotation_Backward270;
				case 2: return VoxelRotation_Down270;
				case 3: return VoxelRotation_Forward90;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		case Dir3_Down:
		{
			switch (rotationClockwiseAmount % 4)
			{
				case 0: return VoxelRotation_Backward180;
				case 1: return VoxelRotation_Right180;
				case 2: return VoxelRotation_Forward180;
				case 3: return VoxelRotation_Left180;
				default: DebugAssert(false); return VoxelRotation_Default;
			}
		};
		default: DebugAssert(false); return VoxelRotation_Default;
	}
}

//Standard rotations guarantee that right/forward basis vectors are in the positive direction along global axis'
VoxelRotation_t GetStandardVoxelRotationForSide(Dir3_t side)
{
	switch (side)
	{
		case Dir3_Right:    return VoxelRotation_Up90;      //GetVoxelRotationWhereTopIsInDir(Dir3_Right,    0)
		case Dir3_Left:     return VoxelRotation_Forward90; //GetVoxelRotationWhereTopIsInDir(Dir3_Left,     3)
		case Dir3_Up:       return VoxelRotation_Forward0;  //GetVoxelRotationWhereTopIsInDir(Dir3_Up,       0)
		case Dir3_Down:     return VoxelRotation_Right180;  //GetVoxelRotationWhereTopIsInDir(Dir3_Down,     1)
		case Dir3_Forward:  return VoxelRotation_Right90;   //GetVoxelRotationWhereTopIsInDir(Dir3_Forward,  3)
		case Dir3_Backward: return VoxelRotation_Up180;     //GetVoxelRotationWhereTopIsInDir(Dir3_Backward, 0)
		default: DebugAssert(false); return VoxelRotation_Default;
	}
}

Dir3_t GetDirForVoxelRotationAndSide(VoxelRotation_t rotation, Dir3_t side, u8* clockwiseAmountOut = nullptr)
{
	if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; }
	switch (rotation)
	{
		// +==============================+
		// |      Forward Rotations       |
		// +==============================+
		case VoxelRotation_Forward0:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Right;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Up;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Forward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Left;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Down;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Backward;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Forward90:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Down;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Right;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Forward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Up;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Left;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Backward;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Forward180:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Left;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Down;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Forward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Right;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Up;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Backward;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Forward270:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Up;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Left;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Forward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Down;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Right;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Backward;
				default: DebugAssert(false); return side;
			}
		}
		
		// +==============================+
		// |       Right Rotations        |
		// +==============================+
		case VoxelRotation_Right0:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Forward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Up;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Left;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Backward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Down;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Right;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Right90:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Forward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Right;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Up;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Backward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Left;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Down;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Right180:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Forward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Down;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Right;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Backward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Up;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Left;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Right270:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Forward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Left;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Down;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Backward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Right;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Up;
				default: DebugAssert(false); return side;
			}
		}
		
		// +==============================+
		// |         Up Rotations         |
		// +==============================+
		case VoxelRotation_Up0:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Left;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Forward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Up;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Right;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Backward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Down;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Up90:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Up;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Forward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Right;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Down;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Backward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Left;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Up180:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Right;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Forward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Down;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Left;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Backward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Up;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Up270:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Down;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Forward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Left;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Up;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Backward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Right;
				default: DebugAssert(false); return side;
			}
		}
		
		// +==============================+
		// |      Backward Rotations      |
		// +==============================+
		case VoxelRotation_Backward0:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Left;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Up;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Backward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Right;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Down;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Forward;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Backward90:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Up;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Right;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Backward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Down;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Left;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Forward;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Backward180:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Right;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Down;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Backward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Left;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Up;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Forward;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Backward270:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Down;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Left;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Backward;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Up;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Right;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Forward;
				default: DebugAssert(false); return side;
			}
		}
		
		// +==============================+
		// |        Left Rotations        |
		// +==============================+
		case VoxelRotation_Left0:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Backward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Up;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Right;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Forward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Down;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Left;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Left90:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Backward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Right;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Down;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Forward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Left;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Up;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Left180:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Backward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Down;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Left;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Forward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Up;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Right;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Left270:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Backward;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Left;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Up;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Forward;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Right;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Down;
				default: DebugAssert(false); return side;
			}
		}
		
		// +==============================+
		// |        Down Rotations        |
		// +==============================+
		case VoxelRotation_Down0:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Left;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Backward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Down;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Right;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Forward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Up;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Down90:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Up;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Backward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Left;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Down;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Forward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Right;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Down180:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Right;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Backward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Up;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Left;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Forward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Down;
				default: DebugAssert(false); return side;
			}
		}
		case VoxelRotation_Down270:
		{
			switch (side)
			{
				case Dir3_Right:    if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 0; } return Dir3_Down;
				case Dir3_Up:       if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Backward;
				case Dir3_Forward:  if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 1; } return Dir3_Right;
				case Dir3_Left:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 2; } return Dir3_Up;
				case Dir3_Down:     if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Forward;
				case Dir3_Backward: if (clockwiseAmountOut != nullptr) { *clockwiseAmountOut = 3; } return Dir3_Left;
				default: DebugAssert(false); return side;
			}
		}
		
		default: DebugAssert(false); return side;
	}
}

VoxelRotation_t RotateVoxelRotationClockwise(VoxelRotation_t rotation)
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return VoxelRotation_Forward90;
		case VoxelRotation_Forward90:   return VoxelRotation_Forward180;
		case VoxelRotation_Forward180:  return VoxelRotation_Forward270;
		case VoxelRotation_Forward270:  return VoxelRotation_Forward0;
		
		case VoxelRotation_Right0:      return VoxelRotation_Right90;
		case VoxelRotation_Right90:     return VoxelRotation_Right180;
		case VoxelRotation_Right180:    return VoxelRotation_Right270;
		case VoxelRotation_Right270:    return VoxelRotation_Right0;
		
		case VoxelRotation_Up0:         return VoxelRotation_Up90;
		case VoxelRotation_Up90:        return VoxelRotation_Up180;
		case VoxelRotation_Up180:       return VoxelRotation_Up270;
		case VoxelRotation_Up270:       return VoxelRotation_Up0;
		
		case VoxelRotation_Backward0:   return VoxelRotation_Backward90;
		case VoxelRotation_Backward90:  return VoxelRotation_Backward180;
		case VoxelRotation_Backward180: return VoxelRotation_Backward270;
		case VoxelRotation_Backward270: return VoxelRotation_Backward0;
		
		case VoxelRotation_Left0:       return VoxelRotation_Left90;
		case VoxelRotation_Left90:      return VoxelRotation_Left180;
		case VoxelRotation_Left180:     return VoxelRotation_Left270;
		case VoxelRotation_Left270:     return VoxelRotation_Left0;
		
		case VoxelRotation_Down0:       return VoxelRotation_Down90;
		case VoxelRotation_Down90:      return VoxelRotation_Down180;
		case VoxelRotation_Down180:     return VoxelRotation_Down270;
		case VoxelRotation_Down270:     return VoxelRotation_Down0;
		
		default: return rotation;
	}
}
VoxelRotation_t RotateVoxelRotationClockwise(VoxelRotation_t rotation, u64 numRotations)
{
	u64 numRotationsModulo = (numRotations % 4);
	VoxelRotation_t result = rotation; 
	if (numRotationsModulo >= 1) { result = RotateVoxelRotationClockwise(result); }
	if (numRotationsModulo >= 2) { result = RotateVoxelRotationClockwise(result); }
	if (numRotationsModulo >= 3) { result = RotateVoxelRotationClockwise(result); }
	return result;
}

VoxelRotation_t RotateVoxelRotationCounterClockwise(VoxelRotation_t rotation)
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return VoxelRotation_Forward270;
		case VoxelRotation_Forward90:   return VoxelRotation_Forward0;
		case VoxelRotation_Forward180:  return VoxelRotation_Forward90;
		case VoxelRotation_Forward270:  return VoxelRotation_Forward180;
		
		case VoxelRotation_Right0:      return VoxelRotation_Right270;
		case VoxelRotation_Right90:     return VoxelRotation_Right0;
		case VoxelRotation_Right180:    return VoxelRotation_Right90;
		case VoxelRotation_Right270:    return VoxelRotation_Right180;
		
		case VoxelRotation_Up0:         return VoxelRotation_Up270;
		case VoxelRotation_Up90:        return VoxelRotation_Up0;
		case VoxelRotation_Up180:       return VoxelRotation_Up90;
		case VoxelRotation_Up270:       return VoxelRotation_Up180;
		
		case VoxelRotation_Backward0:   return VoxelRotation_Backward270;
		case VoxelRotation_Backward90:  return VoxelRotation_Backward0;
		case VoxelRotation_Backward180: return VoxelRotation_Backward90;
		case VoxelRotation_Backward270: return VoxelRotation_Backward180;
		
		case VoxelRotation_Left0:       return VoxelRotation_Left270;
		case VoxelRotation_Left90:      return VoxelRotation_Left0;
		case VoxelRotation_Left180:     return VoxelRotation_Left90;
		case VoxelRotation_Left270:     return VoxelRotation_Left180;
		
		case VoxelRotation_Down0:       return VoxelRotation_Down270;
		case VoxelRotation_Down90:      return VoxelRotation_Down0;
		case VoxelRotation_Down180:     return VoxelRotation_Down90;
		case VoxelRotation_Down270:     return VoxelRotation_Down180;
		
		default: return rotation;
	}
}
VoxelRotation_t RotateVoxelRotationCounterClockwise(VoxelRotation_t rotation, u64 numRotations)
{
	u64 numRotationsModulo = (numRotations % 4);
	VoxelRotation_t result = rotation; 
	if (numRotationsModulo >= 1) { result = RotateVoxelRotationCounterClockwise(result); }
	if (numRotationsModulo >= 2) { result = RotateVoxelRotationCounterClockwise(result); }
	if (numRotationsModulo >= 3) { result = RotateVoxelRotationCounterClockwise(result); }
	return result;
}

// Rotating around the opposite of the normal vec results in a regular "clockwise" rotation
// Rotation Reference: Imagine a screw, if the rotation axis is facing down the threads (away from the head) clockwise rotation (or righty-tighty) is a positive rotation value
//  .-->[+] rotation
// /   .--.
// |  /  //\,.,.,.,.,.,.,.,.,.
// v  | // |\ \ \ \ \ \ \ \ \ > --> axis
//[-] \//  / ' ' ' ' ' ' ' ' '
//     `--'
VoxelRotation_t RotateVoxelRotationAroundAxis(VoxelRotation_t rotation, Dir3_t axis)
{
	switch (rotation)
	{
		// +==============================+
		// |      Forward Rotations       |
		// +==============================+
		case VoxelRotation_Forward0:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Up180;
				case Dir3_Left:     return VoxelRotation_Down180;
				case Dir3_Forward:  return VoxelRotation_Forward270;
				case Dir3_Backward: return VoxelRotation_Forward90;
				case Dir3_Up:       return VoxelRotation_Left0;
				case Dir3_Down:     return VoxelRotation_Right0;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Forward90:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Up270;
				case Dir3_Left:     return VoxelRotation_Down270;
				case Dir3_Forward:  return VoxelRotation_Forward0;
				case Dir3_Backward: return VoxelRotation_Forward180;
				case Dir3_Up:       return VoxelRotation_Left90;
				case Dir3_Down:     return VoxelRotation_Right90;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Forward180:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Up0;
				case Dir3_Left:     return VoxelRotation_Down0;
				case Dir3_Forward:  return VoxelRotation_Forward90;
				case Dir3_Backward: return VoxelRotation_Forward270;
				case Dir3_Up:       return VoxelRotation_Left180;
				case Dir3_Down:     return VoxelRotation_Right180;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Forward270:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Up90;
				case Dir3_Left:     return VoxelRotation_Down90;
				case Dir3_Forward:  return VoxelRotation_Forward180;
				case Dir3_Backward: return VoxelRotation_Forward0;
				case Dir3_Up:       return VoxelRotation_Left270;
				case Dir3_Down:     return VoxelRotation_Right270;
				default: DebugAssert(false); return rotation;
			}
		}
		
		// +==============================+
		// |       Right Rotations        |
		// +==============================+
		case VoxelRotation_Right0:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Right270;
				case Dir3_Left:     return VoxelRotation_Right90;
				case Dir3_Forward:  return VoxelRotation_Down90;
				case Dir3_Backward: return VoxelRotation_Up270;
				case Dir3_Up:       return VoxelRotation_Forward0;
				case Dir3_Down:     return VoxelRotation_Backward0;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Right90:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Right0;
				case Dir3_Left:     return VoxelRotation_Right180;
				case Dir3_Forward:  return VoxelRotation_Down180;
				case Dir3_Backward: return VoxelRotation_Up0;
				case Dir3_Up:       return VoxelRotation_Forward90;
				case Dir3_Down:     return VoxelRotation_Backward90;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Right180:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Right90;
				case Dir3_Left:     return VoxelRotation_Right270;
				case Dir3_Forward:  return VoxelRotation_Down270;
				case Dir3_Backward: return VoxelRotation_Up90;
				case Dir3_Up:       return VoxelRotation_Forward180;
				case Dir3_Down:     return VoxelRotation_Backward180;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Right270:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Right180;
				case Dir3_Left:     return VoxelRotation_Right0;
				case Dir3_Forward:  return VoxelRotation_Down0;
				case Dir3_Backward: return VoxelRotation_Up180;
				case Dir3_Up:       return VoxelRotation_Forward270;
				case Dir3_Down:     return VoxelRotation_Backward270;
				default: DebugAssert(false); return rotation;
			}
		}
		
		// +==============================+
		// |         Up Rotations         |
		// +==============================+
		case VoxelRotation_Up0:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Backward0;
				case Dir3_Left:     return VoxelRotation_Forward180;
				case Dir3_Forward:  return VoxelRotation_Right90;
				case Dir3_Backward: return VoxelRotation_Left270;
				case Dir3_Up:       return VoxelRotation_Up270;
				case Dir3_Down:     return VoxelRotation_Up90;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Up90:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Backward90;
				case Dir3_Left:     return VoxelRotation_Forward270;
				case Dir3_Forward:  return VoxelRotation_Right180;
				case Dir3_Backward: return VoxelRotation_Left0;
				case Dir3_Up:       return VoxelRotation_Up0;
				case Dir3_Down:     return VoxelRotation_Up180;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Up180:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Backward180;
				case Dir3_Left:     return VoxelRotation_Forward0;
				case Dir3_Forward:  return VoxelRotation_Right270;
				case Dir3_Backward: return VoxelRotation_Left90;
				case Dir3_Up:       return VoxelRotation_Up90;
				case Dir3_Down:     return VoxelRotation_Up270;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Up270:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Backward270;
				case Dir3_Left:     return VoxelRotation_Forward90;
				case Dir3_Forward:  return VoxelRotation_Right0;
				case Dir3_Backward: return VoxelRotation_Left180;
				case Dir3_Up:       return VoxelRotation_Up180;
				case Dir3_Down:     return VoxelRotation_Up0;
				default: DebugAssert(false); return rotation;
			}
		}
		
		// +==============================+
		// |      Backward Rotations      |
		// +==============================+
		case VoxelRotation_Backward0:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Down0;
				case Dir3_Left:     return VoxelRotation_Up0;
				case Dir3_Forward:  return VoxelRotation_Backward90;
				case Dir3_Backward: return VoxelRotation_Backward270;
				case Dir3_Up:       return VoxelRotation_Right0;
				case Dir3_Down:     return VoxelRotation_Left0;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Backward90:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Down90;
				case Dir3_Left:     return VoxelRotation_Up90;
				case Dir3_Forward:  return VoxelRotation_Backward180;
				case Dir3_Backward: return VoxelRotation_Backward0;
				case Dir3_Up:       return VoxelRotation_Right90;
				case Dir3_Down:     return VoxelRotation_Left90;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Backward180:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Down180;
				case Dir3_Left:     return VoxelRotation_Up180;
				case Dir3_Forward:  return VoxelRotation_Backward270;
				case Dir3_Backward: return VoxelRotation_Backward90;
				case Dir3_Up:       return VoxelRotation_Right180;
				case Dir3_Down:     return VoxelRotation_Left180;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Backward270:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Down270;
				case Dir3_Left:     return VoxelRotation_Up270;
				case Dir3_Forward:  return VoxelRotation_Backward0;
				case Dir3_Backward: return VoxelRotation_Backward180;
				case Dir3_Up:       return VoxelRotation_Right270;
				case Dir3_Down:     return VoxelRotation_Left270;
				default: DebugAssert(false); return rotation;
			}
		}
		
		// +==============================+
		// |        Left Rotations        |
		// +==============================+
		case VoxelRotation_Left0:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Left90;
				case Dir3_Left:     return VoxelRotation_Left270;
				case Dir3_Forward:  return VoxelRotation_Up90;
				case Dir3_Backward: return VoxelRotation_Down270;
				case Dir3_Up:       return VoxelRotation_Backward0;
				case Dir3_Down:     return VoxelRotation_Forward0;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Left90:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Left180;
				case Dir3_Left:     return VoxelRotation_Left0;
				case Dir3_Forward:  return VoxelRotation_Up180;
				case Dir3_Backward: return VoxelRotation_Down0;
				case Dir3_Up:       return VoxelRotation_Backward90;
				case Dir3_Down:     return VoxelRotation_Forward90;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Left180:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Left270;
				case Dir3_Left:     return VoxelRotation_Left90;
				case Dir3_Forward:  return VoxelRotation_Up270;
				case Dir3_Backward: return VoxelRotation_Down90;
				case Dir3_Up:       return VoxelRotation_Backward180;
				case Dir3_Down:     return VoxelRotation_Forward180;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Left270:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Left0;
				case Dir3_Left:     return VoxelRotation_Left180;
				case Dir3_Forward:  return VoxelRotation_Up0;
				case Dir3_Backward: return VoxelRotation_Down180;
				case Dir3_Up:       return VoxelRotation_Backward270;
				case Dir3_Down:     return VoxelRotation_Forward270;
				default: DebugAssert(false); return rotation;
			}
		}
		
		// +==============================+
		// |        Down Rotations        |
		// +==============================+
		case VoxelRotation_Down0:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Forward180;
				case Dir3_Left:     return VoxelRotation_Backward0;
				case Dir3_Forward:  return VoxelRotation_Left90;
				case Dir3_Backward: return VoxelRotation_Right270;
				case Dir3_Up:       return VoxelRotation_Down90;
				case Dir3_Down:     return VoxelRotation_Down270;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Down90:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Forward270;
				case Dir3_Left:     return VoxelRotation_Backward90;
				case Dir3_Forward:  return VoxelRotation_Left180;
				case Dir3_Backward: return VoxelRotation_Right0;
				case Dir3_Up:       return VoxelRotation_Down180;
				case Dir3_Down:     return VoxelRotation_Down0;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Down180:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Forward0;
				case Dir3_Left:     return VoxelRotation_Backward180;
				case Dir3_Forward:  return VoxelRotation_Left270;
				case Dir3_Backward: return VoxelRotation_Right90;
				case Dir3_Up:       return VoxelRotation_Down270;
				case Dir3_Down:     return VoxelRotation_Down90;
				default: DebugAssert(false); return rotation;
			}
		}
		case VoxelRotation_Down270:
		{
			switch (axis)
			{
				case Dir3_Right:    return VoxelRotation_Forward90;
				case Dir3_Left:     return VoxelRotation_Backward270;
				case Dir3_Forward:  return VoxelRotation_Left0;
				case Dir3_Backward: return VoxelRotation_Right180;
				case Dir3_Up:       return VoxelRotation_Down0;
				case Dir3_Down:     return VoxelRotation_Down180;
				default: DebugAssert(false); return rotation;
			}
		}
		
		default: DebugAssert(false); return rotation;
	}
}

quat GetVoxelRotationQuat(VoxelRotation_t rotation)
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return NewQuat(Vec3_Forward, ToRadians32(0)/2);
		case VoxelRotation_Forward90:   return NewQuat(Vec3_Forward, ToRadians32(90)/2);
		case VoxelRotation_Forward180:  return NewQuat(Vec3_Forward, ToRadians32(180)/2);
		case VoxelRotation_Forward270:  return NewQuat(Vec3_Forward, ToRadians32(270)/2);
		
		case VoxelRotation_Right0:      return QuatMult(NewQuat(Vec3_Right,    ToRadians32(0)/2),   NewQuat(Vec3_Up, ToRadians32(90)/2));
		case VoxelRotation_Right90:     return QuatMult(NewQuat(Vec3_Right,    ToRadians32(90)/2),  NewQuat(Vec3_Up, ToRadians32(90)/2));
		case VoxelRotation_Right180:    return QuatMult(NewQuat(Vec3_Right,    ToRadians32(180)/2), NewQuat(Vec3_Up, ToRadians32(90)/2));
		case VoxelRotation_Right270:    return QuatMult(NewQuat(Vec3_Right,    ToRadians32(270)/2), NewQuat(Vec3_Up, ToRadians32(90)/2));
		
		case VoxelRotation_Up0:         return QuatMult(NewQuat(Vec3_Up,       ToRadians32(180)/2), NewQuat(Vec3_Left, ToRadians32(90)/2));
		case VoxelRotation_Up90:        return QuatMult(NewQuat(Vec3_Up,       ToRadians32(270)/2), NewQuat(Vec3_Left, ToRadians32(90)/2));
		case VoxelRotation_Up180:       return QuatMult(NewQuat(Vec3_Up,       ToRadians32(0)/2),   NewQuat(Vec3_Left, ToRadians32(90)/2));
		case VoxelRotation_Up270:       return QuatMult(NewQuat(Vec3_Up,       ToRadians32(90)/2),  NewQuat(Vec3_Left, ToRadians32(90)/2));
		
		case VoxelRotation_Backward0:   return QuatMult(NewQuat(Vec3_Backward, ToRadians32(0)/2),   NewQuat(Vec3_Up, ToRadians32(180)/2));
		case VoxelRotation_Backward90:  return QuatMult(NewQuat(Vec3_Backward, ToRadians32(90)/2),  NewQuat(Vec3_Up, ToRadians32(180)/2));
		case VoxelRotation_Backward180: return QuatMult(NewQuat(Vec3_Backward, ToRadians32(180)/2), NewQuat(Vec3_Up, ToRadians32(180)/2));
		case VoxelRotation_Backward270: return QuatMult(NewQuat(Vec3_Backward, ToRadians32(270)/2), NewQuat(Vec3_Up, ToRadians32(180)/2));
		
		case VoxelRotation_Left0:       return QuatMult(NewQuat(Vec3_Left,     ToRadians32(0)/2),   NewQuat(Vec3_Up, ToRadians32(270)/2));
		case VoxelRotation_Left90:      return QuatMult(NewQuat(Vec3_Left,     ToRadians32(90)/2),  NewQuat(Vec3_Up, ToRadians32(270)/2));
		case VoxelRotation_Left180:     return QuatMult(NewQuat(Vec3_Left,     ToRadians32(180)/2), NewQuat(Vec3_Up, ToRadians32(270)/2));
		case VoxelRotation_Left270:     return QuatMult(NewQuat(Vec3_Left,     ToRadians32(270)/2), NewQuat(Vec3_Up, ToRadians32(270)/2));
		
		case VoxelRotation_Down0:       return QuatMult(NewQuat(Vec3_Down,     ToRadians32(180)/2), NewQuat(Vec3_Right, ToRadians32(90)/2));
		case VoxelRotation_Down90:      return QuatMult(NewQuat(Vec3_Down,     ToRadians32(270)/2), NewQuat(Vec3_Right, ToRadians32(90)/2));
		case VoxelRotation_Down180:     return QuatMult(NewQuat(Vec3_Down,     ToRadians32(0)/2),   NewQuat(Vec3_Right, ToRadians32(90)/2));
		case VoxelRotation_Down270:     return QuatMult(NewQuat(Vec3_Down,     ToRadians32(90)/2),  NewQuat(Vec3_Right, ToRadians32(90)/2));
		
		default: return Quat_Identity;
	}
}

VoxelRotation_t GetVoxelRotationForNormalAndUp(Dir3_t normalDir, Dir3_t upDir)
{
	for (u64 rIndex = 0; rIndex < VoxelRotation_NumRotations; rIndex++)
	{
		VoxelRotation_t rotation = (VoxelRotation_t)rIndex;
		if (GetVoxelRotationNormalDir(rotation) == normalDir &&
			GetVoxelRotationVDir(rotation) == Dir3Opposite(upDir))
		{
			return rotation;
		}
	}
	DebugAssertMsg(false, "Invalid normalDir + upDir combination passed to GetVoxelRotationForNormalAndUp. The normalDir and upDir are probably in the same or opposite directions, not perpendicular");
	return VoxelRotation_Default;
}

VoxelRotation_t VoxelRotationOpposite(VoxelRotation_t rotation)
{
	switch (rotation)
	{
		case VoxelRotation_Forward0:    return VoxelRotation_Forward0;
		case VoxelRotation_Forward90:   return VoxelRotation_Forward270;
		case VoxelRotation_Forward180:  return VoxelRotation_Forward180;
		case VoxelRotation_Forward270:  return VoxelRotation_Forward90;
		case VoxelRotation_Right0:      return VoxelRotation_Left0;
		case VoxelRotation_Right90:     return VoxelRotation_Up90;
		case VoxelRotation_Right180:    return VoxelRotation_Right180;
		case VoxelRotation_Right270:    return VoxelRotation_Down270;
		case VoxelRotation_Up0:         return VoxelRotation_Up0;
		case VoxelRotation_Up90:        return VoxelRotation_Right90;
		case VoxelRotation_Up180:       return VoxelRotation_Down180;
		case VoxelRotation_Up270:       return VoxelRotation_Left270;
		case VoxelRotation_Backward0:   return VoxelRotation_Backward0;
		case VoxelRotation_Backward90:  return VoxelRotation_Backward90;
		case VoxelRotation_Backward180: return VoxelRotation_Backward180;
		case VoxelRotation_Backward270: return VoxelRotation_Backward270;
		case VoxelRotation_Left0:       return VoxelRotation_Right0;
		case VoxelRotation_Left90:      return VoxelRotation_Down90;
		case VoxelRotation_Left180:     return VoxelRotation_Left180;
		case VoxelRotation_Left270:     return VoxelRotation_Up270;
		case VoxelRotation_Down0:       return VoxelRotation_Down0;
		case VoxelRotation_Down90:      return VoxelRotation_Left90;
		case VoxelRotation_Down180:     return VoxelRotation_Up180;
		case VoxelRotation_Down270:     return VoxelRotation_Right270;
		default: DebugAssert(false); return VoxelRotation_Default;
	}
}

//This should produce the same result as multiplying the rotationAmount transformation matrix onto the startRotation transformation matrix
VoxelRotation_t RotateVoxelRotation(VoxelRotation_t startRotation, VoxelRotation_t rotationAmount)
{
	//TODO: This could probably be a switch statement/lookup table but this is conceptually fine I think
	Dir3_t normalDir = GetVoxelRotationNormalDir(startRotation);
	Dir3_t upDir = Dir3Opposite(GetVoxelRotationVDir(startRotation));
	normalDir = RotateDirByVoxelRotation(normalDir, rotationAmount);
	upDir = RotateDirByVoxelRotation(upDir, rotationAmount);
	return GetVoxelRotationForNormalAndUp(normalDir, upDir);
}

VoxelBasis_t ToVoxelBasis(Dir3_t rightDir, Dir3_t upDir, Dir3_t forwardDir)
{
	VoxelBasis_t result = {};
	result.rightDir = rightDir;
	result.upDir = upDir;
	result.forwardDir = forwardDir;
	result.righti = ToVec3i(result.rightDir);
	result.upi = ToVec3i(result.upDir);
	result.forwardi = ToVec3i(result.forwardDir);
	result.right = ToVec3(result.rightDir);
	result.up = ToVec3(result.upDir);
	result.forward = ToVec3(result.forwardDir);
	result.rotation = GetVoxelRotationForNormalAndUp(result.forwardDir, result.upDir);
	result.rotationQuat = GetVoxelRotationQuat(result.rotation);
	result.isNegativeSide = (result.upDir == Dir3_Left || result.upDir == Dir3_Down || result.upDir == Dir3_Backward);
	return result;
}
VoxelBasis_t ToVoxelBasis(VoxelRotation_t rotation)
{
	VoxelBasis_t result = {};
	result.rightDir = Dir3Opposite(GetVoxelRotationUDir(rotation));
	result.upDir = Dir3Opposite(GetVoxelRotationVDir(rotation));
	result.forwardDir = GetVoxelRotationNormalDir(rotation);
	result.righti = ToVec3i(result.rightDir);
	result.upi = ToVec3i(result.upDir);
	result.forwardi = ToVec3i(result.forwardDir);
	result.right = ToVec3(result.rightDir);
	result.up = ToVec3(result.upDir);
	result.forward = ToVec3(result.forwardDir);
	result.rotation = rotation;
	result.rotationQuat = GetVoxelRotationQuat(result.rotation);
	result.isNegativeSide = (result.upDir == Dir3_Left || result.upDir == Dir3_Down || result.upDir == Dir3_Backward);
	return result;
}

bool IsBasisPositiveOnAxis(const VoxelBasis_t& basis, Axis_t axis)
{
	Dir3_t positiveDir = ToDir3(axis);
	return (basis.rightDir == positiveDir || basis.upDir == positiveDir || basis.forwardDir == positiveDir);
}
v3i GetBoxiMinInBasis(const VoxelBasis_t& basis, boxi boundingBox)
{
	return NewVec3i(
		IsBasisPositiveOnAxis(basis, Axis_X) ? boundingBox.x : (boundingBox.x + boundingBox.width),
		IsBasisPositiveOnAxis(basis, Axis_Y) ? boundingBox.y : (boundingBox.y + boundingBox.height),
		IsBasisPositiveOnAxis(basis, Axis_Z) ? boundingBox.z : (boundingBox.z + boundingBox.depth)
	);
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_VOXEL_ROTATION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
VoxelRotation_Forward0
VoxelRotation_Forward90
VoxelRotation_Forward180
VoxelRotation_Forward270
VoxelRotation_Right0
VoxelRotation_Right90
VoxelRotation_Right180
VoxelRotation_Right270
VoxelRotation_Up0
VoxelRotation_Up90
VoxelRotation_Up180
VoxelRotation_Up270
VoxelRotation_Backward0
VoxelRotation_Backward90
VoxelRotation_Backward180
VoxelRotation_Backward270
VoxelRotation_Left0
VoxelRotation_Left90
VoxelRotation_Left180
VoxelRotation_Left270
VoxelRotation_Down0
VoxelRotation_Down90
VoxelRotation_Down180
VoxelRotation_Down270
VoxelRotation_NumRotations
VoxelRotation_Default
@Types
VoxelRotation_t
VoxelBasis_t
@Functions
#define VoxelRotationMult(startRotation, rotationAmount)
Dir3_t GetVoxelRotationNormalDir(VoxelRotation_t rotation)
Dir3_t GetVoxelRotationUDir(VoxelRotation_t rotation)
Dir3_t GetVoxelRotationVDir(VoxelRotation_t rotation)
u8 GetVoxelRotationClockwiseAmount(VoxelRotation_t rotation)
Dir3_t RotateDirByVoxelRotation(Dir3_t direction, VoxelRotation_t rotation)
v3 RotateVecByVoxelRotation(v3 vector, VoxelRotation_t rotation, bool useVec3HalfAsCenter = false)
v3i RotateVeciByVoxelRotation(v3i vector, VoxelRotation_t rotation)
v3i RotateSizeiByVoxelRotation(v3i size, VoxelRotation_t rotation)
VoxelRotation_t GetVoxelRotationWhereFrontIsInDir(Dir3_t direction, u8 rotationClockwiseAmount = 0)
VoxelRotation_t GetVoxelRotationWhereTopIsInDir(Dir3_t direction, u8 rotationClockwiseAmount = 0)
VoxelRotation_t GetStandardVoxelRotationForSide(Dir3_t side)
Dir3_t GetDirForVoxelRotationAndSide(VoxelRotation_t rotation, Dir3_t side, u8* clockwiseAmountOut = nullptr)
VoxelRotation_t RotateVoxelRotationClockwise(VoxelRotation_t rotation, u64 numRotations = 1)
VoxelRotation_t RotateVoxelRotationCounterClockwise(VoxelRotation_t rotation, u64 numRotations = 1)
VoxelRotation_t RotateVoxelRotationAroundAxis(VoxelRotation_t rotation, Dir3_t axis)
quat GetVoxelRotationQuat(VoxelRotation_t rotation)
VoxelRotation_t GetVoxelRotationForNormalAndUp(Dir3_t normalDir, Dir3_t upDir)
VoxelRotation_t VoxelRotationOpposite(VoxelRotation_t rotation)
VoxelRotation_t RotateVoxelRotation(VoxelRotation_t startRotation, VoxelRotation_t rotationAmount)
VoxelBasis_t ToVoxelBasis(Dir3_t rightDir, Dir3_t upDir, Dir3_t forwardDir)
bool IsBasisPositiveOnAxis(const VoxelBasis_t& basis, Axis_t axis)
v3i GetBoxiMinInBasis(const VoxelBasis_t& basis, boxi boundingBox)
*/
