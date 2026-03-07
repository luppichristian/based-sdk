// MIT License
// Copyright (c) 2026 Christian Luppi

#include "strings/unicode.h"
#include "basic/assert.h"
#include "basic/profiler.h"

// =========================================================================
// Validity
// =========================================================================

func b32 unicode_is_valid(c32 codepoint) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  // Surrogates (U+D800..U+DFFF) and values above U+10FFFF are not scalar values.
  if (codepoint >= 0xD800U && codepoint <= 0xDFFFU) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return codepoint <= 0x10FFFFU ? 1 : 0;
}

// =========================================================================
// UTF-8
// =========================================================================

func sz utf8_encode_size(c32 codepoint) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!unicode_is_valid(codepoint)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (codepoint <= 0x7FU) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  if (codepoint <= 0x7FFU) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 2;
  }
  if (codepoint <= 0xFFFFU) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 3;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 4;
}

func sz utf8_byte_count(c8 first_byte) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  u8 byte = (u8)first_byte;
  if (byte <= 0x7FU) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  if ((byte & 0xE0U) == 0xC0U) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 2;
  }
  if ((byte & 0xF0U) == 0xE0U) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 3;
  }
  if ((byte & 0xF8U) == 0xF0U) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 4;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func c32 utf8_decode(cstr8 ptr, sz* consumed) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (ptr == NULL || consumed == NULL) {
    if (consumed != NULL) {
      *consumed = 0;
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return UNICODE_REPLACEMENT_CHAR;
  }
  assert(ptr != NULL);
  assert(consumed != NULL);
  u8 first = (u8)*ptr;
  sz byte_cnt = utf8_byte_count((c8)first);

  if (byte_cnt == 0) {
    *consumed = 1;
    TracyCZoneEnd(__tracy_zone_ctx);
    return UNICODE_REPLACEMENT_CHAR;
  }

  if (byte_cnt == 1) {
    *consumed = 1;
    TracyCZoneEnd(__tracy_zone_ctx);
    return (c32)first;
  }

  // Extract payload bits from the leading byte.
  static const u8 LEAD_MASKS[4] = {0x7FU, 0x1FU, 0x0FU, 0x07U};
  c32 codepoint = (c32)(first & LEAD_MASKS[byte_cnt - 1]);

  // Accumulate continuation bytes.
  for (sz idx = 1; idx < byte_cnt; idx++) {
    u8 cont = (u8)ptr[idx];
    if ((cont & 0xC0U) != 0x80U) {
      // Truncated sequence: consume what we have.
      *consumed = idx;
      TracyCZoneEnd(__tracy_zone_ctx);
      return UNICODE_REPLACEMENT_CHAR;
    }
    codepoint = (codepoint << 6) | (c32)(cont & 0x3FU);
  }

  // Reject overlong encodings and invalid scalar values.
  if (!unicode_is_valid(codepoint) || utf8_encode_size(codepoint) != byte_cnt) {
    *consumed = byte_cnt;
    TracyCZoneEnd(__tracy_zone_ctx);
    return UNICODE_REPLACEMENT_CHAR;
  }

  *consumed = byte_cnt;
  TracyCZoneEnd(__tracy_zone_ctx);
  return codepoint;
}

func sz utf8_encode(c32 codepoint, c8* out) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out != NULL);
  sz size = utf8_encode_size(codepoint);
  if (size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (size == 1) {
    out[0] = (c8)codepoint;
  } else if (size == 2) {
    out[0] = (c8)(0xC0U | (codepoint >> 6));
    out[1] = (c8)(0x80U | (codepoint & 0x3FU));
  } else if (size == 3) {
    out[0] = (c8)(0xE0U | (codepoint >> 12));
    out[1] = (c8)(0x80U | ((codepoint >> 6) & 0x3FU));
    out[2] = (c8)(0x80U | (codepoint & 0x3FU));
  } else {
    out[0] = (c8)(0xF0U | (codepoint >> 18));
    out[1] = (c8)(0x80U | ((codepoint >> 12) & 0x3FU));
    out[2] = (c8)(0x80U | ((codepoint >> 6) & 0x3FU));
    out[3] = (c8)(0x80U | (codepoint & 0x3FU));
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return size;
}

func sz utf8_codepoint_count(cstr8 src, sz src_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz count = 0;
  sz idx = 0;
  while (idx < src_size) {
    sz consumed;
    utf8_decode(src + idx, &consumed);
    idx += consumed;
    count++;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return count;
}

// =========================================================================
// UTF-16
// =========================================================================

func sz utf16_encode_size(c32 codepoint) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!unicode_is_valid(codepoint)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return codepoint <= 0xFFFFU ? 1 : 2;
}

func c32 utf16_decode(cstr16 ptr, sz* consumed) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  c32 first = (c32)(u16)ptr[0];

  // BMP character (no surrogate).
  if (first < 0xD800U || first > 0xDFFFU) {
    *consumed = 1;
    TracyCZoneEnd(__tracy_zone_ctx);
    return first;
  }

  // Unpaired low surrogate.
  if (first >= 0xDC00U) {
    *consumed = 1;
    TracyCZoneEnd(__tracy_zone_ctx);
    return UNICODE_REPLACEMENT_CHAR;
  }

  // High surrogate: expect a low surrogate next.
  c32 second = (c32)(u16)ptr[1];
  if (second < 0xDC00U || second > 0xDFFFU) {
    *consumed = 1;
    TracyCZoneEnd(__tracy_zone_ctx);
    return UNICODE_REPLACEMENT_CHAR;
  }

  *consumed = 2;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0x10000U + ((first - 0xD800U) << 10) + (second - 0xDC00U);
}

