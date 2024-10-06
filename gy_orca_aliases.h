/*
File:   gy_orca_aliases.h
Author: Taylor Robbins
Date:   10\05\2024
Description:
	** This file adds aliases for all Orca types and functions so they match our normal naming conventions.
	** It also acts like a small buffer between us and the Orca API, that way when it changes we can get
	** notified easily and fix up call sites quickly
*/

#ifndef _GY_ORCA_ALIASES_H
#define _GY_ORCA_ALIASES_H

#if ORCA_COMPILATION

//TODO: Do we need to handle GYLIB_HEADER_ONLY?

// +--------------------------------------------------------------+
// |                         Basic Macros                         |
// +--------------------------------------------------------------+
//NOTE: Clang treats basic "inline" a bit different than I expected, "static inline" does what I want
#define ORCA_INLINE static inline

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
// +===============================+
// | Conflicting Orca Type Aliases |
// +===============================+
typedef oc_vec2      OC_Vec2_t;
typedef oc_vec3      OC_Vec3_t;
typedef oc_vec4      OC_Vec4_t;
typedef oc_vec2i     OC_Vec2i_t;
typedef oc_rect      OC_Rect_t;
typedef oc_mat2x3    OC_Mat2x3_t;
typedef oc_color     OC_Color_t;
typedef oc_str8      OC_Str8_t;
typedef oc_str32     OC_Str32_t;
typedef oc_utf32     OC_Utf32_t;
typedef oc_str8_list OC_Str8List_t;

// +==============================+
// |   Other Orca Type Aliases    |
// +==============================+
typedef oc_mouse_button                 OC_MouseButton_t;
typedef oc_scan_code                    OC_ScanCode_t;
typedef oc_key_code                     OC_KeyCode_t;
typedef oc_surface                      OC_Surface_t;
typedef oc_color_space                  OC_ColorSpace_t;
typedef oc_canvas_renderer              OC_CanvasRenderer_t;
typedef oc_canvas_context               OC_CanvasContext_t;
typedef oc_file                         OC_File_t;
typedef oc_font                         OC_Font_t;
typedef oc_image                        OC_Image_t;
typedef oc_rect_atlas                   OC_RectAtlas_t;
typedef oc_image_region                 OC_ImageRegion_t;
typedef oc_gradient_blend_space         OC_GradientBlendSpace_t;
typedef oc_unicode_range                OC_UnicodeRange_t;
typedef oc_arena                        OC_Arena_t;
typedef oc_arena_options                OC_ArenaOptions_t;
typedef oc_arena_scope                  OC_ArenaScope_t;
typedef oc_font_metrics                 OC_FontMetrics_t;
typedef oc_text_metrics                 OC_TextMetrics_t;
typedef oc_joint_type                   OC_JointType_t;
typedef oc_cap_type                     OC_CapType_t;
typedef oc_io_cmp                       OC_IoCmp_t;
typedef oc_io_req                       OC_IoReq_t;
typedef oc_file_access                  OC_FileAccess_t;
typedef oc_file_open_flags              OC_FileOpenFlags_t;
typedef oc_file_whence                  OC_FileWhence_t;
typedef oc_file_dialog_desc             OC_FileDialogDesc_t;
typedef oc_io_error                     OC_IoError_t;
typedef oc_file_status                  OC_FileStatus_t;
typedef oc_file_open_with_dialog_result OC_FileOpenWithDialogResult_t;
typedef oc_ui_context                   OC_UiContext_t;
typedef oc_event                        OC_Event_t;
typedef oc_ui_style                     OC_UiStyle_t;
typedef oc_ui_style_mask                OC_UiStyleMask_t;
typedef oc_ui_sig                       OC_UiSig_t;
typedef oc_ui_box                       OC_UiBox_t;
typedef oc_ui_text_box_result           OC_UiTextBoxResult_t;
typedef oc_ui_select_popup_info         OC_UiSelectPopupInfo_t;
typedef oc_ui_radio_group_info          OC_UiRadioGroupInfo_t;
typedef oc_ui_flags                     OC_UiFlags_t;
typedef oc_ui_pattern                   OC_UiPattern_t;
typedef oc_ui_selector                  OC_UiSelector_t;
typedef oc_list                         OC_List_t;
typedef oc_list_elt                     OC_ListElement_t;

// +--------------------------------------------------------------+
// |                    Orca Function Aliases                     |
// +--------------------------------------------------------------+
// +==============================+
// |       Orca App Exports       |
// +==============================+
#define OC_OnInit          oc_on_init          // OC_OnInit()
#define OC_OnMouseDown     oc_on_mouse_down    // OC_OnMouseDown(OC_MouseButton_t button)
#define OC_OnMouseUp       oc_on_mouse_up      // OC_OnMouseUp(OC_MouseButton_t button)
#define OC_OnMouseEnter    oc_on_mouse_enter   // OC_OnMouseEnter()
#define OC_OnMouseLeave    oc_on_mouse_leave   // OC_OnMouseLeave()
#define OC_OnMouseMove     oc_on_mouse_move    // OC_OnMouseMove(f32 x, f32 y, f32 deltaX, f32 deltaY)
#define OC_OnMouseWheel    oc_on_mouse_wheel   // OC_OnMouseWheel(f32 deltaX, f32 deltaY)
#define OC_OnKeyDown       oc_on_key_down      // OC_OnKeyDown(OC_ScanCode_t scan, OC_KeyCode_t key)
#define OC_OnKeyUp         oc_on_key_up        // OC_OnKeyUp(OC_ScanCode_t scan, OC_KeyCode_t key)
#define OC_OnFrameRefresh  oc_on_frame_refresh // OC_OnFrameRefresh()
#define OC_OnResize        oc_on_resize        // OC_OnResize(f32 width, f32 height)
#define OC_OnRawEvent      oc_on_raw_event     // OC_OnRawEvent(OC_Event_t* event)
#define OC_OnTerminate     oc_on_terminate     // OC_OnTerminate()

//NOTE: For functions that return a type that we have a GyLib equivalent for AND don't take any types we have equiv for,
//      We need to add an _ at the end to make the default alias unique, so that gy_orca_overloads.h can use the primary alias name (no underscore)

// +==============================+
// |       Orca Window API        |
// +==============================+
ORCA_INLINE void OC_WindowSetTitle(OC_Str8_t title) { oc_window_set_title(title); }
ORCA_INLINE void OC_WindowSetSize(OC_Vec2_t size)   { oc_window_set_size(size); }
ORCA_INLINE void OC_RequestQuit()                   { oc_request_quit(); }

