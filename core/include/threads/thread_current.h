// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "thread.h"

// =========================================================================
c_begin;
// =========================================================================

// Scheduling priority levels for a thread.
typedef enum thread_priority {
  THREAD_PRIORITY_LOW,
  THREAD_PRIORITY_MEDIUM,
  THREAD_PRIORITY_HIGH,
  THREAD_PRIORITY_CRITICAL,
} thread_priority;

// Returns the OS-level identifier of the calling thread.
func u64 thread_id(void);

// Returns the scheduling priority of the calling thread.
// Reflects the last value set via thread_set_priority; defaults to THREAD_PRIORITY_MEDIUM.
func thread_priority thread_get_priority(void);

// Sets the scheduling priority of the calling thread.
func b32 thread_set_priority(thread_priority priority);

// Suspends the calling thread for at least millis milliseconds.
func void thread_sleep(u32 millis);

// Yields the calling thread's remaining time slice to the OS scheduler.
func void thread_yield(void);

// Suspends the calling thread for at least nanos nanoseconds.
func void thread_sleep_ns(u64 nanos);

// Suspends the calling thread for nanos nanoseconds, busy-spinning the
// remainder for high accuracy. Prefer thread_sleep / thread_sleep_ns for
// long durations to avoid wasting CPU.
func void thread_sleep_precise(u64 nanos);

// =========================================================================
c_end;
// =========================================================================
