// MIT License
// Copyright (c) 2026 Christian Luppi

#include "strings/cstrings.h"
#include "strings/char.h"
#include "strings/unicode.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================================
// Internal helpers
// =========================================================================

func sz cstr16_len_impl(const c16* str) {
  sz len = 0;
  while (str[len] != (c16)'\0') {
    len++;
  }
  return len;
}

func sz cstr32_len_impl(const c32* str) {
  sz len = 0;
  while (str[len] != (c32)'\0') {
    len++;
  }
  return len;
}

// =========================================================================
// cstr8 — Basic
// =========================================================================

func sz cstr8_len(const c8* str) {
  return strlen(str);
}

func b32 cstr8_is_empty(const c8* str) {
  return str[0] == '\0' ? 1 : 0;
}

func void cstr8_clear(c8* str) {
  str[0] = '\0';
}

func i32 cstr8_cmp(const c8* lhs, const c8* rhs) {
  return strcmp(lhs, rhs);
}

func i32 cstr8_cmp_n(const c8* lhs, const c8* rhs, sz cnt) {
  return strncmp(lhs, rhs, cnt);
}

func i32 cstr8_cmp_nocase(const c8* lhs, const c8* rhs) {
  while (*lhs != '\0' && *rhs != '\0') {
    u8 lchr = (u8)c8_to_lower(*lhs);
    u8 rchr = (u8)c8_to_lower(*rhs);
    if (lchr < rchr) {
      return -1;
    }
    if (lchr > rchr) {
      return 1;
    }
    lhs++;
    rhs++;
  }
  u8 lchr = (u8)c8_to_lower(*lhs);
  u8 rchr = (u8)c8_to_lower(*rhs);
  if (lchr < rchr) {
    return -1;
  }
  if (lchr > rchr) {
    return 1;
  }
  return 0;
}

// =========================================================================
// cstr8 — Copy / Append
// =========================================================================

func sz cstr8_copy(c8* dst, sz dst_size, const c8* src) {
  if (dst_size == 0) {
    return 0;
  }
  sz src_len = strlen(src);
  sz copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
  memcpy(dst, src, copy_len);
  dst[copy_len] = '\0';
  return copy_len;
}

func sz cstr8_copy_n(c8* dst, sz dst_size, const c8* src, sz cnt) {
  if (dst_size == 0) {
    return 0;
  }
  sz src_len = strlen(src);
  sz max_copy = cnt < src_len ? cnt : src_len;
  sz copy_len = max_copy < dst_size - 1 ? max_copy : dst_size - 1;
  memcpy(dst, src, copy_len);
  dst[copy_len] = '\0';
  return copy_len;
}

func sz cstr8_concat(c8* dst, sz dst_cap, const c8* src) {
  if (dst_cap == 0) {
    return 0;
  }
  sz dst_len = strlen(dst);
  if (dst_len >= dst_cap - 1) {
    return dst_len;
  }
  sz remaining = dst_cap - dst_len - 1;
  sz src_len = strlen(src);
  sz copy_len = src_len < remaining ? src_len : remaining;
  memcpy(dst + dst_len, src, copy_len);
  dst[dst_len + copy_len] = '\0';
  return dst_len + copy_len;
}

func sz cstr8_append_char(c8* dst, sz dst_cap, c8 chr) {
  if (dst_cap == 0) {
    return 0;
  }
  sz len = strlen(dst);
  if (len >= dst_cap - 1) {
    return len;
  }
  dst[len] = chr;
  dst[len + 1] = '\0';
  return len + 1;
}

func void cstr8_truncate(c8* str, sz length) {
  if (strlen(str) > length) {
    str[length] = '\0';
  }
}

func b32 cstr8_format(c8* dst, sz dst_cap, const c8* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(dst, dst_cap, fmt, args);
  va_end(args);
  return (result >= 0 && (sz)result < dst_cap) ? 1 : 0;
}

func b32 cstr8_vformat(c8* dst, sz dst_cap, const c8* fmt, va_list args) {
  int result = vsnprintf(dst, dst_cap, fmt, args);
  return (result >= 0 && (sz)result < dst_cap) ? 1 : 0;
}