// +==============================+
// |      Orca Graphics API       |
// +==============================+
ORCA_INLINE OC_Surface_t OC_SurfaceNil()                                                                                                                    { return oc_surface_nil(); }
ORCA_INLINE bool OC_SurfaceIsNil(OC_Surface_t surface)                                                                                                      { return oc_surface_is_nil(surface); }
ORCA_INLINE void OC_SurfaceDestroy(OC_Surface_t surface)                                                                                                    { oc_surface_destroy(surface); }
ORCA_INLINE OC_Vec2_t OC_SurfaceGetSize_(OC_Surface_t surface)                                                                                              { return oc_surface_get_size(surface); }
ORCA_INLINE OC_Vec2_t OC_SurfaceContentsScaling_(OC_Surface_t surface)                                                                                      { return oc_surface_contents_scaling(surface); }
ORCA_INLINE void OC_SurfaceBringToFront(OC_Surface_t surface)                                                                                               { oc_surface_bring_to_front(surface); }
ORCA_INLINE void OC_SurfaceSendToBack(OC_Surface_t surface)                                                                                                 { oc_surface_send_to_back(surface); }
ORCA_INLINE bool OC_SurfaceGetHidden(OC_Surface_t surface)                                                                                                  { return oc_surface_get_hidden(surface); }
ORCA_INLINE void OC_SurfaceSetHidden(OC_Surface_t surface, bool hidden)                                                                                     { oc_surface_set_hidden(surface, hidden); }
ORCA_INLINE OC_Surface_t OC_GlesSurfaceCreate()                                                                                                             { return oc_gles_surface_create(); }
ORCA_INLINE void OC_GlesSurfaceMakeCurrent(OC_Surface_t surface)                                                                                            { oc_gles_surface_make_current(surface); }
ORCA_INLINE void OC_GlesSurfaceSwapInterval(OC_Surface_t surface, int interval)                                                                             { oc_gles_surface_swap_interval(surface, interval); }
ORCA_INLINE void OC_GlesSurfaceSwapBuffers(OC_Surface_t surface)                                                                                            { oc_gles_surface_swap_buffers(surface); }
ORCA_INLINE OC_Color_t OC_ColorRgba(f32 r, f32 g, f32 b, f32 a)                                                                                             { return oc_color_rgba(r, g, b, a); }
ORCA_INLINE OC_Color_t OC_ColorSrgba(f32 r, f32 g, f32 b, f32 a)                                                                                            { return oc_color_srgba(r, g, b, a); }
ORCA_INLINE OC_Color_t OC_ColorConvert(OC_Color_t color, OC_ColorSpace_t colorSpace)                                                                        { return oc_color_convert(color, colorSpace); }
ORCA_INLINE OC_CanvasRenderer_t OC_CanvasRendererNil()                                                                                                      { return oc_canvas_renderer_nil(); }
ORCA_INLINE bool OC_CanvasRendererIsNil(OC_CanvasRenderer_t renderer)                                                                                       { return oc_canvas_renderer_is_nil(renderer); }
ORCA_INLINE OC_CanvasRenderer_t OC_CanvasRendererCreate()                                                                                                   { return oc_canvas_renderer_create(); }
ORCA_INLINE void OC_CanvasRendererDestroy(OC_CanvasRenderer_t renderer)                                                                                     { oc_canvas_renderer_destroy(renderer); }
ORCA_INLINE void OC_CanvasRender(OC_CanvasRenderer_t renderer, OC_CanvasContext_t context, OC_Surface_t surface)                                            { oc_canvas_render(renderer, context, surface); }
ORCA_INLINE void OC_CanvasPresent(OC_CanvasRenderer_t renderer, OC_Surface_t surface)                                                                       { oc_canvas_present(renderer, surface); }
ORCA_INLINE OC_Surface_t OC_CanvasSurfaceCreate(OC_CanvasRenderer_t renderer)                                                                               { return oc_canvas_surface_create(renderer); }
ORCA_INLINE void OC_CanvasSurfaceSwapInterval(OC_Surface_t surface, int swap)                                                                               { oc_canvas_surface_swap_interval(surface, swap); }
ORCA_INLINE OC_CanvasContext_t OC_CanvasContextNil()                                                                                                        { return oc_canvas_context_nil(); }
ORCA_INLINE bool OC_CanvasContextIsNil(OC_CanvasContext_t context)                                                                                          { return oc_canvas_context_is_nil(context); }
ORCA_INLINE OC_CanvasContext_t OC_CanvasContextCreate()                                                                                                     { return oc_canvas_context_create(); }
ORCA_INLINE void OC_CanvasContextDestroy(OC_CanvasContext_t context)                                                                                        { oc_canvas_context_destroy(context); }
ORCA_INLINE OC_CanvasContext_t OC_CanvasContextSelect(OC_CanvasContext_t context)                                                                           { return oc_canvas_context_select(context); }
ORCA_INLINE void OC_CanvasContextSetMsaaSampleCount(OC_CanvasContext_t context, u32 sampleCount)                                                            { oc_canvas_context_set_msaa_sample_count(context, sampleCount); }
ORCA_INLINE OC_Font_t OC_FontNil()                                                                                                                          { return oc_font_nil(); }
ORCA_INLINE bool OC_FontIsNil(OC_Font_t font)                                                                                                               { return oc_font_is_nil(font); }
ORCA_INLINE OC_Font_t OC_FontCreateFromMemory(OC_Str8_t mem, u32 rangeCount, OC_UnicodeRange_t* ranges)                                                     { return oc_font_create_from_memory(mem, rangeCount, ranges); }
ORCA_INLINE OC_Font_t OC_FontCreateFromFile(OC_File_t file, u32 rangeCount, OC_UnicodeRange_t* ranges)                                                      { return oc_font_create_from_file(file, rangeCount, ranges); }
ORCA_INLINE OC_Font_t OC_FontCreateFromPath(OC_Str8_t path, u32 rangeCount, OC_UnicodeRange_t* ranges)                                                      { return oc_font_create_from_path(path, rangeCount, ranges); }
ORCA_INLINE void OC_FontDestroy(OC_Font_t font)                                                                                                             { oc_font_destroy(font); }
ORCA_INLINE OC_Str32_t OC_FontGetGlyphIndices(OC_Font_t font, OC_Str32_t codePoints, OC_Str32_t backing)                                                    { return oc_font_get_glyph_indices(font, codePoints, backing); }
ORCA_INLINE OC_Str32_t OC_FontPushGlyphIndices(OC_Arena_t* arena, OC_Font_t font, OC_Str32_t codePoints)                                                    { return oc_font_push_glyph_indices(arena, font, codePoints); }
ORCA_INLINE u32 OC_FontGetGlyphIndex(OC_Font_t font, OC_Utf32_t codePoint)                                                                                  { return oc_font_get_glyph_index(font, codePoint); }
ORCA_INLINE OC_FontMetrics_t OC_FontGetMetrics(OC_Font_t font, f32 emSize)                                                                                  { return oc_font_get_metrics(font, emSize); }
ORCA_INLINE OC_FontMetrics_t OC_FontGetMetricsUnscaled(OC_Font_t font)                                                                                      { return oc_font_get_metrics_unscaled(font); }
ORCA_INLINE f32 OC_FontGetScaleForEmPixels(OC_Font_t font, f32 emSize)                                                                                      { return oc_font_get_scale_for_em_pixels(font, emSize); }
ORCA_INLINE OC_TextMetrics_t OC_FontTextMetricsUtr32(OC_Font_t font, f32 fontSize, OC_Str32_t codepoints)                                                   { return oc_font_text_metrics_utf32(font, fontSize, codepoints); }
ORCA_INLINE OC_TextMetrics_t OC_FontTextMetrics(OC_Font_t font, f32 fontSize, OC_Str8_t text)                                                               { return oc_font_text_metrics(font, fontSize, text); }
ORCA_INLINE OC_Image_t OC_ImageNil()                                                                                                                        { return oc_image_nil(); }
ORCA_INLINE bool OC_ImageIsNil(OC_Image_t a)                                                                                                                { return oc_image_is_nil(a); }
ORCA_INLINE OC_Image_t OC_ImageCreate(OC_CanvasRenderer_t renderer, u32 width, u32 height)                                                                  { return oc_image_create(renderer, width, height); }
ORCA_INLINE OC_Image_t OC_ImageCreateFromRgba_8(OC_CanvasRenderer_t renderer, u32 width, u32 height, u8* pixels)                                            { return oc_image_create_from_rgba8(renderer, width, height, pixels); }
ORCA_INLINE OC_Image_t OC_ImageCreateFromMemory(OC_CanvasRenderer_t renderer, OC_Str8_t mem, bool flip)                                                     { return oc_image_create_from_memory(renderer, mem, flip); }
ORCA_INLINE OC_Image_t OC_ImageCreateFromFile(OC_CanvasRenderer_t renderer, OC_File_t file, bool flip)                                                      { return oc_image_create_from_file(renderer, file, flip); }
ORCA_INLINE OC_Image_t OC_ImageCreateFromPath(OC_CanvasRenderer_t renderer, OC_Str8_t path, bool flip)                                                      { return oc_image_create_from_path(renderer, path, flip); }
ORCA_INLINE void OC_ImageDestroy(OC_Image_t image)                                                                                                          { oc_image_destroy(image); }
ORCA_INLINE void OC_ImageUploadRegionRgba8(OC_Image_t image, OC_Rect_t region, u8* pixels)                                                                  { oc_image_upload_region_rgba8(image, region, pixels); }
ORCA_INLINE OC_Vec2_t OC_ImageSize_(OC_Image_t image)                                                                                                       { return oc_image_size(image); }
ORCA_INLINE OC_RectAtlas_t* OC_RectAtlasCreate(OC_Arena_t* arena, i32 width, i32 height)                                                                    { return oc_rect_atlas_create(arena, width, height); }
ORCA_INLINE OC_Rect_t OC_RectAtlasAlloc_(OC_RectAtlas_t* atlas, i32 width, i32 height)                                                                      { return oc_rect_atlas_alloc(atlas, width, height); }
ORCA_INLINE void OC_RectAtlasRecycle(OC_RectAtlas_t* atlas, OC_Rect_t rect)                                                                                 { oc_rect_atlas_recycle(atlas, rect); }
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromRgba_8(OC_RectAtlas_t* atlas, OC_Image_t backingImage, u32 width, u32 height, u8* pixels)                { return oc_image_atlas_alloc_from_rgba8(atlas, backingImage, width, height, pixels); }
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromMemory(OC_RectAtlas_t* atlas, OC_Image_t backingImage, OC_Str8_t mem, bool flip)                         { return oc_image_atlas_alloc_from_memory(atlas, backingImage, mem, flip); }
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromFile(OC_RectAtlas_t* atlas, OC_Image_t backingImage, OC_File_t file, bool flip)                          { return oc_image_atlas_alloc_from_file(atlas, backingImage, file, flip); }
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromPath(OC_RectAtlas_t* atlas, OC_Image_t backingImage, OC_Str8_t path, bool flip)                          { return oc_image_atlas_alloc_from_path(atlas, backingImage, path, flip); }
ORCA_INLINE void OC_ImageAtlasRecycle(OC_RectAtlas_t* atlas, OC_ImageRegion_t imageRgn)                                                                     { oc_image_atlas_recycle(atlas, imageRgn); }
ORCA_INLINE void OC_MatrixPush_(OC_Mat2x3_t matrix)                                                                                                         { oc_matrix_push(matrix); }
ORCA_INLINE void OC_MatrixMultiplyPush_(OC_Mat2x3_t matrix)                                                                                                 { oc_matrix_multiply_push(matrix); }
ORCA_INLINE void OC_MatrixPop()                                                                                                                             { oc_matrix_pop(); }
ORCA_INLINE OC_Mat2x3_t OC_MatrixTop_()                                                                                                                     { return oc_matrix_top(); }
ORCA_INLINE void OC_ClipPush(f32 x, f32 y, f32 w, f32 h)                                                                                                    { oc_clip_push(x, y, w, h); }
ORCA_INLINE void OC_ClipPush(OC_Rect_t rectangle)                                                                                                           { oc_clip_push(rectangle.x, rectangle.y, rectangle.w, rectangle.h); }
ORCA_INLINE void OC_ClipPop()                                                                                                                               { oc_clip_pop(); }
ORCA_INLINE OC_Rect_t OC_ClipTop_()                                                                                                                         { return oc_clip_top(); }
ORCA_INLINE void OC_SetColor(OC_Color_t color)                                                                                                              { oc_set_color(color); }
ORCA_INLINE void OC_SetColorRgba(f32 r, f32 g, f32 b, f32 a)                                                                                                { oc_set_color_rgba(r, g, b, a); }
ORCA_INLINE void OC_SetColorRgba(OC_Color_t color)                                                                                                          { oc_set_color_rgba(color.r, color.g, color.b, color.a); }
ORCA_INLINE void OC_SetColorSrgba(f32 r, f32 g, f32 b, f32 a)                                                                                               { oc_set_color_srgba(r, g, b, a); }
ORCA_INLINE void OC_SetColorSrgba(OC_Color_t color)                                                                                                         { oc_set_color_srgba(color.r, color.g, color.b, color.a); }
ORCA_INLINE void OC_SetGradient(OC_GradientBlendSpace_t blendSpace, OC_Color_t bottomLeft, OC_Color_t bottomRight, OC_Color_t topRight, OC_Color_t topLeft) { oc_set_gradient(blendSpace, bottomLeft, bottomRight, topRight, topLeft); }
ORCA_INLINE void OC_SetWidth(f32 width)                                                                                                                     { oc_set_width(width); }
ORCA_INLINE void OC_SetTolerance(f32 tolerance)                                                                                                             { oc_set_tolerance(tolerance); }
ORCA_INLINE void OC_SetJoint(OC_JointType_t joint)                                                                                                          { oc_set_joint(joint); }
ORCA_INLINE void OC_SetMaxJointExcursion(f32 maxJointExcursion)                                                                                             { oc_set_max_joint_excursion(maxJointExcursion); }
ORCA_INLINE void OC_SetCap(OC_CapType_t cap)                                                                                                                { oc_set_cap(cap); }
ORCA_INLINE void OC_SetFont(OC_Font_t font)                                                                                                                 { oc_set_font(font); }
ORCA_INLINE void OC_SetFontSize(f32 size)                                                                                                                   { oc_set_font_size(size); }
ORCA_INLINE void OC_SetTextFlip(bool flip)                                                                                                                  { oc_set_text_flip(flip); }
ORCA_INLINE void OC_SetImage(OC_Image_t image)                                                                                                              { oc_set_image(image); }
ORCA_INLINE void OC_SetImageSourceRegion(OC_Rect_t region)                                                                                                  { oc_set_image_source_region(region); }
ORCA_INLINE OC_Color_t OC_GetColor_()                                                                                                                       { return oc_get_color(); }
ORCA_INLINE f32 OC_GetWidth()                                                                                                                               { return oc_get_width(); }
ORCA_INLINE f32 OC_GetTolerance()                                                                                                                           { return oc_get_tolerance(); }
ORCA_INLINE OC_JointType_t OC_GetJoint()                                                                                                                    { return oc_get_joint(); }
ORCA_INLINE f32 OC_GetMaxJointExcursion()                                                                                                                   { return oc_get_max_joint_excursion(); }
ORCA_INLINE OC_CapType_t OC_GetCap()                                                                                                                        { return oc_get_cap(); }
ORCA_INLINE OC_Font_t OC_GetFont()                                                                                                                          { return oc_get_font(); }
ORCA_INLINE f32 OC_GetFontSize()                                                                                                                            { return oc_get_font_size(); }
ORCA_INLINE bool OC_GetTextFlip()                                                                                                                           { return oc_get_text_flip(); }
ORCA_INLINE OC_Image_t OC_GetImage()                                                                                                                        { return oc_get_image(); }
ORCA_INLINE OC_Rect_t OC_GetImageSourceRegion_()                                                                                                            { return oc_get_image_source_region(); }
ORCA_INLINE OC_Vec2_t OC_GetPosition_()                                                                                                                     { return oc_get_position(); }
ORCA_INLINE void OC_MoveTo(f32 x, f32 y)                                                                                                                    { oc_move_to(x, y); }
ORCA_INLINE void OC_MoveTo(OC_Vec2_t pos)                                                                                                                   { oc_move_to(pos.x, pos.y); }
ORCA_INLINE void OC_LineTo(f32 x, f32 y)                                                                                                                    { oc_line_to(x, y); }
ORCA_INLINE void OC_LineTo(OC_Vec2_t pos)                                                                                                                   { oc_line_to(pos.x, pos.y); }
ORCA_INLINE void OC_QuadraticTo(f32 x1, f32 y1, f32 x2, f32 y2)                                                                                             { oc_quadratic_to(x1, y1, x2, y2); }
ORCA_INLINE void OC_QuadraticTo(OC_Vec2_t p1, OC_Vec2_t p2)                                                                                                 { oc_quadratic_to(p1.x, p1.y, p2.x, p2.y); }
ORCA_INLINE void OC_CubicTo(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)                                                                                 { oc_cubic_to(x1, y1, x2, y2, x3, y3); }
ORCA_INLINE void OC_CubicTo(OC_Vec2_t p1, OC_Vec2_t p2, OC_Vec2_t p3)                                                                                       { oc_cubic_to(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y); }
ORCA_INLINE void OC_ClosePath()                                                                                                                             { oc_close_path(); }
ORCA_INLINE OC_Rect_t OC_GlyphOutlines_(oc_str32 glyphIndices)                                                                                              { return oc_glyph_outlines(glyphIndices); }
ORCA_INLINE void OC_CodepointsOutlines(oc_str32 string)                                                                                                     { oc_codepoints_outlines(string); }
ORCA_INLINE void OC_TextOutlines(OC_Str8_t string)                                                                                                          { oc_text_outlines(string); }
ORCA_INLINE void OC_Clear()                                                                                                                                 { oc_clear(); }
ORCA_INLINE void OC_Fill()                                                                                                                                  { oc_fill(); }
ORCA_INLINE void OC_Stroke()                                                                                                                                { oc_stroke(); }
ORCA_INLINE void OC_RectangleFill(f32 x, f32 y, f32 w, f32 h)                                                                                               { oc_rectangle_fill(x, y, w, h); }
ORCA_INLINE void OC_RectangleFill(OC_Rect_t rectangle)                                                                                                      { oc_rectangle_fill(rectangle.x, rectangle.y, rectangle.w, rectangle.h); }
ORCA_INLINE void OC_RectangleStroke(f32 x, f32 y, f32 w, f32 h)                                                                                             { oc_rectangle_stroke(x, y, w, h); }
ORCA_INLINE void OC_RectangleStroke(OC_Rect_t rectangle)                                                                                                    { oc_rectangle_stroke(rectangle.x, rectangle.y, rectangle.w, rectangle.h); }
ORCA_INLINE void OC_RoundedRectangleFill(f32 x, f32 y, f32 w, f32 h, f32 r)                                                                                 { oc_rounded_rectangle_fill(x, y, w, h, r); }
ORCA_INLINE void OC_RoundedRectangleFill(OC_Rect_t rectangle, f32 r)                                                                                        { oc_rounded_rectangle_fill(rectangle.x, rectangle.y, rectangle.w, rectangle.h, r); }
ORCA_INLINE void OC_RoundedRectangleStroke(f32 x, f32 y, f32 w, f32 h, f32 r)                                                                               { oc_rounded_rectangle_stroke(x, y, w, h, r); }
ORCA_INLINE void OC_RoundedRectangleStroke(OC_Rect_t rectangle, f32 r)                                                                                      { oc_rounded_rectangle_stroke(rectangle.x, rectangle.y, rectangle.w, rectangle.h, r); }
ORCA_INLINE void OC_EllipseFill(f32 x, f32 y, f32 rx, f32 ry)                                                                                               { oc_ellipse_fill(x, y, rx, ry); }
ORCA_INLINE void OC_EllipseStroke(f32 x, f32 y, f32 rx, f32 ry)                                                                                             { oc_ellipse_stroke(x, y, rx, ry); }
ORCA_INLINE void OC_CircleFill(f32 x, f32 y, f32 r)                                                                                                         { oc_circle_fill(x, y, r); }
ORCA_INLINE void OC_CircleStroke(f32 x, f32 y, f32 r)                                                                                                       { oc_circle_stroke(x, y, r); }
ORCA_INLINE void OC_Arc(f32 x, f32 y, f32 r, f32 arcAngle, f32 startAngle)                                                                                  { oc_arc(x, y, r, arcAngle, startAngle); }
ORCA_INLINE void OC_TextFill(f32 x, f32 y, OC_Str8_t text)                                                                                                  { oc_text_fill(x, y, text); }
ORCA_INLINE void OC_ImageDraw(OC_Image_t image, OC_Rect_t rect)                                                                                             { oc_image_draw(image, rect); }
ORCA_INLINE void OC_ImageDrawRegion(OC_Image_t image, OC_Rect_t srcRegion, OC_Rect_t dstRegion)                                                             { oc_image_draw_region(image, srcRegion, dstRegion); }

