// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/uuid.h"

#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "strings/char.h"
#include "basic/profiler.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

func uuid uuid_zero(void) {
  uuid value = {0};
  return value;
}

func uuid uuid_from_bytes(const u8* bytes) {
  profile_func_begin;
  uuid value = {0};
  if (bytes == NULL) {
    profile_func_end;
    return value;
  }
  assert(bytes != NULL);
  memcpy(value.bytes, bytes, size_of(value.bytes));
  profile_func_end;
  return value;
}

func uuid uuid_from_u64(u64 upper, u64 lower) {
  profile_func_begin;
  uuid value = {0};
  for (sz idx = 0; idx < 8; idx++) {
    u32 shift = (u32)(56U - (u32)idx * 8U);
    value.bytes[idx] = (u8)((upper >> shift) & 0xFFU);
    value.bytes[idx + 8] = (u8)((lower >> shift) & 0xFFU);
  }
  profile_func_end;
  return value;
}

func void uuid_get_bytes(uuid value, u8* dst) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return;
  }
  assert(dst != NULL);
  memcpy(dst, value.bytes, size_of(value.bytes));
  profile_func_end;
}

func u64 uuid_get_upper(uuid value) {
  profile_func_begin;
  u64 result = 0;
  for (sz idx = 0; idx < 8; idx++) {
    result = (result << 8U) | (u64)value.bytes[idx];
  }
  profile_func_end;
  return result;
}

func u64 uuid_get_lower(uuid value) {
  profile_func_begin;
  u64 result = 0;
  for (sz idx = 8; idx < 16; idx++) {
    result = (result << 8U) | (u64)value.bytes[idx];
  }
  profile_func_end;
  return result;
}

func b32 uuid_is_zero(uuid value) {
  profile_func_begin;
  for (sz idx = 0; idx < count_of(value.bytes); idx++) {
    if (value.bytes[idx] != 0U) {
      profile_func_end;
      return false;
    }
  }
  profile_func_end;
  return true;
}

func b32 uuid_equal(uuid lhs, uuid rhs) {
  return memcmp(lhs.bytes, rhs.bytes, size_of(lhs.bytes)) == 0 ? true : false;
}

func i32 uuid_cmp(uuid lhs, uuid rhs) {
  profile_func_begin;
  i32 res = memcmp(lhs.bytes, rhs.bytes, size_of(lhs.bytes));
  profile_func_end;
  return res;
}

func u8 uuid_get_version(uuid value) {
  return (u8)((value.bytes[6] >> 4U) & 0x0FU);
}

func u8 uuid_get_variant(uuid value) {
  return (u8)((value.bytes[8] >> 6U) & 0x03U);
}

func sz uuid_string_length(void) {
  return 36;
}

func b32 uuid_parse_cstr8(cstr8 src, uuid* out) {
  profile_func_begin;
  if (src == NULL || out == NULL) {
    profile_func_end;
    return false;
  }
  assert(src != NULL);
  assert(out != NULL);
  if (cstr8_len(src) != uuid_string_length()) {
    profile_func_end;
    return false;
  }

  uuid value = {0};
  sz src_idx = 0;
  sz byte_idx = 0;
  while (src_idx < uuid_string_length()) {
    if (src_idx == 8 || src_idx == 13 || src_idx == 18 || src_idx == 23) {
      if (src[src_idx] != '-') {
        profile_func_end;
        return false;
      }
      src_idx++;
      continue;
    }

    i32 high = c8_hex_to_nibble(src[src_idx]);
    i32 low = c8_hex_to_nibble(src[src_idx + 1]);
    if (high < 0 || low < 0) {
      profile_func_end;
      return false;
    }

    value.bytes[byte_idx] = (u8)((high << 4) | low);
    byte_idx++;
    src_idx += 2;
  }

  *out = value;
  profile_func_end;
  return true;
}

func b32 uuid_to_cstr8(uuid value, c8* dst, sz cap) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  sz needed = uuid_string_length() + 1;
  if (cap < needed) {
    if (cap > 0) {
      dst[0] = '\0';
    }
    profile_func_end;
    return false;
  }

  sz dst_idx = 0;
  for (sz byte_idx = 0; byte_idx < count_of(value.bytes); byte_idx++) {
    if (dst_idx == 8 || dst_idx == 13 || dst_idx == 18 || dst_idx == 23) {
      dst[dst_idx++] = '-';
    }

    u8 byte_value = value.bytes[byte_idx];
    dst[dst_idx++] = c8_nibble_to_hex((u8)(byte_value >> 4U));
    dst[dst_idx++] = c8_nibble_to_hex((u8)(byte_value & 0x0FU));
  }

  dst[dst_idx] = '\0';
  profile_func_end;
  return true;
}

func b32 uuid_to_cstr16(uuid value, c16* dst, sz cap) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  c8 buffer[37];
  if (!uuid_to_cstr8(value, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c16)'\0';
    }
    profile_func_end;
    return false;
  }

  sz written = cstr8_to_cstr16(buffer, dst, cap);
  profile_func_end;
  return written == uuid_string_length() ? true : false;
}

func b32 uuid_to_cstr32(uuid value, c32* dst, sz cap) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  c8 buffer[37];
  if (!uuid_to_cstr8(value, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c32)'\0';
    }
    profile_func_end;
    return false;
  }

  sz written = cstr8_to_cstr32(buffer, dst, cap);
  profile_func_end;
  return written == uuid_string_length() ? true : false;
}

func b32 uuid_to_str8(uuid value, str8* dst) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  if (dst->cap == 0) {
    dst->size = 0;
    profile_func_end;
    return false;
  }

  b32 success = uuid_to_cstr8(value, dst->ptr, dst->cap);
  dst->size = cstr8_len(dst->ptr);
  profile_func_end;
  return success;
}

func b32 uuid_to_str16(uuid value, str16* dst) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  if (dst->cap == 0) {
    dst->size = 0;
    profile_func_end;
    return false;
  }

  b32 success = uuid_to_cstr16(value, dst->ptr, dst->cap);
  dst->size = cstr16_len(dst->ptr);
  profile_func_end;
  return success;
}

func b32 uuid_to_str32(uuid value, str32* dst) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  if (dst->cap == 0) {
    dst->size = 0;
    profile_func_end;
    return false;
  }

  b32 success = uuid_to_cstr32(value, dst->ptr, dst->cap);
  dst->size = cstr32_len(dst->ptr);
  profile_func_end;
  return success;
}

func uuid uuid_generate_v4(random_series* series) {
  if (!series) {
    return uuid_zero();
  }
  profile_func_begin;
  uuid value = {0};
  for (sz idx = 0; idx < count_of(value.bytes); idx++) {
    value.bytes[idx] = random_series_u8(series);
  }

  // RFC 4122 variant + version 4.
  value.bytes[6] = (u8)((value.bytes[6] & 0x0FU) | 0x40U);
  value.bytes[8] = (u8)((value.bytes[8] & 0x3FU) | 0x80U);
  profile_func_end;
  return value;
}
