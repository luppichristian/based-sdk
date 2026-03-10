// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Atomic Types
// =========================================================================

// All fields must be accessed exclusively through the functions below.
// Direct field access bypasses the atomic guarantees.

typedef struct atomic_i32 {
  i32 val;
} atomic_i32;

typedef struct atomic_u32 {
  u32 val;
} atomic_u32;

typedef struct atomic_i64 {
  i64 val;
} atomic_i64;

typedef struct atomic_u64 {
  u64 val;
} atomic_u64;

// Atomic pointer — a void* that can be exchanged across threads.
typedef struct atomic_ptr {
  void* val;
} atomic_ptr;

typedef enum atomic_memory_order {
  ATOMIC_MEMORY_ORDER_RELAXED = 0,
  ATOMIC_MEMORY_ORDER_CONSUME = 1,
  ATOMIC_MEMORY_ORDER_ACQUIRE = 2,
  ATOMIC_MEMORY_ORDER_RELEASE = 3,
  ATOMIC_MEMORY_ORDER_ACQ_REL = 4,
  ATOMIC_MEMORY_ORDER_SEQ_CST = 5,
} atomic_memory_order;

// =========================================================================
// atomic_i32
// =========================================================================

// Atomically loads and returns the current value.
func i32 atomic_i32_get(atomic_i32* atom);
func i32 atomic_i32_get_explicit(atomic_i32* atom, atomic_memory_order order);

// Atomically replaces the value with val and returns the previous value.
func i32 atomic_i32_set(atomic_i32* atom, i32 val);
func i32 atomic_i32_set_explicit(atomic_i32* atom, i32 val, atomic_memory_order order);

// If the current value equals *expected, replaces it with desired and returns true.
// On failure, writes the current value into *expected and returns false.
// This is also known as "compare-and-swap" or "compare-and-exchange".
func b32 atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired);

// Atomically adds delta and returns the value before the addition.
func i32 atomic_i32_add(atomic_i32* atom, i32 delta);
func i32 atomic_i32_and(atomic_i32* atom, i32 mask);
func i32 atomic_i32_or(atomic_i32* atom, i32 mask);
func i32 atomic_i32_xor(atomic_i32* atom, i32 mask);

// Atomically subtracts delta and returns the value before the subtraction.
func i32 atomic_i32_sub(atomic_i32* atom, i32 delta);

// Compare the current value against val (via a single atomic load).
func b32 atomic_i32_eq(atomic_i32* atom, i32 val);
func b32 atomic_i32_neq(atomic_i32* atom, i32 val);
func b32 atomic_i32_lt(atomic_i32* atom, i32 val);
func b32 atomic_i32_gt(atomic_i32* atom, i32 val);
func b32 atomic_i32_lte(atomic_i32* atom, i32 val);
func b32 atomic_i32_gte(atomic_i32* atom, i32 val);

// =========================================================================
// atomic_u32
// =========================================================================

// Atomically loads and returns the current value.
func u32 atomic_u32_get(atomic_u32* atom);
func u32 atomic_u32_get_explicit(atomic_u32* atom, atomic_memory_order order);

// Atomically replaces the value with val and returns the previous value.
func u32 atomic_u32_set(atomic_u32* atom, u32 val);
func u32 atomic_u32_set_explicit(atomic_u32* atom, u32 val, atomic_memory_order order);

// If the current value equals *expected, replaces it with desired and returns true.
// On failure, writes the current value into *expected and returns false.
// This is also known as "compare-and-swap" or "compare-and-exchange".
func b32 atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired);

// Atomically adds delta and returns the value before the addition.
func u32 atomic_u32_add(atomic_u32* atom, u32 delta);
func u32 atomic_u32_and(atomic_u32* atom, u32 mask);
func u32 atomic_u32_or(atomic_u32* atom, u32 mask);
func u32 atomic_u32_xor(atomic_u32* atom, u32 mask);

// Atomically subtracts delta and returns the value before the subtraction.
func u32 atomic_u32_sub(atomic_u32* atom, u32 delta);

// Compare the current value against val (via a single atomic load).
func b32 atomic_u32_eq(atomic_u32* atom, u32 val);
func b32 atomic_u32_neq(atomic_u32* atom, u32 val);
func b32 atomic_u32_lt(atomic_u32* atom, u32 val);
func b32 atomic_u32_gt(atomic_u32* atom, u32 val);
func b32 atomic_u32_lte(atomic_u32* atom, u32 val);
func b32 atomic_u32_gte(atomic_u32* atom, u32 val);

// =========================================================================
// atomic_i64
// =========================================================================

