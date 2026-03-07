// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "thread.h"

// =========================================================================
// Thread Group
// =========================================================================

// Entry-point for a thread group member.
// idx is the thread's zero-based position within the group.
// arg   is the shared user argument passed to thread_group_create.
typedef i32 (*thread_group_func)(u32 idx, void* arg);

// Per-thread context stored alongside each thread handle. Internal — do not access directly.
typedef struct {
  thread_group_func entry;
  void* arg;
  u32 idx;
} thread_group_slot;

// A fixed-size group of threads all running the same entry function.
// Initialize with thread_group_create; zero-initialize to represent an empty/invalid group.
typedef struct {
  thread* threads;
  thread_group_slot* slots;
  u32 count;
} thread_group;

// Creates a group of count threads, all executing entry(idx, arg).
// Threads start immediately; idx runs from 0 to count-1.
// Returns the group by value; check with thread_group_is_valid on failure.
func thread_group _thread_group_create(u32 count, thread_group_func entry, void* arg, callsite site);

// Like thread_group_create, but each thread is named "<base_name>[<idx>]".
// Names are visible in debuggers and profilers.
func thread_group _thread_group_create_named(
    u32 count,
    thread_group_func entry,
    void* arg,
    cstr8 base_name,
    callsite site);

// Frees internal resources and zeroes the group.
// Passing NULL is safe and does nothing.
func void _thread_group_destroy(thread_group* group, callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define thread_group_create(count, entry, arg) \
  _thread_group_create(count, entry, arg, CALLSITE_HERE)
#define thread_group_create_named(count, entry, arg, base_name) \
  _thread_group_create_named(count, entry, arg, base_name, CALLSITE_HERE)
#define thread_group_destroy(group) _thread_group_destroy(group, CALLSITE_HERE)

// Returns true if the group was successfully created and holds live thread handles.
func b32 thread_group_is_valid(thread_group* group);

// Returns the number of threads in the group.
func u32 thread_group_get_count(thread_group* group);

// Returns the thread handle at the given idx, or NULL if the idx is out of range.
func thread thread_group_get(thread_group* group, u32 idx);

// Blocks until every thread in the group has finished.
// If out_exit_codes is non-NULL it must point to an array of at least thread_group_get_count() i32s;
// each element is written with the corresponding thread's exit code in idx order.
// Returns true if all joins succeeded, false otherwise.
func b32 thread_group_join_all(thread_group* group, i32* out_exit_codes);

// Detaches all threads in the group so they clean up automatically on exit.
func void thread_group_detach_all(thread_group* group);
