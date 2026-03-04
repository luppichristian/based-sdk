// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

#include <stdarg.h>

// =========================================================================
// cstr8 — Null-terminated UTF-8 string utilities
// =========================================================================

// Returns the number of c8 units in str, excluding the null terminator.
func sz cstr8_len(const c8* str);

// Returns 1 if str is empty (first byte is null), 0 otherwise.
func b32 cstr8_is_empty(const c8* str);

// Sets str to the empty string (writes a null terminator at index 0).
func void cstr8_clear(c8* str);

// Lexicographically compares lhs and rhs. Returns 0 if equal, <0 or >0 otherwise.
func i32 cstr8_cmp(const c8* lhs, const c8* rhs);

// Like cstr8_cmp but compares at most cnt code units.
func i32 cstr8_cmp_n(const c8* lhs, const c8* rhs, sz cnt);

// Case-insensitive lexicographic comparison (ASCII letters only). Returns 0, <0, or >0.
func i32 cstr8_cmp_nocase(const c8* lhs, const c8* rhs);

// Copies src into dst (at most dst_size - 1 units) and null-terminates.
// Returns the number of c8 units written, excluding the null terminator.
func sz cstr8_copy(c8* dst, sz dst_size, const c8* src);

// Like cstr8_copy but copies at most cnt units from src.
func sz cstr8_copy_n(c8* dst, sz dst_size, const c8* src, sz cnt);

// Appends src to dst. Returns the new length, or the original length if dst_cap was too small.
func sz cstr8_concat(c8* dst, sz dst_cap, const c8* src);

// Appends a single character to dst. Returns the new length, or original length if dst_cap was too small.
func sz cstr8_append_char(c8* dst, sz dst_cap, c8 chr);

// Truncates str to at most length units and null-terminates. No-op if length >= current length.
func void cstr8_truncate(c8* str, sz length);

// Writes a printf-style formatted string into dst. Returns 1 on success, 0 if dst_cap was too small.
func b32 cstr8_format(c8* dst, sz dst_cap, const c8* fmt, ...);

// Like cstr8_format but takes a va_list instead of variadic arguments.
func b32 cstr8_vformat(c8* dst, sz dst_cap, const c8* fmt, va_list args);

// Appends a printf-style formatted string to the existing content of dst.
// Returns 1 on success, 0 if dst_cap was too small.
func b32 cstr8_append_format(c8* dst, sz dst_cap, const c8* fmt, ...);

// Like cstr8_append_format but takes a va_list instead of variadic arguments.
func b32 cstr8_append_vformat(c8* dst, sz dst_cap, const c8* fmt, va_list args);

// Scans str using a scanf-style format string, writing parsed values into the variadic arguments.
// Returns 1 if at least one value was read successfully, 0 otherwise.
func b32 cstr8_scan(const c8* str, const c8* fmt, ...);

// Returns a pointer to the first occurrence of sub in str, or nullptr if not found.
func const c8* cstr8_find(const c8* str, const c8* sub);

// Returns a pointer to the last occurrence of sub in str, or nullptr if not found.
func const c8* cstr8_find_last(const c8* str, const c8* sub);

// Returns a pointer to the first occurrence of chr in str, or nullptr if not found.
func const c8* cstr8_find_char(const c8* str, c8 chr);

// Returns a pointer to the last occurrence of chr in str, or nullptr if not found.
func const c8* cstr8_find_last_char(const c8* str, c8 chr);

// Returns the number of times chr occurs in str.
func sz cstr8_count_char(const c8* str, c8 chr);

// Returns 1 if str begins with prefix, 0 otherwise.
func b32 cstr8_starts_with(const c8* str, const c8* prefix);

// Returns 1 if str ends with suffix, 0 otherwise.
func b32 cstr8_ends_with(const c8* str, const c8* suffix);

// Converts all ASCII letters in str to uppercase, in place.
func void cstr8_to_upper(c8* str);

// Converts all ASCII letters in str to lowercase, in place.
func void cstr8_to_lower(c8* str);

// Removes leading and trailing ASCII whitespace from str, in place.
func void cstr8_trim(c8* str);

// Replaces every occurrence of from_chr with to_chr in str, in place.
func void cstr8_replace_char(c8* str, c8 from_chr, c8 to_chr);

// Removes every occurrence of chr from str, in place. Returns the number removed.
func sz cstr8_remove_char(c8* str, c8 chr);

// Removes all ASCII whitespace from str, in place. Returns the number of bytes removed.
func sz cstr8_remove_whitespace(c8* str);

