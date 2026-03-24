// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_mouse_test, invalid_buttons_and_state_queries_are_safe) {
  mouse out_id = mouse_get_primary();
  if (out_id != NULL) {
    EXPECT_TRUE(mouse_is_button_down(out_id, (mouse_button)-1) == 0);
    EXPECT_TRUE(mouse_is_button_down(out_id, (mouse_button)9) == 0);

    mouse_state state = mouse_get_state(out_id);
    EXPECT_GE(state, 0U);
  }
}

TEST(input_mouse_test, primary_device_query_is_consistent) {
  mouse out_id = mouse_get_primary();
  if (out_id != NULL) {
    EXPECT_TRUE(mouse_is_valid(out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_MOUSE, devices_get_type(mouse_to_device(out_id)));
  }
}
