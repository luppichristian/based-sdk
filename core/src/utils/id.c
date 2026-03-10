// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/id.h"
#include "utils/digits.h"
#include "basic/assert.h"
#include "basic/profiler.h"

#include <errno.h>
#include <stdlib.h>

// Generates the same helper set for each fixed-width ID wrapper.
// Only the underlying storage type and parse limit differ between variants.
#define ID_DEFINE_TYPE(NAME, TYPE, MAX_VALUE)                                      \
  func NAME NAME##_zero(void) {                                                    \
    profile_func_begin;                                                            \
    NAME ident = {.value = 0};                                                     \
    profile_func_end;                                                              \
    return ident;                                                                  \
  }                                                                                \
                                                                                   \
  func NAME NAME##_make(TYPE value) {                                              \
    profile_func_begin;                                                            \
    NAME ident = {.value = value};                                                 \
    profile_func_end;                                                              \
    return ident;                                                                  \
  }                                                                                \
                                                                                   \
  func TYPE NAME##_get(NAME ident) {                                               \
    return ident.value;                                                            \
  }                                                                                \
                                                                                   \
  func b32 NAME##_is_zero(NAME ident) {                                            \
    return ident.value == (TYPE)0 ? true : false;                                  \
  }                                                                                \
                                                                                   \
  func b32 NAME##_is_valid(NAME ident) {                                           \
    return ident.value != (TYPE)0 ? true : false;                                  \
  }                                                                                \
                                                                                   \
  func NAME NAME##_next(NAME ident) {                                              \
    profile_func_begin;                                                            \
    ident.value = (TYPE)(ident.value + 1);                                         \
    profile_func_end;                                                              \
    return ident;                                                                  \
  }                                                                                \
                                                                                   \
  func i32 NAME##_cmp(NAME lhs, NAME rhs) {                                        \
    profile_func_begin;                                                            \
    if (lhs.value < rhs.value) {                                                   \
      profile_func_end;                                                            \
      return -1;                                                                   \
    }                                                                              \
    if (lhs.value > rhs.value) {                                                   \
      profile_func_end;                                                            \
      return 1;                                                                    \
    }                                                                              \
    profile_func_end;                                                              \
    return 0;                                                                      \
  }                                                                                \
                                                                                   \
  func sz NAME##_string_length(NAME ident) {                                       \
    return u64_digits((u64)ident.value);                                           \
  }                                                                                \
                                                                                   \
  func b32 NAME##_parse_cstr8(cstr8 src, NAME* out) {                              \
    profile_func_begin;                                                            \
    u64 parsed = 0;                                                                \
    if (!cstr8_to_u64(src, (u64)(MAX_VALUE), &parsed)) {                           \
      profile_func_end;                                                            \
      return false;                                                                \
    }                                                                              \
    *out = NAME##_make((TYPE)parsed);                                              \
    profile_func_end;                                                              \
    return true;                                                                   \
  }                                                                                \
                                                                                   \
  func b32 NAME##_to_cstr8(NAME ident, c8* dst, sz cap) {                          \
    profile_func_begin;                                                            \
    sz needed = NAME##_string_length(ident) + 1;                                   \
    if (cap < needed) {                                                            \
      if (cap > 0) {                                                               \
        dst[0] = '\0';                                                             \
      }                                                                            \
      profile_func_end;                                                            \
      return false;                                                                \
    }                                                                              \
    b32 success = cstr8_format(dst, cap, "%llu", (unsigned long long)ident.value); \
    profile_func_end;                                                              \
    return success;                                                                \
  }                                                                                \
                                                                                   \
  func b32 NAME##_to_str8(NAME ident, str8* dst) {                                 \
    profile_func_begin;                                                            \
    if (dst->cap == 0) {                                                           \
      dst->size = 0;                                                               \
      profile_func_end;                                                            \
      return false;                                                                \
    }                                                                              \
    b32 success = NAME##_to_cstr8(ident, dst->ptr, dst->cap);                      \
    dst->size = cstr8_len(dst->ptr);                                               \
    profile_func_end;                                                              \
    return success;                                                                \
  }

// Instantiate the shared helpers for each supported ID width.
ID_DEFINE_TYPE(id8, u8, U8_MAX)
ID_DEFINE_TYPE(id16, u16, U16_MAX)
ID_DEFINE_TYPE(id32, u32, U32_MAX)
ID_DEFINE_TYPE(id64, u64, U64_MAX)

#undef ID_DEFINE_TYPE
