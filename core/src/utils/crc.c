// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/crc.h"
#include "basic/assert.h"
#include "basic/profiler.h"

func u32 crc32_init(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0xFFFFFFFFU;
}

func u32 crc32_update(u32 crc, const void* data, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (size > 0 && data == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return crc;
  }
  assert(size == 0 || data != NULL);
  const u8* bytes = (const u8*)data;
  for (sz idx = 0; idx < size; idx++) {
    crc ^= (u32)bytes[idx];
    for (u32 bit_idx = 0; bit_idx < 8U; bit_idx++) {
      if ((crc & 1U) != 0U) {
        crc = (crc >> 1U) ^ 0xEDB88320U;
      } else {
        crc >>= 1U;
      }
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return crc;
}

func u32 crc32_finalize(u32 crc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return crc ^ 0xFFFFFFFFU;
}

func u32 crc32(const void* data, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return crc32_finalize(crc32_update(crc32_init(), data, size));
}

func u64 crc64_init(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0ULL;
}

func u64 crc64_update(u64 crc, const void* data, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (size > 0 && data == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return crc;
  }
  assert(size == 0 || data != NULL);
  const u8* bytes = (const u8*)data;
  for (sz idx = 0; idx < size; idx++) {
    crc ^= (u64)bytes[idx] << 56U;
    for (u32 bit_idx = 0; bit_idx < 8U; bit_idx++) {
      if ((crc & 0x8000000000000000ULL) != 0ULL) {
        crc = (crc << 1U) ^ 0x42F0E1EBA9EA3693ULL;
      } else {
        crc <<= 1U;
      }
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return crc;
}

func u64 crc64_finalize(u64 crc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return crc;
}

func u64 crc64(const void* data, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return crc64_finalize(crc64_update(crc64_init(), data, size));
}
