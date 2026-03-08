// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  path pathinfo_test_make_file_path(void) {
    path base_path = dir_get_pref("based", "tests");
    if (base_path.buf[0] == '\0') {
      base_path = path_get_current();
    }
    path root_path = path_join_cstr(&base_path, "pathinfo_tests");
    timestamp now_val = timestamp_now();
    cstr8_append_format(
        root_path.buf,
        size_of(root_path.buf),
        "/case_%lld",
        (long long)now_val.microseconds);
    dir_create_recursive(&root_path);
    return path_join_cstr(&root_path, "meta.bin");
  }
}  // namespace

TEST(filesystem_pathinfo_test, get_info_for_existing_file_and_directory) {
  path file_path = pathinfo_test_make_file_path();
  path root_path = path_get_directory(&file_path);
  u8 payload[] = {1, 3, 5, 7};
  ASSERT_TRUE(file_write_all(&file_path, buffer_from(payload, size_of(payload))) != 0);

  pathinfo file_info = {};
  ASSERT_TRUE(pathinfo_get(&file_path, &file_info) != 0);
  EXPECT_TRUE(file_info.exists != 0);
  EXPECT_EQ(PATHINFO_TYPE_FILE, file_info.kind);
  EXPECT_EQ(size_of(payload), file_info.size);
  EXPECT_TRUE(timestamp_is_zero(file_info.write_time) == 0);

  pathinfo dir_info = {};
  ASSERT_TRUE(pathinfo_get(&root_path, &dir_info) != 0);
  EXPECT_TRUE(dir_info.exists != 0);
  EXPECT_EQ(PATHINFO_TYPE_DIRECTORY, dir_info.kind);

  EXPECT_TRUE(dir_remove_recursive(&root_path) != 0);
}

TEST(filesystem_pathinfo_test, invalid_inputs_return_failure) {
  path bad_path = path_from_cstr("");
  pathinfo info_val = {};
  EXPECT_TRUE(pathinfo_get(&bad_path, &info_val) == 0);
  EXPECT_TRUE(pathinfo_get(nullptr, &info_val) == 0);
  EXPECT_TRUE(pathinfo_get(&bad_path, nullptr) == 0);
}
