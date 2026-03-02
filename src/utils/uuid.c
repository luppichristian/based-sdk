// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/uuid.h"

#include "basic/utility_defines.h"

#include <string.h>

func i32 uuid_hex_to_nibble(c8 chr) {
  if (chr >= '0' && chr <= '9') {
    return chr - '0';
  }
  if (chr >= 'a' && chr <= 'f') {
    return chr - 'a' + 10;
  }
  if (chr >= 'A' && chr <= 'F') {
    return chr - 'A' + 10;
  }
  return -1;
}

func c8 uuid_nibble_to_hex(u8 nibble) {
  local_persist const c8 digits[] = "0123456789abcdef";
  return digits[nibble & 0x0FU];
}

func uuid uuid_zero(void) {
  uuid value = {0};
  return value;
}

func uuid uuid_from_bytes(const u8* bytes) {
  uuid value = {0};
  memcpy(value.bytes, bytes, size_of(value.bytes));
  return value;
}

func uuid uuid_from_u64(u64 upper, u64 lower) {
  uuid value = {0};
  for (sz index = 0; index < 8; index++) {
    u32 shift = (u32)(56U - (u32)index * 8U);
    value.bytes[index] = (u8)((upper >> shift) & 0xFFU);
    value.bytes[index + 8] = (u8)((lower >> shift) & 0xFFU);
  }
  return value;
}

func void uuid_get_bytes(uuid value, u8* dst) {
  memcpy(dst, value.bytes, size_of(value.bytes));
}

func u64 uuid_get_upper(uuid value) {
  u64 result = 0;
  for (sz index = 0; index < 8; index++) {
    result = (result << 8U) | (u64)value.bytes[index];
  }
  return result;
}

func u64 uuid_get_lower(uuid value) {
  u64 result = 0;
  for (sz index = 8; index < 16; index++) {
    result = (result << 8U) | (u64)value.bytes[index];
  }
  return result;
}

func b32 uuid_is_zero(uuid value) {
  for (sz index = 0; index < count_of(value.bytes); index++) {
    if (value.bytes[index] != 0U) {
      return 0;
    }
  }
  return 1;
}

func b32 uuid_equal(uuid lhs, uuid rhs) {
  return memcmp(lhs.bytes, rhs.bytes, size_of(lhs.bytes)) == 0 ? 1 : 0;
}

func i32 uuid_cmp(uuid lhs, uuid rhs) {
  return memcmp(lhs.bytes, rhs.bytes, size_of(lhs.bytes));
}

func u8 uuid_get_version(uuid value) {
  return (u8)((value.bytes[6] >> 4U) & 0x0FU);
}

func u8 uuid_get_variant(uuid value) {
  return (u8)((value.bytes[8] >> 5U) & 0x07U);
}

func sz uuid_string_length(void) {
  return 36;
}

func b32 uuid_parse_cstr8(const c8* src, uuid* out) {
  if (cstr8_len(src) != uuid_string_length()) {
    return 0;
  }

  uuid value = {0};
  sz src_index = 0;
  sz byte_index = 0;
  while (src_index < uuid_string_length()) {
    if (src_index == 8 || src_index == 13 || src_index == 18 || src_index == 23) {
      if (src[src_index] != '-') {
        return 0;
      }
      src_index++;
      continue;
    }

    i32 high = uuid_hex_to_nibble(src[src_index]);
    i32 low = uuid_hex_to_nibble(src[src_index + 1]);
    if (high < 0 || low < 0) {
      return 0;
    }

    value.bytes[byte_index] = (u8)((high << 4) | low);
    byte_index++;
    src_index += 2;
  }

  *out = value;
  return 1;
}

func b32 uuid_to_cstr8(uuid value, c8* dst, sz cap) {
  sz needed = uuid_string_length() + 1;
  if (cap < needed) {
    if (cap > 0) {
      dst[0] = '\0';
    }
    return 0;
  }

  sz dst_index = 0;
  for (sz byte_index = 0; byte_index < count_of(value.bytes); byte_index++) {
    if (dst_index == 8 || dst_index == 13 || dst_index == 18 || dst_index == 23) {
      dst[dst_index++] = '-';
    }

    u8 byte_value = value.bytes[byte_index];
    dst[dst_index++] = uuid_nibble_to_hex((u8)(byte_value >> 4U));
    dst[dst_index++] = uuid_nibble_to_hex((u8)(byte_value & 0x0FU));
  }

  dst[dst_index] = '\0';
  return 1;
}

func b32 uuid_to_cstr16(uuid value, c16* dst, sz cap) {
  c8 buffer[37];
  if (!uuid_to_cstr8(value, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c16)'\0';
    }
    return 0;
  }

  sz written = cstr8_to_cstr16(buffer, dst, cap);
  return written == uuid_string_length() ? 1 : 0;
}

func b32 uuid_to_cstr32(uuid value, c32* dst, sz cap) {
  c8 buffer[37];
  if (!uuid_to_cstr8(value, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c32)'\0';
    }
    return 0;
  }

  sz written = cstr8_to_cstr32(buffer, dst, cap);
  return written == uuid_string_length() ? 1 : 0;
}

func b32 uuid_to_str8(uuid value, str8* dst) {
  if (dst->cap == 0) {
    dst->size = 0;
    return 0;
  }

  b32 success = uuid_to_cstr8(value, dst->ptr, dst->cap);
  dst->size = cstr8_len(dst->ptr);
  return success;
}

func b32 uuid_to_str16(uuid value, str16* dst) {
  if (dst->cap == 0) {
    dst->size = 0;
    return 0;
  }

  b32 success = uuid_to_cstr16(value, dst->ptr, dst->cap);
  dst->size = cstr16_len(dst->ptr);
  return success;
}

func b32 uuid_to_str32(uuid value, str32* dst) {
  if (dst->cap == 0) {
    dst->size = 0;
    return 0;
  }

  b32 success = uuid_to_cstr32(value, dst->ptr, dst->cap);
  dst->size = cstr32_len(dst->ptr);
  return success;
}
