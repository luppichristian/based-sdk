// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "cstrings.h"

#include <stdarg.h>

// =========================================================================
// str8 — Variable-length UTF-8 string
// =========================================================================

// Non-owning, mutable, bounded UTF-8 string.
// ptr points to a writable buffer of cap bytes. size tracks the current string
// length excluding the null terminator. Invariant: size < cap, ptr[size] == '\0'.
typedef struct str8 {
  sz size;
  sz cap;
  c8* ptr;
} str8;

// Wraps ptr and reads the current length from the null-terminated content.
func str8 str8_make(c8* ptr, sz cap);

// Wraps ptr and initializes it to an empty string (writes null terminator).
func str8 str8_empty(c8* ptr, sz cap);

// Copies src into ptr, then wraps the result.
func str8 str8_from_cstr(c8* ptr, sz cap, cstr8 src);

// Returns 1 if str has zero bytes, 0 otherwise.
func b32 str8_is_empty(str8 str);

// Lexicographically compares lhs and rhs. Returns 0 if equal, <0 or >0 otherwise.
func i32 str8_cmp(str8 lhs, str8 rhs);

// Case-insensitive lexicographic comparison (ASCII letters only). Returns 0, <0, or >0.
func i32 str8_cmp_nocase(str8 lhs, str8 rhs);

// Returns a pointer to the first occurrence of sub in str, or nullptr if not found.
func cstr8 str8_find(str8 str, cstr8 sub);

// Returns a pointer to the first occurrence of chr in str, or nullptr if not found.
func cstr8 str8_find_char(str8 str, c8 chr);

// Returns a pointer to the last occurrence of sub in str, or nullptr if not found.
func cstr8 str8_find_last(str8 str, cstr8 sub);

// Returns a pointer to the last occurrence of chr in str, or nullptr if not found.
func cstr8 str8_find_last_char(str8 str, c8 chr);

// Returns the number of times chr occurs in str.
func sz str8_count_char(str8 str, c8 chr);

// Returns 1 if str begins with prefix, 0 otherwise.
func b32 str8_starts_with(str8 str, cstr8 prefix);

// Returns 1 if str ends with suffix, 0 otherwise.
func b32 str8_ends_with(str8 str, cstr8 suffix);

// Parses str as a base-10 integer and writes the result to *out.
// Returns 1 on success, 0 on invalid input or overflow.
func b32 str8_to_i64(str8 str, i64* out);

// Parses str as a floating-point number and writes the result to *out.
// Returns 1 on success, 0 on invalid input.
func b32 str8_to_f64(str8 str, f64* out);

// Sets str to the empty string.
func void str8_clear(str8* str);

// Copies src into str (at most cap - 1 bytes). Returns the number of bytes written.
func sz str8_copy(str8* str, cstr8 src);

// Appends src to str. Returns the new length, or original length if cap was too small.
func sz str8_cat(str8* str, cstr8 src);

// Appends a single character to str. Returns the new length, or original length if cap was too small.
func sz str8_append_char(str8* str, c8 chr);

// Truncates str to at most length bytes. No-op if length >= current size.
func void str8_truncate(str8* str, sz length);

// Writes a printf-style formatted string into str. Returns 1 on success, 0 if cap was too small.
func b32 str8_format(str8* str, cstr8 fmt, ...);

// Like str8_format but takes a va_list instead of variadic arguments.
func b32 str8_vformat(str8* str, cstr8 fmt, va_list args);

// Appends a printf-style formatted string to str. Returns 1 on success, 0 if cap was too small.
func b32 str8_append_format(str8* str, cstr8 fmt, ...);

// Like str8_append_format but takes a va_list instead of variadic arguments.
func b32 str8_append_vformat(str8* str, cstr8 fmt, va_list args);

// Converts all ASCII letters in str to uppercase, in place.
func void str8_to_upper(str8* str);

// Converts all ASCII letters in str to lowercase, in place.
func void str8_to_lower(str8* str);

// Removes leading and trailing ASCII whitespace from str, in place.
func void str8_trim(str8* str);

// Replaces every occurrence of from_chr with to_chr in str, in place.
func void str8_replace_char(str8* str, c8 from_chr, c8 to_chr);

// Removes every occurrence of chr from str, in place. Returns the number removed.
func sz str8_remove_char(str8* str, c8 chr);

// Removes all ASCII whitespace from str, in place. Returns the number of bytes removed.
func sz str8_remove_whitespace(str8* str);

// Removes the given prefix from str if present. Returns 1 on success, 0 if not found.
func b32 str8_remove_prefix(str8* str, cstr8 prefix);

// Removes the given suffix from str if present. Returns 1 on success, 0 if not found.
func b32 str8_remove_suffix(str8* str, cstr8 suffix);

// Replaces every occurrence of from with rep in str, in place.
// Stops early if cap would be exceeded. Returns the number of replacements made.
func sz str8_replace(str8* str, cstr8 from, cstr8 rep);

