/*
File:   gy_orca_overloads.h
Author: Taylor Robbins
Date:   10\05\2024
Description:
	** Now that all of the GyLib types have been defined, we'd like to add a few overloads
	** to Orca functions to make them more convenient to call using our own types
*/

#ifndef _GY_ORCA_OVERLOADS_H
#define _GY_ORCA_OVERLOADS_H

#if ORCA_COMPILATION

//TODO: Do we need to handle GYLIB_HEADER_ONLY?

// +--------------------------------------------------------------+
// |                       Orca Window API                        |
// +--------------------------------------------------------------+
ORCA_INLINE void OC_WindowSetTitle(MyStr_t title) { oc_window_set_title(ToOcStr8(title)); }
ORCA_INLINE void OC_WindowSetSize(v2 size)        { oc_window_set_size(ToOcVec2(size)); }

// +==============================+
// |      Orca Graphics API       |
// +==============================+
ORCA_INLINE v2 OC_SurfaceGetSize(OC_Surface_t surface)                                                                                          { return ToVec2(oc_surface_get_size(surface)); }
ORCA_INLINE v2 OC_SurfaceContentsScaling(OC_Surface_t surface)                                                                                  { return ToVec2(oc_surface_contents_scaling(surface)); }
ORCA_INLINE Color_t OC_ColorConvert(Color_t color, OC_ColorSpace_t colorSpace)                                                                  { return ToColor(oc_color_convert(ToOcColor(color), colorSpace)); }
ORCA_INLINE OC_Font_t OC_FontCreateFromMemory(MyStr_t mem, u32 rangeCount, OC_UnicodeRange_t* ranges)                                           { return oc_font_create_from_memory(ToOcStr8(mem), rangeCount, ranges); }
ORCA_INLINE OC_Font_t OC_FontCreateFromPath(MyStr_t path, u32 rangeCount, OC_UnicodeRange_t* ranges)                                            { return oc_font_create_from_path(ToOcStr8(path), rangeCount, ranges); }
ORCA_INLINE OC_TextMetrics_t OC_FontTextMetrics(OC_Font_t font, r32 fontSize, MyStr_t text)                                                     { return oc_font_text_metrics(font, fontSize, ToOcStr8(text)); }
ORCA_INLINE OC_Image_t OC_ImageCreateFromMemory(OC_CanvasRenderer_t renderer, MyStr_t mem, bool flip)                                           { return oc_image_create_from_memory(renderer, ToOcStr8(mem), flip); }
ORCA_INLINE OC_Image_t OC_ImageCreateFromPath(OC_CanvasRenderer_t renderer, MyStr_t path, bool flip)                                            { return oc_image_create_from_path(renderer, ToOcStr8(path), flip); }
ORCA_INLINE void OC_ImageUploadRegionRgba8(OC_Image_t image, rec region, u8* pixels)                                                            { oc_image_upload_region_rgba8(image, ToOcRect(region), pixels); }
ORCA_INLINE v2 OC_ImageSize(OC_Image_t image)                                                                                                   { return ToVec2(oc_image_size(image)); }
ORCA_INLINE rec OC_RectAtlasAlloc(OC_RectAtlas_t* atlas, i32 width, i32 height)                                                                 { return ToRec(oc_rect_atlas_alloc(atlas, width, height)); }
ORCA_INLINE void OC_RectAtlasRecycle(OC_RectAtlas_t* atlas, rec rect)                                                                           { oc_rect_atlas_recycle(atlas, ToOcRect(rect)); }
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromMemory(OC_RectAtlas_t* atlas, OC_Image_t backingImage, MyStr_t mem, bool flip)               { return oc_image_atlas_alloc_from_memory(atlas, backingImage, ToOcStr8(mem), flip); }
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromPath(OC_RectAtlas_t* atlas, OC_Image_t backingImage, MyStr_t path, bool flip)                { return oc_image_atlas_alloc_from_path(atlas, backingImage, ToOcStr8(path), flip); }
// ORCA_INLINE void OC_MatrixPush(mat4 matrix)                                                                                                     { oc_matrix_push(ToOcMat2x3(matrix)); }
// ORCA_INLINE void OC_MatrixMultiplyPush(mat4 matrix)                                                                                             { oc_matrix_multiply_push(ToOcMat2x3(matrix)); }
// ORCA_INLINE mat4 OC_MatrixTop()                                                                                                                 { return ToMat4(oc_matrix_top()); }
ORCA_INLINE void OC_ClipPush(rec rectangle)                                                                                                     { oc_clip_push(rectangle.x, rectangle.y, rectangle.width, rectangle.height); }
ORCA_INLINE rec OC_ClipTop()                                                                                                                    { return ToRec(oc_clip_top()); }
ORCA_INLINE void OC_SetColor(Color_t color)                                                                                                     { oc_set_color(ToOcColor(color)); }
ORCA_INLINE void OC_SetGradient(OC_GradientBlendSpace_t blendSpace, Color_t bottomLeft, Color_t bottomRight, Color_t topRight, Color_t topLeft) { oc_set_gradient(blendSpace, ToOcColor(bottomLeft), ToOcColor(bottomRight), ToOcColor(topRight), ToOcColor(topLeft)); }
ORCA_INLINE void OC_SetImageSourceRegion(rec region)                                                                                            { oc_set_image_source_region(ToOcRect(region)); }
ORCA_INLINE Color_t OC_GetColor()                                                                                                               { return ToColor(oc_get_color()); }
ORCA_INLINE rec OC_GetImageSourceRegion()                                                                                                       { return ToRec(oc_get_image_source_region()); }
ORCA_INLINE v2 OC_GetPosition()                                                                                                                 { return ToVec2(oc_get_position()); }
ORCA_INLINE void OC_MoveTo(v2 pos)                                                                                                              { oc_move_to(pos.x, pos.y); }
ORCA_INLINE void OC_LineTo(v2 pos)                                                                                                              { oc_line_to(pos.x, pos.y); }
ORCA_INLINE void OC_QuadraticTo(v2 p1, v2 p2)                                                                                                   { oc_quadratic_to(p1.x, p1.y, p2.x, p2.y); }
ORCA_INLINE void OC_CubicTo(v2 p1, v2 p2, v2 p3)                                                                                                { oc_cubic_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y); }
ORCA_INLINE rec OC_GlyphOutlines(oc_str32 glyphIndices)                                                                                         { return ToRec(oc_glyph_outlines(glyphIndices)); }
ORCA_INLINE void OC_TextOutlines(MyStr_t string)                                                                                                { oc_text_outlines(ToOcStr8(string)); }
ORCA_INLINE void OC_RectangleFill(rec rectangle)                                                                                                { oc_rectangle_fill(rectangle.x, rectangle.y, rectangle.width, rectangle.height); }
ORCA_INLINE void OC_RectangleStroke(rec rectangle)                                                                                              { oc_rectangle_stroke(rectangle.x, rectangle.y, rectangle.width, rectangle.height); }
ORCA_INLINE void OC_RoundedRectangleFill(rec rectangle, r32 r)                                                                                  { oc_rounded_rectangle_fill(rectangle.x, rectangle.y, rectangle.width, rectangle.height, r); }
ORCA_INLINE void OC_RoundedRectangleStroke(rec rectangle, r32 r)                                                                                { oc_rounded_rectangle_stroke(rectangle.x, rectangle.y, rectangle.width, rectangle.height, r); }
ORCA_INLINE void OC_TextFill(r32 x, r32 y, MyStr_t text)                                                                                        { oc_text_fill(x, y, ToOcStr8(text)); }
ORCA_INLINE void OC_TextFill(v2 pos, MyStr_t text)                                                                                              { oc_text_fill(pos.x, pos.y, ToOcStr8(text)); }
ORCA_INLINE void OC_ImageDraw(OC_Image_t image, rec rect)                                                                                       { oc_image_draw(image, ToOcRect(rect)); }
ORCA_INLINE void OC_ImageDrawRegion(OC_Image_t image, rec srcRegion, rec dstRegion)                                                             { oc_image_draw_region(image, ToOcRect(srcRegion), ToOcRect(dstRegion)); }

