// MIT License
// Copyright (c) 2026 Christian Luppi

#include "strings/char.h"
#include "basic/assert.h"
#include "basic/profiler.h"

func u32 c8_to_code(c8 chr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u32)(u8)chr;
}

func u32 c16_to_code(c16 chr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u32)chr;
}

func u32 c32_to_code(c32 chr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u32)chr;
}

func b32 char_code_is_upper(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code >= (u32)'A' && code <= (u32)'Z';
}

func b32 char_code_is_lower(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code >= (u32)'a' && code <= (u32)'z';
}

func b32 char_code_is_alpha(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return char_code_is_upper(code) || char_code_is_lower(code);
}

func b32 char_code_is_digit(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code >= (u32)'0' && code <= (u32)'9';
}

func b32 char_code_is_xdigit(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return char_code_is_digit(code) || (code >= (u32)'a' && code <= (u32)'f') ||
         (code >= (u32)'A' && code <= (u32)'F');
}

func b32 char_code_is_space(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code == (u32)' ' || code == (u32)'\t' || code == (u32)'\n' || code == (u32)'\r' ||
         code == (u32)'\f' || code == (u32)'\v';
}

func b32 char_code_is_blank(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code == (u32)' ' || code == (u32)'\t';
}

func b32 char_code_is_cntrl(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code <= 0x1FU || code == 0x7FU;
}

func b32 char_code_is_print(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code >= 0x20U && code <= 0x7EU;
}

func b32 char_code_is_graph(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return code >= 0x21U && code <= 0x7EU;
}

func b32 char_code_is_alnum(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return char_code_is_alpha(code) || char_code_is_digit(code);
}

func b32 char_code_is_punct(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return char_code_is_graph(code) && !char_code_is_alnum(code);
}

func u32 char_code_to_lower(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (char_code_is_upper(code)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return code - (u32)'A' + (u32)'a';
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return code;
}

func u32 char_code_to_upper(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (char_code_is_lower(code)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return code - (u32)'a' + (u32)'A';
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return code;
}

func i32 char_code_hex_to_nibble(u32 code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  code = char_code_to_lower(code);
  if (char_code_is_digit(code)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (i32)(code - (u32)'0');
  }
  if (code >= (u32)'a' && code <= (u32)'f') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (i32)(code - (u32)'a' + 10U);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return -1;
}

func c8 char_code_nibble_to_hex(u8 nibble) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  local_persist const c8 digits[] = "0123456789abcdef";
  assert((nibble & 0x0FU) < 16);
  TracyCZoneEnd(__tracy_zone_ctx);
  return digits[nibble & 0x0FU];
}

#define DEFINE_CHAR_FUNCS(PREFIX, TYPE, TO_CODE) \
  func b32 PREFIX##_is_alnum(TYPE chr) {         \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_alnum(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_alpha(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_alpha(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_blank(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_blank(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_cntrl(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_cntrl(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_digit(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_digit(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_graph(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_graph(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_lower(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_lower(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_print(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_print(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_punct(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_punct(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_space(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_space(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_upper(TYPE chr) {           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_upper(TO_CODE(chr));       \
  }                                                 \
  func b32 PREFIX##_is_xdigit(TYPE chr) {          \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_is_xdigit(TO_CODE(chr));      \
  }                                                 \
  func TYPE PREFIX##_to_lower(TYPE chr) {          \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return (TYPE)char_code_to_lower(TO_CODE(chr)); \
  }                                                 \
  func TYPE PREFIX##_to_upper(TYPE chr) {          \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return (TYPE)char_code_to_upper(TO_CODE(chr)); \
  }                                                 \
  func i32 PREFIX##_hex_to_nibble(TYPE chr) {      \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return char_code_hex_to_nibble(TO_CODE(chr));  \
  }                                                 \
  func TYPE PREFIX##_nibble_to_hex(u8 nibble) {    \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);    \
    TracyCZoneEnd(__tracy_zone_ctx);               \
    return (TYPE)char_code_nibble_to_hex(nibble);  \
  }

DEFINE_CHAR_FUNCS(c8, c8, c8_to_code)
DEFINE_CHAR_FUNCS(c16, c16, c16_to_code)
DEFINE_CHAR_FUNCS(c32, c32, c32_to_code)

#undef DEFINE_CHAR_FUNCS