// Lowercases all ASCII letters in str in place, then capitalizes the first character.
func void str8_beautify(str8* str);
func b32 str8_split_next(cstr8_tokenizer* tok, str8* out_token);
func b32 str8_join(str8* dst, cstr8 const* parts, sz part_count, cstr8 delim);

// =========================================================================
// str16 — Variable-length UTF-16 string
// =========================================================================

// Non-owning, mutable, bounded UTF-16 string.
// ptr points to a writable buffer of cap c16 units. size tracks the current string
// length in c16 units excluding the null terminator. Invariant: size < cap, ptr[size] == 0.
typedef struct str16 {
  sz size;
  sz cap;
  c16* ptr;
} str16;

// Wraps ptr and reads the current length from the null-terminated content.
func str16 str16_make(c16* ptr, sz cap);

// Wraps ptr and initializes it to an empty string (writes null terminator).
func str16 str16_empty(c16* ptr, sz cap);

// Copies src into ptr, then wraps the result.
func str16 str16_from_cstr(c16* ptr, sz cap, cstr16 src);

// Returns 1 if str has zero code units, 0 otherwise.
func b32 str16_is_empty(str16 str);

// Lexicographically compares lhs and rhs. Returns 0 if equal, <0 or >0 otherwise.
func i32 str16_cmp(str16 lhs, str16 rhs);

// Case-insensitive lexicographic comparison (ASCII letters only). Returns 0, <0, or >0.
func i32 str16_cmp_nocase(str16 lhs, str16 rhs);

// Returns a pointer to the first occurrence of sub in str, or nullptr if not found.
func cstr16 str16_find(str16 str, cstr16 sub);

// Returns a pointer to the first occurrence of chr in str, or nullptr if not found.
func cstr16 str16_find_char(str16 str, c16 chr);

// Returns a pointer to the last occurrence of sub in str, or nullptr if not found.
func cstr16 str16_find_last(str16 str, cstr16 sub);

// Returns a pointer to the last occurrence of chr in str, or nullptr if not found.
func cstr16 str16_find_last_char(str16 str, c16 chr);

// Returns the number of times chr occurs in str.
func sz str16_count_char(str16 str, c16 chr);

// Returns 1 if str begins with prefix, 0 otherwise.
func b32 str16_starts_with(str16 str, cstr16 prefix);

// Returns 1 if str ends with suffix, 0 otherwise.
func b32 str16_ends_with(str16 str, cstr16 suffix);

// Parses str as a base-10 integer and writes the result to *out.
// Returns 1 on success, 0 on invalid input or overflow.
func b32 str16_to_i64(str16 str, i64* out);

// Parses str as a floating-point number and writes the result to *out.
// Returns 1 on success, 0 on invalid input.
func b32 str16_to_f64(str16 str, f64* out);

// Sets str to the empty string.
func void str16_clear(str16* str);

// Copies src into str (at most cap - 1 units). Returns the number of units written.
func sz str16_copy(str16* str, cstr16 src);

// Appends src to str. Returns the new length, or original length if cap was too small.
func sz str16_cat(str16* str, cstr16 src);

// Appends a single code unit to str. Returns the new length, or original length if cap was too small.
func sz str16_append_char(str16* str, c16 chr);

// Truncates str to at most length code units. No-op if length >= current size.
func void str16_truncate(str16* str, sz length);

// Converts all ASCII letters in str to uppercase, in place.
func void str16_to_upper(str16* str);

// Converts all ASCII letters in str to lowercase, in place.
func void str16_to_lower(str16* str);

// Removes leading and trailing ASCII whitespace from str, in place.
func void str16_trim(str16* str);

// Replaces every occurrence of from_chr with to_chr in str, in place.
func void str16_replace_char(str16* str, c16 from_chr, c16 to_chr);

// Removes every occurrence of chr from str, in place. Returns the number removed.
func sz str16_remove_char(str16* str, c16 chr);

// Removes all ASCII whitespace from str, in place. Returns the number of units removed.
func sz str16_remove_whitespace(str16* str);

// Removes the given prefix from str if present. Returns 1 on success, 0 if not found.
func b32 str16_remove_prefix(str16* str, cstr16 prefix);

// Removes the given suffix from str if present. Returns 1 on success, 0 if not found.
func b32 str16_remove_suffix(str16* str, cstr16 suffix);

// Replaces every occurrence of from with rep in str, in place.
// Stops early if cap would be exceeded. Returns the number of replacements made.
func sz str16_replace(str16* str, cstr16 from, cstr16 rep);

// Lowercases all ASCII letters in str in place, then capitalizes the first character.
func void str16_beautify(str16* str);

// =========================================================================
// str32 — Variable-length UTF-32 string
// =========================================================================

