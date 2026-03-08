// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(windowing_monitor_test, id_conversion_and_invalid_queries) {
  monitor mon_id = monitor_from_native_id(123);
  EXPECT_TRUE(monitor_id_is_valid(mon_id) != 0);
  EXPECT_EQ((up)123, monitor_to_native_id(mon_id));
  EXPECT_TRUE(monitor_id_is_valid(nullptr) == 0);

  monitor_rect out_rect = {};
  monitor_mode out_mode = {};
  EXPECT_TRUE(monitor_get_bounds(nullptr, &out_rect) == 0);
  EXPECT_TRUE(monitor_get_usable_bounds(nullptr, &out_rect) == 0);
  EXPECT_TRUE(monitor_get_mode(nullptr, 0, &out_mode) == 0);
  EXPECT_TRUE(monitor_get_current_mode(nullptr, &out_mode) == 0);
  EXPECT_TRUE(monitor_get_desktop_mode(nullptr, &out_mode) == 0);
  EXPECT_EQ(0.0F, monitor_get_refresh_rate(nullptr));
  EXPECT_EQ(1.0F, monitor_get_content_scale(nullptr));
  EXPECT_EQ(MONITOR_ORIENTATION_UNKNOWN, monitor_get_orientation(nullptr));
}

TEST(windowing_monitor_test, count_and_primary_monitor_queries) {
  sz count_val = monitor_get_count();
  monitor out_id = nullptr;
  if (count_val == 0) {
    EXPECT_TRUE(monitor_get_id(0, &out_id) == 0);
  } else {
    EXPECT_TRUE(monitor_get_id(0, &out_id) != 0);
    EXPECT_TRUE(monitor_id_is_valid(out_id) != 0);
    EXPECT_NE(nullptr, monitor_get_name(out_id));
  }

  monitor prim_id = monitor_get_primary_id();
  if (monitor_id_is_valid(prim_id) != 0) {
    EXPECT_NE(nullptr, monitor_get_name(prim_id));
  }
}
