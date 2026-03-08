// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(system_system_info_test, query) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(info.page_size, 0U);
  EXPECT_GT(info.allocation_granularity, 0U);
}

TEST(system_system_info_test, os_name) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.os_name), 0U);
}

TEST(system_system_info_test, os_version) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.os_version), 0U);
}

TEST(system_system_info_test, architecture_name) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.architecture_name), 0U);
}

TEST(system_system_info_test, computer_name) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.computer_name), 0U);
}

TEST(system_system_info_test, user_name) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.user_name), 0U);
}

TEST(system_system_info_test, user_home) {
  system_info info = {0};
  b32 result = system_info_query(&info);
  EXPECT_NE(0, result);

  EXPECT_GT(cstr8_len(info.user_home), 0U);
}
