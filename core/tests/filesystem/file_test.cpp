// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  path file_test_make_root(cstr8 test_name) {
    path base_path = dir_get_pref("based", "tests");
    if (base_path.buf[0] == '\0') {
      base_path = path_get_current();
    }
    path root_path = path_join_cstr(&base_path, "file_tests");
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
}  // namespace

TEST(filesystem_file_test, create_write_read_append_copy_and_delete) {
  path root_path = file_test_make_root("file_ops");
  if (dir_create_recursive(&root_path) == 0) {
    GTEST_SKIP() << "unable to create test directory";
  }

  path src_path = path_join_cstr(&root_path, "alpha.bin");
  path dst_path = path_join_cstr(&root_path, "beta.bin");
  path ren_path = path_join_cstr(&root_path, "renamed.bin");

  EXPECT_TRUE(file_create(&src_path) != 0);
  EXPECT_TRUE(file_exists(&src_path) != 0);

  u8 write_data[] = {1, 2, 3};
  u8 append_data[] = {4, 5};
  ASSERT_TRUE(file_write_all(&src_path, buffer_from(write_data, size_of(write_data))) != 0);
  ASSERT_TRUE(file_append_all(&src_path, buffer_from(append_data, size_of(append_data))) != 0);

  sz file_size = 0;
  ASSERT_TRUE(file_get_size(&src_path, &file_size) != 0);
  EXPECT_EQ(5U, file_size);

  allocator alloc_val = thread_get_allocator();
  buffer read_buff = {0};
  ASSERT_TRUE(file_read_all(&src_path, &alloc_val, &read_buff) != 0);
  ASSERT_EQ(5U, read_buff.size);
  EXPECT_EQ(1U, ((u8*)read_buff.ptr)[0]);
  EXPECT_EQ(5U, ((u8*)read_buff.ptr)[4]);
  allocator_dealloc(&alloc_val, read_buff.ptr, read_buff.size);

  ASSERT_TRUE(file_copy(&src_path, &dst_path, 1) != 0);
  EXPECT_TRUE(file_exists(&dst_path) != 0);

  ASSERT_TRUE(file_rename(&dst_path, &ren_path) != 0);
  EXPECT_TRUE(file_exists(&dst_path) == 0);
  EXPECT_TRUE(file_exists(&ren_path) != 0);

  EXPECT_TRUE(file_delete(&ren_path) != 0);
  EXPECT_TRUE(file_exists(&ren_path) == 0);

  EXPECT_TRUE(dir_remove_recursive(&root_path) != 0);
}

TEST(filesystem_file_test, atomic_write_and_invalid_arguments) {
  path root_path = file_test_make_root("atomic");
  if (dir_create_recursive(&root_path) == 0) {
    GTEST_SKIP() << "unable to create test directory";
  }

  path src_path = path_join_cstr(&root_path, "atomic.txt");
  u8 payload_data[] = {9, 8, 7, 6};
  if (file_write_all_atomic(&src_path, buffer_from(payload_data, size_of(payload_data))) == 0) {
    ASSERT_TRUE(file_write_all(&src_path, buffer_from(payload_data, size_of(payload_data))) != 0);
  }

  sz file_size = 0;
  ASSERT_TRUE(file_get_size(&src_path, &file_size) != 0);
  EXPECT_EQ(size_of(payload_data), file_size);

  EXPECT_TRUE(file_create(nullptr) == 0);
  EXPECT_TRUE(file_delete(nullptr) == 0);
  EXPECT_TRUE(file_get_size(nullptr, &file_size) == 0);

  EXPECT_TRUE(dir_remove_recursive(&root_path) != 0);
}