// Removes the given prefix from str if present. Returns 1 on success, 0 if prefix was not found.
func b32 cstr8_remove_prefix(c8* str, const c8* prefix);

// Removes the given suffix from str if present. Returns 1 on success, 0 if suffix was not found.
func b32 cstr8_remove_suffix(c8* str, const c8* suffix);

// Replaces every occurrence of from with rep in str, in place.
// Stops early if str_cap would be exceeded. Returns the number of replacements made.
func sz cstr8_replace(c8* str, sz str_cap, const c8* from, const c8* rep);

// Writes the common prefix of lhs and rhs into buf (at most buf_cap - 1 units) and null-terminates.
// Returns the length of the common prefix.
func sz cstr8_common_prefix(const c8* lhs, const c8* rhs, c8* buf, sz buf_cap);

// Lowercases all ASCII letters in str in place, then capitalizes the first character.
func void cstr8_beautify(c8* str);

// Parses str as a base-10 integer and writes the result to *out.
// Returns 1 on success, 0 on invalid input or overflow.
func b32 cstr8_to_i64(const c8* str, i64* out);

// Parses str as a floating-point number and writes the result to *out.
// Returns 1 on success, 0 on invalid input.
func b32 cstr8_to_f64(const c8* str, f64* out);

// =========================================================================
// cstr16 — Null-terminated UTF-16 string utilities
// =========================================================================

// Returns the number of c16 units in str, excluding the null terminator.
func sz cstr16_len(const c16* str);

// Returns 1 if str is empty (first unit is null), 0 otherwise.
func b32 cstr16_is_empty(const c16* str);

// Sets str to the empty string (writes a null terminator at index 0).
func void cstr16_clear(c16* str);

// Lexicographically compares lhs and rhs. Returns 0 if equal, <0 or >0 otherwise.
func i32 cstr16_cmp(const c16* lhs, const c16* rhs);

// Like cstr16_cmp but compares at most cnt code units.
func i32 cstr16_cmp_n(const c16* lhs, const c16* rhs, sz cnt);

// Case-insensitive lexicographic comparison (ASCII letters only). Returns 0, <0, or >0.
func i32 cstr16_cmp_nocase(const c16* lhs, const c16* rhs);

// Copies src into dst (at most dst_size - 1 units) and null-terminates.
// Returns the number of c16 units written, excluding the null terminator.
func sz cstr16_copy(c16* dst, sz dst_size, const c16* src);

// Like cstr16_copy but copies at most cnt units from src.
func sz cstr16_copy_n(c16* dst, sz dst_size, const c16* src, sz cnt);

// Appends src to dst. Returns the new length, or the original length if dst_cap was too small.
func sz cstr16_concat(c16* dst, sz dst_cap, const c16* src);

// Appends a single code unit to dst. Returns the new length, or original length if dst_cap was too small.
func sz cstr16_append_char(c16* dst, sz dst_cap, c16 chr);

// Truncates str to at most length units and null-terminates. No-op if length >= current length.
func void cstr16_truncate(c16* str, sz length);

// Returns a pointer to the first occurrence of sub in str, or nullptr if not found.
func const c16* cstr16_find(const c16* str, const c16* sub);

// Returns a pointer to the last occurrence of sub in str, or nullptr if not found.
func const c16* cstr16_find_last(const c16* str, const c16* sub);

// Returns a pointer to the first occurrence of chr in str, or nullptr if not found.
func const c16* cstr16_find_char(const c16* str, c16 chr);

// Returns a pointer to the last occurrence of chr in str, or nullptr if not found.
func const c16* cstr16_find_last_char(const c16* str, c16 chr);

// Returns the number of times chr occurs in str.
func sz cstr16_count_char(const c16* str, c16 chr);

// Returns 1 if str begins with prefix, 0 otherwise.
func b32 cstr16_starts_with(const c16* str, const c16* prefix);

// Returns 1 if str ends with suffix, 0 otherwise.
func b32 cstr16_ends_with(const c16* str, const c16* suffix);

// Converts all ASCII letters in str to uppercase, in place.
func void cstr16_to_upper(c16* str);

// Converts all ASCII letters in str to lowercase, in place.
func void cstr16_to_lower(c16* str);

// Removes leading and trailing ASCII whitespace from str, in place.
func void cstr16_trim(c16* str);

// Replaces every occurrence of from_chr with to_chr in str, in place.
func void cstr16_replace_char(c16* str, c16 from_chr, c16 to_chr);

