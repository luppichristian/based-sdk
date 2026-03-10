// MIT License
// Copyright (c) 2026 Christian Luppi

#include "strings/cstrings.h"
#include "basic/assert.h"
#include "strings/char.h"
#include "strings/unicode.h"
#include "basic/profiler.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================================
// Internal helpers
// =========================================================================

func sz cstr16_len_impl(cstr16 str) {
  profile_func_begin;
  sz len = 0;
  while (str[len] != (c16)'\0') {
    len++;
  }
  profile_func_end;
  return len;
}

func sz cstr32_len_impl(cstr32 str) {
  profile_func_begin;
  sz len = 0;
  while (str[len] != (c32)'\0') {
    len++;
  }
  profile_func_end;
  return len;
}

// =========================================================================
// cstr8 — Basic
// =========================================================================

func sz cstr8_len(cstr8 str) {
  profile_func_begin;
  if (str == NULL) {
    profile_func_end;
    return 0;
  }
  assert(str != NULL);
  profile_func_end;
  return strlen(str);
}

func b32 cstr8_is_empty(cstr8 str) {
  profile_func_begin;
  if (str == NULL) {
    profile_func_end;
    return true;
  }
  assert(str != NULL);
  profile_func_end;
  return str[0] == '\0' ? true : false;
}

func void cstr8_clear(c8* str) {
  profile_func_begin;
  if (str == NULL) {
    profile_func_end;
    return;
  }
  assert(str != NULL);
  str[0] = '\0';
  profile_func_end;
}

func b32 cstr8_cmp(cstr8 lhs, cstr8 rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  assert(lhs != NULL);
  assert(rhs != NULL);
  while (*lhs != '\0' && *rhs != '\0') {
    if (*lhs != *rhs) {
      profile_func_end;
      return false;
    }
    lhs++;
    rhs++;
  }
  profile_func_end;
  return *lhs == *rhs ? true : false;
}

func b32 cstr8_cmp_n(cstr8 lhs, cstr8 rhs, sz cnt) {
  profile_func_begin;
  if (cnt == 0) {
    profile_func_end;
    return true;
  }
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  for (sz idx = 0; idx < cnt; idx++) {
    if (lhs[idx] != rhs[idx]) {
      profile_func_end;
      return false;
    }
    if (lhs[idx] == '\0') {
      profile_func_end;
      return true;
    }
  }
  profile_func_end;
  return true;
}

func b32 cstr8_cmp_nocase(cstr8 lhs, cstr8 rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  while (*lhs != '\0' && *rhs != '\0') {
    u8 lchr = (u8)c8_to_lower(*lhs);
    u8 rchr = (u8)c8_to_lower(*rhs);
    if (lchr != rchr) {
      profile_func_end;
      return false;
    }
    lhs++;
    rhs++;
  }
  u8 lchr = (u8)c8_to_lower(*lhs);
  u8 rchr = (u8)c8_to_lower(*rhs);
  profile_func_end;
  return lchr == rchr ? true : false;
}

// =========================================================================
// cstr8 — Copy / Append
// =========================================================================

func sz cstr8_copy(c8* dst, sz dst_size, cstr8 src) {
  profile_func_begin;
  if (dst == NULL || src == NULL || dst_size == 0) {
    profile_func_end;
    return 0;
  }
  assert(dst != NULL);
  assert(src != NULL);
  sz src_len = cstr8_len(src);
  sz copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
  memcpy(dst, src, copy_len);
  dst[copy_len] = '\0';
  profile_func_end;
  return copy_len;
}

