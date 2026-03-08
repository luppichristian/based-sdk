// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_profiler_test, tracy_callstack_depth_macro_is_defined) {
  EXPECT_EQ(TRACY_CALLSTACK, 32);
}

TEST(basic_profiler_test, profiler_enable_macro_is_binary_value) {
  EXPECT_TRUE(BASED_PROFILER_ENABLED == 0 || BASED_PROFILER_ENABLED == 1);
}

TEST(basic_profiler_test, tracy_zone_macros_compile_and_execute) {
  TracyCZoneN(test_zone_ctx, "basic_profiler_test_zone", 1);
  i32 test_num = 5;
  test_num += 7;
  TracyCZoneEnd(test_zone_ctx);
  EXPECT_EQ(test_num, 12);
}
