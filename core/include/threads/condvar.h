// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "mutex.h"

// Opaque handle to a condition variable.
typedef void* condvar;

// Creates a new condition variable and returns a handle to it.
func condvar _condvar_create(callsite site);

// Destroys the given condition variable and releases any associated resources.
func b32 _condvar_destroy(condvar cond, callsite site);

// Convenience macros that automatically capture the callsite information for debugging purposes.
#define condvar_create()      _condvar_create(CALLSITE_HERE)
#define condvar_destroy(cond) _condvar_destroy(cond, CALLSITE_HERE)

// Returns true if the given condition variable handle is valid, false otherwise.
func b32 condvar_is_valid(condvar cond);

// Atomically releases mtx and blocks on cond until a signal is received.
// mtx must be locked by the calling thread; it is re-acquired before returning.
func void condvar_wait(condvar cond, mutex mtx);

// Like condvar_wait but returns false if millis milliseconds elapse before a signal arrives.
func b32 condvar_wait_timeout(condvar cond, mutex mtx, u32 millis);

// Wakes one thread waiting on cond.
func void condvar_signal(condvar cond);

// Wakes all threads waiting on cond.
func void condvar_broadcast(condvar cond);