// +==============================+
// |        Orca File API         |
// +==============================+
ORCA_INLINE OC_File_t OC_FileOpen(MyStr_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)                  { return oc_file_open(ToOcStr8(path), rights, flags); }
ORCA_INLINE OC_File_t OC_FileOpenAt(OC_File_t dir, MyStr_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags) { return oc_file_open_at(dir, ToOcStr8(path), rights, flags); }
ORCA_INLINE OC_File_t OC_FileOpenWithRequest(MyStr_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)       { return oc_file_open_with_request(ToOcStr8(path), rights, flags); }

// +==============================+
// |         Orca UI API          |
// +==============================+
ORCA_INLINE void OC_UiBeginFrame(v2 size, OC_UiStyle_t* defaultStyle, OC_UiStyleMask_t mask)     { oc_ui_begin_frame(ToOcVec2(size), defaultStyle, mask); }
ORCA_INLINE OC_UiSig_t OC_UiLabelStr8(MyStr_t label)                                             { return oc_ui_label_str8(ToOcStr8(label)); }
ORCA_INLINE OC_UiTextBoxResult_t OC_UiTextBox(const char* name, OC_Arena_t* arena, MyStr_t text) { return oc_ui_text_box(name, arena, ToOcStr8(text)); }

//TODO: These have types we may wanna convert, but we gotta change the name so the macro is unique
#define OC_UiFrame(size, style, mask) oc_defer_loop(OC_UiBeginFrame(ToOcVec2(size), (style), (mask)), OC_UiEndFrame())
#define OC_UiMenuBar(name)            oc_defer_loop(OC_UiMenuBarBegin(name), OC_UiMenuBarEnd())
#define OC_UiMenu(label)              oc_defer_loop(OC_UiMenuBegin(label), OC_UiMenuEnd())

