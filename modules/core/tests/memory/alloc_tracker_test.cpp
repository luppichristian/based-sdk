// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {
  struct alloc_tracker_callback_state {
    void* user_data;
    void* alloc_ptr;
    void* free_ptr;
    void* realloc_old_ptr;
    void* realloc_new_ptr;
    sz alloc_size;
    sz free_size;
    sz realloc_old_size;
    sz realloc_new_size;
    callsite alloc_site;
    callsite free_site;
    callsite realloc_site;
    u64 alloc_count;
    u64 free_count;
    u64 realloc_count;
  };

  void alloc_tracker_test_alloc_callback(void* user_data, callsite site, void* ptr, sz size) {
    alloc_tracker_callback_state* state = (alloc_tracker_callback_state*)user_data;
    state->user_data = user_data;
    state->alloc_ptr = ptr;
    state->alloc_size = size;
    state->alloc_site = site;
    state->alloc_count += 1;
  }

  void alloc_tracker_test_free_callback(void* user_data, callsite site, void* ptr, sz size) {
    alloc_tracker_callback_state* state = (alloc_tracker_callback_state*)user_data;
    state->free_ptr = ptr;
    state->free_size = size;
    state->free_site = site;
    state->free_count += 1;
  }

  void alloc_tracker_test_realloc_callback(
      void* user_data,
      callsite site,
      void* old_ptr,
      void* new_ptr,
      sz old_size,
      sz new_size) {
    alloc_tracker_callback_state* state = (alloc_tracker_callback_state*)user_data;
    state->realloc_old_ptr = old_ptr;
    state->realloc_new_ptr = new_ptr;
    state->realloc_old_size = old_size;
    state->realloc_new_size = new_size;
    state->realloc_site = site;
    state->realloc_count += 1;
  }
}  // namespace

TEST(memory_alloc_tracker_test, create_starts_zeroed) {
  alloc_tracker tracker = alloc_tracker_create();
  alloc_tracker_stats stats = alloc_tracker_get_stats(&tracker);

  EXPECT_EQ(nullptr, alloc_tracker_get_user_data(&tracker));
  EXPECT_EQ(0U, stats.alloc_calls);
  EXPECT_EQ(0U, stats.calloc_calls);
  EXPECT_EQ(0U, stats.realloc_calls);
  EXPECT_EQ(0U, stats.free_calls);
  EXPECT_EQ(0U, stats.live_allocations);
  EXPECT_EQ(0U, stats.live_allocated_bytes);
  EXPECT_EQ(0U, stats.peak_live_allocated_bytes);
  EXPECT_EQ(0U, stats.total_allocated_bytes);
  EXPECT_EQ(0U, stats.total_freed_bytes);
}

TEST(memory_alloc_tracker_test, allocator_updates_tracker_stats) {
  alloc_tracker tracker = alloc_tracker_create();
  allocator alloc = vmem_get_allocator();
  alloc.tracker = &tracker;

  void* ptr = allocator_calloc(alloc, 4, 8);
  ASSERT_NE(nullptr, ptr);

  alloc_tracker_stats stats = alloc_tracker_get_stats(&tracker);
  EXPECT_EQ(1U, stats.alloc_calls);
  EXPECT_EQ(1U, stats.calloc_calls);
  EXPECT_EQ(0U, stats.realloc_calls);
  EXPECT_EQ(0U, stats.free_calls);
  EXPECT_EQ(1U, stats.live_allocations);
  EXPECT_EQ(32U, stats.live_allocated_bytes);
  EXPECT_EQ(32U, stats.peak_live_allocated_bytes);
  EXPECT_EQ(32U, stats.total_allocated_bytes);
  EXPECT_EQ(0U, stats.total_freed_bytes);

  ptr = allocator_realloc(alloc, ptr, 48);
  ASSERT_NE(nullptr, ptr);

  stats = alloc_tracker_get_stats(&tracker);
  EXPECT_EQ(1U, stats.alloc_calls);
  EXPECT_EQ(1U, stats.calloc_calls);
  EXPECT_EQ(1U, stats.realloc_calls);
  EXPECT_EQ(0U, stats.free_calls);
  EXPECT_EQ(1U, stats.live_allocations);
  EXPECT_EQ(48U, stats.live_allocated_bytes);
  EXPECT_EQ(48U, stats.peak_live_allocated_bytes);
  EXPECT_EQ(48U, stats.total_allocated_bytes);
  EXPECT_EQ(0U, stats.total_freed_bytes);

  allocator_dealloc(alloc, ptr);

  stats = alloc_tracker_get_stats(&tracker);
  EXPECT_EQ(1U, stats.alloc_calls);
  EXPECT_EQ(1U, stats.calloc_calls);
  EXPECT_EQ(1U, stats.realloc_calls);
  EXPECT_EQ(1U, stats.free_calls);
  EXPECT_EQ(0U, stats.live_allocations);
  EXPECT_EQ(0U, stats.live_allocated_bytes);
  EXPECT_EQ(48U, stats.peak_live_allocated_bytes);
  EXPECT_EQ(48U, stats.total_allocated_bytes);
  EXPECT_EQ(48U, stats.total_freed_bytes);
}

