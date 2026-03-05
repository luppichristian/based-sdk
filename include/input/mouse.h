// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "capture.h"
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
func b32 mouse_get_primary_device_id(device_id* out_id);

// Returns the current mouse state in the active window coordinate space.
// key selects the capture stream used by one-shot queries.
func mouse_state mouse_get_state(input_key key);

// Returns the current mouse state in desktop-global coordinates.
func mouse_state mouse_get_global_state(input_key key);

// Returns the relative mouse delta accumulated since the last pump.
func mouse_state mouse_get_relative_state(input_key key);

// Returns 1 if button is currently pressed, 0 otherwise.
func b32 mouse_is_button_down(input_key key, u8 button);

// Returns 1 if button was pressed since last query for key, 0 otherwise.
func b32 mouse_is_button_pressed(input_key key, u8 button);

// Returns 1 if button was released since last query for key, 0 otherwise.
func b32 mouse_is_button_released(input_key key, u8 button);
