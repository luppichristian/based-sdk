// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_utility_defines_test, string_and_concat_macros_expand_values) {
  EXPECT_EQ(cstr8_cmp(strfy(hello_world), "hello_world"), 0);
  EXPECT_EQ(cstr8_cmp(strfy_exp(1234), "1234"), 0);

  i32 cat_exp(value_, 42) = 77;
  EXPECT_EQ(value_42, 77);
}

TEST(basic_utility_defines_test, array_and_bit_helpers_work) {
  i32 vals_arr[] = {4, 8, 12, 16};
  u32 bits_val = 0U;

  EXPECT_EQ(count_of(vals_arr), 4U);
  EXPECT_EQ(size_of_each(vals_arr), size_of(i32));

  bit_set(bits_val, bit(1));
  bit_set(bits_val, bit(3));
  EXPECT_NE(bit_is_set(bits_val, bit(1)), 0);
  EXPECT_NE(bit_is_set(bits_val, bit(3)), 0);

  bit_unset(bits_val, bit(1));
  EXPECT_EQ(bit_is_set(bits_val, bit(1)), 0);

  bit_toggle(bits_val, bit(0));
  EXPECT_NE(bit_is_set(bits_val, bit(0)), 0);
}

TEST(basic_utility_defines_test, layout_range_and_bounds_helpers_work) {
  struct util_pair {
    i32 first;
    i64 second;
  };

  util_pair pair_val = {10, 20};
  i64* second_ptr = &pair_val.second;
  util_pair* pair_ptr = container_of(second_ptr, util_pair, second);
  i32 data_arr[] = {1, 2, 3};

  EXPECT_EQ(offset_of(util_pair, second), align_up(size_of(i32), align_of(i64)));
  EXPECT_EQ(size_of_field(util_pair, second), size_of(i64));
  EXPECT_EQ(pair_ptr, &pair_val);

  EXPECT_NE(in_range(5, 1, 8), 0);
  EXPECT_EQ(in_range(9, 1, 8), 0);
  EXPECT_NE(in_bounds(data_arr, 2), 0);
  EXPECT_EQ(in_bounds(data_arr, 3), 0);
}

TEST(basic_utility_defines_test, swap_alignment_and_scale_macros_work) {
  EXPECT_NE(is_pow2(8), 0);
  EXPECT_EQ(is_pow2(10), 0);
  EXPECT_EQ(align_up(33, 16), 48);
  EXPECT_EQ(align_down(33, 16), 32);
  EXPECT_EQ(kb(2), 2048LL);
  EXPECT_EQ(mb(1), 1048576LL);
  EXPECT_EQ(gb(1), 1073741824LL);
  EXPECT_EQ(th(3), 3000LL);
  EXPECT_EQ(mil(2), 2000000LL);
}
