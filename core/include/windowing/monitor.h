// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

typedef void* monitor;

typedef struct monitor_rect {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
} monitor_rect;

typedef struct monitor_mode {
  i32 width;
  i32 height;
  u32 pixel_format;
  f32 refresh_rate;
} monitor_mode;

typedef enum monitor_orientation {
  MONITOR_ORIENTATION_UNKNOWN = 0,
  MONITOR_ORIENTATION_LANDSCAPE = 1,
  MONITOR_ORIENTATION_LANDSCAPE_FLIPPED = 2,
  MONITOR_ORIENTATION_PORTRAIT = 3,
  MONITOR_ORIENTATION_PORTRAIT_FLIPPED = 4,
} monitor_orientation;

// Identifier conversion helpers.
func b32 monitor_id_is_valid(monitor src);
func monitor monitor_from_native_id(up native_id);
func up monitor_to_native_id(monitor src);

// Global monitor enumeration.
func sz monitor_get_count(void);
func b32 monitor_get_id(sz idx, monitor* out_id);
func monitor monitor_get_primary_id(void);
func cstr8 monitor_get_name(monitor id);

// Monitor geometry.
func b32 monitor_get_bounds(monitor id, monitor_rect* out_rect);
func b32 monitor_get_usable_bounds(monitor id, monitor_rect* out_rect);

// Display mode enumeration.
func sz monitor_get_mode_count(monitor id);
func b32 monitor_get_mode(monitor id, sz idx, monitor_mode* out_mode);
func b32 monitor_get_current_mode(monitor id, monitor_mode* out_mode);
func b32 monitor_get_desktop_mode(monitor id, monitor_mode* out_mode);

// Convenience queries.
func f32 monitor_get_refresh_rate(monitor id);
func f32 monitor_get_content_scale(monitor id);
func monitor_orientation monitor_get_orientation(monitor id);