// Removes every occurrence of chr from str, in place. Returns the number removed.
func sz cstr16_remove_char(c16* str, c16 chr);

// Removes all ASCII whitespace from str, in place. Returns the number of units removed.
func sz cstr16_remove_whitespace(c16* str);

// Removes the given prefix from str if present. Returns 1 on success, 0 if prefix was not found.
func b32 cstr16_remove_prefix(c16* str, const c16* prefix);

// Removes the given suffix from str if present. Returns 1 on success, 0 if suffix was not found.
func b32 cstr16_remove_suffix(c16* str, const c16* suffix);

// Replaces every occurrence of from with rep in str, in place.
// Stops early if str_cap would be exceeded. Returns the number of replacements made.
func sz cstr16_replace(c16* str, sz str_cap, const c16* from, const c16* rep);

// Writes the common prefix of lhs and rhs into buf (at most buf_cap - 1 units) and null-terminates.
// Returns the length of the common prefix.
func sz cstr16_common_prefix(const c16* lhs, const c16* rhs, c16* buf, sz buf_cap);

// Lowercases all ASCII letters in str in place, then capitalizes the first character.
func void cstr16_beautify(c16* str);

// Parses str as a base-10 integer and writes the result to *out.
// Returns 1 on success, 0 on invalid input or overflow.
func b32 cstr16_to_i64(const c16* str, i64* out);

// Parses str as a floating-point number and writes the result to *out.
// Returns 1 on success, 0 on invalid input.
func b32 cstr16_to_f64(const c16* str, f64* out);

// =========================================================================
// cstr32 — Null-terminated UTF-32 string utilities
// =========================================================================

// Returns the number of c32 units in str, excluding the null terminator.
func sz cstr32_len(const c32* str);

// Returns 1 if str is empty (first unit is null), 0 otherwise.
func b32 cstr32_is_empty(const c32* str);

// Sets str to the empty string (writes a null terminator at index 0).
func void cstr32_clear(c32* str);

// Lexicographically compares lhs and rhs. Returns 0 if equal, <0 or >0 otherwise.
func i32 cstr32_cmp(const c32* lhs, const c32* rhs);

// Like cstr32_cmp but compares at most cnt code units.
func i32 cstr32_cmp_n(const c32* lhs, const c32* rhs, sz cnt);

// Case-insensitive lexicographic comparison (ASCII letters only). Returns 0, <0, or >0.
func i32 cstr32_cmp_nocase(const c32* lhs, const c32* rhs);

// Copies src into dst (at most dst_size - 1 units) and null-terminates.
// Returns the number of c32 units written, excluding the null terminator.
func sz cstr32_copy(c32* dst, sz dst_size, const c32* src);

// Like cstr32_copy but copies at most cnt units from src.
func sz cstr32_copy_n(c32* dst, sz dst_size, const c32* src, sz cnt);

// Appends src to dst. Returns the new length, or the original length if dst_cap was too small.
func sz cstr32_concat(c32* dst, sz dst_cap, const c32* src);

// Appends a single codepoint to dst. Returns the new length, or original length if dst_cap was too small.
func sz cstr32_append_char(c32* dst, sz dst_cap, c32 chr);

// Truncates str to at most length units and null-terminates. No-op if length >= current length.
func void cstr32_truncate(c32* str, sz length);

// Returns a pointer to the first occurrence of sub in str, or nullptr if not found.
func const c32* cstr32_find(const c32* str, const c32* sub);

// Returns a pointer to the last occurrence of sub in str, or nullptr if not found.
func const c32* cstr32_find_last(const c32* str, const c32* sub);

// Returns a pointer to the first occurrence of chr in str, or nullptr if not found.
func const c32* cstr32_find_char(const c32* str, c32 chr);

// Returns a pointer to the last occurrence of chr in str, or nullptr if not found.
func const c32* cstr32_find_last_char(const c32* str, c32 chr);

// Returns the number of times chr occurs in str.
func sz cstr32_count_char(const c32* str, c32 chr);

// Returns 1 if str begins with prefix, 0 otherwise.
func b32 cstr32_starts_with(const c32* str, const c32* prefix);

// Returns 1 if str ends with suffix, 0 otherwise.
func b32 cstr32_ends_with(const c32* str, const c32* suffix);

// Converts all ASCII letters in str to uppercase, in place.
func void cstr32_to_upper(c32* str);

// Converts all ASCII letters in str to lowercase, in place.
func void cstr32_to_lower(c32* str);