func sz cstr8_copy_n(c8* dst, sz dst_size, cstr8 src, sz cnt) {
  profile_func_begin;
  if (dst_size == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr8_len(src);
  sz max_copy = cnt < src_len ? cnt : src_len;
  sz copy_len = max_copy < dst_size - 1 ? max_copy : dst_size - 1;
  memcpy(dst, src, copy_len);
  dst[copy_len] = '\0';
  profile_func_end;
  return copy_len;
}

func sz cstr8_cat(c8* dst, sz dst_cap, cstr8 src) {
  profile_func_begin;
  if (dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz dst_len = cstr8_len(dst);
  if (dst_len >= dst_cap - 1) {
    profile_func_end;
    return dst_len;
  }
  sz remaining = dst_cap - dst_len - 1;
  sz src_len = cstr8_len(src);
  sz copy_len = src_len < remaining ? src_len : remaining;
  memcpy(dst + dst_len, src, copy_len);
  dst[dst_len + copy_len] = '\0';
  profile_func_end;
  return dst_len + copy_len;
}

func sz cstr8_append_char(c8* dst, sz dst_cap, c8 chr) {
  profile_func_begin;
  if (dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz len = cstr8_len(dst);
  if (len >= dst_cap - 1) {
    profile_func_end;
    return len;
  }
  dst[len] = chr;
  dst[len + 1] = '\0';
  profile_func_end;
  return len + 1;
}

func void cstr8_truncate(c8* str, sz length) {
  profile_func_begin;
  if (cstr8_len(str) > length) {
    str[length] = '\0';
  }
  profile_func_end;
}

func b32 cstr8_format(c8* dst, sz dst_cap, cstr8 fmt, ...) {
  profile_func_begin;
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(dst, dst_cap, fmt, args);
  va_end(args);
  profile_func_end;
  return (result >= 0 && (sz)result < dst_cap) ? true : false;
}

func b32 cstr8_vformat(c8* dst, sz dst_cap, cstr8 fmt, va_list args) {
  profile_func_begin;
  int result = vsnprintf(dst, dst_cap, fmt, args);
  profile_func_end;
  return (result >= 0 && (sz)result < dst_cap) ? true : false;
}

func b32 cstr8_append_format(c8* dst, sz dst_cap, cstr8 fmt, ...) {
  profile_func_begin;
  sz len = cstr8_len(dst);
  if (len >= dst_cap) {
    profile_func_end;
    return false;
  }
  va_list args;
  va_start(args, fmt);
  b32 result = cstr8_vformat(dst + len, dst_cap - len, fmt, args);
  va_end(args);
  profile_func_end;
  return result;
}

func b32 cstr8_append_vformat(c8* dst, sz dst_cap, cstr8 fmt, va_list args) {
  profile_func_begin;
  sz len = cstr8_len(dst);
  if (len >= dst_cap) {
    profile_func_end;
    return false;
  }
  b32 result = cstr8_vformat(dst + len, dst_cap - len, fmt, args);
  profile_func_end;
  return result;
}

func b32 cstr8_scan(cstr8 str, cstr8 fmt, ...) {
  profile_func_begin;
  va_list args;
  va_start(args, fmt);
  int result = vsscanf(str, fmt, args);
  va_end(args);
  profile_func_end;
  return result > 0 ? true : false;
}

// =========================================================================
// cstr8 — Search
// =========================================================================

func cstr8 cstr8_find(cstr8 str, cstr8 sub) {
  profile_func_begin;
  profile_func_end;
  return strstr(str, sub);
}

func cstr8 cstr8_find_last(cstr8 str, cstr8 sub) {
  profile_func_begin;
  if (sub[0] == '\0') {
    profile_func_end;
    return str + cstr8_len(str);
  }
  sz sub_len = cstr8_len(sub);
  sz str_len = cstr8_len(str);
  if (sub_len > str_len) {
    profile_func_end;
    return NULL;
  }
  cstr8 last = NULL;
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len) == 0) {
      last = str + idx;
    }
  }
  profile_func_end;
  return last;
}

func cstr8 cstr8_find_char(cstr8 str, c8 chr) {
  profile_func_begin;
  profile_func_end;
  return strchr(str, (int)(unsigned char)chr);
}

func cstr8 cstr8_find_last_char(cstr8 str, c8 chr) {
  profile_func_begin;
  profile_func_end;
  return strrchr(str, (int)(unsigned char)chr);
}

func sz cstr8_count_char(cstr8 str, c8 chr) {
  profile_func_begin;
  sz count = 0;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    if (str[idx] == chr) {
      count++;
    }
  }
  profile_func_end;
  return count;
}

func b32 cstr8_starts_with(cstr8 str, cstr8 prefix) {
  profile_func_begin;
  sz prefix_len = cstr8_len(prefix);
  profile_func_end;
  return cstr8_cmp_n(str, prefix, prefix_len);
}

func b32 cstr8_ends_with(cstr8 str, cstr8 suffix) {
  profile_func_begin;
  sz str_len = cstr8_len(str);
  sz suffix_len = cstr8_len(suffix);
  if (suffix_len > str_len) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return cstr8_cmp(str + str_len - suffix_len, suffix);
}

// =========================================================================
// cstr8 — In-place mutation
// =========================================================================

func void cstr8_to_upper(c8* str) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    str[idx] = c8_to_upper(str[idx]);
  }
  profile_func_end;
}

func void cstr8_to_lower(c8* str) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    str[idx] = c8_to_lower(str[idx]);
  }
  profile_func_end;
}

func void cstr8_trim(c8* str) {
  profile_func_begin;
  sz start = 0;
  while (c8_is_space(str[start])) {
    start++;
  }
  sz len = cstr8_len(str + start);
  while (len > 0 && c8_is_space(str[start + len - 1])) {
    len--;
  }
  memmove(str, str + start, len);
  str[len] = '\0';
  profile_func_end;
}

func void cstr8_replace_char(c8* str, c8 from_chr, c8 to_chr) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != '\0'; idx++) {
    if (str[idx] == from_chr) {
      str[idx] = to_chr;
    }
  }
  profile_func_end;
}

func sz cstr8_remove_char(c8* str, c8 chr) {
  profile_func_begin;
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
  profile_func_end;
  return removed;
}

func sz cstr8_remove_whitespace(c8* str) {
  profile_func_begin;
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
  profile_func_end;
  return removed;
}

func b32 cstr8_remove_prefix(c8* str, cstr8 prefix) {
  profile_func_begin;
  sz prefix_len = cstr8_len(prefix);
  if (!cstr8_cmp_n(str, prefix, prefix_len)) {
    profile_func_end;
    return false;
  }
  sz str_len = cstr8_len(str);
  memmove(str, str + prefix_len, str_len - prefix_len + 1);
  profile_func_end;
  return true;
}

