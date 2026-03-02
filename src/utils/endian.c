// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/endian.h"

#include "basic/intrinsics.h"

func b32 endian_is_little(void) {
  u16 value = 1;
  const u8* bytes = (const u8*)&value;
  return bytes[0] == 1U ? 1 : 0;
}

func b32 endian_is_big(void) {
  return endian_is_little() ? 0 : 1;
}

func u16 endian_le16_to_native(u16 value) {
  return endian_is_little() ? value : bswap_u16(value);
}

func u32 endian_le32_to_native(u32 value) {
  return endian_is_little() ? value : bswap_u32(value);
}

func u64 endian_le64_to_native(u64 value) {
  return endian_is_little() ? value : bswap_u64(value);
}

func u16 endian_be16_to_native(u16 value) {
  return endian_is_big() ? value : bswap_u16(value);
}

func u32 endian_be32_to_native(u32 value) {
  return endian_is_big() ? value : bswap_u32(value);
}

func u64 endian_be64_to_native(u64 value) {
  return endian_is_big() ? value : bswap_u64(value);
}

func u16 endian_native_to_le16(u16 value) {
  return endian_le16_to_native(value);
}

func u32 endian_native_to_le32(u32 value) {
  return endian_le32_to_native(value);
}

func u64 endian_native_to_le64(u64 value) {
  return endian_le64_to_native(value);
}

func u16 endian_native_to_be16(u16 value) {
  return endian_be16_to_native(value);
}

func u32 endian_native_to_be32(u32 value) {
  return endian_be32_to_native(value);
}

func u64 endian_native_to_be64(u64 value) {
  return endian_be64_to_native(value);
}
