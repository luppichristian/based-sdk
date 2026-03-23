// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "alloc_tracker.h"

// =========================================================================
c_begin;
// =========================================================================

// Reallocates an existing allocation to a new size.
typedef void* allocator_callback_realloc(void* user_data, callsite site, void* ptr, sz new_size);

// Allocates a new memory block of the requested size.
typedef void* allocator_callback_alloc(void* user_data, callsite site, sz size);

// Releases a previously allocated memory block.
typedef void allocator_callback_free(void* user_data, callsite site, void* ptr);

// Describes a generic allocator callback interface with optional tracking state.
typedef struct allocator {
  // Custom user data passed to the callback functions.
  void* user_data;

  // Optional allocation tracker used to collect allocator-local statistics.
  alloc_tracker* tracker;

  // Callback functions for allocation, deallocation, and reallocation.
  allocator_callback_alloc* alloc_fn;
  allocator_callback_free* dealloc_fn;
  allocator_callback_realloc* realloc_fn;
} allocator;

// Returns the allocator user-data pointer, or NULL when alloc is NULL.
func void* allocator_get_user_data(allocator* alloc);

// Replaces the allocator user-data pointer when alloc is valid.
func void allocator_set_user_data(allocator* alloc, void* user_data);

// Returns the attached allocation tracker, or NULL when none is set.
func alloc_tracker* allocator_get_tracker(allocator* alloc);

// Attaches or clears the allocation tracker when alloc is valid.
func void allocator_set_tracker(allocator* alloc, alloc_tracker* tracker);

// Allocates a block of memory of the given size using the provided allocator.
func void* _allocator_alloc(allocator alloc, sz size, callsite site);

// Allocates a block of memory for an array of elements, initializing it to zero.
func void* _allocator_calloc(allocator alloc, sz count, sz size, callsite site);

// Deallocates a previously allocated block of memory using the provided allocator.
func void _allocator_dealloc(allocator alloc, void* ptr, callsite site);

// Reallocates a block of memory to a new size using the provided allocator.
func void* _allocator_realloc(allocator alloc, void* ptr, sz new_size, callsite site);

// Macros for convenient allocation and deallocation with callsite information.
#define allocator_alloc(alloc, size) \
  _allocator_alloc((alloc), size, CALLSITE_HERE)
#define allocator_dealloc(alloc, ptr) \
  _allocator_dealloc((alloc), ptr, CALLSITE_HERE)
#define allocator_calloc(alloc, count, size) \
  _allocator_calloc((alloc), count, size, CALLSITE_HERE)
#define allocator_realloc(alloc, ptr, new_size) \
  _allocator_realloc((alloc), ptr, new_size, CALLSITE_HERE)

// Helpers for type, array allocation are not provided because you shouldnt
// use this for small allocations.

// =========================================================================
c_end;
// =========================================================================