func b32 cstr8_append_format(c8* dst, sz dst_cap, const c8* fmt, ...) {
  sz len = strlen(dst);
  if (len >= dst_cap) {
    return 0;
  }
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(dst + len, dst_cap - len, fmt, args);
  va_end(args);
  return (result >= 0 && (sz)result < dst_cap - len) ? 1 : 0;
}

func b32 cstr8_append_vformat(c8* dst, sz dst_cap, const c8* fmt, va_list args) {
  sz len = strlen(dst);
  if (len >= dst_cap) {
    return 0;
  }
  int result = vsnprintf(dst + len, dst_cap - len, fmt, args);
  return (result >= 0 && (sz)result < dst_cap - len) ? 1 : 0;
}

func b32 cstr8_scan(const c8* str, const c8* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = vsscanf(str, fmt, args);
  va_end(args);
  return result > 0 ? 1 : 0;
}

// =========================================================================
// cstr8 — Search
// =========================================================================

func const c8* cstr8_find(const c8* str, const c8* sub) {
  return strstr(str, sub);
}

func const c8* cstr8_find_last(const c8* str, const c8* sub) {
  if (sub[0] == '\0') {
    return str + strlen(str);
  }
  sz sub_len = strlen(sub);
  sz str_len = strlen(str);
  if (sub_len > str_len) {
    return NULL;
  }
  const c8* last = NULL;
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len) == 0) {
      last = str + idx;
    }
  }
  return last;
}

func const c8* cstr8_find_char(const c8* str, c8 chr) {
  return strchr(str, (int)(unsigned char)chr);
}

func const c8* cstr8_find_last_char(const c8* str, c8 chr) {
  return strrchr(str, (int)(unsigned char)chr);
}

func sz cstr8_count_char(const c8* str, c8 chr) {
  sz count = 0;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    if (str[idx] == chr) {
      count++;
    }
  }
  return count;
}

func b32 cstr8_starts_with(const c8* str, const c8* prefix) {
  sz prefix_len = strlen(prefix);
  return strncmp(str, prefix, prefix_len) == 0 ? 1 : 0;
}

func b32 cstr8_ends_with(const c8* str, const c8* suffix) {
  sz str_len = strlen(str);
  sz suffix_len = strlen(suffix);
  if (suffix_len > str_len) {
    return 0;
  }
  return strcmp(str + str_len - suffix_len, suffix) == 0 ? 1 : 0;
}

// =========================================================================
// cstr8 — In-place mutation
// =========================================================================

func void cstr8_to_upper(c8* str) {
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    str[idx] = c8_to_upper(str[idx]);
  }
}

func void cstr8_to_lower(c8* str) {
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    str[idx] = c8_to_lower(str[idx]);
  }
}

func void cstr8_trim(c8* str) {
  sz start = 0;
  while (c8_is_space(str[start])) {
    start++;
  }
  sz len = strlen(str + start);
  while (len > 0 && c8_is_space(str[start + len - 1])) {
    len--;
  }
  memmove(str, str + start, len);
  str[len] = '\0';
}

func void cstr8_replace_char(c8* str, c8 from_chr, c8 to_chr) {
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    if (str[idx] == from_chr) {
      str[idx] = to_chr;
    }
  }
}

func sz cstr8_remove_char(c8* str, c8 chr) {
  sz write = 0;
  sz removed = 0;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    if (str[idx] != chr) {
      str[write++] = str[idx];
    } else {
      removed++;
    }
  }
  str[write] = '\0';
  return removed;
}

func sz cstr8_remove_whitespace(c8* str) {
  sz write = 0;
  sz removed = 0;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    if (!c8_is_space(str[idx])) {
      str[write++] = str[idx];
    } else {
      removed++;
    }
  }
  str[write] = '\0';
  return removed;
}

func b32 cstr8_remove_prefix(c8* str, const c8* prefix) {
  sz prefix_len = strlen(prefix);
  if (strncmp(str, prefix, prefix_len) != 0) {
    return 0;
  }
  sz str_len = strlen(str);
  memmove(str, str + prefix_len, str_len - prefix_len + 1);
  return 1;
}

