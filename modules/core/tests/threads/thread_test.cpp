// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct thread_detach_ctx {
    atomic_u32* done;
  } thread_detach_ctx;

  func i32 thread_entry_simple(void* arg) {
    i32* val = static_cast<i32*>(arg);
    if (val != NULL) {
      *val = 42;
    }
    return 0;
  }

  func i32 thread_entry_sleep(void* arg) {
    (void)arg;
    thread_sleep(50);
    return 123;
  }

  func i32 thread_entry_detach(void* arg) {
    thread_detach_ctx* ctx = (thread_detach_ctx*)arg;
    if (ctx != NULL && ctx->done != NULL) {
      atomic_u32_set(ctx->done, 1);
    }
    return 0;
  }

}  // namespace

TEST(threads_thread_test, create_join) {
  i32 result = 0;

  thread thd = thread_create(thread_entry_simple, &result, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  i32 exit_code = 0;
  b32 join_result = thread_join(thd, &exit_code);
  EXPECT_NE(0, join_result);
  EXPECT_EQ(0, exit_code);
  EXPECT_EQ(42, result);
}

TEST(threads_thread_test, create_named) {
#if defined(_WIN32)
  GTEST_SKIP() << "thread naming is unstable on this target";
#endif
  thread thd = thread_create_named(thread_entry_simple, NULL, "test_thread", (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  cstr8 name = thread_get_name(thd);
  EXPECT_NE(nullptr, name);

  thread_join(thd, NULL);
}

TEST(threads_thread_test, get_id) {
  thread thd = thread_create(thread_entry_simple, NULL, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  u64 thread_identifier = 0;
  safe_for (i32 idx = 0; idx < 100 && thread_identifier == 0; idx++) {
    thread_identifier = thread_get_id(thd);
    if (thread_identifier == 0) {
      thread_sleep(1);
    }
  }
  EXPECT_NE(0ULL, thread_identifier);

  thread_join(thd, NULL);
}

TEST(threads_thread_test, detach) {
  atomic_u32 done = {0};
  thread_detach_ctx ctx = {&done};

  thread thd = thread_create(thread_entry_detach, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(thd));

  thread_detach(thd);

  safe_for (i32 idx = 0; idx < 500 && atomic_u32_get(&done) == 0; idx++) {
    thread_sleep(1);
  }

  EXPECT_NE(0U, atomic_u32_get(&done));
}

TEST(threads_thread_test, multiple_threads) {
  constexpr i32 num_threads = 4;
  i32 results[num_threads] = {0, 0, 0, 0};

  thread threads[num_threads];
  safe_for (i32 i = 0; i < num_threads; i++) {
    threads[i] = thread_create(thread_entry_simple, &results[i], (ctx_setup) {0});
    EXPECT_NE(0, thread_is_valid(threads[i]));
  }

  safe_for (i32 i = 0; i < num_threads; i++) {
    thread_join(threads[i], NULL);
  }

  safe_for (i32 i = 0; i < num_threads; i++) {
    EXPECT_EQ(42, results[i]);
  }
}

TEST(threads_thread_test, exit_code) {
  thread thd = thread_create(thread_entry_sleep, NULL, (ctx_setup) {0});

  i32 exit_code = -1;
  thread_join(thd, &exit_code);

  EXPECT_EQ(123, exit_code);
}

TEST(threads_thread_test, parallel_execution) {
  constexpr i32 num_threads = 4;
  constexpr i32 iterations = 1000;
  i32 counters[num_threads] = {0};

  auto entry = [](void* arg) -> i32 {
    i32* counter = static_cast<i32*>(arg);
    safe_for (i32 i = 0; i < iterations; i++) {
      (*counter)++;
    }
    return 0;
  };

  thread threads[num_threads];
  safe_for (i32 i = 0; i < num_threads; i++) {
    threads[i] = thread_create(entry, &counters[i], (ctx_setup) {0});
  }

  safe_for (i32 i = 0; i < num_threads; i++) {
    thread_join(threads[i], NULL);
  }

  safe_for (i32 i = 0; i < num_threads; i++) {
    EXPECT_EQ(iterations, counters[i]);
  }
}
