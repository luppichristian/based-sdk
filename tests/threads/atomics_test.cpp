// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct atomic_increment_ctx {
    atomic_u32* counter;
  } atomic_increment_ctx;

  func i32 atomic_increment_entry(void* arg) {
    atomic_increment_ctx* ctx = (atomic_increment_ctx*)arg;
    for (u32 idx = 0; idx < 10000; idx++) {
      atomic_u32_add(ctx->counter, 1);
    }
    return 0;
  }

}  // namespace

TEST(threads_atomics_test, i32_get_set) {
  atomic_i32 atm = {0};
  EXPECT_EQ(0, atomic_i32_get(&atm));

  i32 old = atomic_i32_set(&atm, 42);
  EXPECT_EQ(0, old);
  EXPECT_EQ(42, atomic_i32_get(&atm));

  old = atomic_i32_set(&atm, -10);
  EXPECT_EQ(42, old);
  EXPECT_EQ(-10, atomic_i32_get(&atm));
}

TEST(threads_atomics_test, i32_add) {
  atomic_i32 atm = {100};

  i32 prev = atomic_i32_add(&atm, 50);
  EXPECT_EQ(100, prev);
  EXPECT_EQ(150, atomic_i32_get(&atm));

  prev = atomic_i32_add(&atm, -75);
  EXPECT_EQ(150, prev);
  EXPECT_EQ(75, atomic_i32_get(&atm));
}

TEST(threads_atomics_test, i32_sub) {
  atomic_i32 atm = {100};

  i32 prev = atomic_i32_sub(&atm, 30);
  EXPECT_EQ(100, prev);
  EXPECT_EQ(70, atomic_i32_get(&atm));
}

TEST(threads_atomics_test, i32_cmpex_success) {
  atomic_i32 atm = {42};

  i32 expected = 42;
  b32 result = atomic_i32_cmpex(&atm, &expected, 100);
  EXPECT_NE(0, result);
  EXPECT_EQ(100, atomic_i32_get(&atm));
}

TEST(threads_atomics_test, i32_cmpex_failure) {
  atomic_i32 atm = {42};

  i32 expected = 99;
  b32 result = atomic_i32_cmpex(&atm, &expected, 100);
  EXPECT_EQ(0, result);
  EXPECT_EQ(42, expected);
  EXPECT_EQ(42, atomic_i32_get(&atm));
}

TEST(threads_atomics_test, i32_and_or_xor) {
  atomic_i32 atm = {0xFF};

  i32 prev = atomic_i32_and(&atm, 0x0F);
  EXPECT_EQ(0xFF, prev);
  EXPECT_EQ(0x0F, atomic_i32_get(&atm));

  prev = atomic_i32_or(&atm, 0xF0);
  EXPECT_EQ(0x0F, prev);
  EXPECT_EQ(0xFF, atomic_i32_get(&atm));

  prev = atomic_i32_xor(&atm, 0xFF);
  EXPECT_EQ(0xFF, prev);
  EXPECT_EQ(0x00, atomic_i32_get(&atm));
}

TEST(threads_atomics_test, i32_comparisons) {
  atomic_i32 atm = {50};

  EXPECT_NE(0, atomic_i32_eq(&atm, 50));
  EXPECT_EQ(0, atomic_i32_eq(&atm, 51));
  EXPECT_EQ(0, atomic_i32_neq(&atm, 50));
  EXPECT_NE(0, atomic_i32_neq(&atm, 51));
  EXPECT_NE(0, atomic_i32_lt(&atm, 51));
  EXPECT_EQ(0, atomic_i32_lt(&atm, 50));
  EXPECT_NE(0, atomic_i32_gt(&atm, 49));
  EXPECT_EQ(0, atomic_i32_gt(&atm, 50));
  EXPECT_NE(0, atomic_i32_lte(&atm, 50));
  EXPECT_EQ(0, atomic_i32_lte(&atm, 49));
  EXPECT_NE(0, atomic_i32_gte(&atm, 50));
  EXPECT_EQ(0, atomic_i32_gte(&atm, 51));
}

TEST(threads_atomics_test, u32_get_set) {
  atomic_u32 atm = {0};
  EXPECT_EQ(0U, atomic_u32_get(&atm));

  u32 old = atomic_u32_set(&atm, 42U);
  EXPECT_EQ(0U, old);
  EXPECT_EQ(42U, atomic_u32_get(&atm));
}

TEST(threads_atomics_test, u32_add_sub) {
  atomic_u32 atm = {100U};

  u32 prev = atomic_u32_add(&atm, 50U);
  EXPECT_EQ(100U, prev);
  EXPECT_EQ(150U, atomic_u32_get(&atm));

  prev = atomic_u32_sub(&atm, 75U);
  EXPECT_EQ(150U, prev);
  EXPECT_EQ(75U, atomic_u32_get(&atm));
}

