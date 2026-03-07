// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

typedef struct camera_id {
  u64 opaque;
} camera_id;

typedef enum camera_position {
  CAMERA_POSITION_UNKNOWN = 0,
  CAMERA_POSITION_FRONT_FACING = 1,
  CAMERA_POSITION_BACK_FACING = 2,
} camera_position;

// Returns 1 if src refers to a concrete camera id, 0 otherwise.
func b32 camera_id_is_valid(camera_id src);

// Builds a camera_id from a backend-native camera identifier.
func camera_id camera_from_native_id(u64 native_id);

// Returns the backend-native camera identifier encoded in src.
func u64 camera_to_native_id(camera_id src);

// Returns the number of currently known camera devices.
func sz camera_get_count(void);

// Writes the camera id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 camera_get_id(sz index, camera_id* out_id);

// Returns a backend-defined camera name for id, or NULL when unavailable.
func cstr8 camera_get_name(camera_id id);

// Returns the camera physical position for id.
func camera_position camera_get_position(camera_id id);
