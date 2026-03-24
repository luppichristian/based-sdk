// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

typedef void* touch;

// =========================================================================
c_begin;
// =========================================================================

typedef u64 finger_id;

// Returns 1 if src refers to a concrete touch handle, 0 otherwise.
func b32 touch_is_valid(touch src);

// Converts src into a touch handle when it refers to a touch device.
func touch device_get_touch(device src);

// Converts src into a generic device handle.
func device touch_to_device(touch src);

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
  finger_id id;
  f32 x;
  f32 y;
  f32 pressure;
} touch_finger_state;

// Returns 1 if touch input support is available, 0 otherwise.
func b32 touch_is_available(void);

// Returns the number of currently known touch devices.
func sz touch_get_total_count(void);

// Returns the touch handle at idx, or NULL when idx is unavailable.
func touch touch_get_from_idx(sz idx);

// Returns the primary touch handle, or NULL when unavailable.
func touch touch_get_primary(void);

// Returns the focused touch handle, or the primary touch when unavailable.
func touch touch_get_focused(void);

// Returns the backend-reported kind for id.
func touch_device_kind touch_get_device_kind(touch src);

// Returns the number of active fingers currently tracked for id.
func sz touch_get_finger_count(touch src);

// Writes the finger state at idx into out_finger. Returns 1 on success, 0 otherwise.
func b32 touch_get_finger(touch src, sz idx, touch_finger_state* out_finger);

// =========================================================================
c_end;
// =========================================================================
