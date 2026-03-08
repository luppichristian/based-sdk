// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  b32 archive_test_iterate_count(const archive_entry_info* info_ptr, void* user_data) {
    if (info_ptr == nullptr || user_data == nullptr) {
      return 0;
    }
    sz* count_ptr = (sz*)user_data;
    *count_ptr += 1;
    return 1;
  }
}  // namespace

TEST(filesystem_archive_test, write_read_and_metadata_roundtrip) {
  archive arc_val = archive_create(nullptr);
  path item_path = path_from_cstr("folder/item.bin");
  u8 src_data[] = {10, 20, 30, 40};
  buffer src_buff = buffer_from(src_data, size_of(src_data));

  ASSERT_TRUE(archive_write_all(&arc_val, &item_path, src_buff) != 0);
  EXPECT_EQ(1U, archive_count(&arc_val));
  EXPECT_TRUE(archive_exists(&arc_val, &item_path) != 0);

  archive_entry_info info_val = {0};
  ASSERT_TRUE(archive_get_entry_info(&arc_val, &item_path, &info_val) != 0);
  EXPECT_TRUE(info_val.is_directory == 0);
  EXPECT_EQ(src_buff.size, info_val.data_size);

  buffer view_buff = {0};
  ASSERT_TRUE(archive_get_entry_data(&arc_val, &item_path, &view_buff) != 0);
  ASSERT_EQ(src_buff.size, view_buff.size);
  for (sz data_idx = 0; data_idx < src_buff.size; data_idx += 1) {
    EXPECT_EQ(src_data[data_idx], ((u8*)view_buff.ptr)[data_idx]);
  }

  allocator alloc_val = thread_get_allocator();
  buffer read_buff = {0};
  ASSERT_TRUE(archive_read_all(&arc_val, &item_path, &alloc_val, &read_buff) != 0);
  ASSERT_EQ(src_buff.size, read_buff.size);
  for (sz data_idx = 0; data_idx < src_buff.size; data_idx += 1) {
    EXPECT_EQ(src_data[data_idx], ((u8*)read_buff.ptr)[data_idx]);
  }
  allocator_dealloc(&alloc_val, read_buff.ptr, read_buff.size);

  archive_destroy(&arc_val);
}

TEST(filesystem_archive_test, iterate_remove_and_clear_entries) {
  archive arc_val = archive_create(nullptr);
  path one_path = path_from_cstr("one.txt");
  path two_path = path_from_cstr("two.txt");
  u8 one_data[] = {1};
  u8 two_data[] = {2, 3};

  ASSERT_TRUE(archive_write_all(&arc_val, &one_path, buffer_from(one_data, size_of(one_data))) != 0);
  ASSERT_TRUE(archive_write_all(&arc_val, &two_path, buffer_from(two_data, size_of(two_data))) != 0);

  sz iter_count = 0;
  EXPECT_TRUE(archive_iterate(&arc_val, archive_test_iterate_count, &iter_count) != 0);
  EXPECT_EQ(2U, iter_count);

  EXPECT_TRUE(archive_remove(&arc_val, &one_path) != 0);
  EXPECT_TRUE(archive_exists(&arc_val, &one_path) == 0);
  EXPECT_EQ(1U, archive_count(&arc_val));

  archive_clear(&arc_val);
  EXPECT_EQ(0U, archive_count(&arc_val));

  archive_destroy(&arc_val);
}