func b32 cstr8_remove_suffix(c8* str, cstr8 suffix) {
  profile_func_begin;
  sz str_len = cstr8_len(str);
  sz suffix_len = cstr8_len(suffix);
  if (suffix_len > str_len || !cstr8_cmp(str + str_len - suffix_len, suffix)) {
    profile_func_end;
    return false;
  }
  str[str_len - suffix_len] = '\0';
  profile_func_end;
  return true;
}

func sz cstr8_replace(c8* str, sz str_cap, cstr8 from, cstr8 rep) {
  profile_func_begin;
  sz from_len = cstr8_len(from);
  sz rep_len = cstr8_len(rep);
  if (from_len == 0) {
    profile_func_end;
    return 0;
  }
  sz count = 0;
  sz pos = 0;
  sz str_len = cstr8_len(str);
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
  profile_func_end;
  return count;
}

func sz cstr8_common_prefix(cstr8 lhs, cstr8 rhs, c8* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz idx = 0;
  while (lhs[idx] != '\0' && rhs[idx] != '\0' && lhs[idx] == rhs[idx] && idx < buf_cap - 1) {
    buf[idx] = lhs[idx];
    idx++;
  }
  buf[idx] = '\0';
  profile_func_end;
  return idx;
}

func void cstr8_beautify(c8* str) {
  profile_func_begin;
  cstr8_to_lower(str);
  str[0] = c8_to_upper(str[0]);
  profile_func_end;
}

// =========================================================================
// cstr8 — Parsing
// =========================================================================

func b32 cstr8_to_i64(cstr8 str, i64* out) {
  profile_func_begin;
  if (str == NULL || out == NULL) {
    profile_func_end;
    return false;
  }
  assert(str != NULL);
  assert(out != NULL);
  char* end;
  errno = 0;
  long long val = strtoll(str, &end, 10);
  if (errno != 0 || end == str || *end != '\0') {
    profile_func_end;
    return false;
  }
  *out = (i64)val;
  profile_func_end;
  return true;
}

func b32 cstr8_to_u64(cstr8 str, u64 max_value, u64* out) {
  profile_func_begin;
  if (str == NULL || out == NULL) {
    profile_func_end;
    return false;
  }
  assert(str != NULL);
  assert(out != NULL);
  if (str[0] == '\0') {
    profile_func_end;
    return false;
  }

  char* end_ptr;
  errno = 0;
  unsigned long long parsed = strtoull(str, &end_ptr, 10);
  if (errno != 0 || end_ptr == str || *end_ptr != '\0') {
    profile_func_end;
    return false;
  }
  if ((u64)parsed > max_value) {
    profile_func_end;
    return false;
  }

  *out = (u64)parsed;
  profile_func_end;
  return true;
}

func b32 cstr8_to_f64(cstr8 str, f64* out) {
  profile_func_begin;
  if (str == NULL || out == NULL) {
    profile_func_end;
    return false;
  }
  assert(str != NULL);
  assert(out != NULL);
  char* end;
  errno = 0;
  double val = strtod(str, &end);
  if (errno != 0 || end == str || *end != '\0') {
    profile_func_end;
    return false;
  }
  *out = val;
  profile_func_end;
  return true;
}

func cstr8_tokenizer cstr8_tokenizer_make(cstr8 src, cstr8 delim) {
  profile_func_begin;
  cstr8_tokenizer tok = {0};
  tok.src = src != NULL ? src : "";
  tok.delim = delim != NULL ? delim : "";
  tok.cursor = 0;
  profile_func_end;
  return tok;
}

func b32 cstr8_tokenizer_next(cstr8_tokenizer* tok, c8* out_buf, sz out_cap) {
  profile_func_begin;
  if (tok == NULL || out_buf == NULL || out_cap == 0) {
    profile_func_end;
    return false;
  }

  sz src_len = cstr8_len(tok->src);
  sz delim_len = cstr8_len(tok->delim);
  if (tok->cursor > src_len) {
    profile_func_end;
    return false;
  }

  if (tok->cursor == src_len) {
    out_buf[0] = '\0';
    tok->cursor = src_len + 1;
    profile_func_end;
    return true;
  }

  sz start = tok->cursor;
  sz end = src_len;
  if (delim_len > 0) {
    cstr8 found = cstr8_find(tok->src + start, tok->delim);
    if (found != NULL) {
      end = start + (sz)(found - (tok->src + start));
      tok->cursor = end + delim_len;
    } else {
      tok->cursor = src_len;
    }
  } else {
    tok->cursor = src_len;
  }

  sz token_len = end - start;
  if (token_len >= out_cap) {
    token_len = out_cap - 1;
  }
  memcpy(out_buf, tok->src + start, token_len);
  out_buf[token_len] = '\0';
  profile_func_end;
  return true;
}

