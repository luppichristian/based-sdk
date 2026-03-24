// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_touch_test, invalid_device_queries_return_safe_defaults) {
  EXPECT_TRUE(touch_is_valid(NULL) == 0);
}

TEST(input_touch_test, count_and_device_consistency) {
  sz count_val = touch_get_total_count();
  EXPECT_TRUE((touch_is_available() != 0) == (count_val > 0));

  touch out_id = NULL;
  if (count_val == 0) {
    EXPECT_EQ(nullptr, touch_get_from_idx(0));
  } else {
    out_id = touch_get_from_idx(0);
    EXPECT_NE(nullptr, out_id);
    EXPECT_EQ(DEVICE_TYPE_TOUCH, devices_get_type(touch_to_device(out_id)));
    EXPECT_TRUE(device_is_valid(touch_to_device(out_id)) != 0);
  }
}
