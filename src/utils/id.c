// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/id.h"
#include "basic/assert.h"
#include "basic/profiler.h"

#include <errno.h>
#include <stdlib.h>

// Shared decimal digit counter reused by all ID widths.
func sz id_u64_digits(u64 value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz digits = 1;
  while (value >= 10ULL) {
    value /= 10ULL;
    digits++;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return digits;
}

// Shared unsigned decimal parser with a caller-supplied upper bound.
func b32 id_parse_u64(cstr8 src, u64 max_value, u64* out) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || out == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);
  assert(out != NULL);
  if (src[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  char* end_ptr;
  errno = 0;
  unsigned long long parsed = strtoull(src, &end_ptr, 10);
  if (errno != 0 || end_ptr == src || *end_ptr != '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if ((u64)parsed > max_value) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  *out = (u64)parsed;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// Generates the same helper set for each fixed-width ID wrapper.
// Only the underlying storage type and parse limit differ between variants.
#define ID_DEFINE_TYPE(NAME, TYPE, MAX_VALUE)                               \
  func NAME NAME##_zero(void) {                                             \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    NAME ident = {.value = 0};                                              \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return ident;                                                           \
  }                                                                         \
                                                                            \
  func NAME NAME##_make(TYPE value) {                                       \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    NAME ident = {.value = value};                                          \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return ident;                                                           \
  }                                                                         \
                                                                            \
  func TYPE NAME##_get(NAME ident) {                                        \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return ident.value;                                                     \
  }                                                                         \
                                                                            \
  func b32 NAME##_is_zero(NAME ident) {                                     \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return ident.value == (TYPE)0 ? 1 : 0;                                  \
  }                                                                         \
                                                                            \
  func b32 NAME##_is_valid(NAME ident) {                                    \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return ident.value != (TYPE)0 ? 1 : 0;                                  \
  }                                                                         \
                                                                            \
  func NAME NAME##_next(NAME ident) {                                       \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    ident.value = (TYPE)(ident.value + 1);                                  \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return ident;                                                           \
  }                                                                         \
                                                                            \
  func i32 NAME##_cmp(NAME lhs, NAME rhs) {                                 \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    if (lhs.value < rhs.value) {                                            \
      TracyCZoneEnd(__tracy_zone_ctx);                                      \
      return -1;                                                            \
    }                                                                       \
    if (lhs.value > rhs.value) {                                            \
      TracyCZoneEnd(__tracy_zone_ctx);                                      \
      return 1;                                                             \
    }                                                                       \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return 0;                                                               \
  }                                                                         \
                                                                            \
  func sz NAME##_string_length(NAME ident) {                                \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return id_u64_digits((u64)ident.value);                                 \
  }                                                                         \
                                                                            \
  func b32 NAME##_parse_cstr8(cstr8 src, NAME* out) {                       \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    u64 parsed = 0;                                                         \
    if (!id_parse_u64(src, (u64)(MAX_VALUE), &parsed)) {                    \
      TracyCZoneEnd(__tracy_zone_ctx);                                      \
      return 0;                                                             \
    }                                                                       \
    *out = NAME##_make((TYPE)parsed);                                       \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return 1;                                                               \
  }                                                                         \
                                                                            \
  func b32 NAME##_to_cstr8(NAME ident, c8* dst, sz cap) {                   \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    sz needed = NAME##_string_length(ident) + 1;                            \
    if (cap < needed) {                                                     \
      if (cap > 0) {                                                        \
        dst[0] = '\0';                                                      \
      }                                                                     \
      TracyCZoneEnd(__tracy_zone_ctx);                                      \
      return 0;                                                             \
    }                                                                       \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return cstr8_format(dst, cap, "%llu", (unsigned long long)ident.value); \
  }                                                                         \
                                                                            \
  func b32 NAME##_to_str8(NAME ident, str8* dst) {                          \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                             \
    if (dst->cap == 0) {                                                    \
      dst->size = 0;                                                        \
      TracyCZoneEnd(__tracy_zone_ctx);                                      \
      return 0;                                                             \
    }                                                                       \
    b32 success = NAME##_to_cstr8(ident, dst->ptr, dst->cap);               \
    dst->size = cstr8_len(dst->ptr);                                        \
    TracyCZoneEnd(__tracy_zone_ctx);                                        \
    return success;                                                         \
  }

// Instantiate the shared helpers for each supported ID width.
ID_DEFINE_TYPE(id8, u8, U8_MAX)
ID_DEFINE_TYPE(id16, u16, U16_MAX)
ID_DEFINE_TYPE(id32, u32, U32_MAX)
ID_DEFINE_TYPE(id64, u64, U64_MAX)

#undef ID_DEFINE_TYPE
