// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_input_state_test, clear_and_serialize_deserialize_roundtrip) {
  input_state src_state = {};
  src_state.timestamp = 1234;
  src_state.keyboard_available = 1;
  src_state.mouse_available = 1;
  src_state.keyboard_mods = (u16)KEYMOD_CTRL;
  src_state.keyboard_down[4] = 1;
  src_state.keyboard_repeat[4] = 2;
  src_state.mouse_button_mask = 3;
  src_state.gamepad_connected[0] = 1;
  src_state.gamepad_axis[0][GAMEPAD_AXIS_LEFTX] = 333;

  sz blob_size = input_state_serialized_size();
  u8* blob_data = new u8[blob_size];
  memset(blob_data, 0, blob_size);

  sz out_size = 0;
  ASSERT_TRUE(input_state_serialize(&src_state, blob_data, blob_size, &out_size) != 0);
  EXPECT_EQ(blob_size, out_size);

  input_state dst_state = {};
  ASSERT_TRUE(input_state_deserialize(blob_data, out_size, &dst_state) != 0);
  EXPECT_EQ(src_state.timestamp, dst_state.timestamp);
  EXPECT_TRUE(dst_state.keyboard_available != 0);
  EXPECT_TRUE(dst_state.keyboard_down[4] != 0);
  EXPECT_EQ(2U, dst_state.keyboard_repeat[4]);
  EXPECT_EQ(333, dst_state.gamepad_axis[0][GAMEPAD_AXIS_LEFTX]);

  input_state_clear(&dst_state);
  EXPECT_EQ(0U, dst_state.timestamp);
  EXPECT_TRUE(dst_state.keyboard_available == 0);
  delete[] blob_data;
}

TEST(input_input_state_test, capture_and_apply_keyboard_message) {
  EXPECT_TRUE(input_state_capture(INPUT_KEY_DEFAULT, nullptr) == 0);

  input_state cap_state = {};
  ASSERT_TRUE(input_state_capture(INPUT_KEY_DEFAULT, &cap_state) != 0);

  msg key_msg = {};
  key_msg.type = MSG_CORE_TYPE_KEY_DOWN;
  key_msg.timestamp = 777;
  msg_core_keyboard_data key_data = {};
  key_data.scancode = 10;
  key_data.modifiers = KEYMOD_SHIFT;
  key_data.down = 1;
  key_data.repeat = 0;
  msg_core_fill_keyboard(&key_msg, &key_data);

  input_state run_state = {};
  input_state_apply_msg(&run_state, &key_msg);
  EXPECT_EQ(777U, run_state.timestamp);
  EXPECT_TRUE(run_state.keyboard_available != 0);
  EXPECT_TRUE(run_state.keyboard_down[10] != 0);
  EXPECT_EQ((u16)KEYMOD_SHIFT, run_state.keyboard_mods);
}
