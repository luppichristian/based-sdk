// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct condvar_signal_ctx {
    mutex mtx;
    condvar cond;
    atomic_i32* counter;
    atomic_u32* started;
  } condvar_signal_ctx;

  typedef struct condvar_broadcast_ctx {
    mutex mtx;
    condvar cond;
    atomic_i32* started;
    atomic_i32* woken;
  } condvar_broadcast_ctx;

  typedef struct condvar_producer_ctx {
    mutex mtx;
    condvar cond;
    i32* item;
    atomic_u32* producer_done;
  } condvar_producer_ctx;

  func i32 condvar_signal_waiter_entry(void* arg) {
    condvar_signal_ctx* ctx = (condvar_signal_ctx*)arg;
    mutex_lock(ctx->mtx);
    atomic_u32_set(ctx->started, 1);
    safe_while (atomic_i32_get(ctx->counter) < 1) {
      condvar_wait(ctx->cond, ctx->mtx);
    }
    mutex_unlock(ctx->mtx);
    return 0;
  }

  func i32 condvar_broadcast_waiter_entry(void* arg) {
    condvar_broadcast_ctx* ctx = (condvar_broadcast_ctx*)arg;
    mutex_lock(ctx->mtx);
    atomic_i32_add(ctx->started, 1);
    safe_while (atomic_i32_get(ctx->woken) == 0) {
      condvar_wait(ctx->cond, ctx->mtx);
    }
    mutex_unlock(ctx->mtx);
    return 0;
  }

  func i32 condvar_producer_entry(void* arg) {
    condvar_producer_ctx* ctx = (condvar_producer_ctx*)arg;
    safe_for (i32 i = 0; i < 10; i++) {
      mutex_lock(ctx->mtx);
      *ctx->item = i;
      condvar_signal(ctx->cond);
      safe_while (*ctx->item >= 0) {
        condvar_wait(ctx->cond, ctx->mtx);
      }
      mutex_unlock(ctx->mtx);
    }
    atomic_u32_set(ctx->producer_done, 1);
    mutex_lock(ctx->mtx);
    condvar_signal(ctx->cond);
    mutex_unlock(ctx->mtx);
    return 0;
  }

}  // namespace

TEST(threads_condvar_test, create_destroy) {
  condvar cond = condvar_create();
  EXPECT_NE(0, condvar_is_valid(cond));
  EXPECT_NE(0, condvar_destroy(cond));
}

TEST(threads_condvar_test, signal_wakes_one) {
  mutex mtx = mutex_create();
  condvar cond = condvar_create();

  atomic_i32 counter = {0};
  atomic_u32 started = {0};
  condvar_signal_ctx ctx = {mtx, cond, &counter, &started};

  thread thd = thread_create(condvar_signal_waiter_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  safe_while (atomic_u32_get(&started) == 0) {
    thread_sleep(1);
  }

  mutex_lock(mtx);
  atomic_i32_set(&counter, 1);
  condvar_signal(cond);
  mutex_unlock(mtx);

  thread_join(thd, nullptr);

  EXPECT_NE(0, condvar_destroy(cond));
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_condvar_test, broadcast_wakes_all) {
  mutex mtx = mutex_create();
  condvar cond = condvar_create();

  constexpr i32 num_waiters = 3;
  atomic_i32 started = {0};
  atomic_i32 woken = {0};
  condvar_broadcast_ctx ctx = {mtx, cond, &started, &woken};
  thread threads[num_waiters] = {0};

  safe_for (i32 i = 0; i < num_waiters; i++) {
    threads[i] = thread_create(condvar_broadcast_waiter_entry, &ctx, (ctx_setup) {0});
    EXPECT_NE(0, thread_is_valid(threads[i]));
  }

  safe_while (atomic_i32_get(&started) < num_waiters) {
    thread_sleep(1);
  }

  mutex_lock(mtx);
  atomic_i32_set(&woken, 1);
  condvar_broadcast(cond);
  mutex_unlock(mtx);

  safe_for (i32 i = 0; i < num_waiters; i++) {
    thread_join(threads[i], nullptr);
  }

  EXPECT_NE(0, condvar_destroy(cond));
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_condvar_test, wait_timeout) {
  mutex mtx = mutex_create();
  condvar cond = condvar_create();

  mutex_lock(mtx);
  b32 result = condvar_wait_timeout(cond, mtx, 50);
  mutex_unlock(mtx);

  EXPECT_EQ(0, result);

  EXPECT_NE(0, condvar_destroy(cond));
  EXPECT_NE(0, mutex_destroy(mtx));
}

TEST(threads_condvar_test, producer_consumer) {
  mutex mtx = mutex_create();
  condvar cond = condvar_create();

  i32 item = -1;
  atomic_u32 producer_done = {0};
  condvar_producer_ctx ctx = {mtx, cond, &item, &producer_done};

  thread producer = thread_create(condvar_producer_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(producer));

  i32 last_received = -1;
  safe_while (atomic_u32_get(&producer_done) == 0 || item >= 0) {
    mutex_lock(mtx);
    if (item >= 0) {
      last_received = item;
      item = -1;
      condvar_signal(cond);
    }
    mutex_unlock(mtx);
  }

  thread_join(producer, nullptr);

  EXPECT_EQ(9, last_received);

  EXPECT_NE(0, condvar_destroy(cond));
  EXPECT_NE(0, mutex_destroy(mtx));
}
