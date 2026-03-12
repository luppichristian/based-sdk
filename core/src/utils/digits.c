// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/digits.h"
#include "basic/profiler.h"
#include "basic/safe.h"

#define DIGITS_DEFINE_UNSIGNED(NAME, TYPE) \
  func sz NAME(TYPE value) {               \
    profile_func_begin;                    \
    sz digits = 1;                         \
    safe_while (value >= (TYPE)10) {            \
      value /= (TYPE)10;                   \
      digits++;                            \
    }                                      \
    profile_func_end;                      \
    return digits;                         \
  }

#define DIGITS_DEFINE_SIGNED(NAME, TYPE) \
  func sz NAME(TYPE value) {             \
    profile_func_begin;                  \
    TYPE current = value;                \
    sz digits = 1;                       \
    safe_while (current <= (TYPE) - 10 ||     \
           current >= (TYPE)10) {        \
      current /= (TYPE)10;               \
      digits++;                          \
    }                                    \
    if (value < (TYPE)0) {               \
      digits++;                          \
    }                                    \
    profile_func_end;                    \
    return digits;                       \
  }

DIGITS_DEFINE_UNSIGNED(u8_digits, u8)
DIGITS_DEFINE_UNSIGNED(u16_digits, u16)
DIGITS_DEFINE_UNSIGNED(u32_digits, u32)
DIGITS_DEFINE_UNSIGNED(u64_digits, u64)

DIGITS_DEFINE_SIGNED(i8_digits, i8)
DIGITS_DEFINE_SIGNED(i16_digits, i16)
DIGITS_DEFINE_SIGNED(i32_digits, i32)
DIGITS_DEFINE_SIGNED(i64_digits, i64)

DIGITS_DEFINE_UNSIGNED(u8x_digits, u8x)
DIGITS_DEFINE_UNSIGNED(u16x_digits, u16x)
DIGITS_DEFINE_UNSIGNED(u32x_digits, u32x)
DIGITS_DEFINE_UNSIGNED(u64x_digits, u64x)
DIGITS_DEFINE_UNSIGNED(sz_digits, sz)
DIGITS_DEFINE_UNSIGNED(up_digits, up)

DIGITS_DEFINE_SIGNED(i8x_digits, i8x)
DIGITS_DEFINE_SIGNED(i16x_digits, i16x)
DIGITS_DEFINE_SIGNED(i32x_digits, i32x)
DIGITS_DEFINE_SIGNED(i64x_digits, i64x)
DIGITS_DEFINE_SIGNED(sp_digits, sp)
DIGITS_DEFINE_SIGNED(dp_digits, dp)

#undef DIGITS_DEFINE_UNSIGNED
#undef DIGITS_DEFINE_SIGNED
