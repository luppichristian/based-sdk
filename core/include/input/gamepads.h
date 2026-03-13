// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Gamepads
// =========================================================================

// Maximum number of cached gamepad slots exposed by this module.
const_var sz GAMEPADS_MAX_COUNT = 4;

// Logical gamepad buttons used by the shared state cache.
typedef enum gamepad_button {
  GAMEPAD_BUTTON_INVALID = -1,
  GAMEPAD_BUTTON_SOUTH = 0,
  GAMEPAD_BUTTON_EAST = 1,
  GAMEPAD_BUTTON_WEST = 2,
  GAMEPAD_BUTTON_NORTH = 3,
  GAMEPAD_BUTTON_BACK = 4,
  GAMEPAD_BUTTON_GUIDE = 5,
  GAMEPAD_BUTTON_START = 6,
  GAMEPAD_BUTTON_LEFT_STICK = 7,
  GAMEPAD_BUTTON_RIGHT_STICK = 8,
  GAMEPAD_BUTTON_LEFT_SHOULDER = 9,
  GAMEPAD_BUTTON_RIGHT_SHOULDER = 10,
  GAMEPAD_BUTTON_DPAD_UP = 11,
  GAMEPAD_BUTTON_DPAD_DOWN = 12,
  GAMEPAD_BUTTON_DPAD_LEFT = 13,
  GAMEPAD_BUTTON_DPAD_RIGHT = 14,
  GAMEPAD_BUTTON_MISC1 = 15,
  GAMEPAD_BUTTON_RIGHT_PADDLE1 = 16,
  GAMEPAD_BUTTON_LEFT_PADDLE1 = 17,
  GAMEPAD_BUTTON_RIGHT_PADDLE2 = 18,
  GAMEPAD_BUTTON_LEFT_PADDLE2 = 19,
  GAMEPAD_BUTTON_TOUCHPAD = 20,
  GAMEPAD_BUTTON_MISC2 = 21,
  GAMEPAD_BUTTON_MISC3 = 22,
  GAMEPAD_BUTTON_MISC4 = 23,
  GAMEPAD_BUTTON_MISC5 = 24,
  GAMEPAD_BUTTON_MISC6 = 25,
  GAMEPAD_BUTTON_COUNT = 26,
} gamepad_button;

// Logical axes exposed by the shared state cache.
typedef enum gamepad_axis {
  GAMEPAD_AXIS_INVALID = -1,
  GAMEPAD_AXIS_LEFTX = 0,
  GAMEPAD_AXIS_LEFTY = 1,
  GAMEPAD_AXIS_RIGHTX = 2,
  GAMEPAD_AXIS_RIGHTY = 3,
  GAMEPAD_AXIS_LEFT_TRIGGER = 4,
  GAMEPAD_AXIS_RIGHT_TRIGGER = 5,
  GAMEPAD_AXIS_COUNT = 6,
} gamepad_axis;

typedef i32 gamepad_touchpad_idx;
typedef i32 gamepad_finger_idx;

typedef enum gamepad_sensor_kind {
  GAMEPAD_SENSOR_KIND_INVALID = -1,
  GAMEPAD_SENSOR_KIND_UNKNOWN = 0,
  GAMEPAD_SENSOR_KIND_ACCEL = 1,
  GAMEPAD_SENSOR_KIND_GYRO = 2,
  GAMEPAD_SENSOR_KIND_ACCEL_L = 3,
  GAMEPAD_SENSOR_KIND_GYRO_L = 4,
  GAMEPAD_SENSOR_KIND_ACCEL_R = 5,
  GAMEPAD_SENSOR_KIND_GYRO_R = 6,
} gamepad_sensor_kind;

// Returns the number of active gamepad slots.
func sz gamepads_get_count(void);

// Returns 1 if slot_idx currently holds a connected gamepad, 0 otherwise.
func b32 gamepads_is_connected(sz slot_idx);

// Returns the device for slot_idx, or NULL when the slot is disconnected.
func device gamepads_get_device(sz slot_idx);

// Returns the cached gamepad name for slot_idx, or NULL when unavailable.
func cstr8 gamepads_get_name(sz slot_idx);

// Returns 1 if the connected gamepad exposes button, 0 otherwise.
func b32 gamepads_has_button(sz slot_idx, gamepad_button button);

// Returns the cached pressed state for button.
func b32 gamepads_get_button(sz slot_idx, gamepad_button button);

// Returns 1 if the connected gamepad exposes axis, 0 otherwise.
func b32 gamepads_has_axis(sz slot_idx, gamepad_axis axis);

// Returns the cached signed axis value for axis.
func i16 gamepads_get_axis(sz slot_idx, gamepad_axis axis);

// Runtime output/control helpers for supported devices.
func b32 gamepads_set_rumble(sz slot_idx, u16 low_freq, u16 high_freq, u32 duration_ms);
func b32 gamepads_set_led(sz slot_idx, u8 red, u8 green, u8 blue);
func b32 gamepads_set_axis_deadzone(sz slot_idx, gamepad_axis axis, i16 deadzone);
func i16 gamepads_get_axis_deadzone(sz slot_idx, gamepad_axis axis);

// =========================================================================
c_end;
// =========================================================================