func b32 cstr8_remove_suffix(c8* str, const c8* suffix) {
  sz str_len = strlen(str);
  sz suffix_len = strlen(suffix);
  if (suffix_len > str_len || strcmp(str + str_len - suffix_len, suffix) != 0) {
    return 0;
  }
  str[str_len - suffix_len] = '\0';
  return 1;
}

func sz cstr8_replace(c8* str, sz str_cap, const c8* from, const c8* rep) {
  sz from_len = strlen(from);
  sz rep_len = strlen(rep);
  if (from_len == 0) {
    return 0;
  }
  sz count = 0;
  sz pos = 0;
  sz str_len = strlen(str);
  while (pos + from_len <= str_len) {
    if (memcmp(str + pos, from, from_len) == 0) {
      sz new_len = str_len - from_len + rep_len;
      if (new_len >= str_cap) {
        break;
      }
      memmove(str + pos + rep_len, str + pos + from_len, str_len - pos - from_len + 1);
      memcpy(str + pos, rep, rep_len);
      str_len = new_len;
      pos += rep_len;
      count++;
    } else {
      pos++;
    }
  }
  return count;
}

func sz cstr8_common_prefix(const c8* lhs, const c8* rhs, c8* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz idx = 0;
  while (lhs[idx] != '\0' && rhs[idx] != '\0' && lhs[idx] == rhs[idx] && idx < buf_cap - 1) {
    buf[idx] = lhs[idx];
    idx++;
  }
  buf[idx] = '\0';
  return idx;
}

func void cstr8_beautify(c8* str) {
  cstr8_to_lower(str);
  str[0] = c8_to_upper(str[0]);
}

// =========================================================================
// cstr8 — Parsing
// =========================================================================

func b32 cstr8_to_i64(const c8* str, i64* out) {
  char* end;
  errno = 0;
  long long val = strtoll(str, &end, 10);
  if (errno != 0 || end == str || *end != '\0') {
    return 0;
  }
  *out = (i64)val;
  return 1;
}

func b32 cstr8_to_f64(const c8* str, f64* out) {
  char* end;
  errno = 0;
  double val = strtod(str, &end);
  if (errno != 0 || end == str || *end != '\0') {
    return 0;
  }
  *out = val;
  return 1;
}

// =========================================================================
// cstr16 — Helpers for ASCII narrowing (for parsing)
// =========================================================================

func b32 cstr16_to_ascii(const c16* str, c8* buf, sz buf_size) {
  sz idx = 0;
  while (str[idx] != (c16)'\0') {
    if (str[idx] > 0x7FU || idx >= buf_size - 1) {
      return 0;
    }
    buf[idx] = (c8)(u8)str[idx];
    idx++;
  }
  buf[idx] = '\0';
  return 1;
}

func b32 cstr32_to_ascii(const c32* str, c8* buf, sz buf_size) {
  sz idx = 0;
  while (str[idx] != (c32)'\0') {
    if (str[idx] > 0x7FU || idx >= buf_size - 1) {
      return 0;
    }
    buf[idx] = (c8)(u8)str[idx];
    idx++;
  }
  buf[idx] = '\0';
  return 1;
}

// =========================================================================
// cstr16 — Basic
// =========================================================================

func sz cstr16_len(const c16* str) {
  return cstr16_len_impl(str);
}

func b32 cstr16_is_empty(const c16* str) {
  return str[0] == (c16)'\0' ? 1 : 0;
}

func void cstr16_clear(c16* str) {
  str[0] = (c16)'\0';
}

func i32 cstr16_cmp(const c16* lhs, const c16* rhs) {
  while (*lhs != 0 && *lhs == *rhs) {
    lhs++;
    rhs++;
  }
  if (*lhs < *rhs) {
    return -1;
  }
  if (*lhs > *rhs) {
    return 1;
  }
  return 0;
}

func i32 cstr16_cmp_n(const c16* lhs, const c16* rhs, sz cnt) {
  for (sz idx = 0; idx < cnt; idx++) {
    if (lhs[idx] != rhs[idx]) {
      return lhs[idx] < rhs[idx] ? -1 : 1;
    }
    if (lhs[idx] == (c16)'\0') {
      return 0;
    }
  }
  return 0;
}

