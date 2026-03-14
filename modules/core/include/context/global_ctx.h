// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "ctx.h"

// =========================================================================
c_begin;
// =========================================================================

// Initializes the process-global context singleton.
// Returns true if already initialized.
func b32 global_ctx_init(ctx_setup setup);

// Destroys the process-global context singleton.
func b32 global_ctx_quit(void);

// Returns true when the global context singleton is initialized.
func b32 global_ctx_is_init(void);

// Returns a pointer to the initialized process-global context payload.
func ctx* global_ctx_get(void);

// Explicit lock helpers for coordinated multi-step access to global_ctx_get().
func void global_ctx_lock(void);
func void global_ctx_unlock(void);

// Convenience wrappers for the shared context payload.
func allocator global_get_allocator(void);
func ctx_setup global_get_setup(void);
func allocator global_get_main_allocator(void);
func log_state* global_get_log_state(void);
func arena* global_get_perm_arena(void);
func arena* global_get_temp_arena(void);
func heap* global_get_perm_heap(void);
func heap* global_get_temp_heap(void);

// User-data access is protected by per-thread permissions.
// By default, the thread that initializes the global context has access to every slot;
// other threads start with no access.
func b32 global_has_user_data_access(ctx_user_data_idx idx);
func void* global_get_user_data(ctx_user_data_idx idx);
func b32 global_set_user_data(ctx_user_data_idx idx, void* user_data);

// Permission helpers for the calling thread.
func b32 global_set_user_data_access(ctx_user_data_idx idx, b8 has_access);
func void global_set_user_data_access_all(b8 has_access);

// Clears temporary allocators in the shared context.
func void global_clear_temp(void);

// Convenience wrapper for configuring the global effective log state.
func void global_log_set_level(log_level level);

// Global-context wrappers for nested log frames.
func void global_log_begin_frame(void);
func log_frame* global_log_end_frame(u32 severity_mask);

// Convenience macros for logging to the process-global main log state.
#define global_log_fatal(...)   _log(global_get_log_state(), LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define global_log_error(...)   _log(global_get_log_state(), LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define global_log_warn(...)    _log(global_get_log_state(), LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define global_log_info(...)    _log(global_get_log_state(), LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define global_log_debug(...)   _log(global_get_log_state(), LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define global_log_verbose(...) _log(global_get_log_state(), LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define global_log_trace(...)   _log(global_get_log_state(), LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)

// =========================================================================
c_end;
// =========================================================================
