// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_crc_test, crc32_init) {
  u32 crc = crc32_init();
  EXPECT_EQ(0xFFFFFFFFU, crc);
}

TEST(utils_crc_test, crc32_update) {
  const char* data = "hello";
  u32 crc = crc32_init();
  crc = crc32_update(crc, data, 5);
  EXPECT_NE(0xFFFFFFFFU, crc);
}

TEST(utils_crc_test, crc32_finalize) {
  const char* data = "hello";
  u32 crc = crc32_init();
  crc = crc32_update(crc, data, 5);
  crc = crc32_finalize(crc);
  EXPECT_EQ(0x3610A686U, crc);
}

TEST(utils_crc_test, crc32_oneshot) {
  const char* data = "hello";
  u32 crc = crc32(data, 5);
  EXPECT_EQ(0x3610A686U, crc);
}

TEST(utils_crc_test, crc32_empty) {
  const char* data = "";
  u32 crc = crc32(data, 0);
  EXPECT_EQ(0x00000000U, crc);
}

TEST(utils_crc_test, crc64_init) {
  u64 crc = crc64_init();
  EXPECT_EQ(0ULL, crc);
}

TEST(utils_crc_test, crc64_update) {
  const char* data = "hello";
  u64 crc = crc64_init();
  crc = crc64_update(crc, data, 5);
  EXPECT_NE(0ULL, crc);
}

TEST(utils_crc_test, crc64_oneshot) {
  const char* data = "hello";
  u64 crc = crc64(data, 5);
  EXPECT_EQ(0x40544A306137B6ECULL, crc);
}

TEST(utils_crc_test, crc64_empty) {
  const char* data = "";
  u64 crc = crc64(data, 0);
  EXPECT_EQ(0x0000000000000000ULL, crc);
}
