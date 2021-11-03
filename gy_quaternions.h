/*
File:   gy_quaternions.h
Author: Taylor Robbins
Date:   11\02\2021
Description:
	** Holds the Quaternion_t (quat) structure that specifies a rotation in 3D space
*/

#ifndef _GY_QUATERNIONS_H
#define _GY_QUATERNIONS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"
#include "gy_matrices.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
union Quaternion_t
{
	r32 values[4];
	v4 vec4;
	struct
	{
		v3 axis;
		r32 angle;
	};
	struct
	{
		r32 x, y, z, w;
	};
};

typedef Quaternion_t quat;

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
quat QuatNormalize(const quat& quaternion)
{
	//TODO: Is this actually what needs to happen?
	quat result = {};
	result.vec4 = Vec4Normalize(quaternion.vec4);
	return result;
}
quat NewQuat(v3 axis, r32 angle, bool normalize = true)
{
	quat result = {};
	result.axis = Vec3Normalize(axis) * SinR32(angle) / 2;
	result.angle = CosR32(angle) / 2;
	if (normalize) { result = QuatNormalize(result); }
	return result;
}
quat NewQuat(v4 vec4, bool normalize = true) //NOTE: This is not the same as assigning the vec4 member directly!
{
	quat result = {};
	result.vec4 = vec4;
	if (normalize) { result = QuatNormalize(result); }
	return result;
}
quat NewQuat(r32 x, r32 y, r32 z, r32 w, bool normalize = true)
{
	quat result = {};
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	if (normalize) { result = QuatNormalize(result); }
	return result;
}

// +--------------------------------------------------------------+
// |                   Simple Value Definitions                   |
// +--------------------------------------------------------------+
#define Quat_Identity NewQuat(0, 0, 0, 1, false)

// +--------------------------------------------------------------+
// |                    Basic Math Operations                     |
// +--------------------------------------------------------------+
r32 QuatGetAngle(const quat& quaternion)
{
	//TODO: Implement me better!
	return quaternion.angle;
}
v3 QuatGetAxis(const quat& quaternion)
{
	v3 result = quaternion.axis;
	r32 resultLength = Vec3Length(result);
	if (resultLength == 0) { return Vec3_Right; }
	else { return result / resultLength; }
}

quat QuatLerp(const quat& start, const quat& end, r32 amount, bool linearly = true, bool normalizeResult = true)
{
	quat result;
	if (linearly)
	{
		result = NewQuat(Vec4Lerp(start.vec4, end.vec4, amount));
	}
	else
	{
		result = start; //TODO: Implement this variant!
	}
	if (normalizeResult) { result = QuatNormalize(result); }
	return result;
}

quat QuatMult(const quat& left, const quat& right, bool normalize = true)
{
	quat result;
	result.angle = left.angle*right.angle - Vec3Dot(left.axis, right.axis);
	result.axis = left.angle*right.axis + right.angle*left.axis + Vec3Cross(left.axis, right.axis);
	if (normalize) { result = QuatNormalize(result); }
	return result;
}

// +--------------------------------------------------------------+
// |                       Matrix Functions                       |
// +--------------------------------------------------------------+
//NOTE: Quaternion should be pre-normalized
mat4 Mat4Quaternion(quat q)
{
	return NewMat4(
		1 - 2*q.y*q.y - 2*q.z*q.z,   2*q.x*q.y - 2*q.z*q.w,     2*q.x*q.z + 2*q.y*q.w,   0,
		  2*q.x*q.y + 2*q.z*q.w,   1 - 2*q.x*q.x - 2*q.z*q.z,   2*q.y*q.z - 2*q.x*q.w,   0,
		  2*q.x*q.z - 2*q.y*q.w,     2*q.y*q.z + 2*q.x*q.w,   1 - 2*q.x*q.x - 2*q.y*q.y, 0,
		            0,                         0,                         0,             1
	);
}

quat QuatLocalRot(quat quaternion, v3 axis, r32 angle)
{
	quat deltaQuat = NewQuat(axis, angle);
	return QuatMult(quaternion, deltaQuat);
}

quat QuatGlobalRot(quat quaternion, v3 axis, r32 angle)
{
	v3 localAxis = Mat4MultiplyRightVec3(axis, Mat4Quaternion(quaternion));
	quat deltaQuat = NewQuat(localAxis, angle);
	return QuatMult(quaternion, deltaQuat);
}

#endif //  _GY_QUATERNIONS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Quat_Identity
@Types
Quaternion_t
quat
@Functions
quat QuatNormalize(const quat& quaternion)
quat NewQuat(r32 x, r32 y, r32 z, r32 w, bool normalize = true)
r32 QuatGetAngle(const quat& q)
v3 QuatGetAxis(const quat& q)
quat QuatLerp(const quat& start, const quat& end, r32 amount, bool linearly = true, bool normalizeResult = true)
quat QuatMult(const quat& left, const quat& right, bool normalize = true)
mat4 Mat4Quaternion(quat q)
quat QuatLocalRot(quat q, v3 axis, r32 angle)
quat QuatGlobalRot(quat q, v3 axis, r32 angle)
*/
