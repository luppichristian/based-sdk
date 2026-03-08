// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_heap_test, create) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  EXPECT_EQ(0U, heap_block_count(&hep));
  EXPECT_EQ(0U, heap_total_size(&hep));
  heap_destroy(&hep);
}

TEST(memory_heap_test, alloc) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  void* ptr = heap_alloc(&hep, 100, 8);
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(1U, heap_block_count(&hep));
  heap_destroy(&hep);
}

TEST(memory_heap_test, alloc_array) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  i32* arr = heap_alloc_array(&hep, i32, 10);
  EXPECT_NE(nullptr, arr);
  arr[0] = 42;
  EXPECT_EQ(42, arr[0]);
  heap_destroy(&hep);
}

TEST(memory_heap_test, dealloc) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  void* ptr = heap_alloc(&hep, 100, 8);
  EXPECT_NE(nullptr, ptr);
  heap_dealloc(&hep, ptr);
  void* ptr2 = heap_alloc(&hep, 100, 8);
  EXPECT_NE(nullptr, ptr2);
  heap_destroy(&hep);
}

TEST(memory_heap_test, realloc) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  i32* arr = heap_alloc_array(&hep, i32, 10);
  arr[0] = 42;
  i32* arr2 = heap_realloc_array(&hep, arr, 10, 20, i32);
  EXPECT_NE(nullptr, arr2);
  EXPECT_EQ(42, arr2[0]);
  heap_destroy(&hep);
}

TEST(memory_heap_test, clear) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  heap_alloc(&hep, 100, 8);
  EXPECT_GT(heap_total_size(&hep), 0U);
  heap_clear(&hep);
  EXPECT_EQ(1U, heap_block_count(&hep));
  EXPECT_GT(heap_total_free(&hep), 0U);
  heap_destroy(&hep);
}

TEST(memory_heap_test, add_block) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  u8 buf[8192];
  heap_add_block(&hep, buf, sizeof(buf));
  EXPECT_EQ(1U, heap_block_count(&hep));
  void* ptr = heap_alloc(&hep, 100, 8);
  EXPECT_NE(nullptr, ptr);
  heap_destroy(&hep);
}

TEST(memory_heap_test, get_allocator) {
  allocator zero_alloc = {0};
  heap hep = heap_create(zero_alloc, nullptr, 4096);
  allocator alloc = heap_get_allocator(&hep);
  EXPECT_NE(nullptr, alloc.alloc_fn);
  void* ptr = allocator_alloc(&alloc, 100);
  EXPECT_NE(nullptr, ptr);
  heap_destroy(&hep);
}
