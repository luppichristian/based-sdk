// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "ctx.h"

// Backward-compatible alias for the context user-data slot count.
#define THREAD_CTX_USER_DATA_COUNT CTX_USER_DATA_COUNT

// Returns the current thread's context, or NULL when this thread has not been initialized.
func ctx* thread_ctx_get(void);

// Returns true when the current thread has an initialized context.
func b32 thread_ctx_is_init(void);

// Initializes the current thread's context. This is thread-local: each thread
// that wants a context must call it for itself.
func b32 thread_ctx_init(allocator main_allocator);

// Destroys the current thread's context and releases owned resources.
func void thread_ctx_quit(void);

// Returns an allocator backed by the current thread's permanent heap.
// If the context is not initialized, a zeroed allocator is returned.
func allocator thread_get_allocator(void);

// Returns the current thread's log state, or the process-global fallback state
// when this thread has no initialized context.
func log_state* thread_get_log_state(void);

// Returns pointers to the current thread's allocators, or NULL if uninitialized.
func arena* thread_get_perm_arena(void);
func arena* thread_get_temp_arena(void);
func heap* thread_get_perm_heap(void);
func heap* thread_get_temp_heap(void);

// Accesses one of the generic user-data slots.
// Out-of-range access returns NULL / false.
func void* thread_get_user_data(ctx_user_data_idx idx);
// Writes one generic user-data slot for the current thread.
// Out-of-range writes return false.
func b32 thread_set_user_data(ctx_user_data_idx idx, void* user_data);

// Clears the temporary allocators for the current thread.
func void thread_clear_temp(void);

// Convenience wrapper for configuring the current thread's effective log state.
func void thread_log_set_level(log_level level);

// Moves retained root-frame messages from the global context log state into
// the current thread context log state. Returns false if the current thread has
// no initialized context.
func b32 thread_log_sync(void);

// Thread-context wrappers for nested log frames.
func void thread_log_begin_frame(void);
func log_frame* thread_log_end_frame(u32 severity_mask);

// Convenience macros for logging against the current thread's effective log state.
#define thread_log_fatal(...)   _log(thread_get_log_state(), LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_error(...)   _log(thread_get_log_state(), LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_warn(...)    _log(thread_get_log_state(), LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_info(...)    _log(thread_get_log_state(), LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_debug(...)   _log(thread_get_log_state(), LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_verbose(...) _log(thread_get_log_state(), LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define thread_log_trace(...)   _log(thread_get_log_state(), LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)
