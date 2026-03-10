// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"

// =========================================================================
c_begin;
// =========================================================================

// Opaque handle to a reader-writer lock.
typedef void* rwlock;

// Creates a new reader-writer lock and returns a handle to it.
// Multiple threads may hold the read lock simultaneously; the write lock is exclusive.
func rwlock _rwlock_create(callsite site);

// Destroys the given reader-writer lock and releases any associated resources.
func b32 _rwlock_destroy(rwlock rw, callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define rwlock_create()    _rwlock_create(CALLSITE_HERE)
#define rwlock_destroy(rw) _rwlock_destroy(rw, CALLSITE_HERE)

// Returns true if the given reader-writer lock handle is valid, false otherwise.
func b32 rwlock_is_valid(rwlock rw);

// Acquires the lock for reading. Blocks if a writer currently holds the lock.
// Read locks are recursive — the calling thread may acquire multiple read locks.
// Do not request a read lock while holding the write lock on the same rwlock.
func void rwlock_read_lock(rwlock rw);

// Releases a previously acquired read lock.
func void rwlock_read_unlock(rwlock rw);

// Acquires the lock for writing. Blocks until all readers and the previous writer release.
// Write locks are NOT recursive.
// Do not request a write lock while holding a read lock on the same rwlock.
func void rwlock_write_lock(rwlock rw);

// Releases the write lock.
func void rwlock_write_unlock(rwlock rw);

// Tries to acquire the read lock without blocking.
// Returns true if the lock was acquired, false if a writer currently holds the lock.
func b32 rwlock_try_read_lock(rwlock rw);

// Tries to acquire the write lock without blocking.
// Returns true if the lock was acquired, false if any thread currently holds the lock.
func b32 rwlock_try_write_lock(rwlock rw);

// Tries to acquire a read lock until timeout_ms expires.
func b32 rwlock_timed_read_lock(rwlock rw, i32 timeout_ms);

// Tries to acquire a write lock until timeout_ms expires.
func b32 rwlock_timed_write_lock(rwlock rw, i32 timeout_ms);

// =========================================================================
c_end;
// =========================================================================
