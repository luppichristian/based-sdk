// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_buffer_test, from) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  EXPECT_EQ(100U, buff.size);
  EXPECT_EQ(data, buff.ptr);
}

TEST(memory_buffer_test, sub_from) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer sub = buffer_sub_from(buff, 20);
  EXPECT_EQ(80U, sub.size);
  EXPECT_EQ(data + 20, sub.ptr);
}

TEST(memory_buffer_test, sub_from_sized) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer sub = buffer_sub_from_sized(buff, 20, 30);
  EXPECT_EQ(30U, sub.size);
  EXPECT_EQ(data + 20, sub.ptr);
}

TEST(memory_buffer_test, slice) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer slice = buffer_slice(buff, 10, 50);
  EXPECT_EQ(40U, slice.size);
  EXPECT_EQ(data + 10, slice.ptr);
}

TEST(memory_buffer_test, split_offset) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer split = buffer_split_offset(&buff, 30);
  EXPECT_EQ(30U, split.size);
  EXPECT_EQ(70U, buff.size);
  EXPECT_EQ(data, split.ptr);
  EXPECT_EQ(data + 30, buff.ptr);
}

TEST(memory_buffer_test, split_size) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer split = buffer_split_size(&buff, 30);
  EXPECT_EQ(30U, split.size);
  EXPECT_EQ(70U, buff.size);
}

TEST(memory_buffer_test, cmp_equal) {
  u8 data1[10] = {1, 2, 3, 4, 5};
  u8 data2[10] = {1, 2, 3, 4, 5};
  buffer lhs = buffer_from(data1, 5);
  buffer rhs = buffer_from(data2, 5);
  EXPECT_TRUE(buffer_cmp(lhs, rhs) != 0);
}

TEST(memory_buffer_test, cmp_not_equal) {
  u8 data1[10] = {1, 2, 3, 4, 5};
  u8 data2[10] = {1, 2, 3, 4, 6};
  buffer lhs = buffer_from(data1, 5);
  buffer rhs = buffer_from(data2, 5);
  EXPECT_FALSE(buffer_cmp(lhs, rhs) != 0);
}

TEST(memory_buffer_test, cmp_common) {
  u8 data1[10] = {1, 2, 3, 4, 5};
  u8 data2[10] = {1, 2, 3, 4, 5, 6, 7};
  buffer lhs = buffer_from(data1, 5);
  buffer rhs = buffer_from(data2, 7);
  EXPECT_TRUE(buffer_cmp_common(lhs, rhs) != 0);
}

TEST(memory_buffer_test, get_ptr) {
  u8 data[100] = {0};
  data[50] = 42;
  buffer buff = buffer_from(data, 100);
  u8* ptr = (u8*)buffer_get_ptr(buff, 50);
  EXPECT_EQ(42U, *ptr);
}

TEST(memory_buffer_test, get_ptr_out_of_bounds) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  void* ptr = buffer_get_ptr(buff, 200);
  EXPECT_EQ(nullptr, ptr);
}

TEST(memory_buffer_test, get_data) {
  u8 data[100] = {0};
  data[50] = 42;
  buffer buff = buffer_from(data, 100);
  u8* ptr = (u8*)buffer_get_data(buff, 50, 10);
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(42U, *ptr);
}

TEST(memory_buffer_test, zero) {
  u8 data[100];
  buffer_set8(buffer_from(data, sizeof(data)), 0xFF);
  buffer buff = buffer_from(data, 100);
  buffer_zero(buff);
  for (sz i = 0; i < 100; i++) {
    EXPECT_EQ(0U, data[i]);
  }
}

TEST(memory_buffer_test, set8) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer_set8(buff, 0x42);
  for (sz i = 0; i < 100; i++) {
    EXPECT_EQ(0x42U, data[i]);
  }
}

TEST(memory_buffer_test, set16) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer_set16(buff, 0x1234);
  EXPECT_EQ(0x34U, data[0]);
  EXPECT_EQ(0x12U, data[1]);
}

TEST(memory_buffer_test, set32) {
  u8 data[100];
  buffer buff = buffer_from(data, 100);
  buffer_set32(buff, 0x12345678);
  EXPECT_EQ(0x78U, data[0]);
  EXPECT_EQ(0x56U, data[1]);
  EXPECT_EQ(0x34U, data[2]);
  EXPECT_EQ(0x12U, data[3]);
}
