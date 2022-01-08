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
#include "gy_rectangles.h"
#include "gy_rays.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
union PrimitiveVert3D_t
{
	v3 position;
	struct { r32 x, y, z; };
};
struct PrimitiveIndex3D_t
{
	u64 index;
	u64 faceIndex;
	union
	{
		v3 normal;
		struct { r32 nX, nY, nZ; };
	};
	union
	{
		v2 texCoord;
		struct { r32 tX, tY, tZ; };
	};
};
struct PrimitiveIndexedVerts_t
{
	MemArena_t* allocArena;
	u64 numVertices;
	PrimitiveVert3D_t* vertices;
	u64 numIndices;
	PrimitiveIndex3D_t* indices;
	u64 numFaces;
};

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

struct Plane_t
{
	union
	{
		v3 normal;
		struct { r32 nX, nY, nZ; };
	};
	r32 distance;
};

union Triangle2D_t
{
	v2 vertices[3];
	r32 valuesR32[3*2];
	struct { v2 vert0, vert1, vert2; };
};

union Triangle3D_t
{
	v3 vertices[3];
	r32 valuesR32[3*3];
	struct { v3 vert0, vert1, vert2; };
};

union Simplex_t
{
	v3 vertices[4];
	r32 valuesR32[4*3];
	struct { v3 vert0, vert1, vert2, vert3; };
};

struct Circle_t
{
	union
	{
		v2 center;
		struct { r32 x, y; };
	};
	r32 radius;
};

struct Sphere_t
{
	union
	{
		v3 center;
		struct { r32 x, y, z; };
	};
	r32 radius;
};

//NOTE: All of the following primitives support translation information only. Rotation information should be encoded alongside these if needed
struct Cylinder_t
{
	union
	{
		v3 base;
		struct { r32 x, y, z; };
	};
	r32 radius;
	r32 height;
};

struct Cone_t
{
	union
	{
		v3 base;
		struct { r32 x, y, z; };
	};
	r32 height;
	r32 radius;
};

struct Pyramid_t
{
	union
	{
		v3 base;
		struct { r32 x, y, z; };
	};
	r32 height;
	union
	{
		v2 baseSize;
		struct { r32 width, depth; };
	};
};

