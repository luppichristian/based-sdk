// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

// ========================================================================
// Input Capture
// ========================================================================

// User-defined capture key used to keep independent one-shot query state.
typedef u32 input_key;

// Default process-global key that preserves legacy single-consumer behavior.
const_var input_key INPUT_KEY_DEFAULT = 0u;

// Maximum number of simultaneously tracked capture keys.
const_var sz INPUT_CAPTURE_MAX_KEYS = 64u;

// Ensures input_key has a capture slot. Returns 1 on success, 0 otherwise.
func b32 input_capture_acquire_key(input_key key);

// Releases a previously acquired key and clears its one-shot query state.
// INPUT_KEY_DEFAULT cannot be released.
func void input_capture_release_key(input_key key);

// Releases all non-default capture keys.
func void input_capture_release_all_keys(void);

// Returns the slot idx for key, creating one when possible.
// Returns INPUT_CAPTURE_MAX_KEYS when no slot is available.
func sz input_capture_get_slot(input_key key);

// Returns a monotonically increasing slot epoch used to detect slot reuse.
func u32 input_capture_get_slot_epoch(sz slot_idx);
