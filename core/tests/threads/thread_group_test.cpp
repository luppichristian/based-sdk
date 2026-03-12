// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  i32 thread_group_entry(u32 idx, void* arg) {
    i32* results = static_cast<i32*>(arg);
    results[idx] = static_cast<i32>(idx) * 10;
    return static_cast<i32>(idx);
  }

  i32 thread_group_entry_noarg(u32 idx, void* arg) {
    (void)idx;
    (void)arg;
    return 0;
  }

}  // namespace

TEST(threads_thread_group_test, create) {
  i32 results[4] = {0, 0, 0, 0};
  ctx_setup setup = thread_get_setup();

  thread_group group = thread_group_create(4, thread_group_entry, results, setup);
  EXPECT_NE(0, thread_group_is_valid(group));
  EXPECT_EQ(4U, thread_group_get_count(group));

  thread_group_join_all(group, nullptr);
  EXPECT_NE(0, thread_group_destroy(group));
}

TEST(threads_thread_group_test, create_named) {
  i32 results[2] = {0, 0};
  ctx_setup setup = thread_get_setup();

  thread_group group = thread_group_create_named(2, thread_group_entry, results, setup, "worker");
  EXPECT_NE(0, thread_group_is_valid(group));

  thread_group_join_all(group, nullptr);
  EXPECT_NE(0, thread_group_destroy(group));
}

TEST(threads_thread_group_test, get) {
  i32 results[3] = {0, 0, 0};
  ctx_setup setup = thread_get_setup();

  thread_group group = thread_group_create(3, thread_group_entry, results, setup);

  thread thd0 = thread_group_get(group, 0);
  thread thd1 = thread_group_get(group, 1);
  thread thd2 = thread_group_get(group, 2);

  EXPECT_NE(0, thread_is_valid(thd0));
  EXPECT_NE(0, thread_is_valid(thd1));
  EXPECT_NE(0, thread_is_valid(thd2));

  thread null_thd = thread_group_get(group, 5);
  EXPECT_EQ(0, thread_is_valid(null_thd));

  thread_group_join_all(group, nullptr);
  EXPECT_NE(0, thread_group_destroy(group));
}

TEST(threads_thread_group_test, join_all) {
  i32 results[3] = {0, 0, 0};
  ctx_setup setup = thread_get_setup();

  thread_group group = thread_group_create(3, thread_group_entry, results, setup);

  i32 exit_codes[3];
  b32 result = thread_group_join_all(group, exit_codes);
  EXPECT_NE(0, result);

  EXPECT_EQ(0, exit_codes[0]);
  EXPECT_EQ(1, exit_codes[1]);
  EXPECT_EQ(2, exit_codes[2]);

  EXPECT_EQ(0, results[0]);
  EXPECT_EQ(10, results[1]);
  EXPECT_EQ(20, results[2]);
  EXPECT_NE(0, thread_group_destroy(group));
}

TEST(threads_thread_group_test, detach_all) {
  ctx_setup setup = thread_get_setup();
  thread_group group = thread_group_create(0, thread_group_entry_noarg, nullptr, setup);

  EXPECT_EQ(0, thread_group_detach_all(group));
}

TEST(threads_thread_group_test, parallel_execution) {
  constexpr u32 num_threads = 4;
  i32 counters[num_threads] = {0, 0, 0, 0};
  ctx_setup setup = thread_get_setup();

  auto entry = [](u32 idx, void* arg) -> i32 {
    i32* counters = static_cast<i32*>(arg);
    safe_for (i32 i = 0; i < 1000; i++) {
      counters[idx]++;
    }
    return 0;
  };

  thread_group group = thread_group_create(num_threads, entry, counters, setup);
  thread_group_join_all(group, nullptr);

  safe_for (u32 i = 0; i < num_threads; i++) {
    EXPECT_EQ(1000, counters[i]);
  }

  EXPECT_NE(0, thread_group_destroy(group));
}

TEST(threads_thread_group_test, destroy_null) {
  thread_group group = nullptr;
  EXPECT_EQ(0, thread_group_destroy(group));
}
