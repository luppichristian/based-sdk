// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  path directory_test_make_root(cstr8 test_name) {
    path base_path = dir_get_pref("based", "tests");
    if (base_path.buf[0] == '\0') {
      base_path = path_get_current();
    }
    path root_path = path_join_cstr(&base_path, "directory_tests");
    timestamp now_val = timestamp_now();
    cstr8_append_format(
        root_path.buf,
        size_of(root_path.buf),
        "/%s_%llu_%lld",
        test_name,
        (unsigned long long)process_id(),
        (long long)now_val.microseconds);
    return root_path;
  }

  b32 directory_test_count_entries(const dir_entry* entry_ptr, void* user_data) {
    if (entry_ptr == nullptr || user_data == nullptr) {
      return 0;
    }
    sz* count_ptr = (sz*)user_data;
    *count_ptr += 1;
    return 1;
  }
}  // namespace

TEST(filesystem_directory_test, create_iterate_copy_and_remove_recursive) {
  path root_path = directory_test_make_root("dir_ops");
  path src_path = path_join_cstr(&root_path, "src");
  path nested_path = path_join_cstr(&src_path, "nested");
  path file_path = path_join_cstr(&src_path, "note.txt");
  path copy_path = path_join_cstr(&root_path, "dst");

  if (dir_create_recursive(&nested_path) == 0) {
    GTEST_SKIP() << "unable to create test directories";
  }
  EXPECT_TRUE(dir_exists(&src_path) != 0);
  EXPECT_TRUE(dir_exists(&nested_path) != 0);

  u8 file_data[] = {4, 5, 6};
  ASSERT_TRUE(file_write_all(&file_path, buffer_from(file_data, size_of(file_data))) != 0);

  sz direct_count = 0;
  ASSERT_TRUE(dir_iterate(&src_path, directory_test_count_entries, &direct_count) != 0);
  EXPECT_GE(direct_count, 2U);

  sz recursive_count = 0;
  if (dir_iterate_recursive(&src_path, directory_test_count_entries, &recursive_count) == 0) {
    GTEST_SKIP() << "recursive directory iteration is not available in this environment";
  }
  EXPECT_GE(recursive_count, direct_count);

  ASSERT_TRUE(dir_copy_recursive(&src_path, &copy_path, 1) != 0);
  EXPECT_TRUE(dir_exists(&copy_path) != 0);

  (void)dir_remove_recursive(&root_path);
}

TEST(filesystem_directory_test, base_pref_and_invalid_paths_are_handled) {
  path base_path = dir_get_base();
  EXPECT_TRUE(base_path.buf[0] != '\0');

  path pref_path = dir_get_pref("based", "tests");
  EXPECT_TRUE(pref_path.buf[0] != '\0');

  path invalid_path = path_from_cstr("");
  EXPECT_TRUE(dir_create(&invalid_path) == 0);
  EXPECT_TRUE(dir_remove(&invalid_path) == 0);
  EXPECT_TRUE(dir_rename(&invalid_path, &invalid_path) == 0);
}
