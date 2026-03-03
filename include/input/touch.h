// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// =========================================================================
// Touch
// =========================================================================

// Touch device tracking modes reported by the backend.
typedef enum touch_device_kind {
  TOUCH_DEVICE_INVALID = -1,
  TOUCH_DEVICE_DIRECT = 0,
  TOUCH_DEVICE_INDIRECT_ABSOLUTE = 1,
  TOUCH_DEVICE_INDIRECT_RELATIVE = 2,
} touch_device_kind;

// Current state for a single tracked finger.
typedef struct touch_finger_state {
  u64 id;
  f32 x;
  f32 y;
  f32 pressure;
} touch_finger_state;

// Returns 1 if touch input support is available, 0 otherwise.
func b32 touch_is_available(void);

// Returns the number of currently known touch devices.
func sz touch_get_count(void);

// Writes the touch device id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 touch_get_device_id(sz index, input_device_id* out_id);

// Returns the backend-reported kind for id.
func touch_device_kind touch_get_device_kind(input_device_id id);

// Returns the number of active fingers currently tracked for id.
func sz touch_get_finger_count(input_device_id id);

// Writes the finger state at index into out_finger. Returns 1 on success, 0 otherwise.
func b32 touch_get_finger(input_device_id id, sz index, touch_finger_state* out_finger);
