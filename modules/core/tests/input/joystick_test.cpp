// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(input_joystick_test, id_conversion_and_generic_conversion_roundtrip) {
  joystick joy_id = joystick_from_native_id(123);
  EXPECT_TRUE(joystick_is_valid(joy_id) != 0);
  EXPECT_EQ((up)123, joystick_to_native_id(joy_id));
  EXPECT_TRUE(joystick_is_connected(NULL) == 0);
  EXPECT_EQ(0U, joystick_get_axis_count(NULL));
  EXPECT_EQ(0U, joystick_get_ball_count(NULL));
  EXPECT_EQ(0U, joystick_get_hat_count(NULL));
  EXPECT_EQ(0U, joystick_get_button_count(NULL));
  EXPECT_EQ(0, joystick_get_axis(NULL, 0));
  EXPECT_EQ(JOYSTICK_HAT_STATE_CENTERED, joystick_get_hat(NULL, 0));
  EXPECT_TRUE(joystick_get_button(NULL, 0) == 0);

  i32 xrel = 1;
  i32 yrel = 1;
  EXPECT_TRUE(joystick_get_ball(NULL, 0, &xrel, &yrel) == 0);
  EXPECT_EQ(0, xrel);
  EXPECT_EQ(0, yrel);

  i32 battery_percent = 42;
  EXPECT_EQ(BATTERY_STATE_UNKNOWN, joystick_get_battery(NULL, &battery_percent));
  EXPECT_EQ(-1, battery_percent);

  EXPECT_EQ(BATTERY_STATE_UNKNOWN, joystick_battery_state_from_native(0));
  EXPECT_EQ(BATTERY_STATE_ERROR, joystick_battery_state_from_native(-1));
  EXPECT_EQ(4, joystick_battery_state_to_native(BATTERY_STATE_CHARGED));

  device dev_id = devices_make_id(DEVICE_TYPE_JOYSTICK, 123);
  ASSERT_TRUE(device_is_valid(dev_id) != 0);
  joystick from_dev = device_get_joystick(dev_id);
  EXPECT_TRUE(joystick_is_valid(from_dev) != 0);
  EXPECT_EQ((up)123, joystick_to_native_id(from_dev));
}

TEST(input_joystick_test, enumeration_consistency_when_devices_exist) {
  sz count_val = joystick_get_total_count();
  joystick out_id = count_val == 0 ? NULL : joystick_get_from_idx(0);
  if (count_val > 0) {
    ASSERT_TRUE(out_id != NULL);
    EXPECT_TRUE(joystick_is_valid(out_id) != 0);
  }
}
