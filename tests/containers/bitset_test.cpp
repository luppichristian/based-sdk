// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

// NOLINTBEGIN(readability-identifier-naming)

TEST(containers_bitset_test, set_clear_toggle) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);

  BITSET_SET(arr, 0);
  EXPECT_EQ(1ULL, arr[0]);
  EXPECT_EQ(0ULL, arr[1]);

  BITSET_SET(arr, 63);
  EXPECT_EQ(0x8000000000000001ULL, arr[0]);

  BITSET_CLEAR(arr, 0);
  EXPECT_EQ(0x8000000000000000ULL, arr[0]);

  BITSET_TOGGLE(arr, 63);
  EXPECT_EQ(0ULL, arr[0]);

  BITSET_SET(arr, 64);
  EXPECT_EQ(1ULL, arr[1]);

  BITSET_CLEAR_ALL(arr, word_count);
  EXPECT_EQ(0ULL, arr[0]);
  EXPECT_EQ(0ULL, arr[1]);
}

TEST(containers_bitset_test, test_macro) {
  u64 arr[2] = {0};

  BITSET_SET(arr, 0);
  EXPECT_NE(0ULL, BITSET_TEST(arr, 0));
  EXPECT_EQ(0ULL, BITSET_TEST(arr, 1));

  BITSET_SET(arr, 63);
  EXPECT_NE(0ULL, BITSET_TEST(arr, 63));

  BITSET_SET(arr, 64);
  EXPECT_NE(0ULL, BITSET_TEST(arr, 64));
}

TEST(containers_bitset_test, set_all_clear_all) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);

  BITSET_SET_ALL(arr, word_count);
  EXPECT_EQ(U64_MAX, arr[0]);
  EXPECT_EQ(U64_MAX, arr[1]);

  BITSET_CLEAR_ALL(arr, word_count);
  EXPECT_EQ(0ULL, arr[0]);
  EXPECT_EQ(0ULL, arr[1]);
}

TEST(containers_bitset_test, count) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);
  sz count = 0;

  BITSET_COUNT(arr, word_count, count);
  EXPECT_EQ(0U, count);

  BITSET_SET(arr, 0);
  BITSET_SET(arr, 10);
  BITSET_SET(arr, 63);
  BITSET_SET(arr, 64);

  BITSET_COUNT(arr, word_count, count);
  EXPECT_EQ(4U, count);

  BITSET_SET_ALL(arr, word_count);
  BITSET_COUNT(arr, word_count, count);
  EXPECT_EQ(128U, count);
}

TEST(containers_bitset_test, first_set) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);
  i32 idx = -1;

  BITSET_FIRST_SET(arr, word_count, idx);
  EXPECT_EQ(-1, idx);

  BITSET_SET(arr, 42);
  BITSET_FIRST_SET(arr, word_count, idx);
  EXPECT_EQ(42, idx);

  BITSET_CLEAR_ALL(arr, word_count);
  BITSET_SET(arr, 64);
  BITSET_FIRST_SET(arr, word_count, idx);
  EXPECT_EQ(64, idx);
}

TEST(containers_bitset_test, first_clear) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);
  i32 idx = -1;

  BITSET_SET_ALL(arr, word_count);
  BITSET_FIRST_CLEAR(arr, word_count, idx);
  EXPECT_EQ(-1, idx);

  BITSET_CLEAR(arr, 10);
  BITSET_FIRST_CLEAR(arr, word_count, idx);
  EXPECT_EQ(10, idx);
}

TEST(containers_bitset_test, find_next_set) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);

  EXPECT_EQ(-1, bitset_find_next_set(arr, word_count, 0));

  BITSET_SET(arr, 5);
  BITSET_SET(arr, 10);
  BITSET_SET(arr, 50);

  EXPECT_EQ(5, bitset_find_next_set(arr, word_count, 0));
  EXPECT_EQ(5, bitset_find_next_set(arr, word_count, 5));
  EXPECT_EQ(10, bitset_find_next_set(arr, word_count, 10));
  EXPECT_EQ(50, bitset_find_next_set(arr, word_count, 50));

  EXPECT_EQ(-1, bitset_find_next_set(nullptr, word_count, 0));
}

TEST(containers_bitset_test, foreach_set) {
  u64 arr[2] = {0};
  sz word_count = BITSET_WORD_COUNT(100);
  i32 idx = 0;
  sz count = 0;

  BITSET_SET(arr, 0);
  BITSET_SET(arr, 5);
  BITSET_SET(arr, 10);
  BITSET_SET(arr, 64);

  BITSET_FOREACH_SET(arr, word_count, idx) {
    count++;
  }

  EXPECT_EQ(4U, count);
}

// NOLINTEND(readability-identifier-naming)
