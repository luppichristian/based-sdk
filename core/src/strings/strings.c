// MIT License
// Copyright (c) 2026 Christian Luppi

#include "strings/strings.h"
#include "basic/assert.h"
#include "basic/profiler.h"

// =========================================================================
// str8
// =========================================================================

func str8 str8_make(c8* ptr, sz cap) {
  profile_func_begin;
  if (ptr == NULL || cap == 0) {
    profile_func_end;
    return (str8) {0};
  }
  assert(ptr != NULL);
  str8 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = cstr8_len(ptr);
  profile_func_end;
  return str;
}

func str8 str8_empty(c8* ptr, sz cap) {
  profile_func_begin;
  str8 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = 0;
  cstr8_clear(ptr);
  profile_func_end;
  return str;
}

func str8 str8_from_cstr(c8* ptr, sz cap, cstr8 src) {
  profile_func_begin;
  str8 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = cstr8_copy(ptr, cap, src);
  profile_func_end;
  return str;
}

func b32 str8_is_empty(str8 str) {
  return str.size == 0 ? true : false;
}

func b32 str8_cmp(str8 lhs, str8 rhs) {
  profile_func_begin;
  b32 res = cstr8_cmp(lhs.ptr, rhs.ptr);
  profile_func_end;
  return res;
}

func b32 str8_cmp_nocase(str8 lhs, str8 rhs) {
  profile_func_begin;
  b32 res = cstr8_cmp_nocase(lhs.ptr, rhs.ptr);
  profile_func_end;
  return res;
}

func cstr8 str8_find(str8 str, cstr8 sub) {
  profile_func_begin;
  cstr8 res = cstr8_find(str.ptr, sub);
  profile_func_end;
  return res;
}

