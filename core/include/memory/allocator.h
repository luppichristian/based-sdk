// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"

// A flexible memory allocator interface that allows users to provide custom allocation strategies.
typedef void* allocator_callback_realloc(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size);
typedef void* allocator_callback_alloc(
    void* user_data,
    callsite site,
    sz size);
typedef void allocator_callback_free(
    void* user_data,
    callsite site,
    void* ptr);

typedef struct allocator {
  // Custom user data passed to the callback functions.
  void* user_data;

  // Callback functions for allocation, deallocation, and reallocation.
  // If reallocation is not provided, it will be emulated using alloc and free.
  allocator_callback_alloc* alloc_fn;
  allocator_callback_free* dealloc_fn;
  allocator_callback_realloc* realloc_fn;
} allocator;

// Allocates a block of memory of the given size using the provided allocator.
func void* _allocator_alloc(allocator* alloc, sz size, callsite site);

// Allocates a block of memory for an array of elements, initializing it to zero.
func void* _allocator_calloc(allocator* alloc, sz count, sz size, callsite site);

// Deallocates a previously allocated block of memory using the provided allocator.
func void _allocator_dealloc(allocator* alloc, void* ptr, sz size, callsite site);

// Reallocates a block of memory to a new size using the provided allocator.
func void* _allocator_realloc(allocator* alloc, void* ptr, sz old_size, sz new_size, callsite site);

// Macros for convenient allocation and deallocation with callsite information.
#define allocator_alloc(alloc, size) \
  _allocator_alloc(alloc, size, CALLSITE_HERE)
#define allocator_dealloc(alloc, ptr, size) \
  _allocator_dealloc(alloc, ptr, size, CALLSITE_HERE)
#define allocator_calloc(alloc, count, size) \
  _allocator_calloc(alloc, count, size, CALLSITE_HERE)
#define allocator_realloc(alloc, ptr, old_size, new_size) \
  _allocator_realloc(alloc, ptr, old_size, new_size, CALLSITE_HERE)