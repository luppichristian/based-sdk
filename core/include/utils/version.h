// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/strings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef union version {
  u32 packed;
  struct {
    u8 major;
    u8 minor;
    u16 patch;
  } parts;
} version;

// Constructs a zero-initialized version.
func version version_zero(void);

// Constructs a version from its packed representation.
func version version_from_packed(u32 packed);

// Constructs a version from major.minor.patch components.
func version version_make(u8 major, u8 minor, u16 patch);

// Returns the packed 32-bit representation.
func u32 version_get_packed(version ver);

// Returns the major component.
func u8 version_get_major(version ver);

// Returns the minor component.
func u8 version_get_minor(version ver);

// Returns the patch component.
func u16 version_get_patch(version ver);

// Returns 1 if all version fields are zero, 0 otherwise.
func b32 version_is_zero(version ver);

// Returns the number of bytes needed for "major.minor.patch",
// excluding the null terminator.
func sz version_string_length(version ver);

// Compares two versions using major, then minor, then patch ordering.
func i32 version_cmp(version lhs, version rhs);

// Formats the version as "major.minor.patch". Returns 1 on success, 0 if cap is too small.
func b32 version_to_cstr8(version ver, c8* dst, sz cap);

// Formats the version as UTF-16. Returns 1 on success, 0 if cap is too small.
func b32 version_to_cstr16(version ver, c16* dst, sz cap);

// Formats the version as UTF-32. Returns 1 on success, 0 if cap is too small.
func b32 version_to_cstr32(version ver, c32* dst, sz cap);

// Writes the formatted version into a bounded UTF-8 string. Returns 1 on success.
func b32 version_to_str8(version ver, str8* dst);

// Writes the formatted version into a bounded UTF-16 string. Returns 1 on success.
func b32 version_to_str16(version ver, str16* dst);

// Writes the formatted version into a bounded UTF-32 string. Returns 1 on success.
func b32 version_to_str32(version ver, str32* dst);

// Parses \"major.minor.patch\".
func b32 version_parse_cstr8(cstr8 src, version* out_ver);

// =========================================================================
c_end;
// =========================================================================
