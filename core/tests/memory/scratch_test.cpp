// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_scratch_test, begin_end_basic) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);

  i32* val = arena_alloc_array(&arn, i32, 10);
  val[0] = 42;
  val[9] = 99;

  sz used_before = arena_total_used(&arn);
  scratch scr = scratch_begin(&arn);

  i32* temp = arena_alloc_array(&arn, i32, 5);
  temp[0] = 100;

  EXPECT_GT(arena_total_used(&arn), used_before);

  scratch_end(&scr);

  EXPECT_EQ(used_before, arena_total_used(&arn));

  EXPECT_EQ(42, val[0]);
  EXPECT_EQ(99, val[9]);

  arena_destroy(&arn);
}

TEST(memory_scratch_test, multiple_scratches) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);

  i32* first = arena_alloc_array(&arn, i32, 5);
  first[0] = 1;

  scratch scr1 = scratch_begin(&arn);
  i32* second = arena_alloc_array(&arn, i32, 5);
  second[0] = 2;
  sz used_after_second = arena_total_used(&arn);

  scratch scr2 = scratch_begin(&arn);
  i32* third = arena_alloc_array(&arn, i32, 5);
  third[0] = 3;
  sz used_after_third = arena_total_used(&arn);

  EXPECT_GT(used_after_third, used_after_second);

  scratch_end(&scr2);

  EXPECT_EQ(used_after_second, arena_total_used(&arn));
  EXPECT_EQ(1, first[0]);
  EXPECT_EQ(2, second[0]);

  sz used_after_first = arena_total_used(&arn);
  scratch_end(&scr1);

  EXPECT_LT(arena_total_used(&arn), used_after_first);
  EXPECT_EQ(1, first[0]);

  arena_destroy(&arn);
}

TEST(memory_scratch_test, scratch_on_empty_arena) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);

  EXPECT_EQ(0U, arena_total_used(&arn));

  scratch scr = scratch_begin(&arn);

  i32* val = arena_alloc_array(&arn, i32, 10);
  val[0] = 555;

  EXPECT_GT(arena_total_used(&arn), 0U);

  scratch_end(&scr);

  EXPECT_EQ(0U, arena_total_used(&arn));

  arena_destroy(&arn);
}

TEST(memory_scratch_test, nested_allocations_released) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);

  scratch scr = scratch_begin(&arn);

  safe_for (int i = 0; i < 100; i++) {
    void* ptr = arena_alloc(&arn, 128, 8);
    EXPECT_NE(nullptr, ptr);
  }

  sz used_before = arena_total_used(&arn);
  EXPECT_GT(used_before, 0U);

  scratch_end(&scr);

  sz used_after = arena_total_used(&arn);
  EXPECT_LT(used_after, used_before);

  arena_destroy(&arn);
}

TEST(memory_scratch_test, preserve_original_allocation) {
  allocator zero_alloc = {0};
  arena arn = arena_create(zero_alloc, nullptr, 4096);

  u8* original = arena_alloc_array(&arn, u8, 100);
  buffer_set8(buffer_from(original, 100), 0xAA);

  scratch scr = scratch_begin(&arn);

  arena_alloc(&arn, 200, 8);

  scratch_end(&scr);

  bool data_preserved = true;
  safe_for (int i = 0; i < 100; i++) {
    if (original[i] != 0xAA) {
      data_preserved = false;
      break;
    }
  }
  EXPECT_TRUE(data_preserved);

  arena_destroy(&arn);
}
