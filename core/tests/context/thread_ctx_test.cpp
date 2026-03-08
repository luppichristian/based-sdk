// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(context_thread_ctx_test, current_thread_context_is_initialized_by_entry) {
  EXPECT_TRUE(thread_ctx_is_init() != 0);
  EXPECT_NE(thread_ctx_get(), nullptr);
  EXPECT_NE(thread_get_allocator().alloc_fn, nullptr);
  EXPECT_NE(thread_get_log_state(), nullptr);
  EXPECT_NE(thread_get_perm_arena(), nullptr);
  EXPECT_NE(thread_get_temp_arena(), nullptr);
  EXPECT_NE(thread_get_perm_heap(), nullptr);
  EXPECT_NE(thread_get_temp_heap(), nullptr);
}

TEST(context_thread_ctx_test, user_data_roundtrip_and_bounds_checks_work) {
  ctx_user_data_idx slot_idx = 3;
  i32 payload_val = 909;

  EXPECT_TRUE(thread_set_user_data(slot_idx, &payload_val) != 0);
  EXPECT_EQ(thread_get_user_data(slot_idx), &payload_val);

  EXPECT_EQ(thread_get_user_data(CTX_USER_DATA_COUNT), nullptr);
  EXPECT_TRUE(thread_set_user_data(CTX_USER_DATA_COUNT, &payload_val) == 0);
}

TEST(context_thread_ctx_test, log_helpers_capture_and_filter_messages) {
#if defined(_WIN32)
  GTEST_SKIP() << "thread log frame filtering is unstable on this target";
#endif
  log_state* state_ptr = thread_get_log_state();
  ASSERT_NE(state_ptr, nullptr);

  log_level old_level = state_ptr->level;
  thread_log_set_level(LOG_LEVEL_TRACE);

  thread_log_begin_frame();
  thread_log_warn("thread warn message");
  thread_log_info("thread info message");
  log_frame* warn_frame = thread_log_end_frame(0);

  ASSERT_NE(warn_frame, nullptr);
  EXPECT_GE(log_frame_msg_count(warn_frame), 2U);
  log_msg* first_msg = log_frame_first(warn_frame);
  ASSERT_NE(first_msg, nullptr);
  EXPECT_EQ(log_msg_level(first_msg), LOG_LEVEL_WARN);
  EXPECT_NE(cstr8_find(log_msg_text(first_msg), "thread warn message"), nullptr);

  log_frame_destroy(warn_frame);
  thread_log_set_level(old_level);
}

TEST(context_thread_ctx_test, log_sync_succeeds_for_initialized_thread_context) {
  EXPECT_TRUE(thread_log_sync() != 0);
}
