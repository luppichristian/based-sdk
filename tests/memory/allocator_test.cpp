// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  sz test_allocated_size;
  sz test_deallocated_size;

  void* test_alloc(void* user_data, callsite site, sz size) {
    (void)user_data;
    (void)site;
    test_allocated_size = size;
    sz alloc_size = size == 0 ? 1 : size;
    void* ptr = vmem_alloc(alloc_size);
    return ptr;
  }

  void test_free(void* user_data, callsite site, void* ptr) {
    (void)user_data;
    (void)site;
    vmem_free(ptr, test_allocated_size == 0 ? 1 : test_allocated_size);
    test_deallocated_size = 1;
  }
}  // namespace

TEST(memory_allocator_test, create_default) {
  allocator alloc = {0};
  EXPECT_EQ(nullptr, alloc.user_data);
  EXPECT_EQ(nullptr, alloc.alloc_fn);
  EXPECT_EQ(nullptr, alloc.dealloc_fn);
  EXPECT_EQ(nullptr, alloc.realloc_fn);
}

TEST(memory_allocator_test, custom_callbacks) {
  allocator alloc = {0};
  alloc.user_data = nullptr;
  alloc.alloc_fn = test_alloc;
  alloc.dealloc_fn = test_free;
  test_allocated_size = 0;
  void* ptr = allocator_alloc(&alloc, 100);
  EXPECT_NE(nullptr, ptr);
  EXPECT_EQ(100U, test_allocated_size);
  allocator_dealloc(&alloc, ptr, 100);
}

TEST(memory_allocator_test, alloc_zero) {
  allocator alloc = {0};
  alloc.alloc_fn = test_alloc;
  alloc.dealloc_fn = test_free;
  void* ptr = allocator_alloc(&alloc, 0);
  EXPECT_NE(nullptr, ptr);
  allocator_dealloc(&alloc, ptr, 0);
}

TEST(memory_allocator_test, dealloc) {
  allocator alloc = {0};
  alloc.alloc_fn = test_alloc;
  alloc.dealloc_fn = test_free;
  test_deallocated_size = 0;
  void* ptr = allocator_alloc(&alloc, 100);
  allocator_dealloc(&alloc, ptr, 100);
  EXPECT_EQ(1U, test_deallocated_size);
}
