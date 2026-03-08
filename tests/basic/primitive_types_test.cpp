// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_primitive_types_test, integer_aliases_have_expected_sizes) {
  EXPECT_EQ(size_of(i8), 1U);
  EXPECT_EQ(size_of(u8), 1U);
  EXPECT_EQ(size_of(i16), 2U);
  EXPECT_EQ(size_of(u16), 2U);
  EXPECT_EQ(size_of(i32), 4U);
  EXPECT_EQ(size_of(u32), 4U);
  EXPECT_EQ(size_of(i64), 8U);
  EXPECT_EQ(size_of(u64), 8U);
}

TEST(basic_primitive_types_test, bool_and_char_aliases_have_expected_sizes) {
  EXPECT_EQ(size_of(b8), 1U);
  EXPECT_EQ(size_of(b16), 2U);
  EXPECT_EQ(size_of(b32), 4U);
  EXPECT_EQ(size_of(b64), 8U);
  EXPECT_EQ(size_of(c8), 1U);
  EXPECT_EQ(size_of(c16), 2U);
  EXPECT_EQ(size_of(c32), 4U);
}

TEST(basic_primitive_types_test, numeric_limits_are_ordered_correctly) {
  EXPECT_LT(I8_MIN, I8_MAX);
  EXPECT_LT(I32_MIN, I32_MAX);
  EXPECT_EQ(U32_MIN, 0U);
  EXPECT_EQ(U64_MIN, 0ULL);
  EXPECT_GE(F32_MAX, 1.0F);
  EXPECT_LE(F32_MIN, -1.0F);
  EXPECT_GE(F64_MAX, 1.0);
  EXPECT_LE(F64_MIN, -1.0);
}

TEST(basic_primitive_types_test, pointer_and_size_types_match_native_width) {
  EXPECT_EQ(size_of(sz), size_of(size_t));
  EXPECT_EQ(size_of(up), size_of(uintptr_t));
  EXPECT_EQ(size_of(sp), size_of(intptr_t));
  EXPECT_EQ(size_of(dp), size_of(ptrdiff_t));
  EXPECT_EQ(SZ_MIN, 0U);
  EXPECT_EQ(UP_MIN, 0U);
}
