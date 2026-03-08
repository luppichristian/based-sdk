// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct semaphore_wait_ctx {
    semaphore sem;
  } semaphore_wait_ctx;

  func i32 semaphore_wait_entry(void* arg) {
    semaphore_wait_ctx* ctx = (semaphore_wait_ctx*)arg;
    semaphore_wait(ctx->sem);
    return 0;
  }

}  // namespace

TEST(threads_semaphore_test, create_destroy) {
  semaphore sem = semaphore_create(1);
  EXPECT_NE(0, semaphore_is_valid(sem));
  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, initial_count) {
  semaphore sem = semaphore_create(2);

  b32 result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  result = semaphore_try_wait(sem);
  EXPECT_EQ(0, result);

  semaphore_signal(sem);

  result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, wait_and_signal) {
  semaphore sem = semaphore_create(0);
  semaphore_wait_ctx ctx = {sem};

  thread thd = thread_create(semaphore_wait_entry, &ctx, (allocator) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  semaphore_signal(sem);
  thread_join(thd, nullptr);

  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, try_wait_success) {
  semaphore sem = semaphore_create(1);

  b32 result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, try_wait_failure) {
  semaphore sem = semaphore_create(0);

  b32 result = semaphore_try_wait(sem);
  EXPECT_EQ(0, result);

  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, wait_timeout_success) {
  semaphore sem = semaphore_create(1);

  b32 result = semaphore_wait_timeout(sem, 1000);
  EXPECT_NE(0, result);

  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, wait_timeout_failure) {
  semaphore sem = semaphore_create(0);

  b32 result = semaphore_wait_timeout(sem, 50);
  EXPECT_EQ(0, result);

  EXPECT_NE(0, semaphore_destroy(sem));
}

TEST(threads_semaphore_test, multiple_signals) {
  semaphore sem = semaphore_create(0);

  semaphore_signal(sem);
  semaphore_signal(sem);
  semaphore_signal(sem);

  b32 result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  result = semaphore_try_wait(sem);
  EXPECT_NE(0, result);

  result = semaphore_try_wait(sem);
  EXPECT_EQ(0, result);

  EXPECT_NE(0, semaphore_destroy(sem));
}
