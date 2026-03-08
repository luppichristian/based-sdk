// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct spinlock_counter_ctx {
    spinlock lock;
    i32* counter;
  } spinlock_counter_ctx;

  func i32 spinlock_counter_entry(void* arg) {
    spinlock_counter_ctx* ctx = (spinlock_counter_ctx*)arg;
    for (i32 i = 0; i < 10000; i++) {
      spinlock_lock(ctx->lock);
      (*ctx->counter)++;
      spinlock_unlock(ctx->lock);
    }
    return 0;
  }

}  // namespace

TEST(threads_spinlock_test, create_destroy) {
  spinlock lock = spinlock_create();
  EXPECT_NE(0, spinlock_is_valid(lock));
  spinlock_destroy(lock);
}

TEST(threads_spinlock_test, lock_unlock) {
  spinlock lock = spinlock_create();

  spinlock_lock(lock);
  spinlock_unlock(lock);

  spinlock_destroy(lock);
}

TEST(threads_spinlock_test, try_lock_success) {
  spinlock lock = spinlock_create();

  b32 result = spinlock_try_lock(lock);
  EXPECT_NE(0, result);

  spinlock_unlock(lock);
  spinlock_destroy(lock);
}

TEST(threads_spinlock_test, try_lock_failure_when_locked) {
  spinlock lock = spinlock_create();

  spinlock_lock(lock);

  b32 result = spinlock_try_lock(lock);
  EXPECT_EQ(0, result);

  spinlock_unlock(lock);
  spinlock_destroy(lock);
}

TEST(threads_spinlock_test, critical_section_protection) {
  spinlock lock = spinlock_create();
  i32 counter = 0;
  constexpr i32 iterations = 10000;
  spinlock_counter_ctx ctx = {lock, &counter};

  thread thd1 = thread_create(spinlock_counter_entry, &ctx, (allocator) {0});
  thread thd2 = thread_create(spinlock_counter_entry, &ctx, (allocator) {0});
  EXPECT_NE(0, thread_is_valid(thd1));
  EXPECT_NE(0, thread_is_valid(thd2));

  thread_join(thd1, nullptr);
  thread_join(thd2, nullptr);

  EXPECT_EQ(iterations * 2, counter);
  spinlock_destroy(lock);
}