TEST(memory_alloc_tracker_test, reset_clears_live_and_total_counters) {
  alloc_tracker tracker = alloc_tracker_create();
  allocator alloc = vmem_get_allocator();
  alloc.tracker = &tracker;

  void* ptr = allocator_alloc(alloc, 24);
  ASSERT_NE(nullptr, ptr);

  alloc_tracker_reset(&tracker);

  alloc_tracker_stats stats = alloc_tracker_get_stats(&tracker);
  EXPECT_EQ(0U, stats.alloc_calls);
  EXPECT_EQ(0U, stats.calloc_calls);
  EXPECT_EQ(0U, stats.realloc_calls);
  EXPECT_EQ(0U, stats.free_calls);
  EXPECT_EQ(0U, stats.live_allocations);
  EXPECT_EQ(0U, stats.live_allocated_bytes);
  EXPECT_EQ(0U, stats.peak_live_allocated_bytes);
  EXPECT_EQ(0U, stats.total_allocated_bytes);
  EXPECT_EQ(0U, stats.total_freed_bytes);

  allocator_dealloc(alloc, ptr);
}

TEST(memory_alloc_tracker_test, callbacks_receive_allocation_events) {
  alloc_tracker_callback_state state = {0};
  alloc_tracker tracker = alloc_tracker_create();
  tracker.user_data = &state;
  tracker.alloc_fn = alloc_tracker_test_alloc_callback;
  tracker.dealloc_fn = alloc_tracker_test_free_callback;
  tracker.realloc_fn = alloc_tracker_test_realloc_callback;

  allocator alloc = vmem_get_allocator();
  alloc.tracker = &tracker;

  void* ptr = allocator_alloc(alloc, 16);
  ASSERT_NE(nullptr, ptr);
  EXPECT_EQ(&state, state.user_data);
  EXPECT_EQ(ptr, state.alloc_ptr);
  EXPECT_EQ(16U, state.alloc_size);
  EXPECT_EQ(1U, state.alloc_count);
  EXPECT_NE(nullptr, state.alloc_site.filename);

  void* old_ptr = ptr;
  ptr = allocator_realloc(alloc, ptr, 40);
  ASSERT_NE(nullptr, ptr);
  EXPECT_EQ(old_ptr, state.realloc_old_ptr);
  EXPECT_EQ(ptr, state.realloc_new_ptr);
  EXPECT_EQ(16U, state.realloc_old_size);
  EXPECT_EQ(40U, state.realloc_new_size);
  EXPECT_EQ(1U, state.realloc_count);
  EXPECT_NE(nullptr, state.realloc_site.filename);

  allocator_dealloc(alloc, ptr);
  EXPECT_EQ(ptr, state.free_ptr);
  EXPECT_EQ(40U, state.free_size);
  EXPECT_EQ(1U, state.free_count);
  EXPECT_NE(nullptr, state.free_site.filename);
}

TEST(memory_alloc_tracker_test, user_data_get_set) {
  alloc_tracker tracker = alloc_tracker_create();
  i32 value = 7;

  EXPECT_EQ(nullptr, alloc_tracker_get_user_data(&tracker));

  alloc_tracker_set_user_data(&tracker, &value);

  EXPECT_EQ(&value, alloc_tracker_get_user_data(&tracker));
}
