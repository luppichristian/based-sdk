// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  typedef struct msg_test_handler_state {
    sz called_count;
    b32 cancel_post;
  } msg_test_handler_state;

  b32 msg_test_count_handler(msg* src, void* user_data) {
    if (src == nullptr || user_data == nullptr) {
      return 1;
    }
    msg_test_handler_state* state_ptr = (msg_test_handler_state*)user_data;
    state_ptr->called_count += 1;
    if (state_ptr->cancel_post) {
      return 0;
    }
    return 1;
  }

  b32 msg_test_filter_reject_type(const msg* src, void* user_data) {
    if (src == nullptr || user_data == nullptr) {
      return 1;
    }
    u32 blocked_type = *(u32*)user_data;
    return src->type == blocked_type ? 0 : 1;
  }
}  // namespace

TEST(input_msg_test, add_remove_handler_and_dispatch) {
  msg_clear_handlers();
  msg_set_filter(nullptr, nullptr);

  msg_test_handler_state state_val = {};
  msg_handler_desc desc_val = {};
  desc_val.handler_fn = msg_test_count_handler;
  desc_val.user_data = &state_val;
  desc_val.priority = 10;
  desc_val.category = MSG_CATEGORY_CORE;
  desc_val.type = MSG_CORE_TYPE_USER + 55;

  u64 handler_id = msg_add_handler(&desc_val);
  ASSERT_NE(0U, handler_id);

  msg post_msg = {};
  post_msg.category = MSG_CATEGORY_CORE;
  post_msg.type = MSG_CORE_TYPE_USER + 55;
  ASSERT_TRUE(msg_post(&post_msg) != 0);
  EXPECT_EQ(1U, state_val.called_count);

  EXPECT_TRUE(msg_remove_handler(handler_id) != 0);
  EXPECT_TRUE(msg_remove_handler(handler_id) == 0);
}

TEST(input_msg_test, handler_filter_matches_category_and_type) {
  msg_clear_handlers();
  msg_set_filter(nullptr, nullptr);

  msg_test_handler_state state_val = {};
  msg_handler_desc desc_val = {};
  desc_val.handler_fn = msg_test_count_handler;
  desc_val.user_data = &state_val;
  desc_val.priority = 10;
  desc_val.category = MSG_CATEGORY_AUDIO;
  desc_val.type = 42;

  u64 handler_id = msg_add_handler(&desc_val);
  ASSERT_NE(0U, handler_id);

  msg post_msg = {};
  post_msg.category = MSG_CATEGORY_CORE;
  post_msg.type = 42;
  ASSERT_TRUE(msg_post(&post_msg) != 0);
  EXPECT_EQ(0U, state_val.called_count);

  post_msg.category = MSG_CATEGORY_AUDIO;
  ASSERT_TRUE(msg_post(&post_msg) != 0);
  EXPECT_EQ(1U, state_val.called_count);

  post_msg.type = 43;
  ASSERT_TRUE(msg_post(&post_msg) != 0);
  EXPECT_EQ(1U, state_val.called_count);

  EXPECT_TRUE(msg_remove_handler(handler_id) != 0);
}

TEST(input_msg_test, filter_blocks_post_immediately) {
  msg_clear_handlers();

  u32 blocked_type = MSG_CORE_TYPE_USER + 77;
  msg_set_filter(msg_test_filter_reject_type, &blocked_type);

  msg blocked_msg = {};
  blocked_msg.category = MSG_CATEGORY_CORE;
  blocked_msg.type = blocked_type;
  EXPECT_TRUE(msg_post(&blocked_msg) == 0);

  msg_set_filter(nullptr, nullptr);
  ASSERT_TRUE(msg_post(&blocked_msg) != 0);
}

TEST(input_msg_test, handler_can_cancel_post_immediately) {
  msg_clear_handlers();
  msg_set_filter(nullptr, nullptr);

  msg_test_handler_state state_val = {};
  state_val.cancel_post = 1;

  msg_handler_desc desc_val = {};
  desc_val.handler_fn = msg_test_count_handler;
  desc_val.user_data = &state_val;
  desc_val.priority = 10;
  desc_val.category = MSG_CATEGORY_CORE;
  desc_val.type = MSG_CORE_TYPE_USER + 88;

  u64 handler_id = msg_add_handler(&desc_val);
  ASSERT_NE(0U, handler_id);

  msg post_msg = {};
  post_msg.category = MSG_CATEGORY_CORE;
  post_msg.type = MSG_CORE_TYPE_USER + 88;

  EXPECT_TRUE(msg_post(&post_msg) == 0);
  EXPECT_EQ(1U, state_val.called_count);

  EXPECT_TRUE(msg_remove_handler(handler_id) != 0);
}
