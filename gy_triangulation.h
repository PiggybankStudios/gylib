/*
File:   gy_triangulation.h
Author: Taylor Robbins
Date:   01\24\2022
Description:
	** Holds functions that help us triangulate various kinds of none-triangle geometry
*/

#ifndef _GY_TRIANGULATION_H
#define _GY_TRIANGULATION_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_math.h"
#include "gy_memory.h"
#include "gy_vectors.h"
#include "gy_rectangles.h"
#include "gy_rays.h"
#include "gy_primitives.h"

// +--------------------------------------------------------------+
// |                     Ear-Clipping Method                      |
// +--------------------------------------------------------------+
u64* Triangulate2DEarClip(MemArena_t* memArena, MemArena_t* tempArena, u64 numVertices, const v2* vertices, u64* numIndicesOut = nullptr)
{
	AssertIf(numVertices > 0, vertices != nullptr);
	AssertIf(memArena != nullptr, tempArena != nullptr);
	
	if (numIndicesOut != nullptr) { *numIndicesOut = 0; }
	
	u64 numIndices = 0;
	u64* result = nullptr;
	
	if (numVertices < 3) { return nullptr; }
	else if (numVertices == 3)
	{
		numIndices = 3;
		if (numIndicesOut != nullptr) { *numIndicesOut = numIndices; }
		if (memArena == nullptr) { return nullptr; }
		result = AllocArray(memArena, u64, numIndices);
		if (result == nullptr) { return nullptr; }
		NotNull(result);
		result[0] = 0;
		result[1] = 1;
		result[2] = 2;
		return result;
	}
	
	u64 numTriangles = numVertices - 2;
	numIndices = numTriangles*3;
	if (numIndicesOut != nullptr) { *numIndicesOut = numIndices; }
	if (memArena == nullptr) { return nullptr; }
	
	result = AllocArray(memArena, u64, numIndices);
	if (result == nullptr) { return nullptr; }
	
	PushMemMark(tempArena);
	u64 tempNumVerts = numVertices;
	v2* tempVerts = AllocArray(tempArena, v2, numVertices);
	u64* tempVertIndices = AllocArray(tempArena, u64, numVertices);
	for (u64 vIndex = 0; vIndex < numVertices; vIndex++)
	{
		tempVerts[vIndex] = vertices[vIndex];
		tempVertIndices[vIndex] = vIndex;
	}
	
	u64 iIndex = 0;
	while (tempNumVerts > 3)
	{
		bool foundEar = false;
		u64 earVertIndex = 0;
		for (u64 vIndex = 0; vIndex < tempNumVerts; vIndex++)
		{
			v2 vert0 = tempVerts[(vIndex + 0) % tempNumVerts];
			v2 vert1 = tempVerts[(vIndex + 1) % tempNumVerts];
			v2 vert2 = tempVerts[(vIndex + 2) % tempNumVerts];
			if (!IsTriangleClockwise(vert0, vert1, vert2)) { continue; }
			
			bool isEar = true;
			for (u64 vIndex2 = 0; vIndex2 < tempNumVerts; vIndex2++)
			{
				if (vIndex2 < vIndex || vIndex2 > vIndex+2)
				{
					if (IsInsideTriangle(tempVerts[vIndex2], vert0, vert1, vert2))
					{
						isEar = false;
						break;
					}
				}
			}
			
			if (isEar)
			{
				foundEar = true;
				earVertIndex = vIndex;
				break;
			}
		}
		if (!foundEar)
		{
			// PrintLine_W("The polygon is invalid after %u vert(s)!", iIndex);
			PopMemMark(tempArena);
			if (memArena != tempArena) { FreeMem(memArena, result); }
			if (numIndicesOut != nullptr) { *numIndicesOut = 0; }
			return nullptr;
		}
		Assert(foundEar);
		
		Assert(iIndex+3 <= numIndices);
		result[iIndex] = tempVertIndices[(earVertIndex + 0) % tempNumVerts]; iIndex++;
		result[iIndex] = tempVertIndices[(earVertIndex + 1) % tempNumVerts]; iIndex++;
		result[iIndex] = tempVertIndices[(earVertIndex + 2) % tempNumVerts]; iIndex++;
		
		//Remove the middle vertex in the ear and shift all vertices above it down by one
		for (u64 vIndex = ((earVertIndex+1)%tempNumVerts); vIndex+1 < tempNumVerts; vIndex++)
		{
			tempVerts[vIndex] = tempVerts[vIndex+1];
			tempVertIndices[vIndex] = tempVertIndices[vIndex+1];
		}
		tempNumVerts--;
	}
	Assert(tempNumVerts == 3);
	
	Assert(iIndex == numIndices-3);
	result[iIndex] = tempVertIndices[0]; iIndex++;
	result[iIndex] = tempVertIndices[1]; iIndex++;
	result[iIndex] = tempVertIndices[2]; iIndex++;
	
	PopMemMark(tempArena);
	
	return result;
}

#endif //  _GY_TRIANGULATION_H
