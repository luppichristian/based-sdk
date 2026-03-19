// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(interface_window_test, invalid_window_id_paths_return_failure) {
  EXPECT_TRUE(window_id_is_valid(NULL) == 0);
  EXPECT_EQ((up)456, window_to_native_id(window_from_native_id(456)));

  i32 xpos_val = 0;
  i32 ypos_val = 0;
  i32 widt_val = 0;
  i32 high_val = 0;
  monitor mon_id = NULL;
  const u8 white_pixels[4] = {255, 255, 255, 255};
  icon icon_id = icon_create_rgba(1, 1, white_pixels, 0, 0);

  EXPECT_TRUE(window_is_valid(NULL) == 0);
  EXPECT_TRUE(window_destroy(NULL) == 0);
  EXPECT_TRUE(window_is_hidden(NULL) == 0);
  EXPECT_TRUE(window_show(NULL) == 0);
  EXPECT_TRUE(window_hide(NULL) == 0);
  EXPECT_TRUE(window_set_pos(NULL, 0, 0) == 0);
  EXPECT_TRUE(window_get_pos(NULL, &xpos_val, &ypos_val) == 0);
  EXPECT_TRUE(window_set_size(NULL, 10, 10) == 0);
  EXPECT_TRUE(window_get_size(NULL, &widt_val, &high_val) == 0);
  EXPECT_TRUE(window_set_mode(NULL, WINDOW_MODE_FULLSCREEN) == 0);
  EXPECT_EQ(WINDOW_MODE_WINDOWED, window_get_mode(NULL));
  EXPECT_TRUE(window_is_minimized(NULL) == 0);
  EXPECT_TRUE(window_is_maximized(NULL) == 0);
  EXPECT_TRUE(window_has_input_focus(NULL) == 0);
  EXPECT_TRUE(window_has_mouse_focus(NULL) == 0);
  EXPECT_TRUE(window_is_topmost(NULL) == 0);
  EXPECT_TRUE(window_set_title(NULL, "x") == 0);
  EXPECT_EQ(nullptr, window_get_title(NULL));
  EXPECT_TRUE(window_set_icon(NULL, icon_id) == 0);
  EXPECT_TRUE(window_set_topmost(NULL, 1) == 0);
  EXPECT_TRUE(window_get_monitor(NULL, &mon_id) == 0);
  EXPECT_TRUE(window_get_creation_monitor(NULL, &mon_id) == 0);
  EXPECT_TRUE(window_center_in_monitor(NULL, NULL, WINDOW_CENTER_AXIS_BOTH) == 0);

  ASSERT_TRUE(icon_id != NULL);
  EXPECT_TRUE(icon_destroy(icon_id) != 0);
}

TEST(interface_window_test, create_modify_and_destroy_window_when_available) {
#if defined(_WIN32)
  GTEST_SKIP() << "window creation is unstable on this target";
#endif
  window win_id = window_create("based test", 320, 200, WINDOW_MODE_WINDOWED, 0);
  if (win_id == NULL) {
    GTEST_SKIP() << "window backend not available";
  }

  ASSERT_TRUE(window_is_valid(win_id) != 0);
  EXPECT_EQ(WINDOW_MODE_WINDOWED, window_get_mode(win_id));
  EXPECT_TRUE(window_is_hidden(win_id) == 0);
  EXPECT_TRUE(window_set_title(win_id, "based test renamed") != 0);
  cstr8 title_val = window_get_title(win_id);
  ASSERT_NE(nullptr, title_val);
  EXPECT_STREQ("based test renamed", title_val);
  EXPECT_TRUE(window_is_minimized(win_id) == 0);
  EXPECT_TRUE(window_is_maximized(win_id) == 0);
  EXPECT_TRUE(window_has_input_focus(win_id) == 0 || window_has_input_focus(win_id) == 1);
  EXPECT_TRUE(window_has_mouse_focus(win_id) == 0 || window_has_mouse_focus(win_id) == 1);
  EXPECT_TRUE(window_is_topmost(win_id) == 0);
  b32 topmost_enabled = window_set_topmost(win_id, 1);
  if (topmost_enabled != 0) {
    EXPECT_TRUE(window_is_topmost(win_id) != 0);
    EXPECT_TRUE(window_set_topmost(win_id, 0) != 0);
    EXPECT_TRUE(window_is_topmost(win_id) == 0);
  }
  EXPECT_TRUE(window_hide(win_id) != 0);
  EXPECT_TRUE(window_is_hidden(win_id) != 0);
  EXPECT_TRUE(window_show(win_id) != 0);
  EXPECT_TRUE(window_is_hidden(win_id) == 0);

  monitor current_monitor_id = NULL;
  monitor creation_monitor_id = NULL;
  EXPECT_TRUE(window_get_monitor(win_id, &current_monitor_id) != 0);
  EXPECT_TRUE(window_get_creation_monitor(win_id, &creation_monitor_id) != 0);
  EXPECT_TRUE(monitor_id_is_valid(current_monitor_id) != 0);
  EXPECT_TRUE(monitor_id_is_valid(creation_monitor_id) != 0);
  EXPECT_TRUE(window_center_in_monitor(win_id, current_monitor_id, WINDOW_CENTER_AXIS_BOTH) != 0);

  const u8 green_pixels[4] = {0, 255, 0, 255};
  icon icon_id = icon_create_rgba(1, 1, green_pixels, 0, 0);
  ASSERT_NE(nullptr, icon_id);
  (void)window_set_icon(win_id, icon_id);
  EXPECT_TRUE(icon_destroy(icon_id) != 0);

  EXPECT_TRUE(window_destroy(win_id) != 0);
}
