// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../memory/allocator.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Thread
// =========================================================================

// Opaque handle to a thread.
typedef void* thread;

// Entry-point signature for a thread. Returns an exit code.
typedef i32 (*thread_func)(void* arg);

// Creates a new thread that executes entry(arg) and returns a handle to it.
// If main_allocator is valid, the new thread automatically initializes/quits
// its own thread-local context around the user entry-point using that allocator.
func thread _thread_create(thread_func entry, void* arg, allocator main_allocator, callsite site);

// Creates a new named thread that executes entry(arg) and returns a handle to it.
// The name is used only for debugging purposes (e.g. visible in debuggers/profilers).
// Thread-context automatic lifecycle matches thread_create.
func thread _thread_create_named(
    thread_func entry,
    void* arg,
    cstr8 name,
    allocator main_allocator,
    callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define thread_create(entry, arg, main_allocator) \
  _thread_create(entry, arg, main_allocator, CALLSITE_HERE)
#define thread_create_named(entry, arg, name, main_allocator) \
  _thread_create_named(entry, arg, name, main_allocator, CALLSITE_HERE)

// Returns true if the given thread handle is valid, false otherwise.
func b32 thread_is_valid(thread thd);

// Blocks until the thread finishes, then stores its exit code in out_exit_code (may be NULL).
// Invalidates the handle — do not use thd after this call.
func b32 thread_join(thread thd, i32* out_exit_code);

// Detaches the thread so it cleans up automatically on exit.
// Invalidates the handle — do not join or detach thd again after this call.
func void thread_detach(thread thd);

// Returns the OS-level identifier of the given thread.
func u64 thread_get_id(thread thd);

// Returns the debug name of the given thread, or NULL if none was set.
func cstr8 thread_get_name(thread thd);

// =========================================================================
c_end;
// =========================================================================
