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

// Returns 1 if src refers to a concrete display id, 0 otherwise.
func b32 display_id_is_valid(display src);

// Builds a display from a backend-native display identifier.
func display display_from_native_id(up native_id);

// Returns the backend-native display identifier encoded in src.
func up display_to_native_id(display src);

// Returns the number of currently known displays.
func sz display_get_count(void);

// Writes the display id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 display_get_id(sz idx, display* out_id);

// Returns the primary display id, or an invalid id on failure.
func display display_get_primary_id(void);

// Returns a backend-defined display name for id, or NULL when unavailable.
func cstr8 display_get_name(display id);

// Writes the full display bounds into out_rect. Returns 1 on success, 0 otherwise.
func b32 display_get_bounds(display id, display_rect* out_rect);

// Writes the usable display bounds into out_rect. Returns 1 on success, 0 otherwise.
func b32 display_get_usable_bounds(display id, display_rect* out_rect);