func i32 cstr16_cmp_nocase(const c16* lhs, const c16* rhs) {
  while (*lhs != (c16)'\0' && *rhs != (c16)'\0') {
    c16 lchr = c16_to_lower(*lhs);
    c16 rchr = c16_to_lower(*rhs);
    if (lchr < rchr) {
      return -1;
    }
    if (lchr > rchr) {
      return 1;
    }
    lhs++;
    rhs++;
  }
  c16 lchr = c16_to_lower(*lhs);
  c16 rchr = c16_to_lower(*rhs);
  if (lchr < rchr) {
    return -1;
  }
  if (lchr > rchr) {
    return 1;
  }
  return 0;
}

// =========================================================================
// cstr16 — Copy / Append
// =========================================================================

func sz cstr16_copy(c16* dst, sz dst_size, const c16* src) {
  if (dst_size == 0) {
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
  memcpy(dst, src, copy_len * sizeof(c16));
  dst[copy_len] = (c16)'\0';
  return copy_len;
}

func sz cstr16_copy_n(c16* dst, sz dst_size, const c16* src, sz cnt) {
  if (dst_size == 0) {
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz max_copy = cnt < src_len ? cnt : src_len;
  sz copy_len = max_copy < dst_size - 1 ? max_copy : dst_size - 1;
  memcpy(dst, src, copy_len * sizeof(c16));
  dst[copy_len] = (c16)'\0';
  return copy_len;
}

func sz cstr16_concat(c16* dst, sz dst_cap, const c16* src) {
  if (dst_cap == 0) {
    return 0;
  }
  sz dst_len = cstr16_len_impl(dst);
  if (dst_len >= dst_cap - 1) {
    return dst_len;
  }
  sz remaining = dst_cap - dst_len - 1;
  sz src_len = cstr16_len_impl(src);
  sz copy_len = src_len < remaining ? src_len : remaining;
  memcpy(dst + dst_len, src, copy_len * sizeof(c16));
  dst[dst_len + copy_len] = (c16)'\0';
  return dst_len + copy_len;
}

func sz cstr16_append_char(c16* dst, sz dst_cap, c16 chr) {
  if (dst_cap == 0) {
    return 0;
  }
  sz len = cstr16_len_impl(dst);
  if (len >= dst_cap - 1) {
    return len;
  }
  dst[len] = chr;
  dst[len + 1] = (c16)'\0';
  return len + 1;
}

func void cstr16_truncate(c16* str, sz length) {
  if (cstr16_len_impl(str) > length) {
    str[length] = (c16)'\0';
  }
}

// =========================================================================
// cstr16 — Search
// =========================================================================

func const c16* cstr16_find(const c16* str, const c16* sub) {
  if (sub[0] == (c16)'\0') {
    return str;
  }
  sz sub_len = cstr16_len_impl(sub);
  sz str_len = cstr16_len_impl(str);
  if (sub_len > str_len) {
    return NULL;
  }
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * sizeof(c16)) == 0) {
      return str + idx;
    }
  }
  return NULL;
}

func const c16* cstr16_find_last(const c16* str, const c16* sub) {
  if (sub[0] == (c16)'\0') {
    return str + cstr16_len_impl(str);
  }
  sz sub_len = cstr16_len_impl(sub);
  sz str_len = cstr16_len_impl(str);
  if (sub_len > str_len) {
    return NULL;
  }
  const c16* last = NULL;
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * sizeof(c16)) == 0) {
      last = str + idx;
    }
  }
  return last;
}

func const c16* cstr16_find_char(const c16* str, c16 chr) {
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      return str + idx;
    }
    if (str[idx] == (c16)'\0') {
      break;
    }
  }
  return NULL;
}

func const c16* cstr16_find_last_char(const c16* str, c16 chr) {
  const c16* last = NULL;
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      last = str + idx;
    }
    if (str[idx] == (c16)'\0') {
      break;
    }
  }
  return last;
}

func sz cstr16_count_char(const c16* str, c16 chr) {
  sz count = 0;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    if (str[idx] == chr) {
      count++;
    }
  }
  return count;
}

