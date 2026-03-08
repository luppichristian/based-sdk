// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  path filestream_test_make_file_path(cstr8 test_name) {
    path base_path = dir_get_pref("based", "tests");
    if (base_path.buf[0] == '\0') {
      base_path = path_get_current();
    }
    path root_path = path_join_cstr(&base_path, "filestream_tests");
    timestamp now_val = timestamp_now();
    cstr8_append_format(
        root_path.buf,
        size_of(root_path.buf),
        "/%s_%lld",
        test_name,
        (long long)now_val.microseconds);
    dir_create_recursive(&root_path);
    return path_join_cstr(&root_path, "stream.bin");
  }
}  // namespace

TEST(filesystem_filestream_test, native_stream_write_seek_read) {
  path file_path = filestream_test_make_file_path("native");
  filestream stm_val = filestream_open(
      &file_path,
      FILESTREAM_OPEN_READ | FILESTREAM_OPEN_WRITE | FILESTREAM_OPEN_CREATE | FILESTREAM_OPEN_TRUNCATE);
  ASSERT_TRUE(filestream_is_open(&stm_val) != 0);

  u8 src_data[] = {3, 6, 9, 12};
  ASSERT_TRUE(filestream_write_exact(&stm_val, src_data, size_of(src_data)) != 0);
  EXPECT_EQ(size_of(src_data), filestream_tell(&stm_val));
  EXPECT_EQ(size_of(src_data), filestream_size(&stm_val));

  ASSERT_TRUE(filestream_seek(&stm_val, 0, FILESTREAM_SEEK_BASIS_BEGIN) != 0);
  u8 dst_data[4] = {0};
  ASSERT_TRUE(filestream_read_exact(&stm_val, dst_data, size_of(dst_data)) != 0);
  EXPECT_EQ(3U, dst_data[0]);
  EXPECT_EQ(12U, dst_data[3]);
  EXPECT_TRUE(filestream_eof(&stm_val) != 0);

  filestream_close(&stm_val);
  EXPECT_TRUE(filestream_is_open(&stm_val) == 0);

  path root_path = path_get_directory(&file_path);
  (void)dir_remove_recursive(&root_path);
}

TEST(filesystem_filestream_test, archive_stream_flushes_into_archive_entry) {
  archive arc_val = archive_create(nullptr);
  path entry_path = path_from_cstr("data/item.bin");
  filestream stm_val = filestream_open_archive(
      &arc_val,
      &entry_path,
      FILESTREAM_OPEN_WRITE | FILESTREAM_OPEN_TRUNCATE | FILESTREAM_OPEN_CREATE);
  ASSERT_TRUE(filestream_is_open(&stm_val) != 0);

  u8 src_data[] = {7, 8, 9};
  ASSERT_TRUE(filestream_write_exact(&stm_val, src_data, size_of(src_data)) != 0);
  ASSERT_TRUE(filestream_flush(&stm_val) != 0);
  filestream_close(&stm_val);

  buffer out_data = {0};
  ASSERT_TRUE(archive_get_entry_data(&arc_val, &entry_path, &out_data) != 0);
  ASSERT_EQ(size_of(src_data), out_data.size);
  EXPECT_EQ(7U, ((u8*)out_data.ptr)[0]);
  EXPECT_EQ(9U, ((u8*)out_data.ptr)[2]);

  archive_destroy(&arc_val);
}
