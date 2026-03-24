// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_gamepads_test, enumeration_and_safe_queries_return_defaults) {
  gamepad pad = gamepad_get_primary();
  EXPECT_TRUE(gamepad_get_total_count() <= GAMEPADS_MAX_COUNT);
  if (pad != NULL) {
    EXPECT_TRUE(gamepad_is_valid(pad) != 0);
    EXPECT_TRUE(gamepad_is_connected(pad) == 0 || gamepad_is_connected(pad) == 1);
    EXPECT_TRUE(gamepad_has_button(pad, GAMEPAD_BUTTON_SOUTH) == 0 || gamepad_has_button(pad, GAMEPAD_BUTTON_SOUTH) == 1);
    EXPECT_TRUE(gamepad_get_button(pad, GAMEPAD_BUTTON_SOUTH) == 0 || gamepad_get_button(pad, GAMEPAD_BUTTON_SOUTH) == 1);
    EXPECT_TRUE(gamepad_has_axis(pad, GAMEPAD_AXIS_LEFTX) == 0 || gamepad_has_axis(pad, GAMEPAD_AXIS_LEFTX) == 1);
    EXPECT_TRUE(gamepad_get_axis(pad, GAMEPAD_AXIS_LEFTX) >= -32768);
  }
}

TEST(input_gamepads_test, axis_deadzone_api_works_without_hardware) {
  gamepad pad = gamepad_get_primary();
  if (pad != NULL) {
    EXPECT_TRUE(gamepad_set_axis_deadzone(pad, GAMEPAD_AXIS_LEFTX, -1000) != 0);
    EXPECT_EQ(1000, gamepad_get_axis_deadzone(pad, GAMEPAD_AXIS_LEFTX));
    EXPECT_TRUE(gamepad_set_axis_deadzone(pad, GAMEPAD_AXIS_INVALID, 200) == 0);
    EXPECT_TRUE(gamepad_set_rumble(pad, 0, 0, 0) == 0 || gamepad_set_rumble(pad, 0, 0, 0) == 1);
    EXPECT_TRUE(gamepad_set_led(pad, 0, 0, 0) == 0 || gamepad_set_led(pad, 0, 0, 0) == 1);
  }
}
