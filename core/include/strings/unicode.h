// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Unicode Utilities
// =========================================================================

// Returned by decode functions when an invalid or malformed sequence is encountered.
const_var c32 UNICODE_REPLACEMENT_CHAR = 0xFFFDU;

// =========================================================================
// Validity
// =========================================================================

// Returns 1 if codepoint is a valid Unicode scalar value.
// Valid range: U+0000..U+D7FF and U+E000..U+10FFFF (surrogates are excluded).
func b32 unicode_is_valid(c32 codepoint);

// =========================================================================
// UTF-8
// =========================================================================

// Returns the number of bytes needed to encode codepoint in UTF-8 (1–4).
// Returns 0 for invalid codepoints.
func sz utf8_encode_size(c32 codepoint);

// Returns the byte length of the UTF-8 sequence whose first byte is first_byte (1–4).
// Returns 0 if first_byte is not a valid UTF-8 leading byte.
func sz utf8_byte_count(c8 first_byte);

// Decodes one UTF-8 codepoint from ptr and sets *consumed to the number of bytes read.
// Returns UNICODE_REPLACEMENT_CHAR on encoding errors (consumed is still advanced).
func c32 utf8_decode(cstr8 ptr, sz* consumed);

// Encodes codepoint into out as UTF-8. out must have room for at least 4 bytes.
// Returns the number of bytes written, or 0 for invalid codepoints.
func sz utf8_encode(c32 codepoint, c8* out);

// Returns the number of Unicode codepoints in the UTF-8 byte sequence [src, src + src_size).
func sz utf8_codepoint_count(cstr8 src, sz src_size);

// =========================================================================
// UTF-16
// =========================================================================

// Returns the number of c16 units needed to encode codepoint in UTF-16 (1 or 2).
// Returns 0 for invalid codepoints.
func sz utf16_encode_size(c32 codepoint);

// Decodes one UTF-16 codepoint from ptr and sets *consumed to the number of c16 units read.
// Returns UNICODE_REPLACEMENT_CHAR on encoding errors (consumed is still advanced).
func c32 utf16_decode(cstr16 ptr, sz* consumed);

// Encodes codepoint into out as UTF-16. out must have room for at least 2 c16 units.
// Returns the number of c16 units written, or 0 for invalid codepoints.
func sz utf16_encode(c32 codepoint, c16* out);

// Returns the number of Unicode codepoints in the UTF-16 sequence [src, src + src_size).
// src_size is measured in c16 units.
func sz utf16_codepoint_count(cstr16 src, sz src_size);

// =========================================================================
// Conversion
// =========================================================================

// Converts the UTF-8 sequence [src, src + src_size) to UTF-16.
// Writes at most dst_cap c16 units into dst. If dst is nullptr, only counts output units.
// Returns the number of c16 units written (or required when dst is nullptr).
// Invalid sequences are replaced with UNICODE_REPLACEMENT_CHAR.
func sz utf8_to_utf16(cstr8 src, sz src_size, c16* dst, sz dst_cap);

// Converts the UTF-8 sequence [src, src + src_size) to UTF-32.
// Writes at most dst_cap c32 units into dst. If dst is nullptr, only counts output units.
// Returns the number of c32 units written (or required when dst is nullptr).
// Invalid sequences are replaced with UNICODE_REPLACEMENT_CHAR.
func sz utf8_to_utf32(cstr8 src, sz src_size, c32* dst, sz dst_cap);

// Converts the UTF-16 sequence [src, src + src_size) to UTF-8.
// src_size is measured in c16 units.
// Writes at most dst_cap bytes into dst. If dst is nullptr, only counts output bytes.
// Returns the number of bytes written (or required when dst is nullptr).
// Invalid sequences are replaced with UNICODE_REPLACEMENT_CHAR.
func sz utf16_to_utf8(cstr16 src, sz src_size, c8* dst, sz dst_cap);

// Converts the UTF-16 sequence [src, src + src_size) to UTF-32.
// src_size is measured in c16 units.
// Writes at most dst_cap c32 units into dst. If dst is nullptr, only counts output units.
// Returns the number of c32 units written (or required when dst is nullptr).
// Invalid sequences are replaced with UNICODE_REPLACEMENT_CHAR.
func sz utf16_to_utf32(cstr16 src, sz src_size, c32* dst, sz dst_cap);

// Converts the UTF-32 sequence [src, src + src_size) to UTF-8.
// src_size is measured in c32 units.
// Writes at most dst_cap bytes into dst. If dst is nullptr, only counts output bytes.
// Returns the number of bytes written (or required when dst is nullptr).
// Invalid codepoints are replaced with UNICODE_REPLACEMENT_CHAR.
func sz utf32_to_utf8(cstr32 src, sz src_size, c8* dst, sz dst_cap);

// Converts the UTF-32 sequence [src, src + src_size) to UTF-16.
// src_size is measured in c32 units.
// Writes at most dst_cap c16 units into dst. If dst is nullptr, only counts output units.
// Returns the number of c16 units written (or required when dst is nullptr).
// Invalid codepoints are replaced with UNICODE_REPLACEMENT_CHAR.
func sz utf32_to_utf16(cstr32 src, sz src_size, c16* dst, sz dst_cap);

// =========================================================================
c_end;
// =========================================================================
