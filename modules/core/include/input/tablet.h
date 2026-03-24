// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../interface/window.h"
#include "devices.h"

typedef void* tablet;

// =========================================================================
c_begin;
// =========================================================================

typedef u32 pen_id;

// Returns 1 if src refers to a concrete tablet handle, 0 otherwise.
func b32 tablet_is_valid(tablet src);

// Converts src into a tablet handle when it refers to a tablet device.
func tablet device_get_tablet(device src);

// Converts src into a generic device handle.
func device tablet_to_device(tablet src);

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

typedef enum tablet_input_flag {
  TABLET_INPUT_FLAG_NONE = 0,
  TABLET_INPUT_FLAG_DOWN = (1U << 0),
  TABLET_INPUT_FLAG_BUTTON_1 = (1U << 1),
  TABLET_INPUT_FLAG_BUTTON_2 = (1U << 2),
  TABLET_INPUT_FLAG_BUTTON_3 = (1U << 3),
  TABLET_INPUT_FLAG_ERASER_TIP = (1U << 30),
} tablet_input_flag;
typedef u32 tablet_input_flags;

typedef enum tablet_button {
  TABLET_BUTTON_PRIMARY = 1,
  TABLET_BUTTON_SECONDARY = 2,
  TABLET_BUTTON_TERTIARY = 3,
} tablet_button;

// Last observed pen state reported by the tablet backend.
typedef struct tablet_pen_state {
  device id;
  pen_id pen_id;
  b32 in_proximity;
  b32 touching;
  b32 eraser;
  tablet_input_flags input_mask;
  window window;
  f32 x;
  f32 y;
  f32 axis_values[TABLET_AXIS_COUNT];
} tablet_pen_state;

// Returns 1 if tablet support is available, 0 otherwise.
func b32 tablet_is_available(void);

// Returns the number of currently known tablet devices.
func sz tablet_get_total_count(void);

// Returns the tablet handle at idx, or NULL when idx is unavailable.
func tablet tablet_get_from_idx(sz idx);

// Returns the primary tablet handle, or NULL when unavailable.
func tablet tablet_get_primary(void);

// Returns the focused tablet handle, or the primary tablet when unavailable.
func tablet tablet_get_focused(void);

// Writes the latest cached pen state into out_state. Returns 1 on success, 0 otherwise.
func b32 tablet_get_last_pen_state(tablet src, tablet_pen_state* out_state);

// Reads a raw HID report from id into dst. Returns 1 on success, 0 otherwise.
func b32 tablet_read_hid_report(tablet src, void* dst, sz capacity, sz* out_size, i32 timeout_ms);

// =========================================================================
c_end;
// =========================================================================
