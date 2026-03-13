// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"
#include "../basic/third_party.h"

#ifndef BASED_CORE_OPAQUE_DEVICE_HANDLES_DEFINED
#  define BASED_CORE_OPAQUE_DEVICE_HANDLES_DEFINED
typedef void* device;
typedef void* camera;
typedef void* sensor;
typedef void* monitor;
#endif

// =========================================================================
c_begin;
// =========================================================================

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

// Global monitor enumeration.
func sz monitor_get_total_count(void);
func monitor monitor_get_from_idx(sz idx);

// Get primary monitor of the system.
func monitor monitor_get_primary_id(void);

// Monitor geometry.
func r2_i32 monitor_get_bounds(monitor mon_id);
func r2_i32 monitor_get_usable_bounds(monitor mon_id);

// Display mode enumeration.
func sz monitor_get_mode_count(monitor mon_id);
func b32 monitor_get_mode(monitor mon_id, sz idx, monitor_mode* out_mode);
func b32 monitor_get_current_mode(monitor mon_id, monitor_mode* out_mode);
func b32 monitor_get_desktop_mode(monitor mon_id, monitor_mode* out_mode);

// Convenience queries.
func cstr8 monitor_get_name(monitor mon_id);
func f32 monitor_get_refresh_rate(monitor mon_id);
func f32 monitor_get_content_scale(monitor mon_id);
func monitor_orientation monitor_get_orientation(monitor mon_id);

// =========================================================================
c_end;
// =========================================================================