// +==============================+
// |        Orca File API         |
// +==============================+
ORCA_INLINE OC_IoCmp_t OC_IoWaitSingleReq(OC_IoReq_t* req)                                                                                                      { return oc_io_wait_single_req(req); }
ORCA_INLINE OC_File_t OC_FileNil()                                                                                                                              { return oc_file_nil(); }
ORCA_INLINE bool OC_FileIsNil(OC_File_t handle)                                                                                                                 { return oc_file_is_nil(handle); }
ORCA_INLINE OC_File_t OC_FileOpen(OC_Str8_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)                                                             { return oc_file_open(path, rights, flags); }
ORCA_INLINE OC_File_t OC_FileOpenAt(OC_File_t dir, OC_Str8_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)                                            { return oc_file_open_at(dir, path, rights, flags); }
ORCA_INLINE void OC_FileClose(OC_File_t file)                                                                                                                   { oc_file_close(file); }
ORCA_INLINE OC_IoError_t OC_FileLastError(OC_File_t handle)                                                                                                     { return oc_file_last_error(handle); }
ORCA_INLINE i64 OC_FilePos(OC_File_t file)                                                                                                                      { return oc_file_pos(file); }
ORCA_INLINE i64 OC_FileSeek(OC_File_t file, i64 offset, OC_FileWhence_t whence)                                                                                 { return oc_file_seek(file, offset, whence); }
ORCA_INLINE u64 OC_FileWrite(OC_File_t file, u64 size, char* buffer)                                                                                            { return oc_file_write(file, size, buffer); }
ORCA_INLINE u64 OC_FileRead(OC_File_t file, u64 size, char* buffer)                                                                                             { return oc_file_read(file, size, buffer); }
ORCA_INLINE OC_FileStatus_t OC_FileGetStatus(OC_File_t file)                                                                                                    { return oc_file_get_status(file); }
ORCA_INLINE u64 OC_FileSize(OC_File_t file)                                                                                                                     { return oc_file_size(file); }
ORCA_INLINE OC_File_t OC_FileOpenWithRequest(OC_Str8_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)                                                  { return oc_file_open_with_request(path, rights, flags); }
ORCA_INLINE OC_FileOpenWithDialogResult_t OC_FileOpenWithDialog(OC_Arena_t* arena, OC_FileAccess_t rights, OC_FileOpenFlags_t flags, OC_FileDialogDesc_t* desc) { return oc_file_open_with_dialog(arena, rights, flags, desc); }

