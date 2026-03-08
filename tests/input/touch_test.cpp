// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_touch_test, invalid_device_queries_return_safe_defaults) {
  device_id bad_id = {};
  EXPECT_EQ(TOUCH_DEVICE_INVALID, touch_get_device_kind(bad_id));
  EXPECT_EQ(0U, touch_get_finger_count(bad_id));

  touch_finger_state out_finger = {};
  EXPECT_TRUE(touch_get_finger(bad_id, 0, &out_finger) == 0);
  EXPECT_EQ(0U, out_finger.id);
}

TEST(input_touch_test, count_and_device_id_consistency) {
  sz count_val = touch_get_count();
  EXPECT_TRUE((touch_is_available() != 0) == (count_val > 0));

  device_id out_id = {};
  if (count_val == 0) {
    EXPECT_TRUE(touch_get_device_id(0, &out_id) == 0);
  } else {
    EXPECT_TRUE(touch_get_device_id(0, &out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_TOUCH, out_id.type);
    EXPECT_TRUE(device_id_is_valid(out_id) != 0);
  }
}