func sz cstr8_join(c8* dst, sz dst_cap, cstr8 const* parts, sz part_count, cstr8 delim) {
  profile_func_begin;
  if (dst == NULL || dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  dst[0] = '\0';
  if (parts == NULL || part_count == 0) {
    profile_func_end;
    return 0;
  }

  sz delim_len = cstr8_len(delim != NULL ? delim : "");
  sz total_len = 0;
  for (sz part_idx = 0; part_idx < part_count; part_idx++) {
    if (part_idx > 0 && delim_len > 0) {
      total_len = cstr8_cat(dst, dst_cap, delim);
    }
    total_len = cstr8_cat(dst, dst_cap, parts[part_idx] != NULL ? parts[part_idx] : "");
  }

  profile_func_end;
  return total_len;
}

// =========================================================================
// cstr16 — Helpers for ASCII narrowing (for parsing)
// =========================================================================

func b32 cstr16_to_ascii(cstr16 str, c8* buf, sz buf_size) {
  profile_func_begin;
  sz idx = 0;
  while (str[idx] != (c16)'\0') {
    if (str[idx] > 0x7FU || idx >= buf_size - 1) {
      profile_func_end;
      return false;
    }
    buf[idx] = (c8)(u8)str[idx];
    idx++;
  }
  buf[idx] = '\0';
  profile_func_end;
  return true;
}

func b32 cstr32_to_ascii(cstr32 str, c8* buf, sz buf_size) {
  profile_func_begin;
  sz idx = 0;
  while (str[idx] != (c32)'\0') {
    if (str[idx] > 0x7FU || idx >= buf_size - 1) {
      profile_func_end;
      return false;
    }
    buf[idx] = (c8)(u8)str[idx];
    idx++;
  }
  buf[idx] = '\0';
  profile_func_end;
  return true;
}

// =========================================================================
// cstr16 — Basic
// =========================================================================

func sz cstr16_len(cstr16 str) {
  profile_func_begin;
  profile_func_end;
  return cstr16_len_impl(str);
}

func b32 cstr16_is_empty(cstr16 str) {
  profile_func_begin;
  profile_func_end;
  return str[0] == (c16)'\0' ? true : false;
}

func void cstr16_clear(c16* str) {
  profile_func_begin;
  str[0] = (c16)'\0';
  profile_func_end;
}

func b32 cstr16_cmp(cstr16 lhs, cstr16 rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  while (*lhs != 0 && *lhs == *rhs) {
    lhs++;
    rhs++;
  }
  profile_func_end;
  return *lhs == *rhs ? true : false;
}

func b32 cstr16_cmp_n(cstr16 lhs, cstr16 rhs, sz cnt) {
  profile_func_begin;
  if (cnt == 0) {
    profile_func_end;
    return true;
  }
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  for (sz idx = 0; idx < cnt; idx++) {
    if (lhs[idx] != rhs[idx]) {
      profile_func_end;
      return false;
    }
    if (lhs[idx] == (c16)'\0') {
      profile_func_end;
      return true;
    }
  }
  profile_func_end;
  return true;
}

func b32 cstr16_cmp_nocase(cstr16 lhs, cstr16 rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  while (*lhs != (c16)'\0' && *rhs != (c16)'\0') {
    c16 lchr = c16_to_lower(*lhs);
    c16 rchr = c16_to_lower(*rhs);
    if (lchr != rchr) {
      profile_func_end;
      return false;
    }
    lhs++;
    rhs++;
  }
  c16 lchr = c16_to_lower(*lhs);
  c16 rchr = c16_to_lower(*rhs);
  profile_func_end;
  return lchr == rchr ? true : false;
}

// =========================================================================
// cstr16 — Copy / Append
// =========================================================================

func sz cstr16_copy(c16* dst, sz dst_size, cstr16 src) {
  profile_func_begin;
  if (dst_size == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
  memcpy(dst, src, copy_len * size_of(c16));
  dst[copy_len] = (c16)'\0';
  profile_func_end;
  return copy_len;
}

func sz cstr16_copy_n(c16* dst, sz dst_size, cstr16 src, sz cnt) {
  profile_func_begin;
  if (dst_size == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz max_copy = cnt < src_len ? cnt : src_len;
  sz copy_len = max_copy < dst_size - 1 ? max_copy : dst_size - 1;
  memcpy(dst, src, copy_len * size_of(c16));
  dst[copy_len] = (c16)'\0';
  profile_func_end;
  return copy_len;
}

func sz cstr16_cat(c16* dst, sz dst_cap, cstr16 src) {
  profile_func_begin;
  if (dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz dst_len = cstr16_len_impl(dst);
  if (dst_len >= dst_cap - 1) {
    profile_func_end;
    return dst_len;
  }
  sz remaining = dst_cap - dst_len - 1;
  sz src_len = cstr16_len_impl(src);
  sz copy_len = src_len < remaining ? src_len : remaining;
  memcpy(dst + dst_len, src, copy_len * size_of(c16));
  dst[dst_len + copy_len] = (c16)'\0';
  profile_func_end;
  return dst_len + copy_len;
}

func sz cstr16_append_char(c16* dst, sz dst_cap, c16 chr) {
  profile_func_begin;
  if (dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz len = cstr16_len_impl(dst);
  if (len >= dst_cap - 1) {
    profile_func_end;
    return len;
  }
  dst[len] = chr;
  dst[len + 1] = (c16)'\0';
  profile_func_end;
  return len + 1;
}

func void cstr16_truncate(c16* str, sz length) {
  profile_func_begin;
  if (cstr16_len_impl(str) > length) {
    str[length] = (c16)'\0';
  }
  profile_func_end;
}

// =========================================================================
// cstr16 — Search
// =========================================================================

func cstr16 cstr16_find(cstr16 str, cstr16 sub) {
  profile_func_begin;
  if (sub[0] == (c16)'\0') {
    profile_func_end;
    return str;
  }
  sz sub_len = cstr16_len_impl(sub);
  sz str_len = cstr16_len_impl(str);
  if (sub_len > str_len) {
    profile_func_end;
    return NULL;
  }
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * size_of(c16)) == 0) {
      profile_func_end;
      return str + idx;
    }
  }
  profile_func_end;
  return NULL;
}

func cstr16 cstr16_find_last(cstr16 str, cstr16 sub) {
  profile_func_begin;
  if (sub[0] == (c16)'\0') {
    profile_func_end;
    return str + cstr16_len_impl(str);
  }
  sz sub_len = cstr16_len_impl(sub);
  sz str_len = cstr16_len_impl(str);
  if (sub_len > str_len) {
    profile_func_end;
    return NULL;
  }
  cstr16 last = NULL;
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * size_of(c16)) == 0) {
      last = str + idx;
    }
  }
  profile_func_end;
  return last;
}

