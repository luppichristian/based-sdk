// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/version.h"
#include "utils/digits.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include "context/thread_ctx.h"

func version version_zero(void) {
  profile_func_begin;
  version ver = {.packed = 0};
  profile_func_end;
  return ver;
}

func version version_from_packed(u32 packed) {
  profile_func_begin;
  version ver = {.packed = packed};
  profile_func_end;
  return ver;
}

func version version_make(u8 major, u8 minor, u16 patch) {
  profile_func_begin;
  version ver = {
      .packed = (u32)major | ((u32)minor << 8) | ((u32)patch << 16),
  };
  profile_func_end;
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
  return ver.packed == 0 ? true : false;
}

func sz version_string_length(version ver) {
  profile_func_begin;
  sz len = u32_digits((u32)version_get_major(ver)) +
           u32_digits((u32)version_get_minor(ver)) +
           u32_digits((u32)version_get_patch(ver)) + 2;
  profile_func_end;
  return len;
}

func i32 version_cmp(version lhs, version rhs) {
  profile_func_begin;
  if (version_get_major(lhs) < version_get_major(rhs)) {
    profile_func_end;
    return -1;
  }
  if (version_get_major(lhs) > version_get_major(rhs)) {
    profile_func_end;
    return 1;
  }
  if (version_get_minor(lhs) < version_get_minor(rhs)) {
    profile_func_end;
    return -1;
  }
  if (version_get_minor(lhs) > version_get_minor(rhs)) {
    profile_func_end;
    return 1;
  }
  if (version_get_patch(lhs) < version_get_patch(rhs)) {
    profile_func_end;
    return -1;
  }
  if (version_get_patch(lhs) > version_get_patch(rhs)) {
    profile_func_end;
    return 1;
  }
  profile_func_end;
  return 0;
}

func b32 version_to_cstr8(version ver, c8* dst, sz cap) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  sz needed = version_string_length(ver) + 1;
  if (cap < needed) {
    if (cap > 0) {
      dst[0] = '\0';
    }
    profile_func_end;
    return false;
  }

  b32 res = cstr8_format(
      dst,
      cap,
      "%u.%u.%u",
      (unsigned int)version_get_major(ver),
      (unsigned int)version_get_minor(ver),
      (unsigned int)version_get_patch(ver));

  profile_func_end;
  return res;
}

func b32 version_to_cstr16(version ver, c16* dst, sz cap) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  c8 buffer[32];
  if (!version_to_cstr8(ver, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c16)'\0';
    }
    profile_func_end;
    return false;
  }

  sz expected = cstr8_len(buffer);
  sz written = cstr8_to_cstr16(buffer, dst, cap);
  profile_func_end;
  return written == expected ? true : false;
}

func b32 version_to_cstr32(version ver, c32* dst, sz cap) {
  profile_func_begin;
  if (dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  c8 buffer[32];
  if (!version_to_cstr8(ver, buffer, count_of(buffer))) {
    if (cap > 0) {
      dst[0] = (c32)'\0';
    }
    profile_func_end;
    return false;
  }

  sz expected = cstr8_len(buffer);
  sz written = cstr8_to_cstr32(buffer, dst, cap);
  profile_func_end;
  return written == expected ? true : false;
}

func b32 version_to_str8(version ver, str8* dst) {
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

  b32 success = version_to_cstr8(ver, dst->ptr, dst->cap);
  dst->size = cstr8_len(dst->ptr);
  profile_func_end;
  return success;
}

func b32 version_to_str16(version ver, str16* dst) {
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

  b32 success = version_to_cstr16(ver, dst->ptr, dst->cap);
  dst->size = cstr16_len(dst->ptr);
  profile_func_end;
  return success;
}

func b32 version_to_str32(version ver, str32* dst) {
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

  b32 success = version_to_cstr32(ver, dst->ptr, dst->cap);
  dst->size = cstr32_len(dst->ptr);
  profile_func_end;
  return success;
}

func b32 version_parse_cstr8(cstr8 src, version* out_ver) {
  profile_func_begin;
  if (src == NULL || out_ver == NULL) {
    profile_func_end;
    return false;
  }

  unsigned int major = 0;
  unsigned int minor = 0;
  unsigned int patch = 0;
  if (!cstr8_scan(src, "%u.%u.%u", &major, &minor, &patch)) {
    thread_log_error("Could not parse version from string: %s", src);
    profile_func_end;
    return false;
  }

  if (major > 0xFFU || minor > 0xFFU || patch > 0xFFFFU) {
    thread_log_error("Could not parse version from string: %s", src);
    profile_func_end;
    return false;
  }

  *out_ver = version_make((u8)major, (u8)minor, (u16)patch);
  profile_func_end;
  return true;
}
