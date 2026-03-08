// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_clipboard_test, invalid_arguments_and_basic_text_cycle) {
  EXPECT_TRUE(clipboard_set_text(nullptr) == 0);
  EXPECT_TRUE(clipboard_get_text(nullptr, 16) == 0);

  b32 set_ok = clipboard_set_text("based clipboard test");
  if (set_ok != 0) {
    c8 out_txt[64] = {0};
    ASSERT_TRUE(clipboard_get_text(out_txt, size_of(out_txt)) != 0);
    EXPECT_NE(nullptr, cstr8_find(out_txt, "based"));

    b32 clear_ok = clipboard_clear();
    EXPECT_TRUE(clear_ok != 0);
  }
}