func b32 cstr16_starts_with(const c16* str, const c16* prefix) {
  sz prefix_len = cstr16_len_impl(prefix);
  return cstr16_cmp_n(str, prefix, prefix_len) == 0 ? 1 : 0;
}

func b32 cstr16_ends_with(const c16* str, const c16* suffix) {
  sz str_len = cstr16_len_impl(str);
  sz suffix_len = cstr16_len_impl(suffix);
  if (suffix_len > str_len) {
    return 0;
  }
  return memcmp(str + str_len - suffix_len, suffix, suffix_len * sizeof(c16)) == 0 ? 1 : 0;
}

// =========================================================================
// cstr16 — In-place mutation
// =========================================================================

func void cstr16_to_upper(c16* str) {
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    str[idx] = c16_to_upper(str[idx]);
  }
}

func void cstr16_to_lower(c16* str) {
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    str[idx] = c16_to_lower(str[idx]);
  }
}

func void cstr16_trim(c16* str) {
  sz start = 0;
  while (c16_is_space(str[start])) {
    start++;
  }
  sz len = cstr16_len_impl(str + start);
  while (len > 0 && c16_is_space(str[start + len - 1])) {
    len--;
  }
  memmove(str, str + start, len * sizeof(c16));
  str[len] = (c16)'\0';
}

func void cstr16_replace_char(c16* str, c16 from_chr, c16 to_chr) {
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    if (str[idx] == from_chr) {
      str[idx] = to_chr;
    }
  }
}

func sz cstr16_remove_char(c16* str, c16 chr) {
  sz write = 0;
  sz removed = 0;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    if (str[idx] != chr) {
      str[write++] = str[idx];
    } else {
      removed++;
    }
  }
  str[write] = (c16)'\0';
  return removed;
}

func sz cstr16_remove_whitespace(c16* str) {
  sz write = 0;
  sz removed = 0;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    if (!c16_is_space(str[idx])) {
      str[write++] = str[idx];
    } else {
      removed++;
    }
  }
  str[write] = (c16)'\0';
  return removed;
}

func b32 cstr16_remove_prefix(c16* str, const c16* prefix) {
  sz prefix_len = cstr16_len_impl(prefix);
  if (cstr16_cmp_n(str, prefix, prefix_len) != 0) {
    return 0;
  }
  sz str_len = cstr16_len_impl(str);
  memmove(str, str + prefix_len, (str_len - prefix_len + 1) * sizeof(c16));
  return 1;
}

func b32 cstr16_remove_suffix(c16* str, const c16* suffix) {
  sz str_len = cstr16_len_impl(str);
  sz suffix_len = cstr16_len_impl(suffix);
  if (suffix_len > str_len) {
    return 0;
  }
  if (memcmp(str + str_len - suffix_len, suffix, suffix_len * sizeof(c16)) != 0) {
    return 0;
  }
  str[str_len - suffix_len] = (c16)'\0';
  return 1;
}

func sz cstr16_replace(c16* str, sz str_cap, const c16* from, const c16* rep) {
  sz from_len = cstr16_len_impl(from);
  sz rep_len = cstr16_len_impl(rep);
  if (from_len == 0) {
    return 0;
  }
  sz count = 0;
  sz pos = 0;
  sz str_len = cstr16_len_impl(str);
  while (pos + from_len <= str_len) {
    if (memcmp(str + pos, from, from_len * sizeof(c16)) == 0) {
      sz new_len = str_len - from_len + rep_len;
      if (new_len >= str_cap) {
        break;
      }
      memmove(str + pos + rep_len, str + pos + from_len, (str_len - pos - from_len + 1) * sizeof(c16));
      memcpy(str + pos, rep, rep_len * sizeof(c16));
      str_len = new_len;
      pos += rep_len;
      count++;
    } else {
      pos++;
    }
  }
  return count;
}

func sz cstr16_common_prefix(const c16* lhs, const c16* rhs, c16* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz idx = 0;
  while (lhs[idx] != (c16)'\0' && rhs[idx] != (c16)'\0' && lhs[idx] == rhs[idx] && idx < buf_cap - 1) {
    buf[idx] = lhs[idx];
    idx++;
  }
  buf[idx] = (c16)'\0';
  return idx;
}

