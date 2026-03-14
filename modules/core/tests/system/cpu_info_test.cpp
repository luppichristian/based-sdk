// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(system_cpu_info_test, query) {
  cpu_info info = {0};
  b32 result = cpu_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(info.logical_core_count, 0U);
  EXPECT_GT(info.cache_line_bytes, 0U);
}

TEST(system_cpu_info_test, vendor_name) {
  cpu_info info = {0};
  b32 result = cpu_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.vendor_name), 0U);
}

TEST(system_cpu_info_test, brand_name) {
  cpu_info info = {0};
  b32 result = cpu_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.brand_name), 0U);
}

TEST(system_cpu_info_test, instruction_sets) {
  cpu_info info = {0};
  b32 result = cpu_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GE(info.instruction_sets.mmx, 0);
  EXPECT_GE(info.instruction_sets.sse, 0);
  EXPECT_GE(info.instruction_sets.sse2, 0);
  EXPECT_GE(info.instruction_sets.popcnt, 0);
}
