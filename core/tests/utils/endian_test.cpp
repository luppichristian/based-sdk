// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_endian_test, is_little_or_big) {
  b32 little = endian_is_little();
  b32 big = endian_is_big();
  EXPECT_TRUE(little || big);
  EXPECT_FALSE(little && big);
}

TEST(utils_endian_test, le16_roundtrip) {
  u16 val = 0x1234U;
  u16 nat = endian_le16_to_native(val);
  u16 back = endian_native_to_le16(nat);
  EXPECT_EQ(val, back);
}

TEST(utils_endian_test, le32_roundtrip) {
  u32 val = 0x12345678U;
  u32 nat = endian_le32_to_native(val);
  u32 back = endian_native_to_le32(nat);
  EXPECT_EQ(val, back);
}

TEST(utils_endian_test, le64_roundtrip) {
  u64 val = 0x123456789ABCDEF0ULL;
  u64 nat = endian_le64_to_native(val);
  u64 back = endian_native_to_le64(nat);
  EXPECT_EQ(val, back);
}

TEST(utils_endian_test, be16_roundtrip) {
  u16 val = 0x1234U;
  u16 nat = endian_be16_to_native(val);
  u16 back = endian_native_to_be16(nat);
  EXPECT_EQ(val, back);
}

TEST(utils_endian_test, be32_roundtrip) {
  u32 val = 0x12345678U;
  u32 nat = endian_be32_to_native(val);
  u32 back = endian_native_to_be32(nat);
  EXPECT_EQ(val, back);
}

TEST(utils_endian_test, be64_roundtrip) {
  u64 val = 0x123456789ABCDEF0ULL;
  u64 nat = endian_be64_to_native(val);
  u64 back = endian_native_to_be64(nat);
  EXPECT_EQ(val, back);
}

TEST(utils_endian_test, le16_values) {
  u16 val = 0x1234U;
  u16 nat = endian_le16_to_native(val);
  if (endian_is_little() != 0) {
    EXPECT_EQ(val, nat);
  } else {
    EXPECT_EQ(0x3412U, nat);
  }
}

TEST(utils_endian_test, be16_values) {
  u16 val = 0x1234U;
  u16 nat = endian_be16_to_native(val);
  if (endian_is_big() != 0) {
    EXPECT_EQ(val, nat);
  } else {
    EXPECT_EQ(0x3412U, nat);
  }
}