func void cstr16_beautify(c16* str) {
  cstr16_to_lower(str);
  str[0] = c16_to_upper(str[0]);
}

// =========================================================================
// cstr16 — Parsing
// =========================================================================

func b32 cstr16_to_i64(const c16* str, i64* out) {
  c8 buf[64];
  if (!cstr16_to_ascii(str, buf, sizeof(buf))) {
    return 0;
  }
  return cstr8_to_i64(buf, out);
}

func b32 cstr16_to_f64(const c16* str, f64* out) {
  c8 buf[256];
  if (!cstr16_to_ascii(str, buf, sizeof(buf))) {
    return 0;
  }
  return cstr8_to_f64(buf, out);
}

// =========================================================================
// cstr32 — Basic
// =========================================================================

func sz cstr32_len(const c32* str) {
  return cstr32_len_impl(str);
}

func b32 cstr32_is_empty(const c32* str) {
  return str[0] == (c32)'\0' ? 1 : 0;
}

func void cstr32_clear(c32* str) {
  str[0] = (c32)'\0';
}

func i32 cstr32_cmp(const c32* lhs, const c32* rhs) {
  while (*lhs != 0 && *lhs == *rhs) {
    lhs++;
    rhs++;
  }
  if (*lhs < *rhs) {
    return -1;
  }
  if (*lhs > *rhs) {
    return 1;
  }
  return 0;
}

func i32 cstr32_cmp_n(const c32* lhs, const c32* rhs, sz cnt) {
  for (sz idx = 0; idx < cnt; idx++) {
    if (lhs[idx] != rhs[idx]) {
      return lhs[idx] < rhs[idx] ? -1 : 1;
    }
    if (lhs[idx] == (c32)'\0') {
      return 0;
    }
  }
  return 0;
}

func i32 cstr32_cmp_nocase(const c32* lhs, const c32* rhs) {
  while (*lhs != (c32)'\0' && *rhs != (c32)'\0') {
    c32 lchr = c32_to_lower(*lhs);
    c32 rchr = c32_to_lower(*rhs);
    if (lchr < rchr) {
      return -1;
    }
    if (lchr > rchr) {
      return 1;
    }
    lhs++;
    rhs++;
  }
  c32 lchr = c32_to_lower(*lhs);
  c32 rchr = c32_to_lower(*rhs);
  if (lchr < rchr) {
    return -1;
  }
  if (lchr > rchr) {
    return 1;
  }
  return 0;
}

// =========================================================================
// cstr32 — Copy / Append
// =========================================================================

func sz cstr32_copy(c32* dst, sz dst_size, const c32* src) {
  if (dst_size == 0) {
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
  memcpy(dst, src, copy_len * sizeof(c32));
  dst[copy_len] = (c32)'\0';
  return copy_len;
}

func sz cstr32_copy_n(c32* dst, sz dst_size, const c32* src, sz cnt) {
  if (dst_size == 0) {
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz max_copy = cnt < src_len ? cnt : src_len;
  sz copy_len = max_copy < dst_size - 1 ? max_copy : dst_size - 1;
  memcpy(dst, src, copy_len * sizeof(c32));
  dst[copy_len] = (c32)'\0';
  return copy_len;
}

func sz cstr32_concat(c32* dst, sz dst_cap, const c32* src) {
  if (dst_cap == 0) {
    return 0;
  }
  sz dst_len = cstr32_len_impl(dst);
  if (dst_len >= dst_cap - 1) {
    return dst_len;
  }
  sz remaining = dst_cap - dst_len - 1;
  sz src_len = cstr32_len_impl(src);
  sz copy_len = src_len < remaining ? src_len : remaining;
  memcpy(dst + dst_len, src, copy_len * sizeof(c32));
  dst[dst_len + copy_len] = (c32)'\0';
  return dst_len + copy_len;
}

func sz cstr32_append_char(c32* dst, sz dst_cap, c32 chr) {
  if (dst_cap == 0) {
    return 0;
  }
  sz len = cstr32_len_impl(dst);
  if (len >= dst_cap - 1) {
    return len;
  }
  dst[len] = chr;
  dst[len + 1] = (c32)'\0';
  return len + 1;
}

func void cstr32_truncate(c32* str, sz length) {
  if (cstr32_len_impl(str) > length) {
    str[length] = (c32)'\0';
  }
}

// =========================================================================
// cstr32 — Search
// =========================================================================

func const c32* cstr32_find(const c32* str, const c32* sub) {
  if (sub[0] == (c32)'\0') {
    return str;
  }
  sz sub_len = cstr32_len_impl(sub);
  sz str_len = cstr32_len_impl(str);
  if (sub_len > str_len) {
    return NULL;
  }
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * sizeof(c32)) == 0) {
      return str + idx;
    }
  }
  return NULL;
}

