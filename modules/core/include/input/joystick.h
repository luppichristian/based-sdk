// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

typedef void* joystick;

// =========================================================================
c_begin;
// =========================================================================

typedef enum joystick_hat_state {
  JOYSTICK_HAT_STATE_CENTERED = 0x00,
  JOYSTICK_HAT_STATE_UP = 0x01,
  JOYSTICK_HAT_STATE_RIGHT = 0x02,
  JOYSTICK_HAT_STATE_DOWN = 0x04,
  JOYSTICK_HAT_STATE_LEFT = 0x08,
  JOYSTICK_HAT_STATE_RIGHT_UP = 0x03,
  JOYSTICK_HAT_STATE_RIGHT_DOWN = 0x06,
  JOYSTICK_HAT_STATE_LEFT_UP = 0x09,
  JOYSTICK_HAT_STATE_LEFT_DOWN = 0x0C,
} joystick_hat_state;

// Returns 1 if src refers to a concrete joystick id, 0 otherwise.
func b32 joystick_is_valid(joystick src);

// Converts src into a joystick handle when it refers to a joystick device.
func joystick device_get_joystick(device src);

// Converts src into a generic device handle.
func device joystick_to_device(joystick src);

// Returns the number of currently known joystick devices.
func sz joystick_get_total_count(void);

// Returns the joystick handle at idx, or NULL when unavailable.
func joystick joystick_get_from_idx(sz idx);

// Returns the primary joystick handle, or NULL when unavailable.
func joystick joystick_get_primary(void);

// Returns the focused joystick handle, or the primary joystick when unavailable.
func joystick joystick_get_focused(void);

// Returns a backend-defined joystick name for id, or NULL when unavailable.
func cstr8 joystick_get_name(joystick joy_id);

// Returns 1 if id is still connected, 0 otherwise.
func b32 joystick_is_connected(joystick joy_id);

// Returns the number of available controls for id.
func sz joystick_get_axis_count(joystick joy_id);
func sz joystick_get_ball_count(joystick joy_id);
func sz joystick_get_hat_count(joystick joy_id);
func sz joystick_get_button_count(joystick joy_id);

// Returns the current state of a control, or 0 / CENTERED on failure.
func i16 joystick_get_axis(joystick joy_id, sz axis_idx);
func b32 joystick_get_ball(joystick joy_id, sz ball_idx, i32* out_xrel, i32* out_yrel);
func joystick_hat_state joystick_get_hat(joystick joy_id, sz hat_idx);
func b32 joystick_get_button(joystick joy_id, sz button_idx);

// Returns the current battery state for id and optionally writes the percent.
func battery_state joystick_get_battery(joystick joy_id, i32* out_percent);

// =========================================================================
c_end;
// =========================================================================
