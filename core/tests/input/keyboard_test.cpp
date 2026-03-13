// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_keyboard_test, invalid_scancodes_and_basic_queries) {
  keyboard_scancode bad_code = 0xFFFFFFFFU;
  EXPECT_TRUE(keyboard_is_key_down(bad_code) == 0);
  EXPECT_EQ(0U, keyboard_get_key_repeat_count(bad_code));

  EXPECT_TRUE(keyboard_has_mods(KEYMOD_NONE) != 0);
  cstr8 name_ptr = keyboard_get_scancode_name(4);
  EXPECT_NE(nullptr, name_ptr);

  (void)keyboard_get_keycode(4, KEYMOD_NONE, 0);
}

TEST(input_keyboard_test, primary_device_matches_availability_expectations) {
  device out_id = keyboard_get_primary_device();
  if (out_id != nullptr) {
    EXPECT_TRUE(device_is_valid(out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_KEYBOARD, devices_get_type(out_id));
  }
}