func const c32* cstr32_find_last(const c32* str, const c32* sub) {
  if (sub[0] == (c32)'\0') {
    return str + cstr32_len_impl(str);
  }
  sz sub_len = cstr32_len_impl(sub);
  sz str_len = cstr32_len_impl(str);
  if (sub_len > str_len) {
    return NULL;
  }
  const c32* last = NULL;
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * sizeof(c32)) == 0) {
      last = str + idx;
    }
  }
  return last;
}

func const c32* cstr32_find_char(const c32* str, c32 chr) {
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      return str + idx;
    }
    if (str[idx] == (c32)'\0') {
      break;
    }
  }
  return NULL;
}

func const c32* cstr32_find_last_char(const c32* str, c32 chr) {
  const c32* last = NULL;
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      last = str + idx;
    }
    if (str[idx] == (c32)'\0') {
      break;
    }
  }
  return last;
}

func sz cstr32_count_char(const c32* str, c32 chr) {
  sz count = 0;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    if (str[idx] == chr) {
      count++;
    }
  }
  return count;
}

func b32 cstr32_starts_with(const c32* str, const c32* prefix) {
  sz prefix_len = cstr32_len_impl(prefix);
  return cstr32_cmp_n(str, prefix, prefix_len) == 0 ? 1 : 0;
}

func b32 cstr32_ends_with(const c32* str, const c32* suffix) {
  sz str_len = cstr32_len_impl(str);
  sz suffix_len = cstr32_len_impl(suffix);
  if (suffix_len > str_len) {
    return 0;
  }
  return memcmp(str + str_len - suffix_len, suffix, suffix_len * sizeof(c32)) == 0 ? 1 : 0;
}

// =========================================================================
// cstr32 — In-place mutation
// =========================================================================

func void cstr32_to_upper(c32* str) {
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    str[idx] = c32_to_upper(str[idx]);
  }
}

func void cstr32_to_lower(c32* str) {
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    str[idx] = c32_to_lower(str[idx]);
  }
}

func void cstr32_trim(c32* str) {
  sz start = 0;
  while (c32_is_space(str[start])) {
    start++;
  }
  sz len = cstr32_len_impl(str + start);
  while (len > 0 && c32_is_space(str[start + len - 1])) {
    len--;
  }
  memmove(str, str + start, len * sizeof(c32));
  str[len] = (c32)'\0';
}

func void cstr32_replace_char(c32* str, c32 from_chr, c32 to_chr) {
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    if (str[idx] == from_chr) {
      str[idx] = to_chr;
    }
  }
}

func sz cstr32_remove_char(c32* str, c32 chr) {
  sz write = 0;
  sz removed = 0;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    if (str[idx] != chr) {
      str[write++] = str[idx];
    } else {
      removed++;
    }
  }
  str[write] = (c32)'\0';
  return removed;
}

func sz cstr32_remove_whitespace(c32* str) {
  sz write = 0;
  sz removed = 0;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    if (!c32_is_space(str[idx])) {
      str[write++] = str[idx];
    } else {
      removed++;
    }
  }
  str[write] = (c32)'\0';
  return removed;
}

