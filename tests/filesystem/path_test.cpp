// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  path path_test_make_root(cstr8 test_name) {
    path base_path = dir_get_pref("based", "tests");
    if (base_path.buf[0] == '\0') {
      base_path = path_get_current();
    }
    path root_path = path_join_cstr(&base_path, "path_tests");
    timestamp now_val = timestamp_now();
    cstr8_append_format(
        root_path.buf,
        size_of(root_path.buf),
        "/%s_%lld",
        test_name,
        (long long)now_val.microseconds);
    return root_path;
  }
}  // namespace

TEST(filesystem_path_test, normalize_join_and_extract_parts) {
  path src_path = path_from_cstr("alpha\\beta//gamma.txt");
  path_normalize(&src_path);
  EXPECT_STREQ("alpha/beta/gamma.txt", src_path.buf);
  EXPECT_TRUE(path_is_relative(&src_path) != 0);
  EXPECT_TRUE(path_ends_with(&src_path, ".txt") != 0);

  path dir_path = path_get_directory(&src_path);
  path nam_path = path_get_name(&src_path);
  path bas_path = path_get_basename(&src_path);
  path ext_path = path_get_extension(&src_path);
  EXPECT_STREQ("alpha/beta", dir_path.buf);
  EXPECT_STREQ("gamma.txt", nam_path.buf);
  EXPECT_STREQ("gamma", bas_path.buf);
  EXPECT_STREQ(".txt", ext_path.buf);
}

TEST(filesystem_path_test, resolve_make_relative_and_current_directory) {
  path cwd_path = path_get_current();
  path root_path = path_test_make_root("cwd");
  root_path = path_resolve(&root_path);
  path nested_path = path_join_cstr(&root_path, "nested");
  ASSERT_TRUE(dir_create_recursive(&nested_path) != 0);

  if (path_set_current(&root_path) == 0) {
    GTEST_SKIP() << "changing current directory is not available in this environment";
  }
  path rel_path = path_from_cstr("nested/file.bin");
  path abs_path = path_resolve(&rel_path);
  EXPECT_TRUE(path_is_absolute(&abs_path) != 0);

  path made_rel = path_make_relative(&abs_path, &root_path);
  EXPECT_STREQ("nested/file.bin", made_rel.buf);

  ASSERT_TRUE(path_set_current(&cwd_path) != 0);
  EXPECT_TRUE(dir_remove_recursive(&root_path) != 0);
}
