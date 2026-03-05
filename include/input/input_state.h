// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "capture.h"
#include "gamepads.h"
#include "keyboard.h"
#include "mouse.h"
#include "msg.h"

// ========================================================================
// Input State
// ========================================================================

const_var sz INPUT_STATE_KEY_CAP = 512u;
const_var sz INPUT_STATE_MOUSE_BUTTON_CAP = 8u;

typedef struct input_state {
  u64 timestamp;
  b32 keyboard_available;
  b32 mouse_available;
  u16 keyboard_mods;
  f32 mouse_x;
  f32 mouse_y;
  f32 mouse_global_x;
  f32 mouse_global_y;
  f32 mouse_relative_x;
  f32 mouse_relative_y;
  u32 mouse_button_mask;
  b32 keyboard_down[INPUT_STATE_KEY_CAP];
  u32 keyboard_repeat[INPUT_STATE_KEY_CAP];
  b32 mouse_button_down[INPUT_STATE_MOUSE_BUTTON_CAP];
  b32 gamepad_connected[GAMEPADS_MAX_COUNT];
  b32 gamepad_button_down[GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT];
  i16 gamepad_axis[GAMEPADS_MAX_COUNT][GAMEPAD_AXIS_COUNT];
} input_state;

// Clears src to a fully zeroed state.
func void input_state_clear(input_state* src);

// Captures a full snapshot from the current input backends.
func b32 input_state_capture(input_key key, input_state* out_state);

// Applies one normalized message to src.
func void input_state_apply_msg(input_state* src, const msg* event_msg);

// Returns the byte size written by input_state_serialize.
func sz input_state_serialized_size(void);

// Serializes src into dst. Returns 1 on success, 0 otherwise.
func b32 input_state_serialize(const input_state* src, void* dst, sz dst_cap, sz* out_size);

// Restores out_state from a serialized payload.
func b32 input_state_deserialize(const void* src, sz src_size, input_state* out_state);
