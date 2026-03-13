// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_gamepads_test, invalid_slots_and_buttons_return_safe_defaults) {
  EXPECT_TRUE(gamepads_is_connected(GAMEPADS_MAX_COUNT) == 0);
  EXPECT_EQ(nullptr, gamepads_get_name(GAMEPADS_MAX_COUNT));

  EXPECT_EQ(nullptr, gamepads_get_device(GAMEPADS_MAX_COUNT));
  EXPECT_TRUE(gamepads_has_button(GAMEPADS_MAX_COUNT, GAMEPAD_BUTTON_SOUTH) == 0);
  EXPECT_TRUE(gamepads_get_button(GAMEPADS_MAX_COUNT, GAMEPAD_BUTTON_SOUTH) == 0);
  EXPECT_TRUE(gamepads_has_axis(GAMEPADS_MAX_COUNT, GAMEPAD_AXIS_LEFTX) == 0);
  EXPECT_EQ(0, gamepads_get_axis(GAMEPADS_MAX_COUNT, GAMEPAD_AXIS_LEFTX));
}

TEST(input_gamepads_test, axis_deadzone_api_works_without_hardware) {
  EXPECT_TRUE(gamepads_set_axis_deadzone(0, GAMEPAD_AXIS_LEFTX, -1000) != 0);
  EXPECT_EQ(1000, gamepads_get_axis_deadzone(0, GAMEPAD_AXIS_LEFTX));

  EXPECT_TRUE(gamepads_set_axis_deadzone(GAMEPADS_MAX_COUNT, GAMEPAD_AXIS_LEFTX, 200) == 0);
  EXPECT_TRUE(gamepads_set_axis_deadzone(0, GAMEPAD_AXIS_INVALID, 200) == 0);
  EXPECT_EQ(0, gamepads_get_axis_deadzone(GAMEPADS_MAX_COUNT, GAMEPAD_AXIS_LEFTX));

  EXPECT_TRUE(gamepads_set_rumble(GAMEPADS_MAX_COUNT, 0, 0, 0) == 0);
  EXPECT_TRUE(gamepads_set_led(GAMEPADS_MAX_COUNT, 0, 0, 0) == 0);
  EXPECT_TRUE(gamepads_get_count() <= GAMEPADS_MAX_COUNT);
}