// +==============================+
// |         Orca UI API          |
// +==============================+
ORCA_INLINE void OC_UiInit(OC_UiContext_t* context)                                                        { oc_ui_init(context); }
ORCA_INLINE OC_UiContext_t* OC_UiGetContext()                                                              { return oc_ui_get_context(); }
ORCA_INLINE void OC_UiSetContext(OC_UiContext_t* context)                                                  { oc_ui_set_context(context); }
ORCA_INLINE void OC_UiProcessEvent(OC_Event_t* event)                                                      { oc_ui_process_event(event); }
ORCA_INLINE void OC_UiBeginFrame(OC_Vec2_t size, OC_UiStyle_t* defaultStyle, OC_UiStyleMask_t mask)        { oc_ui_begin_frame(size, defaultStyle, mask); }
ORCA_INLINE void OC_UiEndFrame()                                                                           { oc_ui_end_frame(); }
ORCA_INLINE void OC_UiDraw()                                                                               { oc_ui_draw(); }
ORCA_INLINE OC_UiSig_t OC_UiLabel(const char* label)                                                       { return oc_ui_label(label); }
ORCA_INLINE OC_UiSig_t OC_UiLabelStr8(OC_Str8_t label)                                                     { return oc_ui_label_str8(label); }
ORCA_INLINE OC_UiSig_t OC_UiButton(const char* label)                                                      { return oc_ui_button(label); }
ORCA_INLINE OC_UiSig_t OC_UiCheckbox(const char* name, bool* checked)                                      { return oc_ui_checkbox(name, checked); }
ORCA_INLINE OC_UiBox_t* OC_UiSlider(const char* name, f32* value)                                          { return oc_ui_slider(name, value); }
ORCA_INLINE OC_UiBox_t* OC_UiScrollbar(const char* name, f32 thumbRatio, f32* scrollValue)                 { return oc_ui_scrollbar(name, thumbRatio, scrollValue); }
ORCA_INLINE OC_UiTextBoxResult_t OC_UiTextBox(const char* name, OC_Arena_t* arena, OC_Str8_t text)         { return oc_ui_text_box(name, arena, text); }
ORCA_INLINE OC_UiSelectPopupInfo_t OC_UiSelectPopup(const char* name, OC_UiSelectPopupInfo_t* info)        { return oc_ui_select_popup(name, info); }
ORCA_INLINE OC_UiRadioGroupInfo_t OC_UiRadioGroup(const char* name, OC_UiRadioGroupInfo_t* info)           { return oc_ui_radio_group(name, info); }
ORCA_INLINE void OC_UiPanelBegin(const char* name, OC_UiFlags_t flags)                                     { oc_ui_panel_begin(name, flags); }
ORCA_INLINE void OC_UiPanelEnd()                                                                           { oc_ui_panel_end(); }
ORCA_INLINE void OC_UiMenuBarBegin(const char* name)                                                       { oc_ui_menu_bar_begin(name); }
ORCA_INLINE void OC_UiMenuBarEnd()                                                                         { oc_ui_menu_bar_end(); }
ORCA_INLINE void OC_UiMenuBegin(const char* label)                                                         { oc_ui_menu_begin(label); }
ORCA_INLINE void OC_UiMenuEnd()                                                                            { oc_ui_menu_end(); }
ORCA_INLINE OC_UiSig_t OC_UiMenuButton(const char* label)                                                  { return oc_ui_menu_button(label); }
ORCA_INLINE void OC_UiTooltip(const char* label)                                                           { oc_ui_tooltip(label); }
ORCA_INLINE void OC_UiStyleNext(OC_UiStyle_t* style, OC_UiStyleMask_t mask)                                { oc_ui_style_next(style, mask); }
ORCA_INLINE void OC_UiPatternPush(OC_Arena_t* arena, OC_UiPattern_t* pattern, OC_UiSelector_t selector)    { oc_ui_pattern_push(arena, pattern, selector); }
ORCA_INLINE OC_UiPattern_t OC_UiPatternAll()                                                               { return oc_ui_pattern_all(); }
ORCA_INLINE OC_UiPattern_t OC_UiPatternOwner()                                                             { return oc_ui_pattern_owner(); }
ORCA_INLINE void OC_UiStyleMatchBefore(OC_UiPattern_t pattern, OC_UiStyle_t* style, OC_UiStyleMask_t mask) { oc_ui_style_match_before(pattern, style, mask); }
ORCA_INLINE void OC_UiStyleMatchAfter(OC_UiPattern_t pattern, OC_UiStyle_t* style, OC_UiStyleMask_t mask)  { oc_ui_style_match_after(pattern, style, mask); }

#define OC_UiFrame_(size, style, mask) oc_defer_loop(OC_UiBeginFrame((size), (style), (mask)), OC_UiEndFrame())
#define OC_UiPanel(s, f)               oc_defer_loop(OC_UiPanelBegin(s, f), OC_UiPanelEnd())
#define OC_UiMenuBar_(name)            oc_defer_loop(OC_UiMenuBarBegin(name), OC_UiMenuBarEnd())
#define OC_UiMenu_(label)              oc_defer_loop(OC_UiMenuBegin(label), OC_UiMenuEnd())

