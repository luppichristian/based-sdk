// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "capture.h"
#include "devices.h"
#include "../windowing/window.h"

// =========================================================================
// Keyboard
// =========================================================================

// Key modifier bitmask compatible with backend events.
typedef enum keymod {
  KEYMOD_NONE = 0x0000u,
  KEYMOD_LSHIFT = 0x0001u,
  KEYMOD_RSHIFT = 0x0002u,
  KEYMOD_LEVEL5 = 0x0004u,
  KEYMOD_LCTRL = 0x0040u,
  KEYMOD_RCTRL = 0x0080u,
  KEYMOD_LALT = 0x0100u,
  KEYMOD_RALT = 0x0200u,
  KEYMOD_LGUI = 0x0400u,
  KEYMOD_RGUI = 0x0800u,
  KEYMOD_NUM = 0x1000u,
  KEYMOD_CAPS = 0x2000u,
  KEYMOD_MODE = 0x4000u,
  KEYMOD_SCROLL = 0x8000u,
  KEYMOD_SHIFT = KEYMOD_LSHIFT | KEYMOD_RSHIFT,
  KEYMOD_CTRL = KEYMOD_LCTRL | KEYMOD_RCTRL,
  KEYMOD_ALT = KEYMOD_LALT | KEYMOD_RALT,
  KEYMOD_GUI = KEYMOD_LGUI | KEYMOD_RGUI,
} keymod;

typedef u32 keyboard_scancode;
typedef i32 keyboard_keycode;
typedef u16 keyboard_raw_key;

// Returns 1 if a keyboard backend is available, 0 otherwise.
func b32 keyboard_is_available(void);

// Writes the primary keyboard device id into out_id. Returns 1 on success, 0 otherwise.
func b32 keyboard_get_primary_device_id(device_id* out_id);

// Returns 1 if the key identified by scancode is currently pressed, 0 otherwise.
// key selects the capture stream used by one-shot queries.
func b32 keyboard_is_key_down(input_key key, keyboard_scancode scancode);

// Returns 1 if the key identified by scancode was pressed since last query for key, 0 otherwise.
func b32 keyboard_is_key_pressed(input_key key, keyboard_scancode scancode);

// Returns 1 if the key identified by scancode was released since last query for key, 0 otherwise.
func b32 keyboard_is_key_released(input_key key, keyboard_scancode scancode);

// Returns the current repeat-event count for scancode while held.
func u32 keyboard_get_key_repeat_count(input_key key, keyboard_scancode scancode);

// Returns the current keyboard modifier bitmask.
func keymod keyboard_get_mods(void);

// Returns 1 when all required_mods are active in the current modifier state.
func b32 keyboard_has_mods(keymod required_mods);

// Returns 1 when required_mods are active and forbidden_mods are inactive.
func b32 keyboard_has_mods_exact(keymod required_mods, keymod forbidden_mods);

// Returns 1 when scancode is down and modifier requirements are satisfied.
func b32 keyboard_is_key_down_mod(input_key key, keyboard_scancode scancode, keymod required_mods, keymod forbidden_mods);

// Resolves a keycode for scancode under the supplied modifier state.
func keyboard_keycode keyboard_get_keycode(keyboard_scancode scancode, keymod modifiers, b32 key_event);

// Returns a backend-defined readable name for scancode.
func cstr8 keyboard_get_scancode_name(keyboard_scancode scancode);

// IME/text-input control for an optional target window.
func b32 keyboard_start_text_input(window opt_window);
func b32 keyboard_stop_text_input(window opt_window);
func b32 keyboard_is_text_input_active(window opt_window);
func b32 keyboard_set_text_input_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height);