func cstr16 cstr16_find_char(cstr16 str, c16 chr) {
  profile_func_begin;
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      profile_func_end;
      return str + idx;
    }
    if (str[idx] == (c16)'\0') {
      break;
    }
  }
  profile_func_end;
  return NULL;
}

func cstr16 cstr16_find_last_char(cstr16 str, c16 chr) {
  profile_func_begin;
  cstr16 last = NULL;
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      last = str + idx;
    }
    if (str[idx] == (c16)'\0') {
      break;
    }
  }
  profile_func_end;
  return last;
}

func sz cstr16_count_char(cstr16 str, c16 chr) {
  profile_func_begin;
  sz count = 0;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    if (str[idx] == chr) {
      count++;
    }
  }
  profile_func_end;
  return count;
}

func b32 cstr16_starts_with(cstr16 str, cstr16 prefix) {
  profile_func_begin;
  sz prefix_len = cstr16_len_impl(prefix);
  profile_func_end;
  return cstr16_cmp_n(str, prefix, prefix_len);
}

func b32 cstr16_ends_with(cstr16 str, cstr16 suffix) {
  profile_func_begin;
  sz str_len = cstr16_len_impl(str);
  sz suffix_len = cstr16_len_impl(suffix);
  if (suffix_len > str_len) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return memcmp(str + str_len - suffix_len, suffix, suffix_len * size_of(c16)) == 0 ? true : false;
}

// =========================================================================
// cstr16 — In-place mutation
// =========================================================================

func void cstr16_to_upper(c16* str) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    str[idx] = c16_to_upper(str[idx]);
  }
  profile_func_end;
}

func void cstr16_to_lower(c16* str) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    str[idx] = c16_to_lower(str[idx]);
  }
  profile_func_end;
}

func void cstr16_trim(c16* str) {
  profile_func_begin;
  sz start = 0;
  while (c16_is_space(str[start])) {
    start++;
  }
  sz len = cstr16_len_impl(str + start);
  while (len > 0 && c16_is_space(str[start + len - 1])) {
    len--;
  }
  memmove(str, str + start, len * size_of(c16));
  str[len] = (c16)'\0';
  profile_func_end;
}

func void cstr16_replace_char(c16* str, c16 from_chr, c16 to_chr) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != (c16)'\0'; idx++) {
    if (str[idx] == from_chr) {
      str[idx] = to_chr;
    }
  }
  profile_func_end;
}

func sz cstr16_remove_char(c16* str, c16 chr) {
  profile_func_begin;
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
  profile_func_end;
  return removed;
}

func sz cstr16_remove_whitespace(c16* str) {
  profile_func_begin;
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
  profile_func_end;
  return removed;
}

func b32 cstr16_remove_prefix(c16* str, cstr16 prefix) {
  profile_func_begin;
  sz prefix_len = cstr16_len_impl(prefix);
  if (!cstr16_cmp_n(str, prefix, prefix_len)) {
    profile_func_end;
    return false;
  }
  sz str_len = cstr16_len_impl(str);
  memmove(str, str + prefix_len, (str_len - prefix_len + 1) * size_of(c16));
  profile_func_end;
  return true;
}

func b32 cstr16_remove_suffix(c16* str, cstr16 suffix) {
  profile_func_begin;
  sz str_len = cstr16_len_impl(str);
  sz suffix_len = cstr16_len_impl(suffix);
  if (suffix_len > str_len) {
    profile_func_end;
    return false;
  }
  if (memcmp(str + str_len - suffix_len, suffix, suffix_len * size_of(c16)) != 0) {
    profile_func_end;
    return false;
  }
  str[str_len - suffix_len] = (c16)'\0';
  profile_func_end;
  return true;
}

