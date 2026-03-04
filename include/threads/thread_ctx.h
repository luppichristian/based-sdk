// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/log.h"
#include "../memory/arena.h"
#include "../memory/heap.h"

// =========================================================================
// Thread Context
// =========================================================================

// Fixed number of generic user-data slots available in each thread-local context.
const_var sz THREAD_CTX_USER_DATA_COUNT = 32;

typedef struct thread_ctx {
  // Set to true after thread_ctx_init succeeds for the current thread.
  b32 is_initialized;

  // Parent allocator used to grow all context-owned allocators.
  allocator main_allocator;

  // Long-lived bump allocator for thread-affine scratch that is reclaimed in bulk.
  arena perm_arena;

  // Short-lived bump allocator that should be reset frequently with thread_ctx_clear_temp.
  arena temp_arena;

  // Long-lived general-purpose allocator for thread-local allocations.
  heap perm_heap;

  // Short-lived general-purpose allocator that should be reset frequently.
  heap temp_heap;

  // Per-thread logging configuration.
  log_state log;

  // Opaque per-thread user storage for higher-level systems.
  void* user_data[THREAD_CTX_USER_DATA_COUNT];
} thread_ctx;

// Returns the current thread's context, or NULL when this thread has not been initialized.
func thread_ctx* thread_ctx_get(void);

// Returns true when the current thread has an initialized context.
func b32 thread_ctx_is_initialized(void);

// Returns an allocator backed by the current thread's permanent heap.
// If the context is not initialized, a zeroed allocator is returned.
func allocator thread_ctx_get_allocator(void);

// Returns the current thread's log state, or the process-global fallback state
// when this thread has no initialized context.
func log_state* thread_ctx_get_log_state(void);

// Returns pointers to the current thread's allocators, or NULL if uninitialized.
func arena* thread_ctx_get_perm_arena(void);
func arena* thread_ctx_get_temp_arena(void);
func heap* thread_ctx_get_perm_heap(void);
func heap* thread_ctx_get_temp_heap(void);

// Accesses one of the generic user-data slots.
// Out-of-range access returns NULL / false.
func void* thread_ctx_get_user_data(sz index);
func b32 thread_ctx_set_user_data(sz index, void* user_data);

// Clears the temporary allocators for the current thread.
func void thread_ctx_clear_temp(void);

// Convenience wrappers for configuring the current thread's effective log state.
func void thread_ctx_log_set_level(log_level level);
func void thread_ctx_log_set_callback(log_callback callback);

// Moves the process-global retained root-frame messages into the current
// thread_ctx log state. The current thread's callback and severity level are
// left unchanged.
// Returns false if the current thread has no initialized thread_ctx.
func b32 thread_ctx_log_sync_from_main(void);

// Thread-context wrappers for nested log frames.
func void thread_ctx_log_begin_frame(void);
func log_frame* thread_ctx_log_end_frame(u32 severity_mask);

// Convenience macros for logging against the current thread's effective log state.
#define thread_log_fatal(...)   _log(thread_ctx_get_log_state(), LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_error(...)   _log(thread_ctx_get_log_state(), LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_warn(...)    _log(thread_ctx_get_log_state(), LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_info(...)    _log(thread_ctx_get_log_state(), LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_debug(...)   _log(thread_ctx_get_log_state(), LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_verbose(...) _log(thread_ctx_get_log_state(), LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_trace(...)   _log(thread_ctx_get_log_state(), LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)

// Initializes the current thread's context. This is thread-local: each thread
// that wants a context must call it for itself. Returns false if the allocator
// is invalid, if this thread has already been initialized, or if the internal
// log state could not be created.
func b32 thread_ctx_init(allocator main_allocator);

// Destroys the current thread's context and releases any blocks owned by its allocators.
// Safe to call even if the current thread was never initialized.
func void thread_ctx_quit(void);