// +--------------------------------------------------------------+
// |                        Orca Util API                         |
// +--------------------------------------------------------------+
ORCA_INLINE void OC_ArenaInit(OC_Arena_t* arena)                                                                                          { oc_arena_init(arena); }
ORCA_INLINE void OC_ArenaInitWithOptions(OC_Arena_t* arena, OC_ArenaOptions_t* options)                                                   { oc_arena_init_with_options(arena, options); }
ORCA_INLINE void OC_ArenaCleanup(OC_Arena_t* arena)                                                                                       { oc_arena_cleanup(arena); }
ORCA_INLINE void* OC_ArenaPush(OC_Arena_t* arena, u64 size)                                                                               { return oc_arena_push(arena, size); }
ORCA_INLINE void* OC_ArenaPushAligned(OC_Arena_t* arena, u64 size, u32 alignment)                                                         { return oc_arena_push_aligned(arena, size, alignment); }
ORCA_INLINE void OC_ArenaClear(OC_Arena_t* arena)                                                                                         { oc_arena_clear(arena); }
ORCA_INLINE OC_ArenaScope_t OC_ArenaScopeBegin(OC_Arena_t* arena)                                                                         { return oc_arena_scope_begin(arena); }
ORCA_INLINE void OC_ArenaScopeEnd(OC_ArenaScope_t scope)                                                                                  { oc_arena_scope_end(scope); }
ORCA_INLINE OC_ArenaScope_t OC_ScratchBegin()                                                                                             { return oc_scratch_begin(); }
ORCA_INLINE void OC_ScratchEnd(OC_ArenaScope_t scope)                                                                                     { oc_scratch_end(scope); }
ORCA_INLINE OC_ArenaScope_t OC_ScratchBeginNext(OC_Arena_t* used)                                                                         { return oc_scratch_begin_next(used); }
ORCA_INLINE void OC_ListInit(OC_List_t* list)                                                                                             { oc_list_init(list); }
ORCA_INLINE bool OC_ListEmpty(OC_List_t list)                                                                                             { return oc_list_empty(list); }
ORCA_INLINE OC_ListElement_t* OC_ListBegin(OC_List_t list)                                                                                { return oc_list_begin(list); }
ORCA_INLINE OC_ListElement_t* OC_ListEnd(OC_List_t* list)                                                                                 { return oc_list_end(list); }
ORCA_INLINE OC_ListElement_t* OC_ListLast(OC_List_t list)                                                                                 { return oc_list_last(list); }
ORCA_INLINE void OC_ListInsert(OC_List_t* list, OC_ListElement_t* afterElt, OC_ListElement_t* elt)                                        { oc_list_insert(list, afterElt, elt); }
ORCA_INLINE void OC_ListInsertBefore(OC_List_t* list, OC_ListElement_t* beforeElt, OC_ListElement_t* elt)                                 { oc_list_insert_before(list, beforeElt, elt); }
ORCA_INLINE void OC_ListRemove(OC_List_t* list, OC_ListElement_t* elt)                                                                    { oc_list_remove(list, elt); }
ORCA_INLINE void OC_ListPushFront(OC_List_t* list, OC_ListElement_t* elt)                                                                 { oc_list_push_front(list, elt); }
ORCA_INLINE OC_ListElement_t* OC_ListPopFront(OC_List_t* list)                                                                            { return oc_list_pop_front(list); }
ORCA_INLINE void OC_ListPushBack(OC_List_t* list, OC_ListElement_t* elt)                                                                  { oc_list_push_back(list, elt); }
ORCA_INLINE OC_ListElement_t* OC_ListPopBack(OC_List_t* list)                                                                             { return oc_list_pop_back(list); }
ORCA_INLINE OC_Str8_t OC_Str8FromBuffer_(u64 len, char* buffer)                                                                           { return oc_str8_from_buffer(len, buffer); }
ORCA_INLINE OC_Str8_t OC_Str8Slice(OC_Str8_t s, u64 start, u64 end)                                                                       { return oc_str8_slice(s, start, end); }
ORCA_INLINE OC_Str8_t OC_Str8PushBuffer_(OC_Arena_t* arena, u64 len, char* buffer)                                                        { return oc_str8_push_buffer(arena, len, buffer); }
ORCA_INLINE OC_Str8_t OC_Str8PushCstring_(OC_Arena_t* arena, const char* str)                                                             { return oc_str8_push_cstring(arena, str); }
ORCA_INLINE OC_Str8_t OC_Str8PushCopy(OC_Arena_t* arena, OC_Str8_t s)                                                                     { return oc_str8_push_copy(arena, s); }
ORCA_INLINE OC_Str8_t OC_Str8PushSlice(OC_Arena_t* arena, OC_Str8_t s, u64 start, u64 end)                                                { return oc_str8_push_slice(arena, s, start, end); }
ORCA_INLINE OC_Str8_t OC_Str8Pushfv_(OC_Arena_t* arena, const char* format, va_list args)                                                 { return oc_str8_pushfv(arena, format, args); }
ORCA_INLINE int OC_Str8Cmp(OC_Str8_t s1, OC_Str8_t s2)                                                                                    { return oc_str8_cmp(s1, s2); }
ORCA_INLINE char* OC_Str8ToCstring(OC_Arena_t* arena, OC_Str8_t string)                                                                   { return oc_str8_to_cstring(arena, string); }
ORCA_INLINE void OC_Str8ListPush(OC_Arena_t* arena, OC_Str8List_t* list, OC_Str8_t str)                                                   { oc_str8_list_push(arena, list, str); }
ORCA_INLINE OC_Str8_t OC_Str8ListCollate(OC_Arena_t* arena, OC_Str8List_t list, OC_Str8_t prefix, OC_Str8_t separator, OC_Str8_t postfix) { return oc_str8_list_collate(arena, list, prefix, separator, postfix); }
ORCA_INLINE OC_Str8_t OC_Str8ListJoin_(OC_Arena_t* arena, OC_Str8List_t list)                                                             { return oc_str8_list_join(arena, list); }
ORCA_INLINE OC_Str8List_t OC_Str8Split(OC_Arena_t* arena, OC_Str8_t str, OC_Str8List_t separators)                                        { return oc_str8_split(arena, str, separators); }
ORCA_INLINE OC_Str8_t OC_PathSliceDirectory(OC_Str8_t path)                                                                               { return oc_path_slice_directory(path); }
ORCA_INLINE OC_Str8_t OC_PathSliceFilename(OC_Str8_t path)                                                                                { return oc_path_slice_filename(path); }
ORCA_INLINE OC_Str8List_t OC_PathSplit(OC_Arena_t* arena, OC_Str8_t path)                                                                 { return oc_path_split(arena, path); }
ORCA_INLINE OC_Str8_t OC_PathJoin_(OC_Arena_t* arena, OC_Str8List_t elements)                                                             { return oc_path_join(arena, elements); }
ORCA_INLINE OC_Str8_t OC_PathAppend(OC_Arena_t* arena, OC_Str8_t parent, OC_Str8_t relPath)                                               { return oc_path_append(arena, parent, relPath); }
ORCA_INLINE bool OC_PathIsAbsolute(OC_Str8_t path)                                                                                        { return oc_path_is_absolute(path); }

#define OC_Str8Pushf(arena, format, ...)                    oc_str8_pushf(arena, format, ##__VA_ARGS__)
#define OC_Str8ListPushf(arena, list, format, ...)          oc_str8_list_pushf(arena, list, format, ##__VA_ARGS__)
#define OC_ArenaPushType(arena, type)                       ((type*)OC_ArenaPushAligned(arena, sizeof(type), _Alignof(type)))
#define OC_ArenaPushArray(arena, type, count)               ((type*)OC_ArenaPushAligned(arena, sizeof(type) * (count), _Alignof(type)))
#define OC_ScratchEnd(scope)                                OC_ArenaScopeEnd(scope)
#define OC_ListNext(element)                                (element)->next
#define OC_ListPrev(element)                                (element)->prev
#define OC_ListEntry(element, type, member)                 oc_container_of(element, type, member)
#define OC_ListNextEntry(list, element, type, member)       ((element->member.next != OC_ListEnd(list)) ? OC_ListEntry(element->member.next, type, member) : 0)
#define OC_ListPrevEntry(list, element, type, member)       ((element->member.prev != OC_ListEnd(list)) ? OC_ListEntry(element->member.prev, type, member) : 0)
#define OC_ListFirstEntry(list, type, member)               (oc_list_checked_entry(OC_ListBegin(list), type, member))
#define OC_ListLastEntry(list, type, member)                (oc_list_checked_entry(OC_ListLast(list), type, member))
#define OC_ListFor(list, element, type, member)             for (type* element = oc_list_checked_entry(OC_ListBegin(list), type, member); element != 0; element = oc_list_checked_entry(element->member.next, type, member))
#define OC_ListForReverse(list, element, type, member)      for (type* element = oc_list_checked_entry(OC_ListLast(list), type, member); element != 0; element = oc_list_checked_entry(element->member.prev, type, member))
#define OC_ListForSafe(list, element, type, member)         for (type* element = oc_list_checked_entry(OC_ListBegin(list), type, member), *__tmp = element ? oc_list_checked_entry(element->member.next, type, member) : 0; element != 0; element = __tmp, __tmp = element ? oc_list_checked_entry(element->member.next, type, member) : 0)
#define OC_ListPopFrontEntry(list, type, member)            (OC_ListEmpty(*list) ? 0 : OC_ListEntry(OC_ListPopFront(list), type, member))
#define OC_ListPopBackEntry(list, type, member)             (OC_ListEmpty(*list) ? 0 : OC_ListEntry(OC_ListPopBack(list), type, member))
#define OC_Log_I(message, ...)                              oc_log_info(message, ##__VA_ARGS__) 
#define OC_Log_W(message, ...)                              oc_log_warning(message, ##__VA_ARGS__) 
#define OC_Log_E(message, ...)                              oc_log_error(message, ...) 
#define OC_AssertFail(file, function, line, test, fmt, ...) oc_assert_fail(file, function, line, #test, fmt, ##__VA_ARGS__)
#define OC_Assert(test, message, ...)                       OC_ASSERT(test, message, ##__VA_ARGS__) 
#define OC_AbortExt(file, function, line, fmt, ...)         oc_abort_ext(file, function, line, fmt, ##__VA_ARGS__) 
#define OC_Abort(message, ...)                              OC_ABORT(message, ##__VA_ARGS__) 