func sz cstr16_replace(c16* str, sz str_cap, cstr16 from, cstr16 rep) {
  profile_func_begin;
  sz from_len = cstr16_len_impl(from);
  sz rep_len = cstr16_len_impl(rep);
  if (from_len == 0) {
    profile_func_end;
    return 0;
  }
  sz count = 0;
  sz pos = 0;
  sz str_len = cstr16_len_impl(str);
  while (pos + from_len <= str_len) {
    if (memcmp(str + pos, from, from_len * size_of(c16)) == 0) {
      sz new_len = str_len - from_len + rep_len;
      if (new_len >= str_cap) {
        break;
      }
      memmove(str + pos + rep_len, str + pos + from_len, (str_len - pos - from_len + 1) * size_of(c16));
      memcpy(str + pos, rep, rep_len * size_of(c16));
      str_len = new_len;
      pos += rep_len;
      count++;
    } else {
      pos++;
    }
  }
  profile_func_end;
  return count;
}

func sz cstr16_common_prefix(cstr16 lhs, cstr16 rhs, c16* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz idx = 0;
  while (lhs[idx] != (c16)'\0' && rhs[idx] != (c16)'\0' && lhs[idx] == rhs[idx] && idx < buf_cap - 1) {
    buf[idx] = lhs[idx];
    idx++;
  }
  buf[idx] = (c16)'\0';
  profile_func_end;
  return idx;
}

func void cstr16_beautify(c16* str) {
  profile_func_begin;
  cstr16_to_lower(str);
  str[0] = c16_to_upper(str[0]);
  profile_func_end;
}

// =========================================================================
// cstr16 — Parsing
// =========================================================================

func b32 cstr16_to_i64(cstr16 str, i64* out) {
  profile_func_begin;
  c8 buf[64];
  if (!cstr16_to_ascii(str, buf, size_of(buf))) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return cstr8_to_i64(buf, out);
}

func b32 cstr16_to_f64(cstr16 str, f64* out) {
  profile_func_begin;
  c8 buf[256];
  if (!cstr16_to_ascii(str, buf, size_of(buf))) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return cstr8_to_f64(buf, out);
}

// =========================================================================
// cstr32 — Basic
// =========================================================================

func sz cstr32_len(cstr32 str) {
  profile_func_begin;
  profile_func_end;
  return cstr32_len_impl(str);
}

func b32 cstr32_is_empty(cstr32 str) {
  profile_func_begin;
  profile_func_end;
  return str[0] == (c32)'\0' ? true : false;
}

func void cstr32_clear(c32* str) {
  profile_func_begin;
  str[0] = (c32)'\0';
  profile_func_end;
}

func b32 cstr32_cmp(cstr32 lhs, cstr32 rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  while (*lhs != 0 && *lhs == *rhs) {
    lhs++;
    rhs++;
  }
  profile_func_end;
  return *lhs == *rhs ? true : false;
}

func b32 cstr32_cmp_n(cstr32 lhs, cstr32 rhs, sz cnt) {
  profile_func_begin;
  if (cnt == 0) {
    profile_func_end;
    return true;
  }
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  for (sz idx = 0; idx < cnt; idx++) {
    if (lhs[idx] != rhs[idx]) {
      profile_func_end;
      return false;
    }
    if (lhs[idx] == (c32)'\0') {
      profile_func_end;
      return true;
    }
  }
  profile_func_end;
  return true;
}

func b32 cstr32_cmp_nocase(cstr32 lhs, cstr32 rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return lhs == rhs ? true : false;
  }
  while (*lhs != (c32)'\0' && *rhs != (c32)'\0') {
    c32 lchr = c32_to_lower(*lhs);
    c32 rchr = c32_to_lower(*rhs);
    if (lchr != rchr) {
      profile_func_end;
      return false;
    }
    lhs++;
    rhs++;
  }
  c32 lchr = c32_to_lower(*lhs);
  c32 rchr = c32_to_lower(*rhs);
  profile_func_end;
  return lchr == rchr ? true : false;
}

// =========================================================================
// cstr32 — Copy / Append
// =========================================================================

