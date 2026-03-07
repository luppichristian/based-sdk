// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"
#include "display.h"

typedef struct window_id {
  u64 opaque;
} window_id;

// Returns 1 if src refers to a concrete window id, 0 otherwise.
func b32 window_id_is_valid(window_id src);

// Builds a window_id from a backend-native window identifier.
func window_id window_from_native_id(u64 native_id);

// Returns the backend-native window identifier encoded in src.
func u64 window_to_native_id(window_id src);

// Returns the number of currently known windows.
func sz window_get_count(void);

// Writes the window id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 window_get_id(sz index, window_id* out_id);

// Returns 1 when id maps to an existing window, 0 otherwise.
func b32 window_is_valid(window_id id);

// Returns a backend-defined title for id, or NULL when unavailable.
func cstr8 window_get_title(window_id id);

// Writes the display currently associated with id into out_display_id. Returns 1 on success, 0 otherwise.
func b32 window_get_display_id(window_id id, display_id* out_display_id);
