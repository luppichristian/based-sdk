// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_arena_test, create) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  EXPECT_EQ(0U, arena_block_count(&arn));
  EXPECT_EQ(0U, arena_total_size(&arn));
  EXPECT_EQ(0U, arena_total_used(&arn));
  arena_destroy(&arn);
}

TEST(memory_arena_test, alloc) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  void* ptr = arena_alloc(&arn, 100, 8);
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(1U, arena_block_count(&arn));
  arena_destroy(&arn);
}

TEST(memory_arena_test, alloc_array) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  i32* arr = arena_alloc_array(&arn, i32, 10);
  EXPECT_NE(nullptr, arr);
  arr[0] = 42;
  EXPECT_EQ(42, arr[0]);
  arena_destroy(&arn);
}

TEST(memory_arena_test, clear) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  void* ptr1 = arena_alloc(&arn, 100, 8);
  void* ptr2 = arena_alloc(&arn, 100, 8);
  EXPECT_GT(arena_total_used(&arn), 0U);
  arena_clear(&arn);
  EXPECT_EQ(1U, arena_block_count(&arn));
  EXPECT_EQ(0U, arena_total_used(&arn));
  void* ptr3 = arena_alloc(&arn, 100, 8);
  EXPECT_GE(ptr3, ptr1);
  arena_destroy(&arn);
}

TEST(memory_arena_test, add_block) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  u8 buf[4096];
  arena_add_block(&arn, buf, sizeof(buf));
  EXPECT_EQ(1U, arena_block_count(&arn));
  EXPECT_GE(arena_total_size(&arn), 4096U);
  void* ptr = arena_alloc(&arn, 100, 8);
  EXPECT_NE(nullptr, ptr);
  EXPECT_GE(ptr, buf);
  arena_destroy(&arn);
}

TEST(memory_arena_test, get_allocator) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  allocator alloc = arena_get_allocator(&arn);
  EXPECT_NE(nullptr, alloc.alloc_fn);
  void* ptr = allocator_alloc(&alloc, 100);
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(1U, arena_block_count(&arn));
  arena_destroy(&arn);
}

TEST(memory_arena_test, total_free) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);
  sz total = arena_total_size(&arn);
  sz used = arena_total_used(&arn);
  sz free = arena_total_free(&arn);
  EXPECT_EQ(total, used + free);
  arena_destroy(&arn);
}
