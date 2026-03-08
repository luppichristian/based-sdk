// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  global_var i32 keyword_global_value = 17;
}  // namespace

const_var i32 KEYWORD_CONST_VALUE = 41;

TEST(basic_keyword_defines_test, align_macros_match_storage_layout) {
  struct align_as(32) aligned_data {
    u8 bytes[32];
  };

  aligned_data data_obj = {{0}};
  up addr_num = (up)&data_obj;

  EXPECT_EQ(align_of(aligned_data), 32U);
  EXPECT_EQ(addr_num % 32U, 0U);
}

TEST(basic_keyword_defines_test, likely_and_unlikely_preserve_boolean_value) {
  i32 eval_one = 3;
  i32 eval_two = 0;

  EXPECT_NE(likely(eval_one > 1), 0);
  EXPECT_EQ(unlikely(eval_two > 1), 0);
}

TEST(basic_keyword_defines_test, size_of_and_static_assert_are_usable) {
  static_assert(size_of(u64) == 8);
  EXPECT_EQ(size_of(i16), 2U);
  EXPECT_EQ(size_of(f64), 8U);
}

TEST(basic_keyword_defines_test, cosmetic_keywords_expand_to_valid_storage) {
  auto counter_fn = []() -> i32 {
    local_persist i32 counter_num = 0;
    counter_num += 1;
    return counter_num;
  };

  EXPECT_EQ(counter_fn(), 1);
  EXPECT_EQ(counter_fn(), 2);
  EXPECT_EQ(keyword_global_value, 17);
  EXPECT_EQ(KEYWORD_CONST_VALUE, 41);
}
