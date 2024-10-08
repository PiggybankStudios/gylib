/*
File:   gy_orca_addons.h
Author: Taylor Robbins
Date:   10\06\2024
Description:
	** These are new functions that Orca does not provide directly, but are closely
	** tied to Orca code, so they make sense to keep tightly couple with the style
	** and availability of Orca code. Usually these are composite functions, i.e.
	** a new function that calls a few of the Orca functions in a particular pattern.
	** We also have functions that do some setup work and call a single Orca function.
	** For example setting subsets of the UI Style using named functions that build
	** an OC_UiStyle_t structure in the proper way to accomplish a particular style change.
*/

#ifndef _GY_ORCA_ADDONS_H
#define _GY_ORCA_ADDONS_H

#if ORCA_COMPILATION

ORCA_INLINE void OC_UiSetNextWidth(r32 width, OC_UiSizeKind_t sizeKind = OC_UI_SIZE_PIXELS)
{
	OC_UiStyle_t style = {};
	style.size.width = { sizeKind, width };
	OC_UiStyleNext(&style, OC_UI_STYLE_SIZE_WIDTH);
}
ORCA_INLINE void OC_UiSetNextHeight(r32 height, OC_UiSizeKind_t sizeKind = OC_UI_SIZE_PIXELS)
{
	OC_UiStyle_t style = {};
	style.size.height = { sizeKind, height };
	OC_UiStyleNext(&style, OC_UI_STYLE_SIZE_HEIGHT);
}

#endif

#endif //  _GY_ORCA_ADDONS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Functions
ORCA_INLINE void OC_UiSetNextWidth(r32 width, OC_UiSizeKind_t kind = OC_UI_SIZE_PIXELS)
ORCA_INLINE void OC_UiSetNextHeight(r32 height, OC_UiSizeKind_t sizeKind = OC_UI_SIZE_PIXELS)
*/