func b32 cstr32_remove_prefix(c32* str, const c32* prefix) {
  sz prefix_len = cstr32_len_impl(prefix);
  if (cstr32_cmp_n(str, prefix, prefix_len) != 0) {
    return 0;
  }
  sz str_len = cstr32_len_impl(str);
  memmove(str, str + prefix_len, (str_len - prefix_len + 1) * sizeof(c32));
  return 1;
}

func b32 cstr32_remove_suffix(c32* str, const c32* suffix) {
  sz str_len = cstr32_len_impl(str);
  sz suffix_len = cstr32_len_impl(suffix);
  if (suffix_len > str_len) {
    return 0;
  }
  if (memcmp(str + str_len - suffix_len, suffix, suffix_len * sizeof(c32)) != 0) {
    return 0;
  }
  str[str_len - suffix_len] = (c32)'\0';
  return 1;
}

func sz cstr32_replace(c32* str, sz str_cap, const c32* from, const c32* rep) {
  sz from_len = cstr32_len_impl(from);
  sz rep_len = cstr32_len_impl(rep);
  if (from_len == 0) {
    return 0;
  }
  sz count = 0;
  sz pos = 0;
  sz str_len = cstr32_len_impl(str);
  while (pos + from_len <= str_len) {
    if (memcmp(str + pos, from, from_len * sizeof(c32)) == 0) {
      sz new_len = str_len - from_len + rep_len;
      if (new_len >= str_cap) {
        break;
      }
      memmove(str + pos + rep_len, str + pos + from_len, (str_len - pos - from_len + 1) * sizeof(c32));
      memcpy(str + pos, rep, rep_len * sizeof(c32));
      str_len = new_len;
      pos += rep_len;
      count++;
    } else {
      pos++;
    }
  }
  return count;
}

func sz cstr32_common_prefix(const c32* lhs, const c32* rhs, c32* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz idx = 0;
  while (lhs[idx] != (c32)'\0' && rhs[idx] != (c32)'\0' && lhs[idx] == rhs[idx] && idx < buf_cap - 1) {
    buf[idx] = lhs[idx];
    idx++;
  }
  buf[idx] = (c32)'\0';
  return idx;
}

func void cstr32_beautify(c32* str) {
  cstr32_to_lower(str);
  str[0] = c32_to_upper(str[0]);
}

// =========================================================================
// cstr32 — Parsing
// =========================================================================

func b32 cstr32_to_i64(const c32* str, i64* out) {
  c8 buf[64];
  if (!cstr32_to_ascii(str, buf, sizeof(buf))) {
    return 0;
  }
  return cstr8_to_i64(buf, out);
}

func b32 cstr32_to_f64(const c32* str, f64* out) {
  c8 buf[256];
  if (!cstr32_to_ascii(str, buf, sizeof(buf))) {
    return 0;
  }
  return cstr8_to_f64(buf, out);
}

// =========================================================================
// Cross-encoding conversion (null-terminated)
// =========================================================================

func sz cstr8_to_cstr16(const c8* src, c16* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz src_len = strlen(src);
  sz written = utf8_to_utf16(src, src_len, buf, buf_cap - 1);
  buf[written] = (c16)'\0';
  return written;
}

func sz cstr8_to_cstr32(const c8* src, c32* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz src_len = strlen(src);
  sz written = utf8_to_utf32(src, src_len, buf, buf_cap - 1);
  buf[written] = (c32)'\0';
  return written;
}

func sz cstr16_to_cstr8(const c16* src, c8* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz written = utf16_to_utf8(src, src_len, buf, buf_cap - 1);
  buf[written] = '\0';
  return written;
}

func sz cstr16_to_cstr32(const c16* src, c32* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz written = utf16_to_utf32(src, src_len, buf, buf_cap - 1);
  buf[written] = (c32)'\0';
  return written;
}

func sz cstr32_to_cstr8(const c32* src, c8* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz written = utf32_to_utf8(src, src_len, buf, buf_cap - 1);
  buf[written] = '\0';
  return written;
}

func sz cstr32_to_cstr16(const c32* src, c16* buf, sz buf_cap) {
  if (buf_cap == 0) {
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz written = utf32_to_utf16(src, src_len, buf, buf_cap - 1);
  buf[written] = (c16)'\0';
  return written;
}
