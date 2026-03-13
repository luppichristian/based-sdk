// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_mouse_test, invalid_buttons_and_state_queries_are_safe) {
  EXPECT_TRUE(mouse_is_button_down((mouse_button)-1) == 0);
  EXPECT_TRUE(mouse_is_button_down((mouse_button)9) == 0);

  mouse_state state = mouse_get_state();
  EXPECT_GE(state, 0U);
}

TEST(input_mouse_test, primary_device_query_is_consistent) {
  device out_id = mouse_get_primary_device();
  if (out_id != nullptr) {
    EXPECT_TRUE(device_is_valid(out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_MOUSE, devices_get_type(out_id));
  }
}
