// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../utils/log_state.h"
#include "../memory/arena.h"
#include "../memory/heap.h"
#include "../threads/mutex.h"

// =========================================================================
c_begin;
// =========================================================================

// Fixed number of generic user-data slots available in each context.
#define CTX_USER_DATA_COUNT 32
typedef sz ctx_user_data_idx;

typedef struct ctx_setup {
  // Parent allocator used to initialize log state and grow enabled allocators.
  allocator main_allocator;

  // Optional mutex shared by enabled arena/heap allocators.
  mutex allocator_mutex;

  // When true, the embedded log_state creates and owns its own mutex.
  b32 use_log_mutex;

  // Enables permanent and temporary arena allocators for this context.
  b32 use_arena_allocs;

  // Enables permanent and temporary heap allocators for this context.
  b32 use_heap_allocs;

  // Enables temporary allocators in addition to permanent ones.
  b32 use_temp_allocs;

  // Default block size used when growing the permanent arena.
  sz perm_arena_block_size;

  // Default block size used when growing the temporary arena.
  sz temp_arena_block_size;

  // Default block size used when growing the permanent heap.
  sz perm_heap_block_size;

  // Default block size used when growing the temporary heap.
  sz temp_heap_block_size;
} ctx_setup;

// Checks if the current setup is valid for a context creation.
func b32 ctx_setup_is_valid(ctx_setup* setup);

// Fills missing setup fields with project defaults.
// A zeroed setup enables arena, heap, and temp allocators by default.
func void ctx_setup_fill_defaults(ctx_setup* setup);

// Shared context payload used by both thread-local and global context wrappers.
typedef struct ctx {
  // Set to true after ctx_init succeeds.
  b32 is_init;

  // Initialization settings used to configure the context-owned subsystems.
  ctx_setup setup;

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

// Initializes a context payload
func b32 ctx_init(ctx* context, ctx_setup setup);

// Destroys a context payload and releases owned allocator resources.
func b32 ctx_quit(ctx* context);

// Returns true if context is non-NULL and initialized.
func b32 ctx_is_init(ctx* context);

// Returns an allocator backed by context->perm_heap.
// Returns a zeroed allocator when context is invalid or uninitialized.
func allocator ctx_get_allocator(ctx* context);

// Returns the context initialization settings, or a zeroed setup when invalid.
func ctx_setup ctx_get_setup(ctx* context);

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
// Writes one generic user-data slot.
// Out-of-range writes return false.
func b32 ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data);

// Clears temporary allocators in the context.
func void ctx_clear_temp(ctx* context);

// =========================================================================
c_end;
// =========================================================================
