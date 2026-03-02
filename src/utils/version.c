// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/version.h"
#include "basic/utility_defines.h"

func sz version_u32_digits(u32 value) {
  sz digits = 1;
  while (value >= 10U) {
    value /= 10U;
    digits++;
  }
  return digits;
}

func version version_zero(void) {
  version ver = {.packed = 0};
  return ver;
}

func version version_from_packed(u32 packed) {
  version ver = {.packed = packed};
  return ver;
}

func version version_make(u8 major, u8 minor, u16 patch) {
  version ver = {
      .packed = (u32)major | ((u32)minor << 8) | ((u32)patch << 16),
  };
  return ver;
}

func u32 version_get_packed(version ver) {
  return ver.packed;
}

func u8 version_get_major(version ver) {
  return (u8)(ver.packed & 0xFFU);
}

func u8 version_get_minor(version ver) {
  return (u8)((ver.packed >> 8) & 0xFFU);
}

func u16 version_get_patch(version ver) {
  return (u16)((ver.packed >> 16) & 0xFFFFU);
}

func b32 version_is_zero(version ver) {
  return ver.packed == 0 ? 1 : 0;
}

func sz version_string_length(version ver) {
  return version_u32_digits((u32)version_get_major(ver)) +
         version_u32_digits((u32)version_get_minor(ver)) +
         version_u32_digits((u32)version_get_patch(ver)) + 2;
}

func i32 version_cmp(version lhs, version rhs) {
  if (version_get_major(lhs) < version_get_major(rhs)) {
    return -1;
  }
  if (version_get_major(lhs) > version_get_major(rhs)) {
    return 1;
  }
  if (version_get_minor(lhs) < version_get_minor(rhs)) {
    return -1;
  }
  if (version_get_minor(lhs) > version_get_minor(rhs)) {
    return 1;
  }
  if (version_get_patch(lhs) < version_get_patch(rhs)) {
    return -1;
  }
  if (version_get_patch(lhs) > version_get_patch(rhs)) {
    return 1;
  }
  return 0;
}

func b32 version_to_cstr8(version ver, c8* dst, sz cap) {
  sz needed = version_string_length(ver) + 1;
  if (cap < needed) {
    if (cap > 0) {
      dst[0] = '\0';
    }
    return 0;
  }

  return cstr8_format(
      dst,
      cap,
      "%u.%u.%u",
      (unsigned int)version_get_major(ver),
      (unsigned int)version_get_minor(ver),
      (unsigned int)version_get_patch(ver));
}

func b32 version_to_cstr16(version ver, c16* dst, sz cap) {
  c8 buffer[32];
  if (!version_to_cstr8(ver, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c16)'\0';
    }
    return 0;
  }

  sz expected = cstr8_len(buffer);
  sz written = cstr8_to_cstr16(buffer, dst, cap);
  return written == expected ? 1 : 0;
}

func b32 version_to_cstr32(version ver, c32* dst, sz cap) {
  c8 buffer[32];
  if (!version_to_cstr8(ver, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c32)'\0';
    }
    return 0;
  }

  sz expected = cstr8_len(buffer);
  sz written = cstr8_to_cstr32(buffer, dst, cap);
  return written == expected ? 1 : 0;
}

func b32 version_to_str8(version ver, str8* dst) {
  if (dst->cap == 0) {
    dst->size = 0;
    return 0;
  }

  b32 success = version_to_cstr8(ver, dst->ptr, dst->cap);
  dst->size = cstr8_len(dst->ptr);
  return success;
}

func b32 version_to_str16(version ver, str16* dst) {
  if (dst->cap == 0) {
    dst->size = 0;
    return 0;
  }

  b32 success = version_to_cstr16(ver, dst->ptr, dst->cap);
  dst->size = cstr16_len(dst->ptr);
  return success;
}

func b32 version_to_str32(version ver, str32* dst) {
  if (dst->cap == 0) {
    dst->size = 0;
    return 0;
  }

  b32 success = version_to_cstr32(ver, dst->ptr, dst->cap);
  dst->size = cstr32_len(dst->ptr);
  return success;
}
