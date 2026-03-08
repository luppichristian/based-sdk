// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(windowing_window_test, invalid_window_id_paths_return_failure) {
  EXPECT_TRUE(window_id_is_valid(nullptr) == 0);
  EXPECT_EQ((up)456, window_to_native_id(window_from_native_id(456)));

  i32 xpos_val = 0;
  i32 ypos_val = 0;
  i32 widt_val = 0;
  i32 high_val = 0;
  monitor mon_id = nullptr;

  EXPECT_TRUE(window_is_valid(nullptr) == 0);
  EXPECT_TRUE(window_destroy(nullptr) == 0);
  EXPECT_TRUE(window_show(nullptr) == 0);
  EXPECT_TRUE(window_hide(nullptr) == 0);
  EXPECT_TRUE(window_move(nullptr, 0, 0) == 0);
  EXPECT_TRUE(window_get_position(nullptr, &xpos_val, &ypos_val) == 0);
  EXPECT_TRUE(window_resize(nullptr, 10, 10) == 0);
  EXPECT_TRUE(window_get_size(nullptr, &widt_val, &high_val) == 0);
  EXPECT_TRUE(window_set_fullscreen(nullptr, 1) == 0);
  EXPECT_TRUE(window_is_fullscreen(nullptr) == 0);
  EXPECT_TRUE(window_set_title(nullptr, "x") == 0);
  EXPECT_EQ(nullptr, window_get_title(nullptr));
  EXPECT_TRUE(window_get_monitor_id(nullptr, &mon_id) == 0);
}

TEST(windowing_window_test, create_modify_and_destroy_window_when_available) {
#if defined(_WIN32)
  GTEST_SKIP() << "window creation is unstable on this target";
#endif
  window win_id = window_create("based test", 320, 200, 0);
  if (win_id == nullptr) {
    GTEST_SKIP() << "window backend not available";
  }

  ASSERT_TRUE(window_is_valid(win_id) != 0);
  EXPECT_TRUE(window_set_title(win_id, "based test renamed") != 0);
  cstr8 title_val = window_get_title(win_id);
  ASSERT_NE(nullptr, title_val);
  EXPECT_STREQ("based test renamed", title_val);

  EXPECT_TRUE(window_destroy(win_id) != 0);
}
