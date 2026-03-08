// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  path filemap_test_make_file_path(cstr8 test_name) {
    path base_path = dir_get_pref("based", "tests");
    if (base_path.buf[0] == '\0') {
      base_path = path_get_current();
    }
    path root_path = path_join_cstr(&base_path, "filemap_tests");
    timestamp now_val = timestamp_now();
    cstr8_append_format(
        root_path.buf,
        size_of(root_path.buf),
        "/%s_%llu_%lld",
        test_name,
        (unsigned long long)process_id(),
        (long long)now_val.microseconds);
    (void)dir_create_recursive(&root_path);
    return path_join_cstr(&root_path, "map.bin");
  }
}  // namespace

TEST(filesystem_filemap_test, open_invalid_path_sets_error_and_stays_closed) {
  path bad_path = path_from_cstr("___based_filemap_missing___/x.bin");
  filemap map_val = filemap_open(&bad_path, FILEMAP_ACCESS_READ);
  EXPECT_TRUE(filemap_is_open(&map_val) == 0);
  EXPECT_EQ(FILEMAP_ERROR_OPEN_FAILED, filemap_get_last_error());
  EXPECT_TRUE(filemap_flush(&map_val) == 0);
  EXPECT_EQ(FILEMAP_ERROR_INVALID_ARGUMENT, filemap_get_last_error());
}

TEST(filesystem_filemap_test, open_read_and_close_map) {
  path file_path = filemap_test_make_file_path("read");
  path root_path = path_get_directory(&file_path);
  if (dir_exists(&root_path) == 0) {
    GTEST_SKIP() << "unable to create test directory";
  }
  u8 data_buf[] = {11, 22, 33};
  ASSERT_TRUE(file_write_all(&file_path, buffer_from(data_buf, size_of(data_buf))) != 0);

  filemap map_val = filemap_open(&file_path, FILEMAP_ACCESS_READ);
  if (filemap_is_open(&map_val) == 0) {
    GTEST_SKIP() << "file mapping unavailable in this environment";
  }
  EXPECT_TRUE(filemap_is_writable(&map_val) == 0);
  EXPECT_EQ(3U, map_val.data_size);
  EXPECT_TRUE(filemap_flush(&map_val) != 0);

  filemap_close(&map_val);
  EXPECT_TRUE(filemap_is_open(&map_val) == 0);

  EXPECT_TRUE(dir_remove_recursive(&root_path) != 0);
}
