// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/log.h"
#include "../memory/arena.h"
#include "../memory/heap.h"

// Fixed number of generic user-data slots available in each context.
const_var sz CTX_USER_DATA_COUNT = 32;
typedef sz ctx_user_data_idx;

// Shared context payload used by both thread-local and global context wrappers.
typedef struct ctx {
  // Set to true after ctx_init succeeds.
  b32 is_init;

  // Parent allocator used to grow all context-owned allocators.
  allocator main_allocator;

  // Long-lived bump allocator for context-affine scratch reclaimed in bulk.
  arena perm_arena;

  // Short-lived bump allocator that should be reset frequently.
  arena temp_arena;

  // Long-lived general-purpose allocator for context-local allocations.
  heap perm_heap;

  // Short-lived general-purpose allocator that should be reset frequently.
  heap temp_heap;

  // Context-local logging configuration and retained frames.
  log_state log;

  // Opaque per-context user storage for higher-level systems.
  void* user_data[CTX_USER_DATA_COUNT];
} ctx;

// Initializes a context payload using main_allocator.
// When use_log_mutex is true, the embedded log_state owns a mutex.
func b32 ctx_init(ctx* context, allocator main_allocator, b32 use_log_mutex);

// Destroys a context payload and releases owned allocator resources.
func void ctx_quit(ctx* context);

// Returns true if context is non-NULL and initialized.
func b32 ctx_is_init(ctx* context);

// Returns an allocator backed by context->perm_heap.
// Returns a zeroed allocator when context is invalid or uninitialized.
func allocator ctx_get_allocator(ctx* context);

// Returns the embedded log_state pointer or NULL when uninitialized.
func log_state* ctx_get_log_state(ctx* context);

// Returns pointers to owned allocators, or NULL when uninitialized.
func arena* ctx_get_perm_arena(ctx* context);
func arena* ctx_get_temp_arena(ctx* context);
func heap* ctx_get_perm_heap(ctx* context);
func heap* ctx_get_temp_heap(ctx* context);

// Accesses one generic user-data slot.
// Out-of-range access returns NULL / false.
func void* ctx_get_user_data(ctx* context, ctx_user_data_idx idx);
func b32 ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data);

// Clears temporary allocators in the context.
func void ctx_clear_temp(ctx* context);
