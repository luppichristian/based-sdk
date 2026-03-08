// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(processes_common_processes_test, open_weblink_invalid) {
  b32 result = process_open_weblink("");
  EXPECT_EQ(0, result);
}

TEST(processes_common_processes_test, open_weblink_null) {
  b32 result = process_open_weblink(nullptr);
  EXPECT_EQ(0, result);
}

TEST(processes_common_processes_test, open_file_window) {
  b32 result = process_open_file_window(nullptr);
  EXPECT_NE(0, result);
}

TEST(processes_common_processes_test, open_terminal_null) {
  b32 result = process_open_terminal(nullptr);
  EXPECT_NE(0, result);
}