// Removes leading and trailing ASCII whitespace from str, in place.
func void cstr32_trim(c32* str);

// Replaces every occurrence of from_chr with to_chr in str, in place.
func void cstr32_replace_char(c32* str, c32 from_chr, c32 to_chr);

// Removes every occurrence of chr from str, in place. Returns the number removed.
func sz cstr32_remove_char(c32* str, c32 chr);

// Removes all ASCII whitespace from str, in place. Returns the number of units removed.
func sz cstr32_remove_whitespace(c32* str);

// Removes the given prefix from str if present. Returns 1 on success, 0 if prefix was not found.
func b32 cstr32_remove_prefix(c32* str, const c32* prefix);

// Removes the given suffix from str if present. Returns 1 on success, 0 if suffix was not found.
func b32 cstr32_remove_suffix(c32* str, const c32* suffix);

// Replaces every occurrence of from with rep in str, in place.
// Stops early if str_cap would be exceeded. Returns the number of replacements made.
func sz cstr32_replace(c32* str, sz str_cap, const c32* from, const c32* rep);

// Writes the common prefix of lhs and rhs into buf (at most buf_cap - 1 units) and null-terminates.
// Returns the length of the common prefix.
func sz cstr32_common_prefix(const c32* lhs, const c32* rhs, c32* buf, sz buf_cap);

// Lowercases all ASCII letters in str in place, then capitalizes the first character.
func void cstr32_beautify(c32* str);

// Parses str as a base-10 integer and writes the result to *out.
// Returns 1 on success, 0 on invalid input or overflow.
func b32 cstr32_to_i64(const c32* str, i64* out);

// Parses str as a floating-point number and writes the result to *out.
// Returns 1 on success, 0 on invalid input.
func b32 cstr32_to_f64(const c32* str, f64* out);

// =========================================================================
// Cross-encoding conversion (null-terminated)
// =========================================================================

// Each function converts a null-terminated source string to the target encoding,
// writing at most buf_cap - 1 output units into buf and always null-terminating.
// Returns the number of code units written, excluding the null terminator.

func sz cstr8_to_cstr16(const c8* src, c16* buf, sz buf_cap);
func sz cstr8_to_cstr32(const c8* src, c32* buf, sz buf_cap);
func sz cstr16_to_cstr8(const c16* src, c8* buf, sz buf_cap);
func sz cstr16_to_cstr32(const c16* src, c32* buf, sz buf_cap);
func sz cstr32_to_cstr8(const c32* src, c8* buf, sz buf_cap);
func sz cstr32_to_cstr16(const c32* src, c16* buf, sz buf_cap);

// =========================================================================
// Capacity constants
// =========================================================================

// Predefined capacity values for the fixed-size string types below.
// Macros (not variables) so they are valid in array bounds / typedefs.
const_var sz STR_CAP_TINY = 16;
const_var sz STR_CAP_SHORT = 64;
const_var sz STR_CAP_MEDIUM = 512;
const_var sz STR_CAP_LONG = 1024;
const_var sz STR_CAP_LARGE = 2048;

// =========================================================================
// str8 — Fixed-capacity UTF-8 strings
// =========================================================================

// Stack-allocated, null-terminated UTF-8 strings with a compile-time capacity.
// All operations are provided via macros that automatically forward countof(buf)
// as the capacity, so the caller never has to spell out a size.
typedef c8 str8_tiny[STR_CAP_TINY];
typedef c8 str8_short[STR_CAP_SHORT];
typedef c8 str8_medium[STR_CAP_MEDIUM];
typedef c8 str8_long[STR_CAP_LONG];
typedef c8 str8_large[STR_CAP_LARGE];

// =========================================================================
// str16 — Fixed-capacity UTF-16 strings
// =========================================================================

typedef c16 str16_tiny[STR_CAP_TINY];
typedef c16 str16_short[STR_CAP_SHORT];
typedef c16 str16_medium[STR_CAP_MEDIUM];
typedef c16 str16_long[STR_CAP_LONG];
typedef c16 str16_large[STR_CAP_LARGE];

// =========================================================================
// str32 — Fixed-capacity UTF-32 strings
// =========================================================================

typedef c32 str32_tiny[STR_CAP_TINY];
typedef c32 str32_short[STR_CAP_SHORT];
typedef c32 str32_medium[STR_CAP_MEDIUM];
typedef c32 str32_long[STR_CAP_LONG];
typedef c32 str32_large[STR_CAP_LARGE];