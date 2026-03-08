// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_id_test, id8_zero) {
  id8 value = id8_zero();
  EXPECT_TRUE(id8_is_zero(value) != 0);
  EXPECT_EQ(0U, id8_get(value));
}

TEST(utils_id_test, id8_make) {
  id8 value = id8_make(42);
  EXPECT_FALSE(id8_is_zero(value) != 0);
  EXPECT_EQ(42U, id8_get(value));
  EXPECT_TRUE(id8_is_valid(value) != 0);
}

TEST(utils_id_test, id8_next) {
  id8 value = id8_make(254);
  id8 nxt = id8_next(value);
  EXPECT_EQ(255U, id8_get(nxt));
  id8 wrap = id8_next(nxt);
  EXPECT_EQ(0U, id8_get(wrap));
}

TEST(utils_id_test, id8_cmp_equal) {
  id8 lhs = id8_make(1);
  id8 rhs = id8_make(1);
  EXPECT_EQ(0, id8_cmp(lhs, rhs));
}

TEST(utils_id_test, id8_cmp_less) {
  id8 lhs = id8_make(1);
  id8 rhs = id8_make(2);
  EXPECT_LT(id8_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id8_cmp_greater) {
  id8 lhs = id8_make(2);
  id8 rhs = id8_make(1);
  EXPECT_GT(id8_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id8_string) {
  id8 value = id8_make(123);
  EXPECT_EQ(3U, id8_string_length(value));
  c8 buf[4];
  b32 success = id8_to_cstr8(value, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("123", buf);
}

TEST(utils_id_test, id8_parse) {
  id8 value;
  b32 success = id8_parse_cstr8("42", &value);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(42U, id8_get(value));
}

TEST(utils_id_test, id16_zero) {
  id16 value = id16_zero();
  EXPECT_TRUE(id16_is_zero(value) != 0);
  EXPECT_EQ(0U, id16_get(value));
}

TEST(utils_id_test, id16_make) {
  id16 value = id16_make(1000);
  EXPECT_FALSE(id16_is_zero(value) != 0);
  EXPECT_EQ(1000U, id16_get(value));
  EXPECT_TRUE(id16_is_valid(value) != 0);
}

TEST(utils_id_test, id16_cmp_equal) {
  id16 lhs = id16_make(1);
  id16 rhs = id16_make(1);
  EXPECT_EQ(0, id16_cmp(lhs, rhs));
}

TEST(utils_id_test, id16_cmp_less) {
  id16 lhs = id16_make(1);
  id16 rhs = id16_make(2);
  EXPECT_LT(id16_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id16_cmp_greater) {
  id16 lhs = id16_make(2);
  id16 rhs = id16_make(1);
  EXPECT_GT(id16_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id16_string) {
  id16 value = id16_make(12345);
  EXPECT_EQ(5U, id16_string_length(value));
  c8 buf[6];
  b32 success = id16_to_cstr8(value, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("12345", buf);
}

TEST(utils_id_test, id32_zero) {
  id32 value = id32_zero();
  EXPECT_TRUE(id32_is_zero(value) != 0);
  EXPECT_EQ(0U, id32_get(value));
}

TEST(utils_id_test, id32_make) {
  id32 value = id32_make(100000);
  EXPECT_FALSE(id32_is_zero(value) != 0);
  EXPECT_EQ(100000U, id32_get(value));
}

TEST(utils_id_test, id32_cmp_equal) {
  id32 lhs = id32_make(1);
  id32 rhs = id32_make(1);
  EXPECT_EQ(0, id32_cmp(lhs, rhs));
}

TEST(utils_id_test, id32_cmp_less) {
  id32 lhs = id32_make(1);
  id32 rhs = id32_make(2);
  EXPECT_LT(id32_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id32_cmp_greater) {
  id32 lhs = id32_make(2);
  id32 rhs = id32_make(1);
  EXPECT_GT(id32_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id32_string) {
  id32 value = id32_make(1234567);
  c8 buf[8];
  b32 success = id32_to_cstr8(value, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("1234567", buf);
}

TEST(utils_id_test, id64_zero) {
  id64 value = id64_zero();
  EXPECT_TRUE(id64_is_zero(value) != 0);
  EXPECT_EQ(0ULL, id64_get(value));
}

TEST(utils_id_test, id64_make) {
  id64 value = id64_make(10000000000ULL);
  EXPECT_FALSE(id64_is_zero(value) != 0);
  EXPECT_EQ(10000000000ULL, id64_get(value));
}

TEST(utils_id_test, id64_cmp_equal) {
  id64 lhs = id64_make(1);
  id64 rhs = id64_make(1);
  EXPECT_EQ(0, id64_cmp(lhs, rhs));
}

TEST(utils_id_test, id64_cmp_less) {
  id64 lhs = id64_make(1);
  id64 rhs = id64_make(2);
  EXPECT_LT(id64_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id64_cmp_greater) {
  id64 lhs = id64_make(2);
  id64 rhs = id64_make(1);
  EXPECT_GT(id64_cmp(lhs, rhs), 0);
}

TEST(utils_id_test, id64_string) {
  id64 value = id64_make(12345678901ULL);
  c8 buf[12];
  b32 success = id64_to_cstr8(value, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("12345678901", buf);
}
