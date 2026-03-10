// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// char8 - ASCII character helpers over c8
// =========================================================================

// Classification.
func b32 c8_is_alnum(c8 chr);
func b32 c8_is_alpha(c8 chr);
func b32 c8_is_blank(c8 chr);
func b32 c8_is_cntrl(c8 chr);
func b32 c8_is_digit(c8 chr);
func b32 c8_is_graph(c8 chr);
func b32 c8_is_lower(c8 chr);
func b32 c8_is_print(c8 chr);
func b32 c8_is_punct(c8 chr);
func b32 c8_is_space(c8 chr);
func b32 c8_is_upper(c8 chr);
func b32 c8_is_xdigit(c8 chr);

// Case conversion.
func c8 c8_to_lower(c8 chr);
func c8 c8_to_upper(c8 chr);

// Hex conversion helpers.
func i32 c8_hex_to_nibble(c8 chr);
func c8 c8_nibble_to_hex(u8 nibble);

// =========================================================================
// char16 - ASCII character helpers over c16
// =========================================================================

// Classification.
func b32 c16_is_alnum(c16 chr);
func b32 c16_is_alpha(c16 chr);
func b32 c16_is_blank(c16 chr);
func b32 c16_is_cntrl(c16 chr);
func b32 c16_is_digit(c16 chr);
func b32 c16_is_graph(c16 chr);
func b32 c16_is_lower(c16 chr);
func b32 c16_is_print(c16 chr);
func b32 c16_is_punct(c16 chr);
func b32 c16_is_space(c16 chr);
func b32 c16_is_upper(c16 chr);
func b32 c16_is_xdigit(c16 chr);

// Case conversion.
func c16 c16_to_lower(c16 chr);
func c16 c16_to_upper(c16 chr);

// Hex conversion helpers.
func i32 c16_hex_to_nibble(c16 chr);
func c16 c16_nibble_to_hex(u8 nibble);

// =========================================================================
// char32 - ASCII character helpers over c32
// =========================================================================

// Classification.
func b32 c32_is_alnum(c32 chr);
func b32 c32_is_alpha(c32 chr);
func b32 c32_is_blank(c32 chr);
func b32 c32_is_cntrl(c32 chr);
func b32 c32_is_digit(c32 chr);
func b32 c32_is_graph(c32 chr);
func b32 c32_is_lower(c32 chr);
func b32 c32_is_print(c32 chr);
func b32 c32_is_punct(c32 chr);
func b32 c32_is_space(c32 chr);
func b32 c32_is_upper(c32 chr);
func b32 c32_is_xdigit(c32 chr);

// Case conversion.
func c32 c32_to_lower(c32 chr);
func c32 c32_to_upper(c32 chr);

// Hex conversion helpers.
func i32 c32_hex_to_nibble(c32 chr);
func c32 c32_nibble_to_hex(u8 nibble);

// =========================================================================
c_end;
// =========================================================================
