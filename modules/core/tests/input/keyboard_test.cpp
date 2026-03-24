// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_keyboard_test, invalid_keys_and_basic_queries) {
  keyboard keyb = keyboard_get_primary();
  if (keyb != NULL) {
    EXPECT_TRUE(keyboard_is_key_down(keyb, VKEY_UNKNOWN) == 0);
    EXPECT_TRUE(keyboard_has_mods(keyb, KEYMOD_NONE) != 0);
    cstr8 name_ptr = keyboard_get_key_name(keyb, VKEY_A);
    EXPECT_NE(nullptr, name_ptr);
    EXPECT_NE(nullptr, keyboard_get_key_name(keyb, VKEY_A));
    EXPECT_NE(nullptr, keyboard_get_key_display_name(keyb, VKEY_A, KEYMOD_NONE, 0));
  }

  EXPECT_TRUE(vkey_is_valid(VKEY_A) != 0);
  EXPECT_TRUE(vkey_is_valid(VKEY_MEDIA_PLAY_PAUSE) != 0);
  EXPECT_TRUE(vkey_is_valid(VKEY_LANG_1) != 0);
}

TEST(input_keyboard_test, primary_device_matches_availability_expectations) {
  keyboard out_id = keyboard_get_primary();
  if (out_id != NULL) {
    EXPECT_TRUE(keyboard_is_valid(out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_KEYBOARD, devices_get_type(keyboard_to_device(out_id)));
  }
}
