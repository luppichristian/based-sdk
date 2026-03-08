// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_pool_test, create) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  EXPECT_EQ(0U, pool_block_count(&pol));
  EXPECT_EQ(64U, pool_slot_size(&pol));
  pool_destroy(&pol);
}

TEST(memory_pool_test, alloc) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  void* ptr = pool_alloc(&pol);
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(1U, pool_block_count(&pol));
  pool_destroy(&pol);
}

TEST(memory_pool_test, alloc_array_typed) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, sizeof(i32), 4);
  i32* ptr = pool_alloc_type(&pol, i32);
  EXPECT_NE(nullptr, ptr);
  *ptr = 42;
  EXPECT_EQ(42, *ptr);
  pool_destroy(&pol);
}

TEST(memory_pool_test, dealloc) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  void* ptr = pool_alloc(&pol);
  EXPECT_NE(nullptr, ptr);
  pool_dealloc(&pol, ptr);
  void* ptr2 = pool_alloc(&pol);
  EXPECT_NE(nullptr, ptr2);
  EXPECT_EQ(ptr, ptr2);
  pool_destroy(&pol);
}

TEST(memory_pool_test, free_count) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  void* ptr = pool_alloc(&pol);
  EXPECT_NE(nullptr, ptr);
  sz free_before = pool_free_count(&pol);
  pool_dealloc(&pol, ptr);
  sz free_after = pool_free_count(&pol);
  EXPECT_GT(free_after, free_before);
  pool_destroy(&pol);
}

TEST(memory_pool_test, clear) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  pool_alloc(&pol);
  pool_alloc(&pol);
  EXPECT_EQ(1U, pool_block_count(&pol));
  pool_clear(&pol);
  EXPECT_GT(pool_free_count(&pol), 0U);
  pool_destroy(&pol);
}

TEST(memory_pool_test, add_block) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  u8 buf[8192];
  pool_add_block(&pol, buf, sizeof(buf));
  EXPECT_EQ(1U, pool_block_count(&pol));
  void* ptr = pool_alloc(&pol);
  EXPECT_NE(nullptr, ptr);
  pool_destroy(&pol);
}

TEST(memory_pool_test, get_allocator) {
  allocator zero_alloc = {0};
  pool pol = pool_create(zero_alloc, nullptr, 4096, 64, 8);
  allocator alloc = pool_get_allocator(&pol);
  EXPECT_NE(nullptr, alloc.alloc_fn);
  void* ptr = allocator_alloc(&alloc, 64);
  EXPECT_NE(nullptr, ptr);
  pool_destroy(&pol);
}
