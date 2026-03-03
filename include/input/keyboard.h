// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// =========================================================================
// Keyboard
// =========================================================================

// Returns 1 if a keyboard backend is available, 0 otherwise.
func b32 keyboard_is_available(void);

// Writes the primary keyboard device id into out_id. Returns 1 on success, 0 otherwise.
func b32 keyboard_get_primary_device_id(input_device_id* out_id);

// Returns 1 if the key identified by scancode is currently pressed, 0 otherwise.
func b32 keyboard_is_key_down(u32 scancode);

// Resolves a keycode for scancode under the supplied modifier state.
func u32 keyboard_get_keycode(u32 scancode, u16 modifiers, b32 key_event);

// Returns a backend-defined readable name for scancode.
func const c8* keyboard_get_scancode_name(u32 scancode);
