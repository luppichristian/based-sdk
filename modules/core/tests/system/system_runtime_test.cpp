// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(system_system_runtime_test, query) {
  system_runtime_info info = {0};
  b32 result = system_runtime_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GE(info.memory_total, 0U);
  EXPECT_GE(info.memory_used, 0U);
  EXPECT_GE(info.memory_available, 0U);
}

TEST(system_system_runtime_test, memory_values) {
  system_runtime_info info = {0};
  b32 result = system_runtime_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GE(info.memory_total, info.memory_used);
  EXPECT_GE(info.memory_total, info.memory_available);
}

TEST(system_system_runtime_test, process_memory) {
  system_runtime_info info = {0};
  b32 result = system_runtime_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GE(info.process_memory_used, 0U);
  EXPECT_GE(info.process_memory_peak, 0U);
  EXPECT_GE(info.process_memory_peak, info.process_memory_used);
}

TEST(system_system_runtime_test, cpu_usage_initial) {
  system_runtime_info info = {0};
  b32 result = system_runtime_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GE(info.cpu_usage_percent, 0.0F);
  EXPECT_LE(info.cpu_usage_percent, 100.0F);
}

TEST(system_system_runtime_test, cpu_usage_delta) {
  system_runtime_info info1 = {0};
  b32 result1 = system_runtime_query(&info1);
  EXPECT_NE(0, result1);

  volatile i64 sum = 0;
  safe_for (i32 i = 0; i < 5000; i++) {
    sum += i;
  }

  system_runtime_info info2 = {0};
  b32 result2 = system_runtime_query(&info2);
  EXPECT_NE(0, result2);

  EXPECT_GE(info2.cpu_usage_percent, 0.0F);
}
