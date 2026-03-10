// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_digits_test, unsigned_exact_width) {
  EXPECT_EQ(1U, u8_digits(0));
  EXPECT_EQ(3U, u8_digits(255));
  EXPECT_EQ(1U, u16_digits(7));
  EXPECT_EQ(5U, u16_digits(65535));
  EXPECT_EQ(10U, u32_digits(U32_MAX));
  EXPECT_EQ(20U, u64_digits(U64_MAX));
}

TEST(utils_digits_test, signed_exact_width) {
  EXPECT_EQ(1U, i8_digits(0));
  EXPECT_EQ(2U, i8_digits(-1));
  EXPECT_EQ(4U, i8_digits(I8_MIN));
  EXPECT_EQ(6U, i16_digits(I16_MIN));
  EXPECT_EQ(11U, i32_digits(I32_MIN));
  EXPECT_EQ(20U, i64_digits(I64_MIN));
}

TEST(utils_digits_test, fast_width_unsigned) {
  EXPECT_EQ(1U, u8x_digits((u8x)0));
  EXPECT_EQ(1U, u16x_digits((u16x)9));
  EXPECT_EQ(2U, u32x_digits((u32x)10));
  EXPECT_EQ(3U, u64x_digits((u64x)999));
}

TEST(utils_digits_test, fast_width_signed) {
  EXPECT_EQ(1U, i8x_digits((i8x)0));
  EXPECT_EQ(2U, i16x_digits((i16x)-1));
  EXPECT_EQ(3U, i32x_digits((i32x)-10));
  EXPECT_EQ(4U, i64x_digits((i64x)-999));
}

TEST(utils_digits_test, platform_width_unsigned) {
  EXPECT_EQ(1U, sz_digits((sz)0));
  EXPECT_EQ(1U, up_digits((up)7));
  EXPECT_EQ(5U, sz_digits((sz)12345));
  EXPECT_EQ(5U, up_digits((up)99999));
}

TEST(utils_digits_test, platform_width_signed) {
  EXPECT_EQ(1U, sp_digits((sp)0));
  EXPECT_EQ(2U, dp_digits((dp)-1));
  EXPECT_EQ(6U, sp_digits((sp)-12345));
  EXPECT_EQ(6U, dp_digits((dp)-99999));
}