// Atomically loads and returns the current value.
func i64 atomic_i64_get(atomic_i64* atom);
func i64 atomic_i64_get_explicit(atomic_i64* atom, atomic_memory_order order);

// Atomically replaces the value with val and returns the previous value.
func i64 atomic_i64_set(atomic_i64* atom, i64 val);
func i64 atomic_i64_set_explicit(atomic_i64* atom, i64 val, atomic_memory_order order);

// If the current value equals *expected, replaces it with desired and returns true.
// On failure, writes the current value into *expected and returns false.
func b32 atomic_i64_cmpex(atomic_i64* atom, i64* expected, i64 desired);

// Atomically adds delta and returns the value before the addition.
func i64 atomic_i64_add(atomic_i64* atom, i64 delta);
func i64 atomic_i64_and(atomic_i64* atom, i64 mask);
func i64 atomic_i64_or(atomic_i64* atom, i64 mask);
func i64 atomic_i64_xor(atomic_i64* atom, i64 mask);

// Atomically subtracts delta and returns the value before the subtraction.
func i64 atomic_i64_sub(atomic_i64* atom, i64 delta);

// Compare the current value against val (via a single atomic load).
func b32 atomic_i64_eq(atomic_i64* atom, i64 val);
func b32 atomic_i64_neq(atomic_i64* atom, i64 val);
func b32 atomic_i64_lt(atomic_i64* atom, i64 val);
func b32 atomic_i64_gt(atomic_i64* atom, i64 val);
func b32 atomic_i64_lte(atomic_i64* atom, i64 val);
func b32 atomic_i64_gte(atomic_i64* atom, i64 val);

// =========================================================================
// atomic_u64
// =========================================================================

// Atomically loads and returns the current value.
func u64 atomic_u64_get(atomic_u64* atom);
func u64 atomic_u64_get_explicit(atomic_u64* atom, atomic_memory_order order);

// Atomically replaces the value with val and returns the previous value.
func u64 atomic_u64_set(atomic_u64* atom, u64 val);
func u64 atomic_u64_set_explicit(atomic_u64* atom, u64 val, atomic_memory_order order);

// If the current value equals *expected, replaces it with desired and returns true.
// On failure, writes the current value into *expected and returns false.
func b32 atomic_u64_cmpex(atomic_u64* atom, u64* expected, u64 desired);

// Atomically adds delta and returns the value before the addition.
func u64 atomic_u64_add(atomic_u64* atom, u64 delta);
func u64 atomic_u64_and(atomic_u64* atom, u64 mask);
func u64 atomic_u64_or(atomic_u64* atom, u64 mask);
func u64 atomic_u64_xor(atomic_u64* atom, u64 mask);

// Atomically subtracts delta and returns the value before the subtraction.
func u64 atomic_u64_sub(atomic_u64* atom, u64 delta);

// Compare the current value against val (via a single atomic load).
func b32 atomic_u64_eq(atomic_u64* atom, u64 val);
func b32 atomic_u64_neq(atomic_u64* atom, u64 val);
func b32 atomic_u64_lt(atomic_u64* atom, u64 val);
func b32 atomic_u64_gt(atomic_u64* atom, u64 val);
func b32 atomic_u64_lte(atomic_u64* atom, u64 val);
func b32 atomic_u64_gte(atomic_u64* atom, u64 val);

// =========================================================================
// atomic_ptr
// =========================================================================

// Atomically loads and returns the current pointer.
func void* atomic_ptr_get(atomic_ptr* atom);

// Atomically replaces the pointer with val and returns the previous pointer.
func void* atomic_ptr_set(atomic_ptr* atom, void* val);

// If the current pointer equals *expected, replaces it with desired and returns true.
// On failure, writes the current pointer into *expected and returns false.
// This is also known as "compare-and-swap" or "compare-and-exchange".
func b32 atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired);

// Compare the current pointer against val (via a single atomic load).
func b32 atomic_ptr_eq(atomic_ptr* atom, void* val);
func b32 atomic_ptr_neq(atomic_ptr* atom, void* val);

// =========================================================================
// Memory Fences
// =========================================================================

// Prevents memory operations after this call from being reordered before it (acquire).
func void atomic_fence_acquire(void);

// Prevents memory operations before this call from being reordered after it (release).
func void atomic_fence_release(void);

// Full sequential-consistency fence — combines acquire and release.
func void atomic_fence(void);

// Emits a CPU pause/yield hint. Use inside spin-wait loops to reduce power and
// improve performance on hyper-threaded CPUs.
func void atomic_pause(void);

// =========================================================================
c_end;
// =========================================================================