#endif //ORCA_COMPILATION

#endif //  _GY_ORCA_ALIASES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Types
OC_Vec2_t
OC_Vec3_t
OC_Vec4_t
OC_Vec2i_t
OC_Rect_t
OC_Mat2x3_t
OC_Color_t
OC_Str8_t
OC_Str32_t
OC_Utf32_t
OC_Str8List_t
OC_MouseButton_t
OC_ScanCode_t
OC_KeyCode_t
OC_Surface_t
OC_ColorSpace_t
OC_CanvasRenderer_t
OC_CanvasContext_t
OC_File_t
OC_Font_t
OC_Image_t
OC_RectAtlas_t
OC_ImageRegion_t
OC_GradientBlendSpace_t
OC_UnicodeRange_t
OC_Arena_t
OC_ArenaOptions_t
OC_ArenaScope_t
OC_FontMetrics_t
OC_TextMetrics_t
OC_JointType_t
OC_CapType_t
OC_IoCmp_t
OC_IoReq_t
OC_FileAccess_t
OC_FileOpenFlags_t
OC_FileWhence_t
OC_FileDialogDesc_t
OC_IoError_t
OC_FileStatus_t
OC_FileOpenWithDialogResult_t
OC_UiContext_t
OC_Event_t
OC_UiStyle_t
OC_UiStyleMask_t
OC_UiSig_t
OC_UiBox_t
OC_UiTextBoxResult_t
OC_UiSelectPopupInfo_t
OC_UiRadioGroupInfo_t
OC_UiFlags_t
OC_UiPattern_t
OC_UiSelector_t
OC_List_t
OC_ListElement_t
@Functions
OC_OnInit()
OC_OnMouseDown(OC_MouseButton_t button)
OC_OnMouseUp(OC_MouseButton_t button)
OC_OnMouseEnter()
OC_OnMouseLeave()
OC_OnMouseMove(f32 x, f32 y, f32 deltaX, f32 deltaY)
OC_OnMouseWheel(f32 deltaX, f32 deltaY)
OC_OnKeyDown(OC_ScanCode_t scan, OC_KeyCode_t key)
OC_OnKeyUp(OC_ScanCode_t scan, OC_KeyCode_t key)
OC_OnFrameRefresh()
OC_OnResize(f32 width, f32 height)
OC_OnRawEvent(OC_Event_t* event)
OC_OnTerminate()
ORCA_INLINE void OC_WindowSetTitle(OC_Str8_t title)
ORCA_INLINE void OC_WindowSetSize(OC_Vec2_t size)
ORCA_INLINE void OC_RequestQuit()
ORCA_INLINE OC_Surface_t OC_SurfaceNil()
ORCA_INLINE bool OC_SurfaceIsNil(OC_Surface_t surface)
ORCA_INLINE void OC_SurfaceDestroy(OC_Surface_t surface)
ORCA_INLINE OC_Vec2_t OC_SurfaceGetSize_(OC_Surface_t surface)
ORCA_INLINE OC_Vec2_t OC_SurfaceContentsScaling_(OC_Surface_t surface)
ORCA_INLINE void OC_SurfaceBringToFront(OC_Surface_t surface)
ORCA_INLINE void OC_SurfaceSendToBack(OC_Surface_t surface)
ORCA_INLINE bool OC_SurfaceGetHidden(OC_Surface_t surface)
ORCA_INLINE void OC_SurfaceSetHidden(OC_Surface_t surface, bool hidden)
ORCA_INLINE OC_Surface_t OC_GlesSurfaceCreate()
ORCA_INLINE void OC_GlesSurfaceMakeCurrent(OC_Surface_t surface)
ORCA_INLINE void OC_GlesSurfaceSwapInterval(OC_Surface_t surface, int interval)
ORCA_INLINE void OC_GlesSurfaceSwapBuffers(OC_Surface_t surface)
ORCA_INLINE OC_Color_t OC_ColorRgba(f32 r, f32 g, f32 b, f32 a)
ORCA_INLINE OC_Color_t OC_ColorSrgba(f32 r, f32 g, f32 b, f32 a)
ORCA_INLINE OC_Color_t OC_ColorConvert(OC_Color_t color, OC_ColorSpace_t colorSpace)
ORCA_INLINE OC_CanvasRenderer_t OC_CanvasRendererNil()
ORCA_INLINE bool OC_CanvasRendererIsNil(OC_CanvasRenderer_t renderer)
ORCA_INLINE OC_CanvasRenderer_t OC_CanvasRendererCreate()
ORCA_INLINE void OC_CanvasRendererDestroy(OC_CanvasRenderer_t renderer)
ORCA_INLINE void OC_CanvasRender(OC_CanvasRenderer_t renderer, OC_CanvasContext_t context, OC_Surface_t surface)
ORCA_INLINE void OC_CanvasPresent(OC_CanvasRenderer_t renderer, OC_Surface_t surface)
ORCA_INLINE OC_Surface_t OC_CanvasSurfaceCreate(OC_CanvasRenderer_t renderer)
ORCA_INLINE void OC_CanvasSurfaceSwapInterval(OC_Surface_t surface, int swap)
ORCA_INLINE OC_CanvasContext_t OC_CanvasContextNil()
ORCA_INLINE bool OC_CanvasContextIsNil(OC_CanvasContext_t context)
ORCA_INLINE OC_CanvasContext_t OC_CanvasContextCreate()
ORCA_INLINE void OC_CanvasContextDestroy(OC_CanvasContext_t context)
ORCA_INLINE OC_CanvasContext_t OC_CanvasContextSelect(OC_CanvasContext_t context)
ORCA_INLINE void OC_CanvasContextSetMsaaSampleCount(OC_CanvasContext_t context, u32 sampleCount)
ORCA_INLINE OC_Font_t OC_FontNil()
ORCA_INLINE bool OC_FontIsNil(OC_Font_t font)
ORCA_INLINE OC_Font_t OC_FontCreateFromMemory(OC_Str8_t mem, u32 rangeCount, OC_UnicodeRange_t* ranges)
ORCA_INLINE OC_Font_t OC_FontCreateFromFile(OC_File_t file, u32 rangeCount, OC_UnicodeRange_t* ranges)
ORCA_INLINE OC_Font_t OC_FontCreateFromPath(OC_Str8_t path, u32 rangeCount, OC_UnicodeRange_t* ranges)
ORCA_INLINE void OC_FontDestroy(OC_Font_t font)
ORCA_INLINE OC_Str32_t OC_FontGetGlyphIndices(OC_Font_t font, OC_Str32_t codePoints, OC_Str32_t backing)
ORCA_INLINE OC_Str32_t OC_FontPushGlyphIndices(OC_Arena_t* arena, OC_Font_t font, OC_Str32_t codePoints)
ORCA_INLINE u32 OC_FontGetGlyphIndex(OC_Font_t font, OC_Utf32_t codePoint)
ORCA_INLINE OC_FontMetrics_t OC_FontGetMetrics(OC_Font_t font, f32 emSize)
ORCA_INLINE OC_FontMetrics_t OC_FontGetMetricsUnscaled(OC_Font_t font)
ORCA_INLINE f32 OC_FontGetScaleForEmPixels(OC_Font_t font, f32 emSize)
ORCA_INLINE OC_TextMetrics_t OC_FontTextMetricsUtr32(OC_Font_t font, f32 fontSize, OC_Str32_t codepoints)
ORCA_INLINE OC_TextMetrics_t OC_FontTextMetrics(OC_Font_t font, f32 fontSize, OC_Str8_t text)
ORCA_INLINE OC_Image_t OC_ImageNil()
ORCA_INLINE bool OC_ImageIsNil(OC_Image_t a)
ORCA_INLINE OC_Image_t OC_ImageCreate(OC_CanvasRenderer_t renderer, u32 width, u32 height)
ORCA_INLINE OC_Image_t OC_ImageCreateFromRgba_8(OC_CanvasRenderer_t renderer, u32 width, u32 height, u8* pixels)
ORCA_INLINE OC_Image_t OC_ImageCreateFromMemory(OC_CanvasRenderer_t renderer, OC_Str8_t mem, bool flip)
ORCA_INLINE OC_Image_t OC_ImageCreateFromFile(OC_CanvasRenderer_t renderer, OC_File_t file, bool flip)
ORCA_INLINE OC_Image_t OC_ImageCreateFromPath(OC_CanvasRenderer_t renderer, OC_Str8_t path, bool flip)
ORCA_INLINE void OC_ImageDestroy(OC_Image_t image)
ORCA_INLINE void OC_ImageUploadRegionRgba8(OC_Image_t image, OC_Rect_t region, u8* pixels)
ORCA_INLINE OC_Vec2_t OC_ImageSize_(OC_Image_t image)
ORCA_INLINE OC_RectAtlas_t* OC_RectAtlasCreate(OC_Arena_t* arena, i32 width, i32 height)
ORCA_INLINE OC_Rect_t OC_RectAtlasAlloc_(OC_RectAtlas_t* atlas, i32 width, i32 height)
ORCA_INLINE void OC_RectAtlasRecycle(OC_RectAtlas_t* atlas, OC_Rect_t rect)
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromRgba_8(OC_RectAtlas_t* atlas, OC_Image_t backingImage, u32 width, u32 height, u8* pixels)
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromMemory(OC_RectAtlas_t* atlas, OC_Image_t backingImage, OC_Str8_t mem, bool flip)
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromFile(OC_RectAtlas_t* atlas, OC_Image_t backingImage, OC_File_t file, bool flip)
ORCA_INLINE OC_ImageRegion_t OC_ImageAtlasAllocFromPath(OC_RectAtlas_t* atlas, OC_Image_t backingImage, OC_Str8_t path, bool flip)
ORCA_INLINE void OC_ImageAtlasRecycle(OC_RectAtlas_t* atlas, OC_ImageRegion_t imageRgn)
ORCA_INLINE void OC_MatrixPush_(OC_Mat2x3_t matrix)
ORCA_INLINE void OC_MatrixMultiplyPush_(OC_Mat2x3_t matrix)
ORCA_INLINE void OC_MatrixPop()
ORCA_INLINE OC_Mat2x3_t OC_MatrixTop_()
ORCA_INLINE void OC_ClipPush(f32 x, f32 y, f32 w, f32 h)
ORCA_INLINE void OC_ClipPush(OC_Rect_t rectangle)
ORCA_INLINE void OC_ClipPop()
ORCA_INLINE OC_Rect_t OC_ClipTop_()
ORCA_INLINE void OC_SetColor(OC_Color_t color)
ORCA_INLINE void OC_SetColorRgba(f32 r, f32 g, f32 b, f32 a)
ORCA_INLINE void OC_SetColorRgba(OC_Color_t color)
ORCA_INLINE void OC_SetColorSrgba(f32 r, f32 g, f32 b, f32 a)
ORCA_INLINE void OC_SetColorSrgba(OC_Color_t color)
ORCA_INLINE void OC_SetGradient(OC_GradientBlendSpace_t blendSpace, OC_Color_t bottomLeft, OC_Color_t bottomRight, OC_Color_t topRight, OC_Color_t topLeft)
ORCA_INLINE void OC_SetWidth(f32 width)
ORCA_INLINE void OC_SetTolerance(f32 tolerance)
ORCA_INLINE void OC_SetJoint(OC_JointType_t joint)
ORCA_INLINE void OC_SetMaxJointExcursion(f32 maxJointExcursion)
ORCA_INLINE void OC_SetCap(OC_CapType_t cap)
ORCA_INLINE void OC_SetFont(OC_Font_t font)
ORCA_INLINE void OC_SetFontSize(f32 size)
ORCA_INLINE void OC_SetTextFlip(bool flip)
ORCA_INLINE void OC_SetImage(OC_Image_t image)
ORCA_INLINE void OC_SetImageSourceRegion(OC_Rect_t region)
ORCA_INLINE OC_Color_t OC_GetColor_()
ORCA_INLINE f32 OC_GetWidth()
ORCA_INLINE f32 OC_GetTolerance()
ORCA_INLINE OC_JointType_t OC_GetJoint()
ORCA_INLINE f32 OC_GetMaxJointExcursion()
ORCA_INLINE OC_CapType_t OC_GetCap()
ORCA_INLINE OC_Font_t OC_GetFont()
ORCA_INLINE f32 OC_GetFontSize()
ORCA_INLINE bool OC_GetTextFlip()
ORCA_INLINE OC_Image_t OC_GetImage()
ORCA_INLINE OC_Rect_t OC_GetImageSourceRegion_()
ORCA_INLINE OC_Vec2_t OC_GetPosition_()
ORCA_INLINE void OC_MoveTo(f32 x, f32 y)
ORCA_INLINE void OC_MoveTo(OC_Vec2_t pos)
ORCA_INLINE void OC_LineTo(f32 x, f32 y)
ORCA_INLINE void OC_LineTo(OC_Vec2_t pos)
ORCA_INLINE void OC_QuadraticTo(f32 x1, f32 y1, f32 x2, f32 y2)
ORCA_INLINE void OC_QuadraticTo(OC_Vec2_t p1, OC_Vec2_t p2)
ORCA_INLINE void OC_CubicTo(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
ORCA_INLINE void OC_CubicTo(OC_Vec2_t p1, OC_Vec2_t p2, OC_Vec2_t p3)
ORCA_INLINE void OC_ClosePath()
ORCA_INLINE OC_Rect_t OC_GlyphOutlines_(oc_str32 glyphIndices)
ORCA_INLINE void OC_CodepointsOutlines(oc_str32 string)
ORCA_INLINE void OC_TextOutlines(OC_Str8_t string)
ORCA_INLINE void OC_Clear()
ORCA_INLINE void OC_Fill()
ORCA_INLINE void OC_Stroke()
ORCA_INLINE void OC_RectangleFill(f32 x, f32 y, f32 w, f32 h)
ORCA_INLINE void OC_RectangleFill(OC_Rect_t rectangle)
ORCA_INLINE void OC_RectangleStroke(f32 x, f32 y, f32 w, f32 h)
ORCA_INLINE void OC_RectangleStroke(OC_Rect_t rectangle)
ORCA_INLINE void OC_RoundedRectangleFill(f32 x, f32 y, f32 w, f32 h, f32 r)
ORCA_INLINE void OC_RoundedRectangleFill(OC_Rect_t rectangle, f32 r)
ORCA_INLINE void OC_RoundedRectangleStroke(f32 x, f32 y, f32 w, f32 h, f32 r)
ORCA_INLINE void OC_RoundedRectangleStroke(OC_Rect_t rectangle, f32 r)
ORCA_INLINE void OC_EllipseFill(f32 x, f32 y, f32 rx, f32 ry)
ORCA_INLINE void OC_EllipseStroke(f32 x, f32 y, f32 rx, f32 ry)
ORCA_INLINE void OC_CircleFill(f32 x, f32 y, f32 r)
ORCA_INLINE void OC_CircleStroke(f32 x, f32 y, f32 r)
ORCA_INLINE void OC_Arc(f32 x, f32 y, f32 r, f32 arcAngle, f32 startAngle)
ORCA_INLINE void OC_TextFill(f32 x, f32 y, OC_Str8_t text)
ORCA_INLINE void OC_ImageDraw(OC_Image_t image, OC_Rect_t rect)
ORCA_INLINE void OC_ImageDrawRegion(OC_Image_t image, OC_Rect_t srcRegion, OC_Rect_t dstRegion)
ORCA_INLINE OC_IoCmp_t OC_IoWaitSingleReq(OC_IoReq_t* req)
ORCA_INLINE OC_File_t OC_FileNil()
ORCA_INLINE bool OC_FileIsNil(OC_File_t handle)
ORCA_INLINE OC_File_t OC_FileOpen(OC_Str8_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)
ORCA_INLINE OC_File_t OC_FileOpenAt(OC_File_t dir, OC_Str8_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)
ORCA_INLINE void OC_FileClose(OC_File_t file)
ORCA_INLINE OC_IoError_t OC_FileLastError(OC_File_t handle)
ORCA_INLINE i64 OC_FilePos(OC_File_t file)
ORCA_INLINE i64 OC_FileSeek(OC_File_t file, i64 offset, OC_FileWhence_t whence)
ORCA_INLINE u64 OC_FileWrite(OC_File_t file, u64 size, char* buffer)
ORCA_INLINE u64 OC_FileRead(OC_File_t file, u64 size, char* buffer)
ORCA_INLINE OC_FileStatus_t OC_FileGetStatus(OC_File_t file)
ORCA_INLINE u64 OC_FileSize(OC_File_t file)
ORCA_INLINE OC_File_t OC_FileOpenWithRequest(OC_Str8_t path, OC_FileAccess_t rights, OC_FileOpenFlags_t flags)
ORCA_INLINE OC_FileOpenWithDialogResult_t OC_FileOpenWithDialog(OC_Arena_t* arena, OC_FileAccess_t rights, OC_FileOpenFlags_t flags, OC_FileDialogDesc_t* desc)
ORCA_INLINE void OC_UiInit(OC_UiContext_t* context)
ORCA_INLINE OC_UiContext_t* OC_UiGetContext()
ORCA_INLINE void OC_UiSetContext(OC_UiContext_t* context)
ORCA_INLINE void OC_UiProcessEvent(OC_Event_t* event)
ORCA_INLINE void OC_UiBeginFrame(OC_Vec2_t size, OC_UiStyle_t* defaultStyle, OC_UiStyleMask_t mask)
ORCA_INLINE void OC_UiEndFrame()
ORCA_INLINE void OC_UiDraw()
ORCA_INLINE OC_UiSig_t OC_UiLabel(const char* label)
ORCA_INLINE OC_UiSig_t OC_UiLabelStr8(OC_Str8_t label)
ORCA_INLINE OC_UiSig_t OC_UiButton(const char* label)
ORCA_INLINE OC_UiSig_t OC_UiCheckbox(const char* name, bool* checked)
ORCA_INLINE OC_UiBox_t* OC_UiSlider(const char* name, f32* value)
ORCA_INLINE OC_UiBox_t* OC_UiScrollbar(const char* name, f32 thumbRatio, f32* scrollValue)
ORCA_INLINE OC_UiTextBoxResult_t OC_UiTextBox(const char* name, OC_Arena_t* arena, OC_Str8_t text)
ORCA_INLINE OC_UiSelectPopupInfo_t OC_UiSelectPopup(const char* name, OC_UiSelectPopupInfo_t* info)
ORCA_INLINE OC_UiRadioGroupInfo_t OC_UiRadioGroup(const char* name, OC_UiRadioGroupInfo_t* info)
ORCA_INLINE void OC_UiPanelBegin(const char* name, OC_UiFlags_t flags)
ORCA_INLINE void OC_UiPanelEnd()
ORCA_INLINE void OC_UiMenuBarBegin(const char* name)
ORCA_INLINE void OC_UiMenuBarEnd()
ORCA_INLINE void OC_UiMenuBegin(const char* label)
ORCA_INLINE void OC_UiMenuEnd()
ORCA_INLINE OC_UiSig_t OC_UiMenuButton(const char* label)
ORCA_INLINE void OC_UiTooltip(const char* label)
ORCA_INLINE void OC_UiStyleNext(OC_UiStyle_t* style, OC_UiStyleMask_t mask)
ORCA_INLINE void OC_UiPatternPush(OC_Arena_t* arena, OC_UiPattern_t* pattern, OC_UiSelector_t selector)
ORCA_INLINE OC_UiPattern_t OC_UiPatternAll()
ORCA_INLINE OC_UiPattern_t OC_UiPatternOwner()
ORCA_INLINE void OC_UiStyleMatchBefore(OC_UiPattern_t pattern, OC_UiStyle_t* style, OC_UiStyleMask_t mask)
ORCA_INLINE void OC_UiStyleMatchAfter(OC_UiPattern_t pattern, OC_UiStyle_t* style, OC_UiStyleMask_t mask)
#define OC_UiFrame_(size, style, mask)
#define OC_UiPanel(s, f)
#define OC_UiMenuBar_(name)
#define OC_UiMenu_(label)
ORCA_INLINE void OC_ArenaInit(OC_Arena_t* arena)
ORCA_INLINE void OC_ArenaInitWithOptions(OC_Arena_t* arena, OC_ArenaOptions_t* options)
ORCA_INLINE void OC_ArenaCleanup(OC_Arena_t* arena)
ORCA_INLINE void* OC_ArenaPush(OC_Arena_t* arena, u64 size)
ORCA_INLINE void* OC_ArenaPushAligned(OC_Arena_t* arena, u64 size, u32 alignment)
ORCA_INLINE void OC_ArenaClear(OC_Arena_t* arena)
ORCA_INLINE OC_ArenaScope_t OC_ArenaScopeBegin(OC_Arena_t* arena)
ORCA_INLINE void OC_ArenaScopeEnd(OC_ArenaScope_t scope)
ORCA_INLINE OC_ArenaScope_t OC_ScratchBegin()
ORCA_INLINE void OC_ScratchEnd(OC_ArenaScope_t scope)
ORCA_INLINE OC_ArenaScope_t OC_ScratchBeginNext(OC_Arena_t* used)
ORCA_INLINE void OC_ListInit(OC_List_t* list)
ORCA_INLINE bool OC_ListEmpty(OC_List_t list)
ORCA_INLINE OC_ListElement_t* OC_ListBegin(OC_List_t list)
ORCA_INLINE OC_ListElement_t* OC_ListEnd(OC_List_t* list)
ORCA_INLINE OC_ListElement_t* OC_ListLast(OC_List_t list)
ORCA_INLINE void OC_ListInsert(OC_List_t* list, OC_ListElement_t* afterElt, OC_ListElement_t* elt)
ORCA_INLINE void OC_ListInsertBefore(OC_List_t* list, OC_ListElement_t* beforeElt, OC_ListElement_t* elt)
ORCA_INLINE void OC_ListRemove(OC_List_t* list, OC_ListElement_t* elt)
ORCA_INLINE void OC_ListPushFront(OC_List_t* list, OC_ListElement_t* elt)
ORCA_INLINE OC_ListElement_t* OC_ListPopFront(OC_List_t* list)
ORCA_INLINE void OC_ListPushBack(OC_List_t* list, OC_ListElement_t* elt)
ORCA_INLINE OC_ListElement_t* OC_ListPopBack(OC_List_t* list)
ORCA_INLINE OC_Str8_t OC_Str8FromBuffer_(u64 len, char* buffer)
ORCA_INLINE OC_Str8_t OC_Str8Slice(OC_Str8_t s, u64 start, u64 end)
ORCA_INLINE OC_Str8_t OC_Str8PushBuffer_(OC_Arena_t* arena, u64 len, char* buffer)
ORCA_INLINE OC_Str8_t OC_Str8PushCstring_(OC_Arena_t* arena, const char* str)
ORCA_INLINE OC_Str8_t OC_Str8PushCopy(OC_Arena_t* arena, OC_Str8_t s)
ORCA_INLINE OC_Str8_t OC_Str8PushSlice(OC_Arena_t* arena, OC_Str8_t s, u64 start, u64 end)
ORCA_INLINE OC_Str8_t OC_Str8Pushfv_(OC_Arena_t* arena, const char* format, va_list args)
ORCA_INLINE int OC_Str8Cmp(OC_Str8_t s1, OC_Str8_t s2)
ORCA_INLINE char* OC_Str8ToCstring(OC_Arena_t* arena, OC_Str8_t string)
ORCA_INLINE void OC_Str8ListPush(OC_Arena_t* arena, OC_Str8List_t* list, OC_Str8_t str)
ORCA_INLINE OC_Str8_t OC_Str8ListCollate(OC_Arena_t* arena, OC_Str8List_t list, OC_Str8_t prefix, OC_Str8_t separator, OC_Str8_t postfix)
ORCA_INLINE OC_Str8_t OC_Str8ListJoin_(OC_Arena_t* arena, OC_Str8List_t list)
ORCA_INLINE OC_Str8List_t OC_Str8Split(OC_Arena_t* arena, OC_Str8_t str, OC_Str8List_t separators)
ORCA_INLINE OC_Str8_t OC_PathSliceDirectory(OC_Str8_t path)
ORCA_INLINE OC_Str8_t OC_PathSliceFilename(OC_Str8_t path)
ORCA_INLINE OC_Str8List_t OC_PathSplit(OC_Arena_t* arena, OC_Str8_t path)
ORCA_INLINE OC_Str8_t OC_PathJoin_(OC_Arena_t* arena, OC_Str8List_t elements)
ORCA_INLINE OC_Str8_t OC_PathAppend(OC_Arena_t* arena, OC_Str8_t parent, OC_Str8_t relPath)
ORCA_INLINE bool OC_PathIsAbsolute(OC_Str8_t path)
#define OC_Str8Pushf(arena, format, ...)
#define OC_Str8ListPushf(arena, list, format, ...)
#define OC_ArenaPushType(arena, type)
#define OC_ArenaPushArray(arena, type, count)
#define OC_ScratchEnd(scope)
#define OC_ListNext(element)
#define OC_ListPrev(element)
#define OC_ListEntry(element, type, member)
#define OC_ListNextEntry(list, element, type, member)
#define OC_ListPrevEntry(list, element, type, member)
#define OC_ListFirstEntry(list, type, member)
#define OC_ListLastEntry(list, type, member)
#define OC_ListFor(list, element, type, member)
#define OC_ListForReverse(list, element, type, member)
#define OC_ListForSafe(list, element, type, member)
#define OC_ListPopFrontEntry(list, type, member)
#define OC_ListPopBackEntry(list, type, member)
#define OC_Log_I(message, ...)
#define OC_Log_W(message, ...)
#define OC_Log_E(message, ...)
#define OC_AssertFail(file, function, line, test, fmt, ...)
#define OC_Assert(test, message, ...)
#define OC_AbortExt(file, function, line, fmt, ...)
#define OC_Abort(message, ...)
*/