func cstr8 str8_find_char(str8 str, c8 chr) {
  profile_func_begin;
  cstr8 res = cstr8_find_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func cstr8 str8_find_last(str8 str, cstr8 sub) {
  profile_func_begin;
  cstr8 res = cstr8_find_last(str.ptr, sub);
  profile_func_end;
  return res;
}

func cstr8 str8_find_last_char(str8 str, c8 chr) {
  profile_func_begin;
  cstr8 res = cstr8_find_last_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func sz str8_count_char(str8 str, c8 chr) {
  profile_func_begin;
  sz res = cstr8_count_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func b32 str8_starts_with(str8 str, cstr8 prefix) {
  profile_func_begin;
  b32 res = cstr8_starts_with(str.ptr, prefix);
  profile_func_end;
  return res;
}

func b32 str8_ends_with(str8 str, cstr8 suffix) {
  profile_func_begin;
  b32 res = cstr8_ends_with(str.ptr, suffix);
  profile_func_end;
  return res;
}

func u32 str8_hash32(str8 str) {
  profile_func_begin;
  u32 res = cstr8_hash32(str.ptr);
  profile_func_end;
  return res;
}

func u64 str8_hash64(str8 str) {
  profile_func_begin;
  u64 res = cstr8_hash64(str.ptr);
  profile_func_end;
  return res;
}

func b32 str8_to_i64(str8 str, i64* out) {
  profile_func_begin;
  b32 res = cstr8_to_i64(str.ptr, out);
  profile_func_end;
  return res;
}

func b32 str8_to_u64(str8 str, u64 max_value, u64* out) {
  profile_func_begin;
  b32 res = cstr8_to_u64(str.ptr, max_value, out);
  profile_func_end;
  return res;
}

func b32 str8_to_f64(str8 str, f64* out) {
  profile_func_begin;
  b32 res = cstr8_to_f64(str.ptr, out);
  profile_func_end;
  return res;
}

func void str8_clear(str8* str) {
  profile_func_begin;
  if (str == NULL || str->ptr == NULL) {
    profile_func_end;
    return;
  }
  assert(str != NULL);
  cstr8_clear(str->ptr);
  str->size = 0;
  profile_func_end;
}

func sz str8_copy(str8* str, cstr8 src) {
  profile_func_begin;
  str->size = cstr8_copy(str->ptr, str->cap, src);
  profile_func_end;
  return str->size;
}

func sz str8_cat(str8* str, cstr8 src) {
  profile_func_begin;
  str->size = cstr8_cat(str->ptr, str->cap, src);
  profile_func_end;
  return str->size;
}

func sz str8_append_char(str8* str, c8 chr) {
  profile_func_begin;
  str->size = cstr8_append_char(str->ptr, str->cap, chr);
  profile_func_end;
  return str->size;
}

func void str8_truncate(str8* str, sz length) {
  profile_func_begin;
  cstr8_truncate(str->ptr, length);
  if (length < str->size) {
    str->size = length;
  }
  profile_func_end;
}

func b32 str8_format(str8* str, cstr8 fmt, ...) {
  profile_func_begin;
  if (str == NULL || str->ptr == NULL || fmt == NULL) {
    profile_func_end;
    return false;
  }
  assert(fmt != NULL);
  va_list args;
  va_start(args, fmt);
  b32 result = cstr8_vformat(str->ptr, str->cap, fmt, args);
  va_end(args);
  str->size = cstr8_len(str->ptr);
  profile_func_end;
  return result;
}

func b32 str8_vformat(str8* str, cstr8 fmt, va_list args) {
  profile_func_begin;
  b32 result = cstr8_vformat(str->ptr, str->cap, fmt, args);
  str->size = cstr8_len(str->ptr);
  profile_func_end;
  return result;
}

func b32 str8_append_format(str8* str, cstr8 fmt, ...) {
  profile_func_begin;
  va_list args;
  va_start(args, fmt);
  b32 result = cstr8_append_vformat(str->ptr, str->cap, fmt, args);
  va_end(args);
  str->size = cstr8_len(str->ptr);
  profile_func_end;
  return result;
}

func b32 str8_append_vformat(str8* str, cstr8 fmt, va_list args) {
  profile_func_begin;
  b32 result = cstr8_append_vformat(str->ptr, str->cap, fmt, args);
  str->size = cstr8_len(str->ptr);
  profile_func_end;
  return result;
}

func void str8_to_upper(str8* str) {
  profile_func_begin;
  cstr8_to_upper(str->ptr);
  profile_func_end;
}

func void str8_to_lower(str8* str) {
  profile_func_begin;
  cstr8_to_lower(str->ptr);
  profile_func_end;
}

func void str8_trim(str8* str) {
  profile_func_begin;
  cstr8_trim(str->ptr);
  str->size = cstr8_len(str->ptr);
  profile_func_end;
}

func void str8_replace_char(str8* str, c8 from_chr, c8 to_chr) {
  profile_func_begin;
  cstr8_replace_char(str->ptr, from_chr, to_chr);
  profile_func_end;
}

func sz str8_remove_char(str8* str, c8 chr) {
  profile_func_begin;
  sz removed = cstr8_remove_char(str->ptr, chr);
  str->size -= removed;
  profile_func_end;
  return removed;
}

func sz str8_remove_whitespace(str8* str) {
  profile_func_begin;
  sz removed = cstr8_remove_whitespace(str->ptr);
  str->size -= removed;
  profile_func_end;
  return removed;
}

func b32 str8_remove_prefix(str8* str, cstr8 prefix) {
  profile_func_begin;
  b32 result = cstr8_remove_prefix(str->ptr, prefix);
  if (result) {
    str->size = cstr8_len(str->ptr);
  }
  profile_func_end;
  return result;
}

func b32 str8_remove_suffix(str8* str, cstr8 suffix) {
  profile_func_begin;
  b32 result = cstr8_remove_suffix(str->ptr, suffix);
  if (result) {
    str->size = cstr8_len(str->ptr);
  }
  profile_func_end;
  return result;
}

func sz str8_replace(str8* str, cstr8 from, cstr8 rep) {
  profile_func_begin;
  sz count = cstr8_replace(str->ptr, str->cap, from, rep);
  str->size = cstr8_len(str->ptr);
  profile_func_end;
  return count;
}

func void str8_beautify(str8* str) {
  profile_func_begin;
  cstr8_beautify(str->ptr);
  profile_func_end;
}

// =========================================================================
// str16
// =========================================================================

func str16 str16_make(c16* ptr, sz cap) {
  profile_func_begin;
  str16 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = cstr16_len(ptr);
  profile_func_end;
  return str;
}

func str16 str16_empty(c16* ptr, sz cap) {
  profile_func_begin;
  str16 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = 0;
  cstr16_clear(ptr);
  profile_func_end;
  return str;
}

func str16 str16_from_cstr(c16* ptr, sz cap, cstr16 src) {
  profile_func_begin;
  str16 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = cstr16_copy(ptr, cap, src);
  profile_func_end;
  return str;
}

func b32 str16_is_empty(str16 str) {
  return str.size == 0 ? true : false;
}

func b32 str16_cmp(str16 lhs, str16 rhs) {
  profile_func_begin;
  b32 res = cstr16_cmp(lhs.ptr, rhs.ptr);
  profile_func_end;
  return res;
}

func b32 str16_cmp_nocase(str16 lhs, str16 rhs) {
  profile_func_begin;
  b32 res = cstr16_cmp_nocase(lhs.ptr, rhs.ptr);
  profile_func_end;
  return res;
}

func cstr16 str16_find(str16 str, cstr16 sub) {
  profile_func_begin;
  cstr16 res = cstr16_find(str.ptr, sub);
  profile_func_end;
  return res;
}

func cstr16 str16_find_char(str16 str, c16 chr) {
  profile_func_begin;
  cstr16 res = cstr16_find_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func cstr16 str16_find_last(str16 str, cstr16 sub) {
  profile_func_begin;
  cstr16 res = cstr16_find_last(str.ptr, sub);
  profile_func_end;
  return res;
}

func cstr16 str16_find_last_char(str16 str, c16 chr) {
  profile_func_begin;
  cstr16 res = cstr16_find_last_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func sz str16_count_char(str16 str, c16 chr) {
  profile_func_begin;
  sz res = cstr16_count_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func b32 str16_starts_with(str16 str, cstr16 prefix) {
  profile_func_begin;
  b32 res = cstr16_starts_with(str.ptr, prefix);
  profile_func_end;
  return res;
}

func b32 str16_ends_with(str16 str, cstr16 suffix) {
  profile_func_begin;
  b32 res = cstr16_ends_with(str.ptr, suffix);
  profile_func_end;
  return res;
}

func u32 str16_hash32(str16 str) {
  profile_func_begin;
  u32 res = cstr16_hash32(str.ptr);
  profile_func_end;
  return res;
}

func u64 str16_hash64(str16 str) {
  profile_func_begin;
  u64 res = cstr16_hash64(str.ptr);
  profile_func_end;
  return res;
}

func b32 str16_to_i64(str16 str, i64* out) {
  profile_func_begin;
  b32 res = cstr16_to_i64(str.ptr, out);
  profile_func_end;
  return res;
}

func b32 str16_to_f64(str16 str, f64* out) {
  profile_func_begin;
  b32 res = cstr16_to_f64(str.ptr, out);
  profile_func_end;
  return res;
}

func void str16_clear(str16* str) {
  profile_func_begin;
  cstr16_clear(str->ptr);
  str->size = 0;
  profile_func_end;
}

func sz str16_copy(str16* str, cstr16 src) {
  profile_func_begin;
  str->size = cstr16_copy(str->ptr, str->cap, src);
  profile_func_end;
  return str->size;
}

func sz str16_cat(str16* str, cstr16 src) {
  profile_func_begin;
  str->size = cstr16_cat(str->ptr, str->cap, src);
  profile_func_end;
  return str->size;
}

func sz str16_append_char(str16* str, c16 chr) {
  profile_func_begin;
  str->size = cstr16_append_char(str->ptr, str->cap, chr);
  profile_func_end;
  return str->size;
}

func void str16_truncate(str16* str, sz length) {
  profile_func_begin;
  cstr16_truncate(str->ptr, length);
  if (length < str->size) {
    str->size = length;
  }
  profile_func_end;
}

func void str16_to_upper(str16* str) {
  profile_func_begin;
  cstr16_to_upper(str->ptr);
  profile_func_end;
}

func void str16_to_lower(str16* str) {
  profile_func_begin;
  cstr16_to_lower(str->ptr);
  profile_func_end;
}

func void str16_trim(str16* str) {
  profile_func_begin;
  cstr16_trim(str->ptr);
  str->size = cstr16_len(str->ptr);
  profile_func_end;
}

func void str16_replace_char(str16* str, c16 from_chr, c16 to_chr) {
  profile_func_begin;
  cstr16_replace_char(str->ptr, from_chr, to_chr);
  profile_func_end;
}

func sz str16_remove_char(str16* str, c16 chr) {
  profile_func_begin;
  sz removed = cstr16_remove_char(str->ptr, chr);
  str->size -= removed;
  profile_func_end;
  return removed;
}

func sz str16_remove_whitespace(str16* str) {
  profile_func_begin;
  sz removed = cstr16_remove_whitespace(str->ptr);
  str->size -= removed;
  profile_func_end;
  return removed;
}

func b32 str16_remove_prefix(str16* str, cstr16 prefix) {
  profile_func_begin;
  b32 result = cstr16_remove_prefix(str->ptr, prefix);
  if (result) {
    str->size = cstr16_len(str->ptr);
  }
  profile_func_end;
  return result;
}

func b32 str16_remove_suffix(str16* str, cstr16 suffix) {
  profile_func_begin;
  b32 result = cstr16_remove_suffix(str->ptr, suffix);
  if (result) {
    str->size = cstr16_len(str->ptr);
  }
  profile_func_end;
  return result;
}

func sz str16_replace(str16* str, cstr16 from, cstr16 rep) {
  profile_func_begin;
  sz count = cstr16_replace(str->ptr, str->cap, from, rep);
  str->size = cstr16_len(str->ptr);
  profile_func_end;
  return count;
}

func void str16_beautify(str16* str) {
  profile_func_begin;
  cstr16_beautify(str->ptr);
  profile_func_end;
}

// =========================================================================
// str32
// =========================================================================

func str32 str32_make(c32* ptr, sz cap) {
  profile_func_begin;
  str32 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = cstr32_len(ptr);
  profile_func_end;
  return str;
}

func str32 str32_empty(c32* ptr, sz cap) {
  profile_func_begin;
  str32 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = 0;
  cstr32_clear(ptr);
  profile_func_end;
  return str;
}

func str32 str32_from_cstr(c32* ptr, sz cap, cstr32 src) {
  profile_func_begin;
  str32 str;
  str.ptr = ptr;
  str.cap = cap;
  str.size = cstr32_copy(ptr, cap, src);
  profile_func_end;
  return str;
}

func b32 str32_is_empty(str32 str) {
  return str.size == 0 ? true : false;
}

func b32 str32_cmp(str32 lhs, str32 rhs) {
  profile_func_begin;
  b32 res = cstr32_cmp(lhs.ptr, rhs.ptr);
  profile_func_end;
  return res;
}

func b32 str32_cmp_nocase(str32 lhs, str32 rhs) {
  profile_func_begin;
  b32 res = cstr32_cmp_nocase(lhs.ptr, rhs.ptr);
  profile_func_end;
  return res;
}

func cstr32 str32_find(str32 str, cstr32 sub) {
  profile_func_begin;
  cstr32 res = cstr32_find(str.ptr, sub);
  profile_func_end;
  return res;
}

func cstr32 str32_find_char(str32 str, c32 chr) {
  profile_func_begin;
  cstr32 res = cstr32_find_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func cstr32 str32_find_last(str32 str, cstr32 sub) {
  profile_func_begin;
  cstr32 res = cstr32_find_last(str.ptr, sub);
  profile_func_end;
  return res;
}

func cstr32 str32_find_last_char(str32 str, c32 chr) {
  profile_func_begin;
  cstr32 res = cstr32_find_last_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func sz str32_count_char(str32 str, c32 chr) {
  profile_func_begin;
  sz res = cstr32_count_char(str.ptr, chr);
  profile_func_end;
  return res;
}

func b32 str32_starts_with(str32 str, cstr32 prefix) {
  profile_func_begin;
  b32 res = cstr32_starts_with(str.ptr, prefix);
  profile_func_end;
  return res;
}

func b32 str32_ends_with(str32 str, cstr32 suffix) {
  profile_func_begin;
  b32 res = cstr32_ends_with(str.ptr, suffix);
  profile_func_end;
  return res;
}

func u32 str32_hash32(str32 str) {
  profile_func_begin;
  u32 res = cstr32_hash32(str.ptr);
  profile_func_end;
  return res;
}

func u64 str32_hash64(str32 str) {
  profile_func_begin;
  u64 res = cstr32_hash64(str.ptr);
  profile_func_end;
  return res;
}

func b32 str32_to_i64(str32 str, i64* out) {
  profile_func_begin;
  b32 res = cstr32_to_i64(str.ptr, out);
  profile_func_end;
  return res;
}

func b32 str32_to_f64(str32 str, f64* out) {
  profile_func_begin;
  b32 res = cstr32_to_f64(str.ptr, out);
  profile_func_end;
  return res;
}

func void str32_clear(str32* str) {
  profile_func_begin;
  cstr32_clear(str->ptr);
  str->size = 0;
  profile_func_end;
}

func sz str32_copy(str32* str, cstr32 src) {
  profile_func_begin;
  str->size = cstr32_copy(str->ptr, str->cap, src);
  profile_func_end;
  return str->size;
}

func sz str32_cat(str32* str, cstr32 src) {
  profile_func_begin;
  str->size = cstr32_cat(str->ptr, str->cap, src);
  profile_func_end;
  return str->size;
}

func sz str32_append_char(str32* str, c32 chr) {
  profile_func_begin;
  str->size = cstr32_append_char(str->ptr, str->cap, chr);
  profile_func_end;
  return str->size;
}

func void str32_truncate(str32* str, sz length) {
  profile_func_begin;
  cstr32_truncate(str->ptr, length);
  if (length < str->size) {
    str->size = length;
  }
  profile_func_end;
}

func void str32_to_upper(str32* str) {
  profile_func_begin;
  cstr32_to_upper(str->ptr);
  profile_func_end;
}

func void str32_to_lower(str32* str) {
  profile_func_begin;
  cstr32_to_lower(str->ptr);
  profile_func_end;
}

func void str32_trim(str32* str) {
  profile_func_begin;
  cstr32_trim(str->ptr);
  str->size = cstr32_len(str->ptr);
  profile_func_end;
}

func void str32_replace_char(str32* str, c32 from_chr, c32 to_chr) {
  profile_func_begin;
  cstr32_replace_char(str->ptr, from_chr, to_chr);
  profile_func_end;
}

func sz str32_remove_char(str32* str, c32 chr) {
  profile_func_begin;
  sz removed = cstr32_remove_char(str->ptr, chr);
  str->size -= removed;
  profile_func_end;
  return removed;
}

func sz str32_remove_whitespace(str32* str) {
  profile_func_begin;
  sz removed = cstr32_remove_whitespace(str->ptr);
  str->size -= removed;
  profile_func_end;
  return removed;
}

func b32 str32_remove_prefix(str32* str, cstr32 prefix) {
  profile_func_begin;
  b32 result = cstr32_remove_prefix(str->ptr, prefix);
  if (result) {
    str->size = cstr32_len(str->ptr);
  }
  profile_func_end;
  return result;
}

func b32 str32_remove_suffix(str32* str, cstr32 suffix) {
  profile_func_begin;
  b32 result = cstr32_remove_suffix(str->ptr, suffix);
  if (result) {
    str->size = cstr32_len(str->ptr);
  }
  profile_func_end;
  return result;
}

func sz str32_replace(str32* str, cstr32 from, cstr32 rep) {
  profile_func_begin;
  sz count = cstr32_replace(str->ptr, str->cap, from, rep);
  str->size = cstr32_len(str->ptr);
  profile_func_end;
  return count;
}

func void str32_beautify(str32* str) {
  profile_func_begin;
  cstr32_beautify(str->ptr);
  profile_func_end;
}

// =========================================================================
// Cross-encoding conversion
// =========================================================================

func sz str8_to_str16(str8 src, str16* dst) {
  profile_func_begin;
  if (dst == NULL || dst->ptr == NULL) {
    profile_func_end;
    return 0;
  }
  assert(dst != NULL);
  dst->size = cstr8_to_cstr16(src.ptr, dst->ptr, dst->cap);
  profile_func_end;
  return dst->size;
}

func sz str8_to_str32(str8 src, str32* dst) {
  profile_func_begin;
  dst->size = cstr8_to_cstr32(src.ptr, dst->ptr, dst->cap);
  profile_func_end;
  return dst->size;
}

func sz str16_to_str8(str16 src, str8* dst) {
  profile_func_begin;
  dst->size = cstr16_to_cstr8(src.ptr, dst->ptr, dst->cap);
  profile_func_end;
  return dst->size;
}

func sz str16_to_str32(str16 src, str32* dst) {
  profile_func_begin;
  dst->size = cstr16_to_cstr32(src.ptr, dst->ptr, dst->cap);
  profile_func_end;
  return dst->size;
}

func sz str32_to_str8(str32 src, str8* dst) {
  profile_func_begin;
  dst->size = cstr32_to_cstr8(src.ptr, dst->ptr, dst->cap);
  profile_func_end;
  return dst->size;
}

func sz str32_to_str16(str32 src, str16* dst) {
  profile_func_begin;
  dst->size = cstr32_to_cstr16(src.ptr, dst->ptr, dst->cap);
  profile_func_end;
  return dst->size;
}
