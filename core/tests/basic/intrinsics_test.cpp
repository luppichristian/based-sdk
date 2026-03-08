// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_intrinsics_test, popcount_counts_bits_for_u32_and_u64) {
  EXPECT_EQ(popcount_u32(0xF0F00001U), 9);
  EXPECT_EQ(popcount_u64(0xFFFF00000000000FULL), 20);
}

TEST(basic_intrinsics_test, ctz_and_clz_handle_zero_and_non_zero_values) {
  EXPECT_EQ(ctz_u32(0U), 32);
  EXPECT_EQ(ctz_u32(0x10U), 4);
  EXPECT_EQ(ctz_u64(0ULL), 64);
  EXPECT_EQ(ctz_u64(0x100000000ULL), 32);

  EXPECT_EQ(clz_u32(0U), 32);
  EXPECT_EQ(clz_u32(0x10U), 27);
  EXPECT_EQ(clz_u64(0ULL), 64);
  EXPECT_EQ(clz_u64(0x100000000ULL), 31);
}

TEST(basic_intrinsics_test, bsr_reports_highest_set_bit_or_zero_for_empty_input) {
  EXPECT_EQ(bsr_u32(0U), 0);
  EXPECT_EQ(bsr_u32(0x10U), 4);
  EXPECT_EQ(bsr_u64(0ULL), 0);
  EXPECT_EQ(bsr_u64(0x8000000000000000ULL), 63);
}

TEST(basic_intrinsics_test, bswap_reverses_byte_order) {
  EXPECT_EQ(bswap_u16(0x1234U), 0x3412U);
  EXPECT_EQ(bswap_u32(0x12345678U), 0x78563412U);
  EXPECT_EQ(bswap_u64(0x1122334455667788ULL), 0x8877665544332211ULL);
}

TEST(basic_intrinsics_test, rotates_wrap_bits_across_word_size) {
  EXPECT_EQ(rotl_u32(0x80000001U, 1), 0x00000003U);
  EXPECT_EQ(rotr_u32(0x00000003U, 1), 0x80000001U);
  EXPECT_EQ(rotl_u64(0x8000000000000001ULL, 1), 0x0000000000000003ULL);
  EXPECT_EQ(rotr_u64(0x0000000000000003ULL, 1), 0x8000000000000001ULL);
}