func sz cstr32_copy(c32* dst, sz dst_size, cstr32 src) {
  profile_func_begin;
  if (dst_size == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz copy_len = src_len < dst_size - 1 ? src_len : dst_size - 1;
  memcpy(dst, src, copy_len * size_of(c32));
  dst[copy_len] = (c32)'\0';
  profile_func_end;
  return copy_len;
}

func sz cstr32_copy_n(c32* dst, sz dst_size, cstr32 src, sz cnt) {
  profile_func_begin;
  if (dst_size == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz max_copy = cnt < src_len ? cnt : src_len;
  sz copy_len = max_copy < dst_size - 1 ? max_copy : dst_size - 1;
  memcpy(dst, src, copy_len * size_of(c32));
  dst[copy_len] = (c32)'\0';
  profile_func_end;
  return copy_len;
}

func sz cstr32_cat(c32* dst, sz dst_cap, cstr32 src) {
  profile_func_begin;
  if (dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz dst_len = cstr32_len_impl(dst);
  if (dst_len >= dst_cap - 1) {
    profile_func_end;
    return dst_len;
  }
  sz remaining = dst_cap - dst_len - 1;
  sz src_len = cstr32_len_impl(src);
  sz copy_len = src_len < remaining ? src_len : remaining;
  memcpy(dst + dst_len, src, copy_len * size_of(c32));
  dst[dst_len + copy_len] = (c32)'\0';
  profile_func_end;
  return dst_len + copy_len;
}

func sz cstr32_append_char(c32* dst, sz dst_cap, c32 chr) {
  profile_func_begin;
  if (dst_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz len = cstr32_len_impl(dst);
  if (len >= dst_cap - 1) {
    profile_func_end;
    return len;
  }
  dst[len] = chr;
  dst[len + 1] = (c32)'\0';
  profile_func_end;
  return len + 1;
}

func void cstr32_truncate(c32* str, sz length) {
  profile_func_begin;
  if (cstr32_len_impl(str) > length) {
    str[length] = (c32)'\0';
  }
  profile_func_end;
}

// =========================================================================
// cstr32 — Search
// =========================================================================

func cstr32 cstr32_find(cstr32 str, cstr32 sub) {
  profile_func_begin;
  if (sub[0] == (c32)'\0') {
    profile_func_end;
    return str;
  }
  sz sub_len = cstr32_len_impl(sub);
  sz str_len = cstr32_len_impl(str);
  if (sub_len > str_len) {
    profile_func_end;
    return NULL;
  }
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * size_of(c32)) == 0) {
      profile_func_end;
      return str + idx;
    }
  }
  profile_func_end;
  return NULL;
}

func cstr32 cstr32_find_last(cstr32 str, cstr32 sub) {
  profile_func_begin;
  if (sub[0] == (c32)'\0') {
    profile_func_end;
    return str + cstr32_len_impl(str);
  }
  sz sub_len = cstr32_len_impl(sub);
  sz str_len = cstr32_len_impl(str);
  if (sub_len > str_len) {
    profile_func_end;
    return NULL;
  }
  cstr32 last = NULL;
  sz limit = str_len - sub_len;
  for (sz idx = 0; idx <= limit; idx++) {
    if (memcmp(str + idx, sub, sub_len * size_of(c32)) == 0) {
      last = str + idx;
    }
  }
  profile_func_end;
  return last;
}

func cstr32 cstr32_find_char(cstr32 str, c32 chr) {
  profile_func_begin;
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      profile_func_end;
      return str + idx;
    }
    if (str[idx] == (c32)'\0') {
      break;
    }
  }
  profile_func_end;
  return NULL;
}

func cstr32 cstr32_find_last_char(cstr32 str, c32 chr) {
  profile_func_begin;
  cstr32 last = NULL;
  for (sz idx = 0;; idx++) {
    if (str[idx] == chr) {
      last = str + idx;
    }
    if (str[idx] == (c32)'\0') {
      break;
    }
  }
  profile_func_end;
  return last;
}

func sz cstr32_count_char(cstr32 str, c32 chr) {
  profile_func_begin;
  sz count = 0;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    if (str[idx] == chr) {
      count++;
    }
  }
  profile_func_end;
  return count;
}

func b32 cstr32_starts_with(cstr32 str, cstr32 prefix) {
  profile_func_begin;
  sz prefix_len = cstr32_len_impl(prefix);
  profile_func_end;
  return cstr32_cmp_n(str, prefix, prefix_len);
}

func b32 cstr32_ends_with(cstr32 str, cstr32 suffix) {
  profile_func_begin;
  sz str_len = cstr32_len_impl(str);
  sz suffix_len = cstr32_len_impl(suffix);
  if (suffix_len > str_len) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return memcmp(str + str_len - suffix_len, suffix, suffix_len * size_of(c32)) == 0 ? true : false;
}

// =========================================================================
// cstr32 — In-place mutation
// =========================================================================

func void cstr32_to_upper(c32* str) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    str[idx] = c32_to_upper(str[idx]);
  }
  profile_func_end;
}

func void cstr32_to_lower(c32* str) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    str[idx] = c32_to_lower(str[idx]);
  }
  profile_func_end;
}

func void cstr32_trim(c32* str) {
  profile_func_begin;
  sz start = 0;
  while (c32_is_space(str[start])) {
    start++;
  }
  sz len = cstr32_len_impl(str + start);
  while (len > 0 && c32_is_space(str[start + len - 1])) {
    len--;
  }
  memmove(str, str + start, len * size_of(c32));
  str[len] = (c32)'\0';
  profile_func_end;
}

func void cstr32_replace_char(c32* str, c32 from_chr, c32 to_chr) {
  profile_func_begin;
  for (sz idx = 0; str[idx] != (c32)'\0'; idx++) {
    if (str[idx] == from_chr) {
      str[idx] = to_chr;
    }
  }
  profile_func_end;
}

