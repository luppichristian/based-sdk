// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_clipboard_test, invalid_arguments_and_basic_text_cycle) {
  EXPECT_TRUE(clipboard_set_text(nullptr) == 0);
  EXPECT_TRUE(clipboard_get_text(nullptr, 16) == 0);
  EXPECT_TRUE(clipboard_set_data(nullptr, buffer_from(nullptr, 0)) == 0);
  EXPECT_TRUE(clipboard_get_data(nullptr, buffer_from(nullptr, 0), nullptr) == 0);

  b32 set_ok = clipboard_set_text("based clipboard test");
  if (set_ok != 0) {
    c8 out_txt[64] = {0};
    ASSERT_TRUE(clipboard_get_text(out_txt, size_of(out_txt)) != 0);
    EXPECT_NE(nullptr, cstr8_find(out_txt, "based"));

    b32 clear_ok = clipboard_clear();
    EXPECT_TRUE(clear_ok != 0);
  }
}

TEST(input_clipboard_test, generic_media_data_cycle) {
  u8 media_bytes[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0xDE, 0xAD, 0xBE, 0xEF};
  u8 out_bytes[size_of(media_bytes)] = {0};
  u8 tiny_bytes[4] = {0};
  sz data_size = 0;

  ASSERT_TRUE(clipboard_set_data("image/png", buffer_from(media_bytes, size_of(media_bytes))) != 0);
  EXPECT_TRUE(clipboard_has_data("image/png") != 0);
  EXPECT_TRUE(clipboard_get_data("image/png", buffer_from(nullptr, 0), &data_size) != 0);
  EXPECT_EQ(size_of(media_bytes), data_size);
  EXPECT_TRUE(clipboard_get_data("image/png", buffer_from(tiny_bytes, size_of(tiny_bytes)), &data_size) == 0);
  EXPECT_EQ(size_of(media_bytes), data_size);
  ASSERT_TRUE(clipboard_get_data("image/png", buffer_from(out_bytes, size_of(out_bytes)), &data_size) != 0);
  EXPECT_EQ(size_of(media_bytes), data_size);
  EXPECT_TRUE(mem_cmp(media_bytes, out_bytes, data_size) != 0);
  EXPECT_TRUE(clipboard_clear() != 0);
}
