// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"

// =========================================================================
c_begin;
// =========================================================================

// Opaque handle to a spinlock.
// Spinlocks busy-wait and are only appropriate for very short critical sections where
// the overhead of an OS mutex is unjustified. Prefer mutex for longer or blocking operations.
// Spinlocks must NOT be locked recursively.
typedef void* spinlock;

// Creates a new spinlock in the unlocked state.
func spinlock _spinlock_create(callsite site);

// Destroys the spinlock and releases its resources.
func void _spinlock_destroy(spinlock sl, callsite site);

// Convenience macros that automatically pass the callsite information.
#define spinlock_create()    _spinlock_create(CALLSITE_HERE)
#define spinlock_destroy(sl) _spinlock_destroy(sl, CALLSITE_HERE)

// Returns true if the spinlock handle is valid, false otherwise.
func b32 spinlock_is_valid(spinlock sl);

// Acquires the spinlock, busy-spinning until it becomes available.
func void spinlock_lock(spinlock sl);

// Releases the spinlock.
func void spinlock_unlock(spinlock sl);

// Tries to acquire the spinlock without spinning.
// Returns true if the lock was acquired, false if it is already held.
func b32 spinlock_try_lock(spinlock sl);

// =========================================================================
c_end;
// =========================================================================
