// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct thread_id_ctx {
    u64* out_id;
  } thread_id_ctx;

  func i32 thread_id_entry(void* arg) {
    thread_id_ctx* ctx = (thread_id_ctx*)arg;
    *ctx->out_id = thread_id();
    return 0;
  }

}  // namespace

TEST(threads_thread_current_test, get_id) {
  u64 thread_identifier = thread_id();
  EXPECT_NE(0ULL, thread_identifier);
}

TEST(threads_thread_current_test, id_is_unique_per_thread) {
  u64 main_id = thread_id();
  EXPECT_NE(0ULL, main_id);

  u64 other_id = 0;
  thread_id_ctx ctx = {&other_id};

  thread thd = thread_create(thread_id_entry, &ctx, (allocator) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  b32 joined = thread_join(thd, nullptr);
  EXPECT_NE(0, joined);

  EXPECT_NE(0ULL, ctx.out_id[0]);
  EXPECT_NE(main_id, ctx.out_id[0]);
}

TEST(threads_thread_current_test, sleep) {
  timestamp start = timestamp_now();
  thread_sleep(50);
  timestamp end = timestamp_now();

  timestamp elapsed = timestamp_sub(end, start);
  EXPECT_GE(timestamp_as_milliseconds(elapsed), 45.0);
}

TEST(threads_thread_current_test, yield) {
  thread_yield();
}

TEST(threads_thread_current_test, sleep_ns) {
  timestamp start = timestamp_now();
  thread_sleep_ns(50000000ULL);
  timestamp end = timestamp_now();

  timestamp elapsed = timestamp_sub(end, start);
  EXPECT_GE(timestamp_as_microseconds(elapsed), 40000LL);
}

TEST(threads_thread_current_test, get_set_priority) {
  thread_priority original = thread_get_priority();

  b32 set_result = thread_set_priority(THREAD_PRIORITY_HIGH);
  EXPECT_NE(0, set_result);

  thread_priority new_priority = thread_get_priority();
  EXPECT_EQ(THREAD_PRIORITY_HIGH, new_priority);

  thread_set_priority(original);
}
