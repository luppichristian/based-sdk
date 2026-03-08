// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(context_ctx_test, init_exposes_allocator_log_and_sub_allocators) {
  ctx local_ctx = {0};
  allocator main_alloc = vmem_get_allocator();

  b32 init_ok = ctx_init(&local_ctx, main_alloc, NULL, 0);
  ASSERT_TRUE(init_ok != 0);
  EXPECT_TRUE(ctx_is_init(&local_ctx) != 0);

  allocator ctx_alloc = ctx_get_allocator(&local_ctx);
  EXPECT_NE(ctx_alloc.alloc_fn, nullptr);
  EXPECT_NE(ctx_get_log_state(&local_ctx), nullptr);
  EXPECT_NE(ctx_get_perm_arena(&local_ctx), nullptr);
  EXPECT_NE(ctx_get_temp_arena(&local_ctx), nullptr);
  EXPECT_NE(ctx_get_perm_heap(&local_ctx), nullptr);
  EXPECT_NE(ctx_get_temp_heap(&local_ctx), nullptr);

  ctx_quit(&local_ctx);
  EXPECT_TRUE(ctx_is_init(&local_ctx) == 0);
}

TEST(context_ctx_test, user_data_slots_support_set_get_and_bounds_checks) {
  ctx local_ctx = {0};
  allocator main_alloc = vmem_get_allocator();
  ASSERT_TRUE(ctx_init(&local_ctx, main_alloc, NULL, 0) != 0);

  i32 payload_val = 123;
  ctx_user_data_idx slot_idx = 5;

  EXPECT_TRUE(ctx_set_user_data(&local_ctx, slot_idx, &payload_val) != 0);
  EXPECT_EQ(ctx_get_user_data(&local_ctx, slot_idx), &payload_val);

  EXPECT_EQ(ctx_get_user_data(&local_ctx, CTX_USER_DATA_COUNT), nullptr);
  EXPECT_TRUE(ctx_set_user_data(&local_ctx, CTX_USER_DATA_COUNT, &payload_val) == 0);

  ctx_quit(&local_ctx);
}

TEST(context_ctx_test, clear_temp_resets_temp_allocators_without_destroying_context) {
  ctx local_ctx = {0};
  allocator main_alloc = vmem_get_allocator();
  ASSERT_TRUE(ctx_init(&local_ctx, main_alloc, NULL, 0) != 0);

  arena* temp_arena = ctx_get_temp_arena(&local_ctx);
  heap* temp_heap = ctx_get_temp_heap(&local_ctx);
  ASSERT_NE(temp_arena, nullptr);
  ASSERT_NE(temp_heap, nullptr);

  void* arena_ptr = arena_alloc(temp_arena, 128, align_of(u64));
  void* heap_ptr = heap_alloc(temp_heap, 128, align_of(u64));
  ASSERT_NE(arena_ptr, nullptr);
  ASSERT_NE(heap_ptr, nullptr);

  ctx_clear_temp(&local_ctx);

  void* arena_ptr2 = arena_alloc(temp_arena, 128, align_of(u64));
  void* heap_ptr2 = heap_alloc(temp_heap, 128, align_of(u64));
  EXPECT_NE(arena_ptr2, nullptr);
  EXPECT_NE(heap_ptr2, nullptr);

  ctx_quit(&local_ctx);
}
