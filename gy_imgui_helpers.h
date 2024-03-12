/*
File:   gy_imgui_helpers.h
Author: Taylor Robbins
Date:   03\11\2024
Description:
	** This file adds a bunch of helper functions to convert between Dear ImGui types
	** and GyLib types. It is not included by default in gy.h but should be included
	** AFTER you include imgui.h
*/

#ifndef _GY_IMGUI_HELPERS_H
#define _GY_IMGUI_HELPERS_H

#define START_IMGUI_NAMESPACE namespace ImGui {
#define END_IMGUI_NAMESPACE }

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	#ifdef _GY_VECTORS_H
	ImVec4 ToImVec4(v4 vector);
	v4 FromImVec4(ImVec4 vector);
	#endif //_GY_VECTORS_H
	#ifdef _GY_COLORS_H
	u32 ToImCol32(Color_t color);
	Color_t FromImCol32(u32 colorU32);
	ImVec4 ToImVec4(Color_t color);
	#endif //_GY_COLORS_H
#else

// +--------------------------------------------------------------+
// |                     gy_vectors.h Helpers                     |
// +--------------------------------------------------------------+
#ifdef _GY_VECTORS_H
ImVec4 ToImVec4(v4 vector)
{
	return ImVec4(vector.x, vector.y, vector.z, vector.w);
}
v4 FromImVec4(ImVec4 vector)
{
	return NewVec4(vector.x, vector.y, vector.z, vector.w);
}
#endif //_GY_VECTORS_H

// +--------------------------------------------------------------+
// |                     gy_colors.h Helpers                      |
// +--------------------------------------------------------------+
#ifdef _GY_COLORS_H
u32 ToImCol32(Color_t color)
{
	return IM_COL32(color.r, color.g, color.b, color.a);
}
Color_t FromImCol32(u32 colorU32)
{
	return NewColor(
		(u8)((colorU32 & (0xFF << IM_COL32_R_SHIFT)) >> IM_COL32_R_SHIFT),
		(u8)((colorU32 & (0xFF << IM_COL32_G_SHIFT)) >> IM_COL32_G_SHIFT),
		(u8)((colorU32 & (0xFF << IM_COL32_B_SHIFT)) >> IM_COL32_B_SHIFT),
		(u8)((colorU32 & (0xFF << IM_COL32_A_SHIFT)) >> IM_COL32_A_SHIFT)
	);
}
ImVec4 ToImVec4(Color_t color)
{
	return ImVec4(
		color.r / 255.0f,
		color.g / 255.0f,
		color.b / 255.0f,
		color.a / 255.0f
	);
}
#endif //_GY_COLORS_H

START_IMGUI_NAMESPACE



END_IMGUI_NAMESPACE

#endif // GYLIB_HEADER_ONLY

#endif //  _GY_IMGUI_HELPERS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
START_IMGUI_NAMESPACE
END_IMGUI_NAMESPACE
@Types
@Functions
ImVec4 ToImVec4(v4 vector)
v4 FromImVec4(ImVec4 vector)
u32 ToImCol32(Color_t color)
Color_t FromImCol32(u32 colorU32)
ImVec4 ToImVec4(Color_t color)
*/
