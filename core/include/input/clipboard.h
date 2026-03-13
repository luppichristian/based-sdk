// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../memory/buffer.h"

// =========================================================================
c_begin;
// =========================================================================

// Returns 1 if clipboard text data is currently available, 0 otherwise.
func b32 clipboard_has_text(void);

// Returns 1 if clipboard data is currently available for mime_type, 0 otherwise.
func b32 clipboard_has_data(cstr8 mime_type);

// Replaces clipboard text with src. Returns 1 on success, 0 otherwise.
func b32 clipboard_set_text(cstr8 src);

// Replaces clipboard contents with data for mime_type. Returns 1 on success, 0 otherwise.
func b32 clipboard_set_data(cstr8 mime_type, buffer src_data);

// Copies clipboard text into out_text and always null-terminates when out_capacity > 0.
// Returns 1 on success, 0 otherwise.
func b32 clipboard_get_text(c8* out_text, sz out_capacity);

// Copies clipboard data for mime_type into out_data when it fits.
// When out_size is provided it receives the full clipboard payload size even on failure.
// Pass a zero-sized buffer to query the required size without copying.
func b32 clipboard_get_data(cstr8 mime_type, buffer out_data, sz* out_size);

// Clears clipboard contents. Returns 1 on success, 0 otherwise.
func b32 clipboard_clear(void);

// =========================================================================
c_end;
// =========================================================================