func sz cstr32_remove_char(c32* str, c32 chr) {
  profile_func_begin;
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
  profile_func_end;
  return removed;
}

func sz cstr32_remove_whitespace(c32* str) {
  profile_func_begin;
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
  profile_func_end;
  return removed;
}

func b32 cstr32_remove_prefix(c32* str, cstr32 prefix) {
  profile_func_begin;
  sz prefix_len = cstr32_len_impl(prefix);
  if (!cstr32_cmp_n(str, prefix, prefix_len)) {
    profile_func_end;
    return false;
  }
  sz str_len = cstr32_len_impl(str);
  memmove(str, str + prefix_len, (str_len - prefix_len + 1) * size_of(c32));
  profile_func_end;
  return true;
}

func b32 cstr32_remove_suffix(c32* str, cstr32 suffix) {
  profile_func_begin;
  sz str_len = cstr32_len_impl(str);
  sz suffix_len = cstr32_len_impl(suffix);
  if (suffix_len > str_len) {
    profile_func_end;
    return false;
  }
  if (memcmp(str + str_len - suffix_len, suffix, suffix_len * size_of(c32)) != 0) {
    profile_func_end;
    return false;
  }
  str[str_len - suffix_len] = (c32)'\0';
  profile_func_end;
  return true;
}

func sz cstr32_replace(c32* str, sz str_cap, cstr32 from, cstr32 rep) {
  profile_func_begin;
  sz from_len = cstr32_len_impl(from);
  sz rep_len = cstr32_len_impl(rep);
  if (from_len == 0) {
    profile_func_end;
    return 0;
  }
  sz count = 0;
  sz pos = 0;
  sz str_len = cstr32_len_impl(str);
  while (pos + from_len <= str_len) {
    if (memcmp(str + pos, from, from_len * size_of(c32)) == 0) {
      sz new_len = str_len - from_len + rep_len;
      if (new_len >= str_cap) {
        break;
      }
      memmove(str + pos + rep_len, str + pos + from_len, (str_len - pos - from_len + 1) * size_of(c32));
      memcpy(str + pos, rep, rep_len * size_of(c32));
      str_len = new_len;
      pos += rep_len;
      count++;
    } else {
      pos++;
    }
  }
  profile_func_end;
  return count;
}

func sz cstr32_common_prefix(cstr32 lhs, cstr32 rhs, c32* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz idx = 0;
  while (lhs[idx] != (c32)'\0' && rhs[idx] != (c32)'\0' && lhs[idx] == rhs[idx] && idx < buf_cap - 1) {
    buf[idx] = lhs[idx];
    idx++;
  }
  buf[idx] = (c32)'\0';
  profile_func_end;
  return idx;
}

func void cstr32_beautify(c32* str) {
  profile_func_begin;
  cstr32_to_lower(str);
  str[0] = c32_to_upper(str[0]);
  profile_func_end;
}

// =========================================================================
// cstr32 — Parsing
// =========================================================================

func b32 cstr32_to_i64(cstr32 str, i64* out) {
  profile_func_begin;
  c8 buf[64];
  if (!cstr32_to_ascii(str, buf, size_of(buf))) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return cstr8_to_i64(buf, out);
}

func b32 cstr32_to_f64(cstr32 str, f64* out) {
  profile_func_begin;
  c8 buf[256];
  if (!cstr32_to_ascii(str, buf, size_of(buf))) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return cstr8_to_f64(buf, out);
}

// =========================================================================
// Cross-encoding conversion (null-terminated)
// =========================================================================

func sz cstr8_to_cstr16(cstr8 src, c16* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr8_len(src);
  sz written = utf8_to_utf16(src, src_len, buf, buf_cap - 1);
  buf[written] = (c16)'\0';
  profile_func_end;
  return written;
}

func sz cstr8_to_cstr32(cstr8 src, c32* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr8_len(src);
  sz written = utf8_to_utf32(src, src_len, buf, buf_cap - 1);
  buf[written] = (c32)'\0';
  profile_func_end;
  return written;
}

func sz cstr16_to_cstr8(cstr16 src, c8* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz written = utf16_to_utf8(src, src_len, buf, buf_cap - 1);
  buf[written] = '\0';
  profile_func_end;
  return written;
}

func sz cstr16_to_cstr32(cstr16 src, c32* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr16_len_impl(src);
  sz written = utf16_to_utf32(src, src_len, buf, buf_cap - 1);
  buf[written] = (c32)'\0';
  profile_func_end;
  return written;
}

func sz cstr32_to_cstr8(cstr32 src, c8* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz written = utf32_to_utf8(src, src_len, buf, buf_cap - 1);
  buf[written] = '\0';
  profile_func_end;
  return written;
}

func sz cstr32_to_cstr16(cstr32 src, c16* buf, sz buf_cap) {
  profile_func_begin;
  if (buf_cap == 0) {
    profile_func_end;
    return 0;
  }
  sz src_len = cstr32_len_impl(src);
  sz written = utf32_to_utf16(src, src_len, buf, buf_cap - 1);
  buf[written] = (c16)'\0';
  profile_func_end;
  return written;
}
