// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  i32 compare_int(const void* lhs_ptr, const void* rhs_ptr, void* user_data) {
    (void)user_data;
    i32 lhs = *(const i32*)lhs_ptr;
    i32 rhs = *(const i32*)rhs_ptr;
    return lhs - rhs;
  }

  i32 compare_int_reverse(const void* lhs_ptr, const void* rhs_ptr, void* user_data) {
    (void)user_data;
    i32 lhs = *(const i32*)lhs_ptr;
    i32 rhs = *(const i32*)rhs_ptr;
    return rhs - lhs;
  }
}  // namespace

TEST(containers_sort_test, check_unsorted) {
  i32 arr[] = {3, 1, 2};
  b32 result = sort_check(arr, 3, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(0, result);
}

TEST(containers_sort_test, check_sorted) {
  i32 arr[] = {1, 2, 3};
  b32 result = sort_check(arr, 3, sizeof(i32), compare_int, nullptr);
  EXPECT_NE(0, result);
}

TEST(containers_sort_test, bubble_sort) {
  i32 arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
  sz result = sort_bubble(arr, 8, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(8U, result);

  EXPECT_EQ(1, arr[0]);
  EXPECT_EQ(1, arr[1]);
  EXPECT_EQ(2, arr[2]);
  EXPECT_EQ(3, arr[3]);
  EXPECT_EQ(4, arr[4]);
  EXPECT_EQ(5, arr[5]);
  EXPECT_EQ(6, arr[6]);
  EXPECT_EQ(9, arr[7]);
}

TEST(containers_sort_test, bubble_sort_already_sorted) {
  i32 arr[] = {1, 2, 3, 4, 5};
  sz result = sort_bubble(arr, 5, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(5U, result);
  EXPECT_EQ(1, arr[0]);
  EXPECT_EQ(5, arr[4]);
}

TEST(containers_sort_test, selection_sort) {
  i32 arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
  sz result = sort_selection(arr, 8, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(8U, result);

  EXPECT_EQ(1, arr[0]);
  EXPECT_EQ(1, arr[1]);
  EXPECT_EQ(2, arr[2]);
  EXPECT_EQ(3, arr[3]);
  EXPECT_EQ(4, arr[4]);
  EXPECT_EQ(5, arr[5]);
  EXPECT_EQ(6, arr[6]);
  EXPECT_EQ(9, arr[7]);
}

TEST(containers_sort_test, insertion_sort) {
  i32 arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
  sz result = sort_insertion(arr, 8, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(8U, result);

  EXPECT_EQ(1, arr[0]);
  EXPECT_EQ(1, arr[1]);
  EXPECT_EQ(2, arr[2]);
  EXPECT_EQ(3, arr[3]);
  EXPECT_EQ(4, arr[4]);
  EXPECT_EQ(5, arr[5]);
  EXPECT_EQ(6, arr[6]);
  EXPECT_EQ(9, arr[7]);
}

TEST(containers_sort_test, quick_sort) {
  allocator zero_alloc = {0};
  i32 arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
  sz result = sort_quick(arr, 8, sizeof(i32), compare_int, nullptr, zero_alloc);
  EXPECT_EQ(8U, result);

  EXPECT_EQ(1, arr[0]);
  EXPECT_EQ(1, arr[1]);
  EXPECT_EQ(2, arr[2]);
  EXPECT_EQ(3, arr[3]);
  EXPECT_EQ(4, arr[4]);
  EXPECT_EQ(5, arr[5]);
  EXPECT_EQ(6, arr[6]);
  EXPECT_EQ(9, arr[7]);
}

TEST(containers_sort_test, merge_sort) {
  allocator zero_alloc = {0};
  i32 arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
  sz result = sort_merge(arr, 8, sizeof(i32), compare_int, nullptr, zero_alloc);
  EXPECT_EQ(8U, result);

  EXPECT_EQ(1, arr[0]);
  EXPECT_EQ(1, arr[1]);
  EXPECT_EQ(2, arr[2]);
  EXPECT_EQ(3, arr[3]);
  EXPECT_EQ(4, arr[4]);
  EXPECT_EQ(5, arr[5]);
  EXPECT_EQ(6, arr[6]);
  EXPECT_EQ(9, arr[7]);
}

TEST(containers_sort_test, reverse_sort) {
  allocator zero_alloc = {0};
  i32 arr[] = {1, 2, 3, 4, 5};
  sz result = sort_quick(arr, 5, sizeof(i32), compare_int_reverse, nullptr, zero_alloc);
  EXPECT_EQ(5U, result);

  EXPECT_EQ(5, arr[0]);
  EXPECT_EQ(4, arr[1]);
  EXPECT_EQ(3, arr[2]);
  EXPECT_EQ(2, arr[3]);
  EXPECT_EQ(1, arr[4]);
}

TEST(containers_sort_test, single_element) {
  allocator zero_alloc = {0};
  i32 arr[] = {42};

  sz result = sort_bubble(arr, 1, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(1U, result);
  EXPECT_EQ(42, arr[0]);

  result = sort_quick(arr, 1, sizeof(i32), compare_int, nullptr, zero_alloc);
  EXPECT_EQ(1U, result);
}

TEST(containers_sort_test, empty_array) {
  allocator zero_alloc = {0};
  i32 arr[] = {0};

  sz result = sort_bubble(arr, 0, sizeof(i32), compare_int, nullptr);
  EXPECT_EQ(0U, result);

  result = sort_quick(arr, 0, sizeof(i32), compare_int, nullptr, zero_alloc);
  EXPECT_EQ(0U, result);
}

TEST(containers_sort_test, radix32) {
  u32 arr[] = {329, 457, 657, 839, 436, 720, 355};
  sz result = sort_radix32(arr, 7);
  EXPECT_EQ(7U, result);

  EXPECT_EQ(329U, arr[0]);
  EXPECT_EQ(355U, arr[1]);
  EXPECT_EQ(436U, arr[2]);
  EXPECT_EQ(457U, arr[3]);
  EXPECT_EQ(657U, arr[4]);
  EXPECT_EQ(720U, arr[5]);
  EXPECT_EQ(839U, arr[6]);
}

TEST(containers_sort_test, radix64) {
  u64 arr[] = {329ULL, 457ULL, 657ULL, 839ULL, 436ULL, 720ULL, 355ULL};
  sz result = sort_radix64(arr, 7);
  EXPECT_EQ(7U, result);

  EXPECT_EQ(329ULL, arr[0]);
  EXPECT_EQ(355ULL, arr[1]);
  EXPECT_EQ(436ULL, arr[2]);
  EXPECT_EQ(457ULL, arr[3]);
  EXPECT_EQ(657ULL, arr[4]);
  EXPECT_EQ(720ULL, arr[5]);
  EXPECT_EQ(839ULL, arr[6]);
}
