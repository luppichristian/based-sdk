// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct mutex_lock_ctx {
    mutex mtx;
    atomic_u32* ready;
    atomic_u32* done;
  } mutex_lock_ctx;

  typedef struct mutex_counter_ctx {
    mutex mtx;
    i32* counter;
    i32 iterations;
  } mutex_counter_ctx;

  func i32 mutex_lock_entry(void* arg) {
    mutex_lock_ctx* ctx = (mutex_lock_ctx*)arg;
    mutex_lock(ctx->mtx);
    atomic_u32_set(ctx->ready, 1);
    safe_while (atomic_u32_get(ctx->done) == 0) {
      thread_sleep(10);
    }
    mutex_unlock(ctx->mtx);
    return 0;
  }

  func i32 mutex_counter_entry(void* arg) {
    mutex_counter_ctx* ctx = (mutex_counter_ctx*)arg;
    safe_for (i32 i = 0; i < ctx->iterations; i++) {
      mutex_lock(ctx->mtx);
      (*ctx->counter)++;
      mutex_unlock(ctx->mtx);
    }
    return 0;
  }

}  // namespace

TEST(threads_mutex_test, create_destroy) {
  mutex mtx = mutex_create();
  EXPECT_NE(0, mutex_is_valid(mtx));
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_mutex_test, lock_unlock) {
  mutex mtx = mutex_create();

  mutex_lock(mtx);
  mutex_unlock(mtx);

  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_mutex_test, trylock) {
  mutex mtx = mutex_create();

  b32 result = mutex_trylock(mtx);
  EXPECT_NE(0, result);

  mutex_unlock(mtx);
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_mutex_test, trylock_when_locked) {
  mutex mtx = mutex_create();

  mutex_lock(mtx);

  b32 result = mutex_trylock(mtx);
  EXPECT_NE(0, result);

  mutex_unlock(mtx);

  mutex_unlock(mtx);
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_mutex_test, timed_lock) {
  mutex mtx = mutex_create();

  b32 result = mutex_timed_lock(mtx, 1000);
  EXPECT_NE(0, result);

  mutex_unlock(mtx);
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_mutex_test, recursive_lock_by_different_thread) {
  mutex mtx = mutex_create();
  atomic_u32 ready = {0};
  atomic_u32 done = {0};
  mutex_lock_ctx ctx = {mtx, &ready, &done};

  thread thd = thread_create(mutex_lock_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  safe_while (atomic_u32_get(&ready) == 0) {
    thread_sleep(1);
  }

  b32 result = mutex_trylock(mtx);
  EXPECT_EQ(0, result);

  atomic_u32_set(&done, 1);
  thread_join(thd, NULL);

  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_mutex_test, critical_section_protection) {
  mutex mtx = mutex_create();
  i32 counter = 0;
  constexpr i32 iterations = 5000;
  mutex_counter_ctx ctx = {mtx, &counter, iterations};

  thread thd1 = thread_create(mutex_counter_entry, &ctx, (ctx_setup) {0});
  thread thd2 = thread_create(mutex_counter_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd1));
  EXPECT_NE(0, thread_is_valid(thd2));

  thread_join(thd1, NULL);
  thread_join(thd2, NULL);

  EXPECT_EQ(iterations * 2, counter);
  EXPECT_NE(0, mutex_destroy(mtx));
}