union Wedge_t
{
	box bounds;
	struct
	{
		union
		{
			v3 bottomLeft;
			struct { r32 x, y, z; };
			struct { r32 left, bottom, back; };
		};
		union
		{
			v3 size;
			struct { r32 width, height, depth; };
		};
	};
};

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
PrimitiveIndex3D_t NewPrimitiveIndex3D(u64 index, u64 faceIndex, v3 normal, v2 texCoord)
{
	PrimitiveIndex3D_t result;
	result.index = index;
	result.faceIndex = faceIndex;
	result.normal = normal;
	result.texCoord = texCoord;
	return result;
}
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
Plane_t NewPlane(v3 normal, r32 distance)
{
	Plane_t result;
	result.normal = normal;
	result.distance = distance;
	return result;
}
Plane_t NewPlane(r32 nX, r32 nY, r32 nZ, r32 distance)
{
	Plane_t result;
	result.nX = nX;
	result.nY = nY;
	result.nZ = nZ;
	result.distance = distance;
	return result;
}
Triangle2D_t NewTriangle2D(v2 vert0, v2 vert1, v2 vert2)
{
	Triangle2D_t result;
	result.vert0 = vert0;
	result.vert1 = vert1;
	result.vert2 = vert2;
	return result;
}
Triangle3D_t NewTriangle3D(v3 vert0, v3 vert1, v3 vert2)
{
	Triangle3D_t result;
	result.vert0 = vert0;
	result.vert1 = vert1;
	result.vert2 = vert2;
	return result;
}
Simplex_t NewSimplex(v3 vert0, v3 vert1, v3 vert2, v3 vert3)
{
	Simplex_t result;
	result.vert0 = vert0;
	result.vert1 = vert1;
	result.vert2 = vert2;
	result.vert3 = vert3;
	return result;
}
Circle_t NewCircle(v2 center, r32 radius)
{
	Circle_t result;
	result.center = center;
	result.radius = radius;
	return result;
}
Circle_t NewCircle(r32 x, r32 y, r32 radius)
{
	Circle_t result;
	result.x = x;
	result.y = y;
	result.radius = radius;
	return result;
}
Sphere_t NewSphere(v3 center, r32 radius)
{
	Sphere_t result;
	result.center = center;
	result.radius = radius;
	return result;
}
Sphere_t NewSphere(r32 x, r32 y, r32 z, r32 radius)
{
	Sphere_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.radius = radius;
	return result;
}
Cylinder_t NewCylinder(v3 base, r32 height, r32 radius)
{
	Cylinder_t result;
	result.base = base;
	result.height = height;
	result.radius = radius;
	return result;
}
Cylinder_t NewCylinder(r32 x, r32 y, r32 z, r32 height, r32 radius)
{
	Cylinder_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.radius = radius;
	return result;
}
Cone_t NewCone(v3 base, r32 height, r32 radius)
{
	Cone_t result;
	result.base = base;
	result.height = height;
	result.radius = radius;
	return result;
}
Cone_t NewCone(r32 x, r32 y, r32 z, r32 height, r32 radius)
{
	Cone_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.radius = radius;
	return result;
}
Pyramid_t NewPyramid(v3 base, r32 height, v2 baseSize)
{
	Pyramid_t result;
	result.base = base;
	result.height = height;
	result.baseSize = baseSize;
	return result;
}
Pyramid_t NewPyramid(r32 x, r32 y, r32 z, r32 height, v2 baseSize)
{
	Pyramid_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.baseSize = baseSize;
	return result;
}
Pyramid_t NewPyramid(r32 x, r32 y, r32 z, r32 height, r32 baseWidth, r32 baseDepth)
{
	Pyramid_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.width = baseWidth;
	result.depth = baseDepth;
	return result;
}
Pyramid_t NewPyramid(v3 base, r32 height, r32 baseWidth, r32 baseDepth)
{
	Pyramid_t result;
	result.base = base;
	result.height = height;
	result.width = baseWidth;
	result.depth = baseDepth;
	return result;
}
Wedge_t NewWedge(box bounds)
{
	Wedge_t result;
	result.bounds = bounds;
	return result;
}
Wedge_t NewWedge(v3 bottomLeft, v3 size)
{
	Wedge_t result;
	result.bottomLeft = bottomLeft;
	result.size = size;
	return result;
}
Wedge_t NewWedge(v3 bottomLeft, r32 width, r32 height, r32 depth)
{
	Wedge_t result;
	result.bottomLeft = bottomLeft;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
Wedge_t NewWedge(r32 x, r32 y, r32 z, v3 size)
{
	Wedge_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.size = size;
	return result;
}
Wedge_t NewWedge(r32 x, r32 y, r32 z, r32 width, r32 height, r32 depth)
{
	Wedge_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}

// +--------------------------------------------------------------+
// |                 Vertex Generating Functions                  |
// +--------------------------------------------------------------+
void FreePrimitiveIndexedVerts(PrimitiveIndexedVerts_t* indexedVerts)
{
	NotNull(indexedVerts);
	if (indexedVerts->vertices != nullptr)
	{
		NotNull(indexedVerts->allocArena);
		Assert(indexedVerts->numVertices > 0);
		FreeMem(indexedVerts->allocArena, indexedVerts->vertices, sizeof(PrimitiveVert3D_t) * indexedVerts->numVertices);
	}
	if (indexedVerts->indices != nullptr)
	{
		NotNull(indexedVerts->allocArena);
		Assert(indexedVerts->numIndices > 0);
		FreeMem(indexedVerts->allocArena, indexedVerts->indices, sizeof(u64) * indexedVerts->numIndices);
	}
	ClearPointer(indexedVerts);
}

//NOTE: Passing nullptr for memArena in these functions will fill out the result struct with numVertices, numIndices, and numFaces but nothing else

PrimitiveIndexedVerts_t GenerateVertsForBox(Box_t boundingBox, MemArena_t* memArena)
{
	PrimitiveIndexedVerts_t result = {};
	result.numVertices = 8;
	result.numIndices = (6*2*3); //6 faces, 2 triangles/face, 3 indices/triangle = 36 indices
	result.numFaces = 6;
	if (memArena == nullptr) { return result; }
	
	result.allocArena = memArena;
	result.vertices = AllocArray(memArena, PrimitiveVert3D_t, result.numVertices);
	NotNull(result.vertices);
	result.indices = AllocArray(memArena, PrimitiveIndex3D_t, result.numIndices);
	NotNull(result.indices);
	
	const u64 blbIndex = 0; //bottomLeftBack
	const u64 brbIndex = 1; //bottomRightBack
	const u64 blfIndex = 2; //bottomLeftFront
	const u64 brfIndex = 3; //bottomRightFront
	const u64 tlbIndex = 4; //topLeftBack
	const u64 trbIndex = 5; //topRightBack
	const u64 tlfIndex = 6; //topLeftFront
	const u64 trfIndex = 7; //topRightFront
	result.vertices[blbIndex].position = NewVec3(boundingBox.x,                     boundingBox.y,                      boundingBox.z);
	result.vertices[brbIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y,                      boundingBox.z);
	result.vertices[blfIndex].position = NewVec3(boundingBox.x,                     boundingBox.y,                      boundingBox.z + boundingBox.depth);
	result.vertices[brfIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y,                      boundingBox.z + boundingBox.depth);
	result.vertices[tlbIndex].position = NewVec3(boundingBox.x,                     boundingBox.y + boundingBox.height, boundingBox.z);
	result.vertices[trbIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, boundingBox.z);
	result.vertices[tlfIndex].position = NewVec3(boundingBox.x,                     boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
	result.vertices[trfIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
	
	u64 iIndex = 0;
	//top face (+y)
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 0, Vec3_Up,      NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 0, Vec3_Up,      NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 0, Vec3_Up,      NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 0, Vec3_Up,      NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 0, Vec3_Up,      NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 0, Vec3_Up,      NewVec2(1, 0));
	
	//right face (+x)
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 1, Vec3_Right,   NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 1, Vec3_Right,   NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 1, Vec3_Right,   NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 1, Vec3_Right,   NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 1, Vec3_Right,   NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 1, Vec3_Right,   NewVec2(1, 0));
	
	//front face (+z)
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 2, Vec3_Forward, NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 2, Vec3_Forward, NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 2, Vec3_Forward, NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 2, Vec3_Forward, NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 2, Vec3_Forward, NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 2, Vec3_Forward, NewVec2(1, 0));
	
	//left face (-x)
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 3, Vec3_Left,    NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 3, Vec3_Left,    NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 3, Vec3_Left,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 3, Vec3_Left,    NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 3, Vec3_Left,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 3, Vec3_Left,    NewVec2(1, 0));
	
	//back face (-z)
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 4, Vec3_Back,    NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 4, Vec3_Back,    NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 4, Vec3_Back,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 4, Vec3_Back,    NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 4, Vec3_Back,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 4, Vec3_Back,    NewVec2(1, 0));
	
	//bottom face (-y)
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 5, Vec3_Down,    NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 5, Vec3_Down,    NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 5, Vec3_Down,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 5, Vec3_Down,    NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 5, Vec3_Down,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 5, Vec3_Down,    NewVec2(1, 0));
	
	Assert(iIndex == result.numIndices);
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
PrimitiveIndex3D_t NewPrimitiveIndex3D(u64 index, u64 faceIndex, v3 normal, v2 texCoord)
Frustum_t NewFrustum(v3 origin, v3 direction, v3 upVector, v2 fov, r32 zNear, r32 zFar)
Plane_t NewPlane(v3 normal, r32 distance)
Triangle2D_t NewTriangle2D(v2 vert0, v2 vert1, v2 vert2)
Triangle3D_t NewTriangle3D(v3 vert0, v3 vert1, v3 vert2)
Simplex_t NewSimplex(v3 vert0, v3 vert1, v3 vert2, v3 vert3)
Circle_t NewCircle(v2 center, r32 radius)
Sphere_t NewSphere(v3 center, r32 radius)
Cylinder_t NewCylinder(v3 base, r32 height, r32 radius)
Cone_t NewCone(v3 base, r32 height, r32 radius)
Pyramid_t NewPyramid(v3 base, r32 height, v2 baseSize)
Wedge_t NewWedge(v3 bottomLeft, v3 size)
void FreePrimitiveIndexedVerts(PrimitiveIndexedVerts_t* indexedVerts)
PrimitiveIndexedVerts_t GenerateVertsForBox(Box_t box, MemArena_t* memArena)
*/
