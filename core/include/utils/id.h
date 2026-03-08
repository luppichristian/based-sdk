// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/strings.h"

// =========================================================================
// id8
// =========================================================================

typedef struct id8 {
  u8 value;
} id8;

static_assert(size_of(id8) == size_of(u8));

// Constructs the zero / invalid 8-bit ID.
func id8 id8_zero(void);

// Wraps a raw 8-bit value.
func id8 id8_make(u8 value);

// Returns the raw 8-bit value.
func u8 id8_get(id8 ident);

// Returns 1 if the ID is zero, 0 otherwise.
func b32 id8_is_zero(id8 ident);

// Returns 1 if the ID is non-zero, 0 otherwise.
func b32 id8_is_valid(id8 ident);

// Returns the next numeric ID (wraps on overflow).
func id8 id8_next(id8 ident);

// Compares two IDs numerically. Returns 0 if equal, <0 or >0 otherwise.
func i32 id8_cmp(id8 lhs, id8 rhs);

// Returns the decimal string length excluding the null terminator.
func sz id8_string_length(id8 ident);

// Parses an unsigned base-10 ID. Returns 1 on success.
func b32 id8_parse_cstr8(cstr8 src, id8* out);

// Formats the ID as unsigned base-10 text. Returns 1 on success.
func b32 id8_to_cstr8(id8 ident, c8* dst, sz cap);

// Writes the decimal text into a bounded UTF-8 string. Returns 1 on success.
func b32 id8_to_str8(id8 ident, str8* dst);

// =========================================================================
// id16
// =========================================================================

typedef struct id16 {
  u16 value;
} id16;

static_assert(size_of(id16) == size_of(u16));

// Constructs the zero / invalid 16-bit ID.
func id16 id16_zero(void);

// Wraps a raw 16-bit value.
func id16 id16_make(u16 value);

// Returns the raw 16-bit value.
func u16 id16_get(id16 ident);

// Returns 1 if the ID is zero, 0 otherwise.
func b32 id16_is_zero(id16 ident);

// Returns 1 if the ID is non-zero, 0 otherwise.
func b32 id16_is_valid(id16 ident);

// Returns the next numeric ID (wraps on overflow).
func id16 id16_next(id16 ident);

// Compares two IDs numerically. Returns 0 if equal, <0 or >0 otherwise.
func i32 id16_cmp(id16 lhs, id16 rhs);

// Returns the decimal string length excluding the null terminator.
func sz id16_string_length(id16 ident);

// Parses an unsigned base-10 ID. Returns 1 on success.
func b32 id16_parse_cstr8(cstr8 src, id16* out);

// Formats the ID as unsigned base-10 text. Returns 1 on success.
func b32 id16_to_cstr8(id16 ident, c8* dst, sz cap);

// Writes the decimal text into a bounded UTF-8 string. Returns 1 on success.
func b32 id16_to_str8(id16 ident, str8* dst);

// =========================================================================
// id32
// =========================================================================

typedef struct id32 {
  u32 value;
} id32;

static_assert(size_of(id32) == size_of(u32));

// Constructs the zero / invalid 32-bit ID.
func id32 id32_zero(void);

// Wraps a raw 32-bit value.
func id32 id32_make(u32 value);

// Returns the raw 32-bit value.
func u32 id32_get(id32 ident);

// Returns 1 if the ID is zero, 0 otherwise.
func b32 id32_is_zero(id32 ident);

// Returns 1 if the ID is non-zero, 0 otherwise.
func b32 id32_is_valid(id32 ident);

// Returns the next numeric ID (wraps on overflow).
func id32 id32_next(id32 ident);

// Compares two IDs numerically. Returns 0 if equal, <0 or >0 otherwise.
func i32 id32_cmp(id32 lhs, id32 rhs);

// Returns the decimal string length excluding the null terminator.
func sz id32_string_length(id32 ident);

// Parses an unsigned base-10 ID. Returns 1 on success.
func b32 id32_parse_cstr8(cstr8 src, id32* out);

// Formats the ID as unsigned base-10 text. Returns 1 on success.
func b32 id32_to_cstr8(id32 ident, c8* dst, sz cap);

// Writes the decimal text into a bounded UTF-8 string. Returns 1 on success.
func b32 id32_to_str8(id32 ident, str8* dst);

// =========================================================================
// id64
// =========================================================================

typedef struct id64 {
  u64 value;
} id64;

static_assert(size_of(id64) == size_of(u64));

// Constructs the zero / invalid 64-bit ID.
func id64 id64_zero(void);

// Wraps a raw 64-bit value.
func id64 id64_make(u64 value);

// Returns the raw 64-bit value.
func u64 id64_get(id64 ident);

// Returns 1 if the ID is zero, 0 otherwise.
func b32 id64_is_zero(id64 ident);

// Returns 1 if the ID is non-zero, 0 otherwise.
func b32 id64_is_valid(id64 ident);

// Returns the next numeric ID (wraps on overflow).
func id64 id64_next(id64 ident);

// Compares two IDs numerically. Returns 0 if equal, <0 or >0 otherwise.
func i32 id64_cmp(id64 lhs, id64 rhs);

// Returns the decimal string length excluding the null terminator.
func sz id64_string_length(id64 ident);

// Parses an unsigned base-10 ID. Returns 1 on success.
func b32 id64_parse_cstr8(cstr8 src, id64* out);

// Formats the ID as unsigned base-10 text. Returns 1 on success.
func b32 id64_to_cstr8(id64 ident, c8* dst, sz cap);

// Writes the decimal text into a bounded UTF-8 string. Returns 1 on success.
func b32 id64_to_str8(id64 ident, str8* dst);
