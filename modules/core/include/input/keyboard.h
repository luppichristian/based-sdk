// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"
#include "vkeys.h"

typedef void* keyboard;

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Keyboard
// =========================================================================

// Returns 1 if src refers to a concrete keyboard handle, 0 otherwise.
func b32 keyboard_is_valid(keyboard src);

// Converts src into a keyboard handle when it refers to a keyboard device.
func keyboard device_get_keyboard(device src);

// Converts src into a generic device handle.
func device keyboard_to_device(keyboard src);

// Returns 1 if a keyboard backend is available, 0 otherwise.
func b32 keyboard_is_available(void);

// Returns the primary keyboard handle, or NULL when unavailable.
func keyboard keyboard_get_primary(void);

// Returns the focused keyboard handle, or the primary keyboard when unavailable.
func keyboard keyboard_get_focused(void);

// Returns 1 if the key identified by key is currently pressed, 0 otherwise.
func b32 keyboard_is_key_down(keyboard src, vkey key);

// Returns the current keyboard modifier bitmask.
func keymod keyboard_get_mods(keyboard src);

// Returns 1 when all required_mods are active in the current modifier state.
func b32 keyboard_has_mods(keyboard src, keymod required_mods);

// Returns 1 when required_mods are active and forbidden_mods are inactive.
func b32 keyboard_has_mods_exact(keyboard src, keymod required_mods, keymod forbidden_mods);

// Returns 1 when key is down and modifier requirements are satisfied.
func b32 keyboard_is_key_down_mod(keyboard src, vkey key, keymod required_mods, keymod forbidden_mods);

// Returns a readable physical-key name for key.
func cstr8 keyboard_get_key_name(keyboard src, vkey key);

// Returns a readable translated key name for key under the supplied keyboard layout state.
func cstr8 keyboard_get_key_display_name(keyboard src, vkey key, keymod modifiers, b32 key_event);

// =========================================================================
c_end;
// =========================================================================
