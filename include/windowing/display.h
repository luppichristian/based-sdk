// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

typedef void* display;

typedef struct display_rect {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
} display_rect;

typedef struct display_mode {
  i32 width;
  i32 height;
  u32 pixel_format;
  f32 refresh_rate;
} display_mode;

typedef enum display_orientation {
  DISPLAY_ORIENTATION_UNKNOWN = 0,
  DISPLAY_ORIENTATION_LANDSCAPE = 1,
  DISPLAY_ORIENTATION_LANDSCAPE_FLIPPED = 2,
  DISPLAY_ORIENTATION_PORTRAIT = 3,
  DISPLAY_ORIENTATION_PORTRAIT_FLIPPED = 4,
} display_orientation;

func b32 display_id_is_valid(display src);
func display display_from_native_id(up native_id);
func up display_to_native_id(display src);

func sz display_get_count(void);
func b32 display_get_id(sz idx, display* out_id);
func display display_get_primary_id(void);
func cstr8 display_get_name(display id);

func b32 display_get_bounds(display id, display_rect* out_rect);
func b32 display_get_usable_bounds(display id, display_rect* out_rect);

func sz display_get_mode_count(display id);
func b32 display_get_mode(display id, sz idx, display_mode* out_mode);
func b32 display_get_current_mode(display id, display_mode* out_mode);
func b32 display_get_desktop_mode(display id, display_mode* out_mode);

func f32 display_get_refresh_rate(display id);
func f32 display_get_content_scale(display id);
func display_orientation display_get_orientation(display id);