TEST(threads_atomics_test, u32_cmpex) {
  atomic_u32 atm = {42U};

  u32 expected = 42U;
  b32 result = atomic_u32_cmpex(&atm, &expected, 100U);
  EXPECT_NE(0, result);
  EXPECT_EQ(100U, atomic_u32_get(&atm));

  expected = 99U;
  result = atomic_u32_cmpex(&atm, &expected, 50U);
  EXPECT_EQ(0, result);
}

TEST(threads_atomics_test, i64_get_set) {
  atomic_i64 atm = {0};
  EXPECT_EQ(0LL, atomic_i64_get(&atm));

  i64 old = atomic_i64_set(&atm, 12345678901234LL);
  EXPECT_EQ(0LL, old);
  EXPECT_EQ(12345678901234LL, atomic_i64_get(&atm));
}

TEST(threads_atomics_test, i64_add) {
  atomic_i64 atm = {10000000000LL};

  i64 prev = atomic_i64_add(&atm, 5000000000LL);
  EXPECT_EQ(10000000000LL, prev);
  EXPECT_EQ(15000000000LL, atomic_i64_get(&atm));
}

TEST(threads_atomics_test, i64_cmpex) {
  atomic_i64 atm = {9999999999LL};

  i64 expected = 9999999999LL;
  b32 result = atomic_i64_cmpex(&atm, &expected, 11111111111LL);
  EXPECT_NE(0, result);
  EXPECT_EQ(11111111111LL, atomic_i64_get(&atm));
}

TEST(threads_atomics_test, u64_get_set) {
  atomic_u64 atm = {0ULL};
  EXPECT_EQ(0ULL, atomic_u64_get(&atm));

  u64 old = atomic_u64_set(&atm, 9876543210ULL);
  EXPECT_EQ(0ULL, old);
  EXPECT_EQ(9876543210ULL, atomic_u64_get(&atm));
}

TEST(threads_atomics_test, u64_add_sub) {
  atomic_u64 atm = {100ULL};

  u64 prev = atomic_u64_add(&atm, 50ULL);
  EXPECT_EQ(100ULL, prev);
  EXPECT_EQ(150ULL, atomic_u64_get(&atm));

  prev = atomic_u64_sub(&atm, 75ULL);
  EXPECT_EQ(150ULL, prev);
  EXPECT_EQ(75ULL, atomic_u64_get(&atm));
}

TEST(threads_atomics_test, u64_cmpex) {
  atomic_u64 atm = {42ULL};

  u64 expected = 42ULL;
  b32 result = atomic_u64_cmpex(&atm, &expected, 100ULL);
  EXPECT_NE(0, result);
  EXPECT_EQ(100ULL, atomic_u64_get(&atm));
}

TEST(threads_atomics_test, ptr_get_set) {
  atomic_ptr atm = {nullptr};
  EXPECT_EQ(nullptr, atomic_ptr_get(&atm));

  int value = 42;
  void* ptr = &value;
  void* old = atomic_ptr_set(&atm, ptr);
  EXPECT_EQ(nullptr, old);
  EXPECT_EQ(ptr, atomic_ptr_get(&atm));
}

TEST(threads_atomics_test, ptr_cmpex) {
  int val_a = 10;
  int val_b = 20;
  atomic_ptr atm = {&val_a};

  void* expected = &val_a;
  b32 result = atomic_ptr_cmpex(&atm, &expected, &val_b);
  EXPECT_NE(0, result);
  EXPECT_EQ(&val_b, atomic_ptr_get(&atm));

  expected = &val_a;
  result = atomic_ptr_cmpex(&atm, &expected, &val_a);
  EXPECT_EQ(0, result);
}

TEST(threads_atomics_test, ptr_comparisons) {
  int value = 42;
  atomic_ptr atm = {&value};

  EXPECT_NE(0, atomic_ptr_eq(&atm, &value));
  EXPECT_EQ(0, atomic_ptr_eq(&atm, nullptr));
  EXPECT_EQ(0, atomic_ptr_neq(&atm, &value));
  EXPECT_NE(0, atomic_ptr_neq(&atm, nullptr));
}

TEST(threads_atomics_test, fence) {
  atomic_fence_acquire();
  atomic_fence_release();
  atomic_fence();
}

TEST(threads_atomics_test, pause) {
  atomic_pause();
}

TEST(threads_atomics_test, concurrent_increment) {
  atomic_u32 counter = {0};
  constexpr u32 num_threads = 4;
  constexpr u32 increments_per_thread = 10000;
  atomic_increment_ctx ctx = {&counter};
  thread threads[num_threads] = {0};

  for (u32 idx = 0; idx < num_threads; idx++) {
    threads[idx] = thread_create(atomic_increment_entry, &ctx, (allocator) {0});
    EXPECT_NE(0, thread_is_valid(threads[idx]));
  }

  for (u32 idx = 0; idx < num_threads; idx++) {
    thread_join(threads[idx], nullptr);
  }

  EXPECT_EQ(num_threads * increments_per_thread, atomic_u32_get(&counter));
}
