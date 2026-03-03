// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// =========================================================================
// Mouse
// =========================================================================

// Mouse buttons reported by the shared mouse helpers.
typedef enum mouse_button {
  MOUSE_BUTTON_LEFT = 1,
  MOUSE_BUTTON_MIDDLE = 2,
  MOUSE_BUTTON_RIGHT = 3,
  MOUSE_BUTTON_X1 = 4,
  MOUSE_BUTTON_X2 = 5,
} mouse_button;

// Mouse position plus a bitmask of currently pressed buttons.
typedef struct mouse_state {
  f32 x;
  f32 y;
  u32 button_mask;
} mouse_state;

// Returns 1 if a mouse backend is available, 0 otherwise.
func b32 mouse_is_available(void);

// Writes the primary mouse device id into out_id. Returns 1 on success, 0 otherwise.
func b32 mouse_get_primary_device_id(input_device_id* out_id);

// Returns the current mouse state in the active window coordinate space.
func mouse_state mouse_get_state(void);

// Returns the current mouse state in desktop-global coordinates.
func mouse_state mouse_get_global_state(void);

// Returns the relative mouse delta accumulated since the last pump.
func mouse_state mouse_get_relative_state(void);

// Returns 1 if button is currently pressed, 0 otherwise.
func b32 mouse_is_button_down(u8 button);

// Converts window coordinates into normalized coordinates in the [-1, 1] range.
func b32 mouse_window_to_normalized(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y);

// Converts normalized coordinates in the [-1, 1] range into window coordinates.
func b32 mouse_normalized_to_window(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y);

// Converts window coordinates into physical pixel coordinates.
func b32 mouse_window_to_pixels(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y);

// Converts physical pixel coordinates into window coordinates.
func b32 mouse_pixels_to_window(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y);
