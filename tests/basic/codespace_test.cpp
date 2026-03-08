// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_codespace_test, callsite_here_captures_line_and_strings) {
  u32 line_num = __LINE__ + 1;
  callsite site_now = CALLSITE_HERE;

  EXPECT_EQ(site_now.line, line_num);
  EXPECT_NE(site_now.filename, nullptr);
  EXPECT_NE(site_now.function, nullptr);
  EXPECT_NE(cstr8_find(site_now.filename, "codespace_test.cpp"), nullptr);
}

TEST(basic_codespace_test, callsite_struct_fields_roundtrip) {
  callsite site_val = {"fake_file.c", "fake_func", 77};

  EXPECT_EQ(cstr8_cmp(site_val.filename, "fake_file.c"), 0);
  EXPECT_EQ(cstr8_cmp(site_val.function, "fake_func"), 0);
  EXPECT_EQ(site_val.line, 77U);
}

TEST(basic_codespace_test, src_loc_keeps_filename_and_line) {
  src_loc source_loc = {"module.c", 12};

  EXPECT_EQ(cstr8_cmp(source_loc.filename, "module.c"), 0);
  EXPECT_EQ(source_loc.line, 12U);
}
