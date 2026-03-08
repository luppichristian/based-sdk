// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_mouse_test, invalid_buttons_and_state_queries_are_safe) {
  EXPECT_TRUE(mouse_is_button_down(INPUT_KEY_DEFAULT, 0) == 0);
  EXPECT_TRUE(mouse_is_button_pressed(INPUT_KEY_DEFAULT, 0) == 0);
  EXPECT_TRUE(mouse_is_button_released(INPUT_KEY_DEFAULT, 0) == 0);
  EXPECT_TRUE(mouse_is_button_down(INPUT_KEY_DEFAULT, 9) == 0);

  mouse_state local_state = mouse_get_state(INPUT_KEY_DEFAULT);
  mouse_state glob_state = mouse_get_global_state(INPUT_KEY_DEFAULT);
  mouse_state rel_state = mouse_get_relative_state(INPUT_KEY_DEFAULT);
  EXPECT_GE(local_state.button_mask, 0U);
  EXPECT_GE(glob_state.button_mask, 0U);
  EXPECT_GE(rel_state.button_mask, 0U);
}

TEST(input_mouse_test, primary_device_id_query_is_consistent) {
  device_id out_id = {};
  b32 has_primary = mouse_get_primary_device_id(&out_id);
  if (has_primary != 0) {
    EXPECT_TRUE(device_id_is_valid(out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_MOUSE, out_id.type);
  }
}
