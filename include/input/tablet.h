// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// =========================================================================
// Tablet
// =========================================================================

// Continuous tablet axes captured in tablet_pen_state.axis_values.
typedef enum tablet_axis {
  TABLET_AXIS_PRESSURE = 0,
  TABLET_AXIS_XTILT = 1,
  TABLET_AXIS_YTILT = 2,
  TABLET_AXIS_DISTANCE = 3,
  TABLET_AXIS_ROTATION = 4,
  TABLET_AXIS_SLIDER = 5,
  TABLET_AXIS_TANGENTIAL_PRESSURE = 6,
  TABLET_AXIS_COUNT = 7,
} tablet_axis;

// Last observed pen state reported by the tablet backend.
typedef struct tablet_pen_state {
  device_id id;
  u32 pen_id;
  b32 in_proximity;
  b32 touching;
  b32 eraser;
  u32 input_mask;
  u32 window_id;
  f32 x;
  f32 y;
  f32 axis_values[TABLET_AXIS_COUNT];
} tablet_pen_state;

// Returns 1 if tablet support is available, 0 otherwise.
func b32 tablet_is_available(void);

// Returns the number of currently known tablet devices.
func sz tablet_get_count(void);

// Writes the tablet device id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 tablet_get_device_id(sz index, device_id* out_id);

// Writes the latest cached pen state into out_state. Returns 1 on success, 0 otherwise.
func b32 tablet_get_last_pen_state(tablet_pen_state* out_state);

// Reads a raw HID report from id into dst. Returns 1 on success, 0 otherwise.
func b32 tablet_read_hid_report(device_id id, void* dst, sz capacity, sz* out_size, i32 timeout_ms);
