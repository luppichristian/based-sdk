// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(context_global_ctx_test, singleton_and_wrappers_are_available_after_entry_init) {
  EXPECT_TRUE(global_ctx_is_init() != 0);
  EXPECT_NE(global_ctx_get(), nullptr);
  EXPECT_NE(global_ctx_get_shared(), nullptr);
  EXPECT_NE(global_get_allocator().alloc_fn, nullptr);
  EXPECT_NE(global_get_main_allocator().alloc_fn, nullptr);
  EXPECT_NE(global_get_log_state(), nullptr);
  EXPECT_NE(global_get_perm_arena(), nullptr);
  EXPECT_NE(global_get_temp_arena(), nullptr);
  EXPECT_NE(global_get_perm_heap(), nullptr);
  EXPECT_NE(global_get_temp_heap(), nullptr);
}

TEST(context_global_ctx_test, user_data_permissions_gate_access_per_slot) {
  ctx_user_data_idx slot_idx = 4;
  i32 payload_val = 44;

  b32 had_access = global_has_user_data_access(slot_idx);
  EXPECT_TRUE(had_access != 0);

  EXPECT_TRUE(global_set_user_data(slot_idx, &payload_val) != 0);
  EXPECT_EQ(global_get_user_data(slot_idx), &payload_val);

  EXPECT_TRUE(global_set_user_data_access(slot_idx, 0) != 0);
  EXPECT_TRUE(global_has_user_data_access(slot_idx) == 0);
  EXPECT_EQ(global_get_user_data(slot_idx), nullptr);
  EXPECT_TRUE(global_set_user_data(slot_idx, &payload_val) == 0);

  EXPECT_TRUE(global_set_user_data_access(slot_idx, 1) != 0);
  EXPECT_TRUE(global_has_user_data_access(slot_idx) != 0);
  EXPECT_TRUE(global_set_user_data(slot_idx, &payload_val) != 0);
  EXPECT_EQ(global_get_user_data(slot_idx), &payload_val);

  EXPECT_TRUE(global_has_user_data_access(CTX_USER_DATA_COUNT) == 0);
  EXPECT_TRUE(global_set_user_data_access(CTX_USER_DATA_COUNT, 1) == 0);
}

TEST(context_global_ctx_test, global_log_frame_helpers_capture_messages) {
  log_level old_level = global_get_log_state()->level;
  global_log_set_level(LOG_LEVEL_TRACE);

  global_log_begin_frame();
  global_log_info("global frame message");
  log_frame* info_frame = global_log_end_frame(log_level_mask(LOG_LEVEL_INFO));

  ASSERT_NE(info_frame, nullptr);
  EXPECT_EQ(log_frame_msg_count(info_frame), 1U);
  log_msg* frame_msg = log_frame_first(info_frame);
  ASSERT_NE(frame_msg, nullptr);
  EXPECT_EQ(log_msg_level(frame_msg), LOG_LEVEL_INFO);
  EXPECT_NE(cstr8_find(log_msg_text(frame_msg), "global frame message"), nullptr);

  log_frame_destroy(info_frame);
  global_log_set_level(old_level);
}
