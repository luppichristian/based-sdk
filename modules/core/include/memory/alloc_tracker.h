// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../threads/atomics.h"

// =========================================================================
c_begin;
// =========================================================================

// Snapshot of allocation activity accumulated by an allocator tracker.
typedef struct alloc_tracker_stats {
  u64 alloc_calls;
  u64 calloc_calls;
  u64 realloc_calls;
  u64 free_calls;
  u64 live_allocations;
  u64 live_allocated_bytes;
  u64 peak_live_allocated_bytes;
  u64 total_allocated_bytes;
  u64 total_freed_bytes;
} alloc_tracker_stats;

// Called after a successful allocation tracked through an allocator.
typedef void alloc_tracker_callback_alloc(void* user_data, callsite site, void* ptr, sz size);

// Called after a successful deallocation tracked through an allocator.
typedef void alloc_tracker_callback_free(void* user_data, callsite site, void* ptr, sz size);

// Called after a successful reallocation tracked through an allocator.
typedef void alloc_tracker_callback_realloc(void* user_data, callsite site, void* old_ptr, void* new_ptr, sz old_size, sz new_size);

// Stores optional callbacks plus atomic counters for allocator-local memory tracking.
typedef struct alloc_tracker {
  // Custom user data passed to tracker callbacks.
  void* user_data;

  // Optional callbacks fired on successful tracked allocation events.
  alloc_tracker_callback_alloc* alloc_fn;
  alloc_tracker_callback_free* dealloc_fn;
  alloc_tracker_callback_realloc* realloc_fn;

  // API call counters.
  atomic_u64 alloc_calls;
  atomic_u64 calloc_calls;
  atomic_u64 realloc_calls;
  atomic_u64 free_calls;

  // Live and cumulative byte/allocation counters.
  atomic_u64 live_allocations;
  atomic_u64 live_allocated_bytes;
  atomic_u64 peak_live_allocated_bytes;
  atomic_u64 total_allocated_bytes;
  atomic_u64 total_freed_bytes;
} alloc_tracker;

// Creates a zero-initialized allocator tracker.
func alloc_tracker alloc_tracker_create(void);

// Returns the tracker user-data pointer, or NULL when tracker is NULL.
func void* alloc_tracker_get_user_data(alloc_tracker* tracker);

// Replaces the tracker user-data pointer when tracker is valid.
func void alloc_tracker_set_user_data(alloc_tracker* tracker, void* user_data);

// Resets all tracked counters back to zero.
func void alloc_tracker_reset(alloc_tracker* tracker);

// Reads the current tracker counters into a plain snapshot.
func alloc_tracker_stats alloc_tracker_get_stats(alloc_tracker* tracker);

// Records allocator API entry counters.
func void alloc_tracker_on_alloc_call(alloc_tracker* tracker);
func void alloc_tracker_on_calloc_call(alloc_tracker* tracker);
func void alloc_tracker_on_realloc_call(alloc_tracker* tracker);
func void alloc_tracker_on_free_call(alloc_tracker* tracker);

// Records successful allocation-size transitions.
func void alloc_tracker_on_alloc_success(alloc_tracker* tracker, callsite site, void* ptr, sz size);
func void alloc_tracker_on_free_success(alloc_tracker* tracker, callsite site, void* ptr, sz size);
func void alloc_tracker_on_realloc_success(
    alloc_tracker* tracker,
    callsite site,
    void* old_ptr,
    void* new_ptr,
    sz old_size,
    sz new_size);

// =========================================================================
c_end;
// =========================================================================