// +--------------------------------------------------------------+
// |                        Orca Util API                         |
// +--------------------------------------------------------------+
ORCA_INLINE MyStr_t OC_Str8FromBuffer(u64 len, char* buffer)                                                                      { return ToStr(oc_str8_from_buffer(len, buffer)); }
ORCA_INLINE MyStr_t OC_Str8Slice(MyStr_t s, u64 start, u64 end)                                                                   { return ToStr(oc_str8_slice(ToOcStr8(s), start, end)); }
ORCA_INLINE MyStr_t OC_Str8PushBuffer(OC_Arena_t* arena, u64 len, char* buffer)                                                   { return ToStr(oc_str8_push_buffer(arena, len, buffer)); }
ORCA_INLINE MyStr_t OC_Str8PushCstring(OC_Arena_t* arena, const char* str)                                                        { return ToStr(oc_str8_push_cstring(arena, str)); }
ORCA_INLINE MyStr_t OC_Str8PushCopy(OC_Arena_t* arena, MyStr_t s)                                                                 { return ToStr(oc_str8_push_copy(arena, ToOcStr8(s))); }
ORCA_INLINE MyStr_t OC_Str8PushSlice(OC_Arena_t* arena, MyStr_t s, u64 start, u64 end)                                            { return ToStr(oc_str8_push_slice(arena, ToOcStr8(s), start, end)); }
ORCA_INLINE MyStr_t OC_Str8Pushfv(OC_Arena_t* arena, const char* format, va_list args)                                            { return ToStr(oc_str8_pushfv(arena, format, args)); }
ORCA_INLINE int OC_Str8Cmp(MyStr_t s1, MyStr_t s2)                                                                                { return oc_str8_cmp(ToOcStr8(s1), ToOcStr8(s2)); }
ORCA_INLINE char* OC_Str8ToCstring(OC_Arena_t* arena, MyStr_t string)                                                             { return oc_str8_to_cstring(arena, ToOcStr8(string)); }
ORCA_INLINE void OC_Str8ListPush(OC_Arena_t* arena, OC_Str8List_t* list, MyStr_t str)                                             { oc_str8_list_push(arena, list, ToOcStr8(str)); }
ORCA_INLINE MyStr_t OC_Str8ListCollate(OC_Arena_t* arena, OC_Str8List_t list, MyStr_t prefix, MyStr_t separator, MyStr_t postfix) { return ToStr(oc_str8_list_collate(arena, list, ToOcStr8(prefix), ToOcStr8(separator), ToOcStr8(postfix))); }
ORCA_INLINE MyStr_t OC_Str8ListJoin(OC_Arena_t* arena, OC_Str8List_t list)                                                        { return ToStr(oc_str8_list_join(arena, list)); }
ORCA_INLINE OC_Str8List_t OC_Str8Split(OC_Arena_t* arena, MyStr_t str, OC_Str8List_t separators)                                  { return oc_str8_split(arena, ToOcStr8(str), separators); }
ORCA_INLINE MyStr_t OC_PathSliceDirectory(MyStr_t path)                                                                           { return ToStr(oc_path_slice_directory(ToOcStr8(path))); }
ORCA_INLINE MyStr_t OC_PathSliceFilename(MyStr_t path)                                                                            { return ToStr(oc_path_slice_filename(ToOcStr8(path))); }
ORCA_INLINE OC_Str8List_t OC_PathSplit(OC_Arena_t* arena, MyStr_t path)                                                           { return oc_path_split(arena, ToOcStr8(path)); }
ORCA_INLINE MyStr_t OC_PathJoin(OC_Arena_t* arena, OC_Str8List_t elements)                                                        { return ToStr(oc_path_join(arena, elements)); }
ORCA_INLINE MyStr_t OC_PathAppend(OC_Arena_t* arena, MyStr_t parent, MyStr_t relPath)                                             { return ToStr(oc_path_append(arena, ToOcStr8(parent), ToOcStr8(relPath))); }
ORCA_INLINE bool OC_PathIsAbsolute(MyStr_t path)                                                                                  { return oc_path_is_absolute(ToOcStr8(path)); }

