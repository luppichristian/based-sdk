// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/strings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct uuid {
  u8 bytes[16];
} uuid;

static_assert(size_of(uuid) == 16);

// Constructs an all-zero UUID.
func uuid uuid_zero(void);

// Copies a UUID from a 16-byte array.
func uuid uuid_from_bytes(const u8* bytes);

// Constructs a UUID from its upper and lower 64-bit halves.
func uuid uuid_from_u64(u64 upper, u64 lower);

// Copies the raw 16-byte payload into dst.
func void uuid_get_bytes(uuid value, u8* dst);

// Returns the upper 64 bits in big-endian UUID byte order.
func u64 uuid_get_upper(uuid value);

// Returns the lower 64 bits in big-endian UUID byte order.
func u64 uuid_get_lower(uuid value);

// Returns 1 if all bytes are zero, 0 otherwise.
func b32 uuid_is_zero(uuid value);

// Returns 1 if both UUIDs are byte-identical, 0 otherwise.
func b32 uuid_equal(uuid lhs, uuid rhs);

// Lexicographically compares the raw bytes. Returns 0 if equal, <0 or >0 otherwise.
func i32 uuid_cmp(uuid lhs, uuid rhs);

// Returns the UUID version nibble (usually 1..8), or 0 if unset.
func u8 uuid_get_version(uuid value);

// Returns the RFC 4122 variant field (top bits of byte 8).
func u8 uuid_get_variant(uuid value);

// Returns the canonical string length ("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"),
// excluding the null terminator.
func sz uuid_string_length(void);

// Parses a canonical lowercase/uppercase UUID string with hyphens.
func b32 uuid_parse_cstr8(cstr8 src, uuid* out);

// Formats the UUID into canonical lowercase text. Returns 1 on success.
func b32 uuid_to_cstr8(uuid value, c8* dst, sz cap);

// Formats the UUID into UTF-16 canonical text. Returns 1 on success.
func b32 uuid_to_cstr16(uuid value, c16* dst, sz cap);

// Formats the UUID into UTF-32 canonical text. Returns 1 on success.
func b32 uuid_to_cstr32(uuid value, c32* dst, sz cap);

// Writes canonical lowercase text into a bounded UTF-8 string. Returns 1 on success.
func b32 uuid_to_str8(uuid value, str8* dst);

// Writes canonical lowercase text into a bounded UTF-16 string. Returns 1 on success.
func b32 uuid_to_str16(uuid value, str16* dst);

// Writes canonical lowercase text into a bounded UTF-32 string. Returns 1 on success.
func b32 uuid_to_str32(uuid value, str32* dst);

// Generates a random UUIDv4.
func uuid uuid_generate_v4(void);

// =========================================================================
c_end;
// =========================================================================
