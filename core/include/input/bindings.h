// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "capture.h"
#include "gamepads.h"
#include "keyboard.h"
#include "mouse.h"
#include "msg.h"

// =========================================================================
c_begin;
// =========================================================================

// ========================================================================
// Input Bindings
// ========================================================================

const_var sz BINDINGS_MAX_COUNT = 256u;
const_var sz BINDING_MAX_KEYS = 8u;
const_var sz BINDING_MAX_MOUSE_BUTTONS = 8u;
const_var sz BINDING_MAX_GAMEPAD_BUTTONS = 8u;

typedef struct binding_keyboard_combo {
  sz count;
  keyboard_scancode scancodes[BINDING_MAX_KEYS];
  keymod required_mods;
  keymod forbidden_mods;
} binding_keyboard_combo;

typedef struct binding_mouse_combo {
  sz count;
  u8 buttons[BINDING_MAX_MOUSE_BUTTONS];
} binding_mouse_combo;

typedef struct binding_gamepad_combo {
  sz count;
  gamepad_button buttons[BINDING_MAX_GAMEPAD_BUTTONS];
  b32 any_slot;
  sz slot_idx;
} binding_gamepad_combo;

typedef struct binding_desc {
  u32 binding_id;
  b32 enabled;
  binding_keyboard_combo keyboard;
  binding_mouse_combo mouse;
  binding_gamepad_combo gamepad;
} binding_desc;

// Removes every binding and resets runtime state.
func void bindings_clear(void);

// Adds or replaces one binding identified by desc->binding_id.
func b32 bindings_add(const binding_desc* desc);

// Removes one binding identified by binding_id.
func b32 bindings_remove(u32 binding_id);

// Returns 1 if a binding with binding_id exists, 0 otherwise.
func b32 bindings_has(u32 binding_id);

// Enables or disables a binding. Returns 1 on success, 0 otherwise.
func b32 bindings_set_enabled(u32 binding_id, b32 enabled);

// Returns 1 when the binding is currently active, 0 otherwise.
func b32 bindings_is_down(u32 binding_id);

// Returns 1 when the binding became active since last query for key.
func b32 bindings_is_pressed(input_key key, u32 binding_id);

// Returns 1 when the binding became inactive since last query for key.
func b32 bindings_is_released(input_key key, u32 binding_id);

// Updates binding transitions from one message payload.
func void bindings_on_msg(const msg* src);

// =========================================================================
c_end;
// =========================================================================
