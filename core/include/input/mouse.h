// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Mouse
// =========================================================================

// Mouse button bit indices used with bit(MOUSE_BUTTON_*).
typedef enum mouse_button {
  MOUSE_BUTTON_LEFT = 0,
  MOUSE_BUTTON_MIDDLE = 1,
  MOUSE_BUTTON_RIGHT = 2,
  MOUSE_BUTTON_X1 = 3,
  MOUSE_BUTTON_X2 = 4,
  MOUSE_BUTTON_COUNT = 5,
} mouse_button;

typedef enum mouse_wheel_direction {
  MOUSE_WHEEL_DIRECTION_NORMAL = 0,
  MOUSE_WHEEL_DIRECTION_FLIPPED = 1,
} mouse_wheel_direction;

// Bitmask of currently pressed buttons. Use bit(MOUSE_BUTTON_*) to test buttons.
typedef u32 mouse_state;

// Returns 1 if a mouse backend is available, 0 otherwise.
func b32 mouse_is_available(void);

// Returns the primary mouse device, or NULL when unavailable.
func device mouse_get_primary_device(void);

// Returns the current mouse button state as a bitmask.
func mouse_state mouse_get_state(void);

// Returns 1 if button is currently pressed, 0 otherwise.
func b32 mouse_is_button_down(mouse_button button);

// =========================================================================
c_end;
// =========================================================================