func sz utf16_encode(c32 codepoint, c16* out) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz size = utf16_encode_size(codepoint);
  if (size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (size == 1) {
    out[0] = (c16)codepoint;
  } else {
    c32 val = codepoint - 0x10000U;
    out[0] = (c16)(0xD800U + (val >> 10));
    out[1] = (c16)(0xDC00U + (val & 0x3FFU));
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return size;
}

func sz utf16_codepoint_count(cstr16 src, sz src_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz count = 0;
  sz idx = 0;
  while (idx < src_size) {
    sz consumed;
    utf16_decode(src + idx, &consumed);
    idx += consumed;
    count++;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return count;
}

// =========================================================================
// Conversion — shared emit helper
// =========================================================================

// Writes unit_cnt units from units[] into dst at position *out_cnt, if space allows.
// When dst is NULL the write is skipped (count-only mode).
static void emit_c8(c8* dst, sz dst_cap, sz* out_cnt, cstr8 units, sz unit_cnt) {
  if (dst != NULL && *out_cnt + unit_cnt <= dst_cap) {
    for (sz idx = 0; idx < unit_cnt; idx++) {
      dst[*out_cnt + idx] = units[idx];
    }
  }
  *out_cnt += unit_cnt;
}

static void emit_c16(c16* dst, sz dst_cap, sz* out_cnt, cstr16 units, sz unit_cnt) {
  if (dst != NULL && *out_cnt + unit_cnt <= dst_cap) {
    for (sz idx = 0; idx < unit_cnt; idx++) {
      dst[*out_cnt + idx] = units[idx];
    }
  }
  *out_cnt += unit_cnt;
}

static void emit_c32(c32* dst, sz dst_cap, sz* out_cnt, c32 unit) {
  if (dst != NULL && *out_cnt < dst_cap) {
    dst[*out_cnt] = unit;
  }
  (*out_cnt)++;
}

// =========================================================================
// Conversion — UTF-8 source
// =========================================================================

func sz utf8_to_utf16(cstr8 src, sz src_size, c16* dst, sz dst_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz out_cnt = 0;
  sz idx = 0;
  while (idx < src_size) {
    sz consumed;
    c32 codepoint = utf8_decode(src + idx, &consumed);
    idx += consumed;

    c16 units[2];
    sz unit_cnt = utf16_encode(codepoint, units);
    emit_c16(dst, dst_cap, &out_cnt, units, unit_cnt);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return out_cnt;
}

func sz utf8_to_utf32(cstr8 src, sz src_size, c32* dst, sz dst_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz out_cnt = 0;
  sz idx = 0;
  while (idx < src_size) {
    sz consumed;
    c32 codepoint = utf8_decode(src + idx, &consumed);
    idx += consumed;
    emit_c32(dst, dst_cap, &out_cnt, codepoint);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return out_cnt;
}

// =========================================================================
// Conversion — UTF-16 source
// =========================================================================

func sz utf16_to_utf8(cstr16 src, sz src_size, c8* dst, sz dst_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz out_cnt = 0;
  sz idx = 0;
  while (idx < src_size) {
    sz consumed;
    c32 codepoint = utf16_decode(src + idx, &consumed);
    idx += consumed;

    c8 units[4];
    sz unit_cnt = utf8_encode(codepoint, units);
    emit_c8(dst, dst_cap, &out_cnt, units, unit_cnt);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return out_cnt;
}

func sz utf16_to_utf32(cstr16 src, sz src_size, c32* dst, sz dst_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz out_cnt = 0;
  sz idx = 0;
  while (idx < src_size) {
    sz consumed;
    c32 codepoint = utf16_decode(src + idx, &consumed);
    idx += consumed;
    emit_c32(dst, dst_cap, &out_cnt, codepoint);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return out_cnt;
}

// =========================================================================
// Conversion — UTF-32 source
// =========================================================================

func sz utf32_to_utf8(cstr32 src, sz src_size, c8* dst, sz dst_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz out_cnt = 0;
  for (sz idx = 0; idx < src_size; idx++) {
    c32 codepoint = unicode_is_valid(src[idx]) ? src[idx] : UNICODE_REPLACEMENT_CHAR;
    c8 units[4];
    sz unit_cnt = utf8_encode(codepoint, units);
    emit_c8(dst, dst_cap, &out_cnt, units, unit_cnt);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return out_cnt;
}

func sz utf32_to_utf16(cstr32 src, sz src_size, c16* dst, sz dst_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz out_cnt = 0;
  for (sz idx = 0; idx < src_size; idx++) {
    c32 codepoint = unicode_is_valid(src[idx]) ? src[idx] : UNICODE_REPLACEMENT_CHAR;
    c16 units[2];
    sz unit_cnt = utf16_encode(codepoint, units);
    emit_c16(dst, dst_cap, &out_cnt, units, unit_cnt);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return out_cnt;
}
