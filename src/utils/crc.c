// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/crc.h"

func u32 crc32_init(void) {
  return 0xFFFFFFFFU;
}

func u32 crc32_update(u32 crc, const void* data, sz size) {
  const u8* bytes = (const u8*)data;
  for (sz index = 0; index < size; index++) {
    crc ^= (u32)bytes[index];
    for (u32 bit_index = 0; bit_index < 8U; bit_index++) {
      if ((crc & 1U) != 0U) {
        crc = (crc >> 1U) ^ 0xEDB88320U;
      } else {
        crc >>= 1U;
      }
    }
  }
  return crc;
}

func u32 crc32_finalize(u32 crc) {
  return crc ^ 0xFFFFFFFFU;
}

func u32 crc32(const void* data, sz size) {
  return crc32_finalize(crc32_update(crc32_init(), data, size));
}

func u64 crc64_init(void) {
  return 0ULL;
}

func u64 crc64_update(u64 crc, const void* data, sz size) {
  const u8* bytes = (const u8*)data;
  for (sz index = 0; index < size; index++) {
    crc ^= (u64)bytes[index] << 56U;
    for (u32 bit_index = 0; bit_index < 8U; bit_index++) {
      if ((crc & 0x8000000000000000ULL) != 0ULL) {
        crc = (crc << 1U) ^ 0x42F0E1EBA9EA3693ULL;
      } else {
        crc <<= 1U;
      }
    }
  }
  return crc;
}

func u64 crc64_finalize(u64 crc) {
  return crc;
}

func u64 crc64(const void* data, sz size) {
  return crc64_finalize(crc64_update(crc64_init(), data, size));
}
