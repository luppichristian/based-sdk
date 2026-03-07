// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

typedef void* camera;

typedef enum camera_position {
  CAMERA_POSITION_UNKNOWN = 0,
  CAMERA_POSITION_FRONT_FACING = 1,
  CAMERA_POSITION_BACK_FACING = 2,
} camera_position;

// Returns 1 if src refers to a concrete camera id, 0 otherwise.
func b32 camera_id_is_valid(camera src);

// Builds a camera from a backend-native camera identifier.
func camera camera_from_native_id(up native_id);

// Returns the backend-native camera identifier encoded in src.
func up camera_to_native_id(camera src);

// Returns the number of currently known camera devices.
func sz camera_get_count(void);

// Writes the camera id at idx into out_id. Returns 1 on success, 0 otherwise.
func b32 camera_get_id(sz idx, camera* out_id);

// Returns a backend-defined camera name for id, or NULL when unavailable.
func cstr8 camera_get_name(camera id);

// Returns the camera physical position for id.
func camera_position camera_get_position(camera id);
