// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/endian.h"

#include "basic/assert.h"
#include "basic/intrinsics.h"
#include "basic/profiler.h"

func b32 endian_is_little(void) {
  u16 value = 1;
  const u8* bytes = (const u8*)&value;
  assert(bytes != NULL);
  return bytes[0] == 1U ? true : false;
}

func b32 endian_is_big(void) {
  return endian_is_little() ? false : true;
}

func u16 endian_le16_to_native(u16 value) {
  profile_func_begin;
  u16 res = endian_is_little() ? value : bswap_u16(value);
  profile_func_end;
  return res;
}

func u32 endian_le32_to_native(u32 value) {
  profile_func_begin;
  u32 res = endian_is_little() ? value : bswap_u32(value);
  profile_func_end;
  return res;
}

func u64 endian_le64_to_native(u64 value) {
  profile_func_begin;
  u64 res = endian_is_little() ? value : bswap_u64(value);
  profile_func_end;
  return res;
}

func u16 endian_be16_to_native(u16 value) {
  profile_func_begin;
  u16 res = endian_is_big() ? value : bswap_u16(value);
  profile_func_end;
  return res;
}

func u32 endian_be32_to_native(u32 value) {
  profile_func_begin;
  u32 res = endian_is_big() ? value : bswap_u32(value);
  profile_func_end;
  return res;
}

func u64 endian_be64_to_native(u64 value) {
  profile_func_begin;
  u64 res = endian_is_big() ? value : bswap_u64(value);
  profile_func_end;
  return res;
}

func u16 endian_native_to_le16(u16 value) {
  profile_func_begin;
  u16 res = endian_le16_to_native(value);
  profile_func_end;
  return res;
}

func u32 endian_native_to_le32(u32 value) {
  profile_func_begin;
  u32 res = endian_le32_to_native(value);
  profile_func_end;
  return res;
}

func u64 endian_native_to_le64(u64 value) {
  profile_func_begin;
  u64 res = endian_le64_to_native(value);
  profile_func_end;
  return res;
}

func u16 endian_native_to_be16(u16 value) {
  profile_func_begin;
  u16 res = endian_be16_to_native(value);
  profile_func_end;
  return res;
}

func u32 endian_native_to_be32(u32 value) {
  profile_func_begin;
  u32 res = endian_be32_to_native(value);
  profile_func_end;
  return res;
}

func u64 endian_native_to_be64(u64 value) {
  profile_func_begin;
  u64 res = endian_be64_to_native(value);
  profile_func_end;
  return res;
}
