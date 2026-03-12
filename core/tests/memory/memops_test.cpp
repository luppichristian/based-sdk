// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_memops_test, set_copy_compare_and_zero_work) {
  u32 src_words[4] = {0};
  u32 dst_words[4] = {0};

  mem_set32(src_words, 0x1234ABCDU, count_of(src_words));
  mem_cpy(dst_words, src_words, size_of(src_words));

  EXPECT_TRUE(mem_cmp(src_words, dst_words, size_of(src_words)));

  mem_zero(dst_words, size_of(dst_words));
  EXPECT_FALSE(mem_cmp(src_words, dst_words, size_of(src_words)));
}

TEST(memory_memops_test, align_helpers_match_expected_boundaries) {
  u8 storage[32] = {0};
  void* ptr = storage + 3;

  EXPECT_EQ(storage + 8, mem_align_forward(ptr, 8));
  EXPECT_EQ(storage, mem_align_backward(ptr, 8));
  EXPECT_EQ((up)(storage + 16), mem_align_forward_up((up)(storage + 9), 8));
  EXPECT_EQ((up)(storage + 8), mem_align_backward_up((up)(storage + 9), 8));
}