#define OC_MyStrPushf(arena, format, ...)  ToStr(oc_str8_pushf(arena, format, ##__VA_ARGS__))

#endif //ORCA_COMPILATION

#endif //  _GY_ORCA_OVERLOADS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Functions
ORCA_INLINE void OC_WindowSetTitle(MyStr_t title)
ORCA_INLINE void OC_WindowSetSize(v2 size)
ORCA_INLINE v2 OC_SurfaceGetSize(OC_Surface_t surface)
ORCA_INLINE v2 OC_SurfaceContentsScaling(OC_Surface_t surface)
ORCA_INLINE Color_t OC_ColorConvert(Color_t color, OC_ColorSpace_t colorSpace)
ORCA_INLINE OC_Font_t OC_FontCreateFromMemory(MyStr_t mem, u32 rangeCount, OC_UnicodeRange_t* ranges)
ORCA_INLINE OC_Font_t OC_FontCreateFromPath(MyStr_t path, u32 rangeCount, OC_UnicodeRange_t* ranges)
ORCA_INLINE OC_TextMetrics_t OC_FontTextMetrics(OC_Font_t font, r32 fontSize, MyStr_t text)
ORCA_INLINE OC_Image_t OC_ImageCreateFromMemory(OC_CanvasRenderer_t renderer, MyStr_t mem, bool flip)
ORCA_INLINE OC_Image_t OC_ImageCreateFromPath(OC_CanvasRenderer_t renderer, MyStr_t path, bool flip)
ORCA_INLINE void OC_ImageUploadRegionRgba8(OC_Image_t image, rec region, u8* pixels)
ORCA_INLINE v2 OC_ImageSize(OC_Image_t image)
ORCA_INLINE rec OC_RectAtlasAlloc(OC_RectAtlas_t* atlas, i32 width, i32 height)
ORCA_INLINE void OC_RectAtlasRecycle(OC_RectAtlas_t* atlas, rec rect)
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromMemory(OC_RectAtlas_t* atlas, OC_Image_t backingImage, MyStr_t mem, bool flip)
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromPath(OC_RectAtlas_t* atlas, OC_Image_t backingImage, MyStr_t path, bool flip)
// ORCA_INLINE void OC_MatrixPush(mat4 matrix)
// ORCA_INLINE void OC_MatrixMultiplyPush(mat4 matrix)
// ORCA_INLINE mat4 OC_MatrixTop()
ORCA_INLINE void OC_ClipPush(rec rectangle)
ORCA_INLINE rec OC_ClipTop()
ORCA_INLINE void OC_SetColor(Color_t color)
ORCA_INLINE void OC_SetGradient(OC_GradientBlendSpace_t blendSpace, Color_t bottomLeft, Color_t bottomRight, Color_t topRight, Color_t topLeft)
ORCA_INLINE void OC_SetImageSourceRegion(rec region)
ORCA_INLINE Color_t OC_GetColor()
ORCA_INLINE rec OC_GetImageSourceRegion()
ORCA_INLINE v2 OC_GetPosition()
ORCA_INLINE void OC_MoveTo(v2 pos)
ORCA_INLINE void OC_LineTo(v2 pos)
ORCA_INLINE void OC_QuadraticTo(v2 p1, v2 p2)
ORCA_INLINE void OC_CubicTo(v2 p1, v2 p2, v2 p3)
ORCA_INLINE rec OC_GlyphOutlines(oc_str32 glyphIndices)
ORCA_INLINE void OC_TextOutlines(MyStr_t string)
ORCA_INLINE void OC_RectangleFill(rec rectangle)
ORCA_INLINE void OC_RectangleStroke(rec rectangle)
ORCA_INLINE void OC_RoundedRectangleFill(rec rectangle, r32 r)
ORCA_INLINE void OC_RoundedRectangleStroke(rec rectangle, r32 r)
ORCA_INLINE void OC_TextFill(r32 x, r32 y, MyStr_t text)
ORCA_INLINE void OC_TextFill(v2 pos, MyStr_t text)
ORCA_INLINE void OC_ImageDraw(OC_Image_t image, rec rect)
ORCA_INLINE void OC_ImageDrawRegion(OC_Image_t image, rec srcRegion, rec dstRegion)
ORCA_INLINE OC_File_t OC_FileOpen(MyStr_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)
ORCA_INLINE OC_File_t OC_FileOpenAt(OC_File_t dir, MyStr_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)
ORCA_INLINE OC_File_t OC_FileOpenWithRequest(MyStr_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)
ORCA_INLINE void OC_UiBeginFrame(v2 size, OC_UiStyle_t* defaultStyle, OC_UiStyleMask_t mask)
ORCA_INLINE OC_UiSig_t OC_UiLabelStr8(MyStr_t label)
ORCA_INLINE OC_UiTextBoxResult_t OC_UiTextBox(const char* name, OC_Arena_t* arena, MyStr_t text)
#define OC_UiFrame(size, style, mask)
#define OC_UiMenuBar(name)
#define OC_UiMenu(label)
ORCA_INLINE MyStr_t OC_Str8FromBuffer(u64 len, char* buffer)
ORCA_INLINE MyStr_t OC_Str8Slice(MyStr_t s, u64 start, u64 end)
ORCA_INLINE MyStr_t OC_Str8PushBuffer(OC_Arena_t* arena, u64 len, char* buffer)
ORCA_INLINE MyStr_t OC_Str8PushCstring(OC_Arena_t* arena, const char* str)
ORCA_INLINE MyStr_t OC_Str8PushCopy(OC_Arena_t* arena, MyStr_t s)
ORCA_INLINE MyStr_t OC_Str8PushSlice(OC_Arena_t* arena, MyStr_t s, u64 start, u64 end)
ORCA_INLINE MyStr_t OC_Str8Pushfv(OC_Arena_t* arena, const char* format, va_list args)
ORCA_INLINE int OC_Str8Cmp(MyStr_t s1, MyStr_t s2)
ORCA_INLINE char* OC_Str8ToCstring(OC_Arena_t* arena, MyStr_t string)
ORCA_INLINE void OC_Str8ListPush(OC_Arena_t* arena, OC_Str8List_t* list, MyStr_t str)
ORCA_INLINE MyStr_t OC_Str8ListCollate(OC_Arena_t* arena, OC_Str8List_t list, MyStr_t prefix, MyStr_t separator, MyStr_t postfix)
ORCA_INLINE MyStr_t OC_Str8ListJoin(OC_Arena_t* arena, OC_Str8List_t list)
ORCA_INLINE OC_Str8List_t OC_Str8Split(OC_Arena_t* arena, MyStr_t str, OC_Str8List_t separators)
ORCA_INLINE MyStr_t OC_PathSliceDirectory(MyStr_t path)
ORCA_INLINE MyStr_t OC_PathSliceFilename(MyStr_t path)
ORCA_INLINE OC_Str8List_t OC_PathSplit(OC_Arena_t* arena, MyStr_t path)
ORCA_INLINE MyStr_t OC_PathJoin(OC_Arena_t* arena, OC_Str8List_t elements)
ORCA_INLINE MyStr_t OC_PathAppend(OC_Arena_t* arena, MyStr_t parent, MyStr_t relPath)
ORCA_INLINE bool OC_PathIsAbsolute(MyStr_t path)
#define OC_MyStrPushf(arena, format, ...)
*/