// Non-owning, mutable, bounded UTF-32 string.
// ptr points to a writable buffer of cap c32 units. size tracks the current string
// length in codepoints excluding the null terminator. Invariant: size < cap, ptr[size] == 0.
typedef struct str32 {
  sz size;
  sz cap;
  c32* ptr;
} str32;

// Wraps ptr and reads the current length from the null-terminated content.
func str32 str32_make(c32* ptr, sz cap);

// Wraps ptr and initializes it to an empty string (writes null terminator).
func str32 str32_empty(c32* ptr, sz cap);

// Copies src into ptr, then wraps the result.
func str32 str32_from_cstr(c32* ptr, sz cap, cstr32 src);

// Returns 1 if str has zero codepoints, 0 otherwise.
func b32 str32_is_empty(str32 str);

// Lexicographically compares lhs and rhs. Returns 0 if equal, <0 or >0 otherwise.
func i32 str32_cmp(str32 lhs, str32 rhs);

// Case-insensitive lexicographic comparison (ASCII letters only). Returns 0, <0, or >0.
func i32 str32_cmp_nocase(str32 lhs, str32 rhs);

// Returns a pointer to the first occurrence of sub in str, or nullptr if not found.
func cstr32 str32_find(str32 str, cstr32 sub);

// Returns a pointer to the first occurrence of chr in str, or nullptr if not found.
func cstr32 str32_find_char(str32 str, c32 chr);

// Returns a pointer to the last occurrence of sub in str, or nullptr if not found.
func cstr32 str32_find_last(str32 str, cstr32 sub);

// Returns a pointer to the last occurrence of chr in str, or nullptr if not found.
func cstr32 str32_find_last_char(str32 str, c32 chr);

// Returns the number of times chr occurs in str.
func sz str32_count_char(str32 str, c32 chr);

// Returns 1 if str begins with prefix, 0 otherwise.
func b32 str32_starts_with(str32 str, cstr32 prefix);

// Returns 1 if str ends with suffix, 0 otherwise.
func b32 str32_ends_with(str32 str, cstr32 suffix);

// Parses str as a base-10 integer and writes the result to *out.
// Returns 1 on success, 0 on invalid input or overflow.
func b32 str32_to_i64(str32 str, i64* out);

// Parses str as a floating-point number and writes the result to *out.
// Returns 1 on success, 0 on invalid input.
func b32 str32_to_f64(str32 str, f64* out);

// Sets str to the empty string.
func void str32_clear(str32* str);

// Copies src into str (at most cap - 1 units). Returns the number of units written.
func sz str32_copy(str32* str, cstr32 src);

// Appends src to str. Returns the new length, or original length if cap was too small.
func sz str32_cat(str32* str, cstr32 src);

// Appends a single codepoint to str. Returns the new length, or original length if cap was too small.
func sz str32_append_char(str32* str, c32 chr);

// Truncates str to at most length codepoints. No-op if length >= current size.
func void str32_truncate(str32* str, sz length);

// Converts all ASCII letters in str to uppercase, in place.
func void str32_to_upper(str32* str);

// Converts all ASCII letters in str to lowercase, in place.
func void str32_to_lower(str32* str);

// Removes leading and trailing ASCII whitespace from str, in place.
func void str32_trim(str32* str);

// Replaces every occurrence of from_chr with to_chr in str, in place.
func void str32_replace_char(str32* str, c32 from_chr, c32 to_chr);

// Removes every occurrence of chr from str, in place. Returns the number removed.
func sz str32_remove_char(str32* str, c32 chr);

// Removes all ASCII whitespace from str, in place. Returns the number of units removed.
func sz str32_remove_whitespace(str32* str);

// Removes the given prefix from str if present. Returns 1 on success, 0 if not found.
func b32 str32_remove_prefix(str32* str, cstr32 prefix);

// Removes the given suffix from str if present. Returns 1 on success, 0 if not found.
func b32 str32_remove_suffix(str32* str, cstr32 suffix);

// Replaces every occurrence of from with rep in str, in place.
// Stops early if cap would be exceeded. Returns the number of replacements made.
func sz str32_replace(str32* str, cstr32 from, cstr32 rep);

// Lowercases all ASCII letters in str in place, then capitalizes the first character.
func void str32_beautify(str32* str);

// =========================================================================
// Cross-encoding conversion
// =========================================================================

// Each function converts a source string to the target encoding,
// writing at most dst->cap - 1 output units into dst and always null-terminating.
// Updates dst->size on return.
// Returns the number of code units written, excluding the null terminator.

func sz str8_to_str16(str8 src, str16* dst);
func sz str8_to_str32(str8 src, str32* dst);
func sz str16_to_str8(str16 src, str8* dst);
func sz str16_to_str32(str16 src, str32* dst);
func sz str32_to_str8(str32 src, str8* dst);
func sz str32_to_str16(str32 src, str16* dst);
