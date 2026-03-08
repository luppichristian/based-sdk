// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
// Clipboard
// =========================================================================

// Returns 1 if clipboard text data is currently available, 0 otherwise.
func b32 clipboard_has_text(void);

// Replaces clipboard text with src. Returns 1 on success, 0 otherwise.
func b32 clipboard_set_text(cstr8 src);

// Copies clipboard text into out_text and always null-terminates when out_capacity > 0.
// Returns 1 on success, 0 otherwise.
func b32 clipboard_get_text(c8* out_text, sz out_capacity);

// Clears clipboard text content. Returns 1 on success, 0 otherwise.
func b32 clipboard_clear(void);
