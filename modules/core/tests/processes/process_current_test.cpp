// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(processes_process_current_test, get_id) {
  u64 proc_id = process_id();
  EXPECT_NE(0U, proc_id);
}

TEST(processes_process_current_test, get_priority) {
  process_priority prio = process_get_priority();
  EXPECT_GE(prio, PROCESS_PRIORITY_LOW);
  EXPECT_LE(prio, PROCESS_PRIORITY_REALTIME);
}

TEST(processes_process_current_test, set_priority) {
  process_priority original = process_get_priority();

  b32 set_result = process_set_priority(PROCESS_PRIORITY_NORMAL);
  EXPECT_NE(0, set_result);

  process_priority after = process_get_priority();
  EXPECT_EQ(PROCESS_PRIORITY_NORMAL, after);

  process_set_priority(original);
}

TEST(processes_process_current_test, set_priority_low) {
  b32 set_result = process_set_priority(PROCESS_PRIORITY_LOW);
  EXPECT_NE(0, set_result);

  process_priority prio = process_get_priority();
  EXPECT_EQ(PROCESS_PRIORITY_LOW, prio);
}

TEST(processes_process_current_test, set_priority_high) {
  process_priority original = process_get_priority();
  b32 set_result = process_set_priority(PROCESS_PRIORITY_HIGH);
  if (set_result == 0) {
    process_priority current = process_get_priority();
    EXPECT_NE(PROCESS_PRIORITY_HIGH, current);
    process_set_priority(original);
    return;
  }
  EXPECT_NE(0, set_result);

  process_priority prio = process_get_priority();
  EXPECT_EQ(PROCESS_PRIORITY_HIGH, prio);

  process_set_priority(original);
}

TEST(processes_process_current_test, is_unique) {
  b32 unique = process_is_unique();
  EXPECT_NE(0, unique);
}
