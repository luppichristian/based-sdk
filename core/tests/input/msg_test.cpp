// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  typedef struct msg_test_handler_state {
    sz called_count;
  } msg_test_handler_state;

  msg_handler_result msg_test_count_handler(msg* src, msg_handler_stage stage, void* user_data) {
    if (src == nullptr || user_data == nullptr) {
      return MSG_HANDLER_RESULT_CONTINUE;
    }
    msg_test_handler_state* state_ptr = (msg_test_handler_state*)user_data;
    if (stage == MSG_HANDLER_STAGE_BEFORE_POST) {
      state_ptr->called_count += 1;
    }
    return MSG_HANDLER_RESULT_CONTINUE;
  }

  b32 msg_test_filter_reject_type(const msg* src, void* user_data) {
    if (src == nullptr || user_data == nullptr) {
      return 1;
    }
    u32 blocked_type = *(u32*)user_data;
    return src->type == blocked_type ? 0 : 1;
  }
}  // namespace

TEST(input_msg_test, add_remove_handler_and_before_post_dispatch) {
  msg_clear_handlers();
  msg_set_filter(nullptr, nullptr);

  msg_test_handler_state state_val = {};
  msg_handler_desc desc_val = {};
  desc_val.handler_fn = msg_test_count_handler;
  desc_val.user_data = &state_val;
  desc_val.priority = 10;
  desc_val.type_min = MSG_CORE_TYPE_USER + 55;
  desc_val.type_max = MSG_CORE_TYPE_USER + 55;

  u64 handler_id = msg_add_handler(&desc_val);
  ASSERT_NE(0U, handler_id);

  msg post_msg = {};
  post_msg.category = MSG_CATEGORY_CORE;
  post_msg.type = MSG_CORE_TYPE_USER + 55;
  ASSERT_TRUE(msg_post(&post_msg) != 0);
  EXPECT_EQ(1U, state_val.called_count);

  msg_flush(post_msg.type, post_msg.type);
  EXPECT_TRUE(msg_remove_handler(handler_id) != 0);
  EXPECT_TRUE(msg_remove_handler(handler_id) == 0);
}

TEST(input_msg_test, filter_blocks_posts_and_count_flush_work_for_type_range) {
  msg_clear_handlers();

  u32 blocked_type = MSG_CORE_TYPE_USER + 77;
  msg_set_filter(msg_test_filter_reject_type, &blocked_type);

  msg blocked_msg = {};
  blocked_msg.category = MSG_CATEGORY_CORE;
  blocked_msg.type = blocked_type;
  EXPECT_TRUE(msg_post(&blocked_msg) == 0);

  msg_set_filter(nullptr, nullptr);
  ASSERT_TRUE(msg_post(&blocked_msg) != 0);
  EXPECT_GE(msg_count(blocked_type, blocked_type), 1);
  msg_flush(blocked_type, blocked_type);
  EXPECT_EQ(0, msg_count(blocked_type, blocked_type));
}
