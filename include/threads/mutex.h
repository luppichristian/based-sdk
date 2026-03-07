// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"

// Opaque handle to a mutex.
typedef void* mutex;

// Creates a new mutex and returns a handle to it.
func mutex _mutex_create(callsite site);

// Destroys the given mutex and releases any associated resources.
func b32 _mutex_destroy(mutex mtx, callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define mutex_create()     _mutex_create(CALLSITE_HERE)
#define mutex_destroy(mtx) _mutex_destroy(mtx, CALLSITE_HERE)

// Returns true if the given mutex handle is valid, false otherwise.
func b32 mutex_is_valid(mutex mtx);

// Locks the given mutex.
// If the mutex is already locked by another thread,
// this function will block until the mutex becomes available.
func void mutex_lock(mutex mtx);

// Tries to lock the given mutex without blocking.
// Returns true if the lock was acquired, false if the mutex is already held by another thread.
func b32 mutex_trylock(mutex mtx);

// Tries to lock the mutex until timeout_ms expires.
func b32 mutex_timed_lock(mutex mtx, i32 timeout_ms);

// Unlocks the given mutex.
// The mutex must be locked by the current thread before calling this function.
func void mutex_unlock(mutex mtx);
