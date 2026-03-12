// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_vmem_test, page_size) {
  sz page_sz = vmem_page_size();
  EXPECT_GT(page_sz, 0U);
  EXPECT_TRUE((page_sz & (page_sz - 1)) == 0);
}

TEST(memory_vmem_test, reserve_basic) {
  sz page_sz = vmem_page_size();
  void* ptr = vmem_reserve(page_sz);
  EXPECT_NE(nullptr, ptr);

  b32 result = vmem_release(ptr, page_sz);
  EXPECT_NE(0, result);
}

TEST(memory_vmem_test, reserve_commit_release) {
  sz page_sz = vmem_page_size();
  void* ptr = vmem_reserve(page_sz * 2);
  EXPECT_NE(nullptr, ptr);

  b32 commit_result = vmem_commit(ptr, page_sz);
  EXPECT_NE(0, commit_result);

  b32 decommit_result = vmem_decommit(ptr, page_sz);
  EXPECT_NE(0, decommit_result);

  b32 release_result = vmem_release(ptr, page_sz * 2);
  EXPECT_NE(0, release_result);
}

TEST(memory_vmem_test, commit_decommit_multiple) {
  sz page_sz = vmem_page_size();
  void* ptr = vmem_reserve(page_sz * 4);
  EXPECT_NE(nullptr, ptr);

  b32 commit1 = vmem_commit(ptr, page_sz);
  EXPECT_NE(0, commit1);

  u8* off_ptr = (u8*)ptr + page_sz;
  b32 commit2 = vmem_commit(off_ptr, page_sz);
  EXPECT_NE(0, commit2);

  b32 decommit1 = vmem_decommit(ptr, page_sz);
  EXPECT_NE(0, decommit1);

  b32 decommit2 = vmem_decommit(off_ptr, page_sz);
  EXPECT_NE(0, decommit2);

  b32 release = vmem_release(ptr, page_sz * 4);
  EXPECT_NE(0, release);
}

TEST(memory_vmem_test, alloc_basic) {
  sz page_sz = vmem_page_size();
  void* ptr = vmem_alloc(page_sz);
  EXPECT_NE(nullptr, ptr);

  b32 result = vmem_free(ptr, page_sz);
  EXPECT_NE(0, result);
}

TEST(memory_vmem_test, alloc_zero_init) {
  sz size = 4096;
  void* ptr = vmem_calloc(10, size / 10);
  EXPECT_NE(nullptr, ptr);

  u8* byte_ptr = (u8*)ptr;
  bool all_zero = true;
  safe_for (sz i = 0; i < size; i++) {
    if (byte_ptr[i] != 0) {
      all_zero = false;
      break;
    }
  }
  EXPECT_TRUE(all_zero);

  vmem_free(ptr, size);
}

TEST(memory_vmem_test, realloc_basic) {
  sz old_size = 1024;
  void* ptr = vmem_alloc(old_size);
  EXPECT_NE(nullptr, ptr);

  u8* byte_ptr = (u8*)ptr;
  safe_for (sz i = 0; i < 100; i++) {
    byte_ptr[i] = (u8)(i & 0xFF);
  }

  sz new_size = 2048;
  void* new_ptr = vmem_realloc(ptr, old_size, new_size);
  EXPECT_NE(nullptr, new_ptr);

  u8* new_byte_ptr = (u8*)new_ptr;
  bool data_preserved = true;
  safe_for (sz i = 0; i < 100; i++) {
    if (new_byte_ptr[i] != (u8)(i & 0xFF)) {
      data_preserved = false;
      break;
    }
  }
  EXPECT_TRUE(data_preserved);

  vmem_free(new_ptr, new_size);
}

TEST(memory_vmem_test, realloc_shrink) {
  sz old_size = 2048;
  void* ptr = vmem_alloc(old_size);
  EXPECT_NE(nullptr, ptr);

  u8* byte_ptr = (u8*)ptr;
  safe_for (sz i = 0; i < 100; i++) {
    byte_ptr[i] = (u8)((i + 50) & 0xFF);
  }

  sz new_size = 512;
  void* new_ptr = vmem_realloc(ptr, old_size, new_size);
  EXPECT_NE(nullptr, new_ptr);

  u8* new_byte_ptr = (u8*)new_ptr;
  bool data_preserved = true;
  safe_for (sz i = 0; i < 100; i++) {
    if (new_byte_ptr[i] != (u8)((i + 50) & 0xFF)) {
      data_preserved = false;
      break;
    }
  }
  EXPECT_TRUE(data_preserved);

  vmem_free(new_ptr, new_size);
}

TEST(memory_vmem_test, realloc_null) {
  void* ptr = vmem_realloc(nullptr, 0, 1024);
  EXPECT_NE(nullptr, ptr);
  vmem_free(ptr, 1024);
}

TEST(memory_vmem_test, get_stats_initial) {
  vmem_stats stats = vmem_get_stats();
  EXPECT_GT(stats.page_size, 0U);
}

TEST(memory_vmem_test, get_stats_after_operations) {
  vmem_stats stats_before = vmem_get_stats();
  u64 alloc_calls_before = stats_before.alloc_calls;

  sz page_sz = vmem_page_size();
  void* ptr = vmem_alloc(page_sz);
  EXPECT_NE(nullptr, ptr);

  vmem_stats stats_after = vmem_get_stats();
  EXPECT_GT(stats_after.alloc_calls, alloc_calls_before);

  vmem_free(ptr, page_sz);
}

TEST(memory_vmem_test, alloc_larger_than_page) {
  sz large_size = vmem_page_size() * 10;
  void* ptr = vmem_alloc(large_size);
  EXPECT_NE(nullptr, ptr);

  u8* byte_ptr = (u8*)ptr;
  byte_ptr[0] = 0xAB;
  byte_ptr[large_size - 1] = 0xCD;

  EXPECT_EQ(0xAB, byte_ptr[0]);
  EXPECT_EQ(0xCD, byte_ptr[large_size - 1]);

  b32 result = vmem_free(ptr, large_size);
  EXPECT_NE(0, result);
}

TEST(memory_vmem_test, multiple_allocations) {
  void* ptrs[10];
  safe_for (int i = 0; i < 10; i++) {
    ptrs[i] = vmem_alloc(1024);
    EXPECT_NE(nullptr, ptrs[i]);
  }

  safe_for (int i = 0; i < 10; i++) {
    b32 result = vmem_free(ptrs[i], 1024);
    EXPECT_NE(0, result);
  }
}
