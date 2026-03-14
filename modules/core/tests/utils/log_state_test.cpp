// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_log_state_test, level_labels_are_stable) {
  EXPECT_TRUE(cstr8_cmp(log_level_to_str(LOG_LEVEL_FATAL), "FATAL"));
  EXPECT_TRUE(cstr8_cmp(log_level_to_str(LOG_LEVEL_ERROR), "ERROR"));
  EXPECT_TRUE(cstr8_cmp(log_level_to_str(LOG_LEVEL_WARN), "WARN "));
  EXPECT_TRUE(cstr8_cmp(log_level_to_str(LOG_LEVEL_TRACE), "TRACE"));
}

TEST(utils_log_state_test, state_init_and_quit_toggle_init_flag) {
  log_state state_val = {0};

  b32 init_ok = log_state_init(&state_val, NULL, global_get_allocator());
  EXPECT_TRUE(init_ok != 0);
  EXPECT_TRUE(log_state_is_init(&state_val) != 0);

  log_state_quit(&state_val);
  EXPECT_TRUE(log_state_is_init(&state_val) == 0);
}

TEST(utils_log_state_test, frame_capture_and_filtering_work) {
  log_state state_val = {0};
  b32 init_ok = log_state_init(&state_val, NULL, global_get_allocator());
  ASSERT_TRUE(init_ok != 0);

  log_state_set_level(&state_val, LOG_LEVEL_TRACE);
  log_state_begin_frame(&state_val);

  log_state_warn(&state_val, "warn message %d", 7);
  log_state_info(&state_val, "info message");

  u32 warn_mask = bit(LOG_LEVEL_WARN);
  log_frame* warn_frame = log_state_end_frame(&state_val, warn_mask);

  ASSERT_NE(warn_frame, nullptr);
  EXPECT_TRUE(log_frame_has_msgs(warn_frame) != 0);
  EXPECT_EQ(log_frame_msg_count(warn_frame), 1U);

  log_msg* first_msg = log_frame_first(warn_frame);
  ASSERT_NE(first_msg, nullptr);
  EXPECT_EQ(log_msg_level(first_msg), LOG_LEVEL_WARN);
  EXPECT_NE(cstr8_find(log_msg_text(first_msg), "warn message"), nullptr);
  EXPECT_EQ(log_msg_next(first_msg), nullptr);

  log_state_quit(&state_val);
}

TEST(utils_log_state_test, state_sync_moves_root_messages_between_states) {
  log_state dst_state = {0};
  log_state src_state = {0};
  ASSERT_TRUE(log_state_init(&dst_state, NULL, global_get_allocator()) != 0);
  ASSERT_TRUE(log_state_init(&src_state, NULL, global_get_allocator()) != 0);

  log_state_set_level(&src_state, LOG_LEVEL_TRACE);
  log_state_set_level(&dst_state, LOG_LEVEL_TRACE);

  ASSERT_NE(dst_state.root_frame, nullptr);
  ASSERT_NE(src_state.root_frame, nullptr);
  log_state_info(&src_state, "sync me");
  EXPECT_EQ(dst_state.root_frame->msg_count, 0U);
  EXPECT_EQ(src_state.root_frame->msg_count, 1U);

  log_state_sync(&dst_state, &src_state);
  EXPECT_EQ(dst_state.root_frame->msg_count, 1U);
  EXPECT_EQ(src_state.root_frame->msg_count, 0U);

  log_state_quit(&src_state);
  log_state_quit(&dst_state);
}
