// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../memory/buffer.h"
#include "../strings/cstrings.h"
#include "devices.h"

// =========================================================================
c_begin;
// =========================================================================

typedef enum camera_pos {
  CAMERA_POS_UNKNOWN = 0,
  CAMERA_POS_FRONT_FACING = 1,
  CAMERA_POS_BACK_FACING = 2,
} camera_pos;

// Returns 1 if src refers to a concrete camera id, 0 otherwise.
func b32 camera_is_valid(camera src);

// Returns the number of currently known camera devices.
func sz camera_get_total_count(void);

// Writes the camera id at idx into out_id. Returns 1 on success, 0 otherwise.
func b32 camera_get_id(sz idx, camera* out_id);

// Returns a backend-defined camera name for id, or NULL when unavailable.
func cstr8 camera_get_name(camera cam_id);

// Returns the camera physical pos for id.
func camera_pos camera_get_pos(camera cam_id);

// Device lifecycle and frame retrieval.
func b32 camera_open(camera cam_id);
func b32 camera_close(camera cam_id);
func b32 camera_start(camera cam_id);
func b32 camera_stop(camera cam_id);
func b32 camera_read(camera cam_id, buffer* out_frame);

// =========================================================================
c_end;
// =========================================================================
