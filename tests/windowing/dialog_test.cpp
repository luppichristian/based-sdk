// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(windowing_dialog_test, validation_failures_do_not_require_user_interaction) {
  EXPECT_TRUE(dialog_show_message_box(nullptr, nullptr, nullptr) == 0);

  dialog_message_box bad_box = {};
  bad_box.title = "Invalid";
  bad_box.message = "Missing buttons array";
  bad_box.button_count = 1;
  bad_box.buttons = nullptr;
  EXPECT_TRUE(dialog_show_message_box(nullptr, &bad_box, nullptr) == 0);
}
