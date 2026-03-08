// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_version_test, zero) {
  version ver = version_zero();
  EXPECT_TRUE(version_is_zero(ver) != 0);
  EXPECT_EQ(0U, version_get_packed(ver));
}

TEST(utils_version_test, from_packed) {
  version ver = version_from_packed(0x00010203);
  EXPECT_EQ(3U, version_get_major(ver));
  EXPECT_EQ(2U, version_get_minor(ver));
  EXPECT_EQ(1U, version_get_patch(ver));
}

TEST(utils_version_test, make) {
  version ver = version_make(1, 2, 3);
  EXPECT_EQ(1U, version_get_major(ver));
  EXPECT_EQ(2U, version_get_minor(ver));
  EXPECT_EQ(3U, version_get_patch(ver));
}

TEST(utils_version_test, get_packed) {
  version ver = version_make(1, 2, 3);
  u32 packed = version_get_packed(ver);
  EXPECT_EQ(0x00030201U, packed);
}

TEST(utils_version_test, string_length) {
  version ver = version_make(1, 2, 3);
  EXPECT_EQ(5U, version_string_length(ver));
}

TEST(utils_version_test, to_cstr8) {
  version ver = version_make(1, 2, 3);
  c8 buf[16];
  b32 success = version_to_cstr8(ver, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("1.2.3", buf);
}

TEST(utils_version_test, to_cstr8_small_buffer) {
  version ver = version_make(10, 20, 300);
  c8 buf[10];
  b32 success = version_to_cstr8(ver, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("10.20.300", buf);
}

TEST(utils_version_test, to_cstr8_large_version) {
  version ver = version_make(255, 255, 65535);
  c8 buf[20];
  b32 success = version_to_cstr8(ver, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("255.255.65535", buf);
}

TEST(utils_version_test, parse_cstr8) {
  version ver;
  b32 success = version_parse_cstr8("1.2.3", &ver);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(1U, version_get_major(ver));
  EXPECT_EQ(2U, version_get_minor(ver));
  EXPECT_EQ(3U, version_get_patch(ver));
}

TEST(utils_version_test, parse_cstr8_invalid) {
  version ver = version_make(0, 0, 0);
  b32 success = version_parse_cstr8("invalid", &ver);
  EXPECT_FALSE(success != 0);
}

TEST(utils_version_test, parse_cstr8_large_patch) {
  version ver;
  b32 success = version_parse_cstr8("1.2.65535", &ver);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(1U, version_get_major(ver));
  EXPECT_EQ(2U, version_get_minor(ver));
  EXPECT_EQ(65535U, version_get_patch(ver));
}

TEST(utils_version_test, cmp_equal) {
  version lhs = version_make(1, 2, 3);
  version rhs = version_make(1, 2, 3);
  EXPECT_EQ(0, version_cmp(lhs, rhs));
}

TEST(utils_version_test, cmp_major_less) {
  version lhs = version_make(1, 2, 3);
  version rhs = version_make(2, 0, 0);
  EXPECT_LT(version_cmp(lhs, rhs), 0);
}

TEST(utils_version_test, cmp_minor_less) {
  version lhs = version_make(1, 1, 3);
  version rhs = version_make(1, 2, 0);
  EXPECT_LT(version_cmp(lhs, rhs), 0);
}

TEST(utils_version_test, cmp_patch_less) {
  version lhs = version_make(1, 2, 1);
  version rhs = version_make(1, 2, 3);
  EXPECT_LT(version_cmp(lhs, rhs), 0);
}
