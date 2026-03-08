// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_msg_core_test, keyboard_fill_and_get_roundtrip_uses_type_defaults) {
  msg key_msg = {};
  msg_core_keyboard_data key_data = {};
  key_data.scancode = 12;
  key_data.down = 1;
  key_data.repeat = 0;
  key_data.modifiers = KEYMOD_CTRL;

  msg_core_fill_keyboard(&key_msg, &key_data);
  EXPECT_EQ(MSG_CATEGORY_CORE, key_msg.category);
  EXPECT_EQ(MSG_CORE_TYPE_KEY_DOWN, key_msg.type);

  msg_core_keyboard_data* out_key = msg_core_get_keyboard(&key_msg);
  ASSERT_NE(nullptr, out_key);
  EXPECT_EQ(12U, out_key->scancode);
  EXPECT_TRUE(out_key->down != 0);
  EXPECT_EQ(KEYMOD_CTRL, out_key->modifiers);
}

TEST(input_msg_core_test, category_or_type_mismatch_returns_null_accessor) {
  msg bad_msg = {};
  bad_msg.category = MSG_CATEGORY_USER0;
  bad_msg.type = MSG_CORE_TYPE_KEY_DOWN;
  EXPECT_EQ(nullptr, msg_core_get_keyboard(&bad_msg));

  msg wrong_type_msg = {};
  wrong_type_msg.category = MSG_CATEGORY_CORE;
  wrong_type_msg.type = MSG_CORE_TYPE_MOUSE_BUTTON_DOWN;
  EXPECT_EQ(nullptr, msg_core_get_keyboard(&wrong_type_msg));

  msg watch_msg = {};
  msg_core_pathwatch_data watch_data = {};
  watch_data.event_kind = MSG_CORE_PATHWATCH_EVENT_ITEM;
  watch_data.pathwatch_id = 5;
  watch_data.watch_id = 7;
  watch_data.action = PATHWATCH_ACTION_MODIFY;
  msg_core_fill_pathwatch(&watch_msg, &watch_data);
  msg_core_pathwatch_data* out_watch = msg_core_get_pathwatch(&watch_msg);
  ASSERT_NE(nullptr, out_watch);
  EXPECT_EQ(5, out_watch->pathwatch_id);
  EXPECT_EQ(7, out_watch->watch_id);
}
