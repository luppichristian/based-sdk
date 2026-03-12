// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/atomics.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include <stdatomic.h>
#include "basic/safe.h"

// Layout-compatibility assertions:
//   atomic_i32 { i32 val; }  <->  SDL_AtomicInt { int value; }    (i32 == int32_t == int)
//   atomic_u32 { u32 val; }  <->  SDL_AtomicU32 { Uint32 value; } (u32 == uint32_t == Uint32)
//   atomic_i64 { i64 val; }  <->  _Atomic int64_t                 (C11; lock-free on all targets)
//   atomic_u64 { u64 val; }  <->  _Atomic uint64_t                (C11; lock-free on all targets)
// The (void*) intermediate cast is the idiomatic C way to suppress strict-aliasing analysis
// while preserving defined pointer-conversion semantics.
static_assert(size_of(atomic_i32) == size_of(SDL_AtomicInt));
static_assert(size_of(atomic_u32) == size_of(SDL_AtomicU32));
static_assert(size_of(atomic_i64) == size_of(_Atomic int64_t));
static_assert(size_of(atomic_u64) == size_of(_Atomic uint64_t));

func memory_order atomic_map_order(atomic_memory_order order) {
  switch (order) {
    case ATOMIC_MEMORY_ORDER_RELAXED:
      return memory_order_relaxed;
    case ATOMIC_MEMORY_ORDER_CONSUME:
      return memory_order_consume;
    case ATOMIC_MEMORY_ORDER_ACQUIRE:
      return memory_order_acquire;
    case ATOMIC_MEMORY_ORDER_RELEASE:
      return memory_order_release;
    case ATOMIC_MEMORY_ORDER_ACQ_REL:
      return memory_order_acq_rel;
    case ATOMIC_MEMORY_ORDER_SEQ_CST:
    default:
      return memory_order_seq_cst;
  }
}

// =========================================================================
// atomic_i32
// =========================================================================

func i32 atomic_i32_get(atomic_i32* atom) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return SDL_GetAtomicInt((SDL_AtomicInt*)(void*)atom);
}

func i32 atomic_i32_get_explicit(atomic_i32* atom, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  i32 result = (i32)atomic_load_explicit((_Atomic int32_t*)(void*)atom, atomic_map_order(order));
  return result;
}

func i32 atomic_i32_set(atomic_i32* atom, i32 val) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i32)SDL_SetAtomicInt((SDL_AtomicInt*)(void*)atom, (int)val);
}

func i32 atomic_i32_set_explicit(atomic_i32* atom, i32 val, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  i32 expected = (i32)atomic_load_explicit((_Atomic int32_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic int32_t*)(void*)atom, (int32_t)val, atomic_map_order(order));
  return expected;
}

func b32 atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired) {
  if (atom == NULL || expected == NULL) {
    return false;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicInt((SDL_AtomicInt*)(void*)atom, (int)*expected, (int)desired)) {
    return true;
  }
  *expected = (i32)SDL_GetAtomicInt((SDL_AtomicInt*)(void*)atom);
  return false;
}

func i32 atomic_i32_add(atomic_i32* atom, i32 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i32)SDL_AddAtomicInt((SDL_AtomicInt*)(void*)atom, (int)delta);
}

func i32 atomic_i32_and(atomic_i32* atom, i32 mask) {
  if (atom == NULL) {
    return 0;
  }
  i32 result = (i32)atomic_fetch_and((_Atomic int32_t*)(void*)atom, (int32_t)mask);
  return result;
}

func i32 atomic_i32_or(atomic_i32* atom, i32 mask) {
  if (atom == NULL) {
    return 0;
  }
  i32 result = (i32)atomic_fetch_or((_Atomic int32_t*)(void*)atom, (int32_t)mask);
  return result;
}

func i32 atomic_i32_xor(atomic_i32* atom, i32 mask) {
  if (atom == NULL) {
    return 0;
  }
  i32 result = (i32)atomic_fetch_xor((_Atomic int32_t*)(void*)atom, (int32_t)mask);
  return result;
}

func i32 atomic_i32_sub(atomic_i32* atom, i32 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i32)SDL_AddAtomicInt((SDL_AtomicInt*)(void*)atom, -(int)delta);
}

func b32 atomic_i32_eq(atomic_i32* atom, i32 val) {
  return atomic_i32_get(atom) == val;
}
func b32 atomic_i32_neq(atomic_i32* atom, i32 val) {
  return atomic_i32_get(atom) != val;
}
func b32 atomic_i32_lt(atomic_i32* atom, i32 val) {
  return atomic_i32_get(atom) < val;
}
func b32 atomic_i32_gt(atomic_i32* atom, i32 val) {
  return atomic_i32_get(atom) > val;
}
func b32 atomic_i32_lte(atomic_i32* atom, i32 val) {
  return atomic_i32_get(atom) <= val;
}
func b32 atomic_i32_gte(atomic_i32* atom, i32 val) {
  return atomic_i32_get(atom) >= val;
}

// =========================================================================
// atomic_u32
// =========================================================================

func u32 atomic_u32_get(atomic_u32* atom) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (u32)SDL_GetAtomicU32((SDL_AtomicU32*)(void*)atom);
}

func u32 atomic_u32_get_explicit(atomic_u32* atom, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  u32 result = (u32)atomic_load_explicit((_Atomic uint32_t*)(void*)atom, atomic_map_order(order));
  return result;
}

func u32 atomic_u32_set(atomic_u32* atom, u32 val) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (u32)SDL_SetAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)val);
}

func u32 atomic_u32_set_explicit(atomic_u32* atom, u32 val, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  u32 expected = (u32)atomic_load_explicit((_Atomic uint32_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic uint32_t*)(void*)atom, (uint32_t)val, atomic_map_order(order));
  return expected;
}

func b32 atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired) {
  if (atom == NULL || expected == NULL) {
    return false;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)*expected, (Uint32)desired)) {
    return true;
  }
  *expected = (u32)SDL_GetAtomicU32((SDL_AtomicU32*)(void*)atom);
  return false;
}

// SDL3 has no SDL_AddAtomicU32 — implement via CAS loop.
func u32 atomic_u32_add(atomic_u32* atom, u32 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  u32 old = atomic_u32_get(atom);
  safe_while (!SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)old, (Uint32)(old + delta))) {
    old = atomic_u32_get(atom);
  }
  return old;
}

func u32 atomic_u32_sub(atomic_u32* atom, u32 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  u32 old = atomic_u32_get(atom);
  safe_while (!SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)old, (Uint32)(old - delta))) {
    old = atomic_u32_get(atom);
  }
  return old;
}

func u32 atomic_u32_and(atomic_u32* atom, u32 mask) {
  if (atom == NULL) {
    return 0;
  }
  u32 result = (u32)atomic_fetch_and((_Atomic uint32_t*)(void*)atom, (uint32_t)mask);
  return result;
}

func u32 atomic_u32_or(atomic_u32* atom, u32 mask) {
  if (atom == NULL) {
    return 0;
  }
  u32 result = (u32)atomic_fetch_or((_Atomic uint32_t*)(void*)atom, (uint32_t)mask);
  return result;
}

func u32 atomic_u32_xor(atomic_u32* atom, u32 mask) {
  if (atom == NULL) {
    return 0;
  }
  u32 result = (u32)atomic_fetch_xor((_Atomic uint32_t*)(void*)atom, (uint32_t)mask);
  return result;
}

func b32 atomic_u32_eq(atomic_u32* atom, u32 val) {
  return atomic_u32_get(atom) == val;
}
func b32 atomic_u32_neq(atomic_u32* atom, u32 val) {
  return atomic_u32_get(atom) != val;
}
func b32 atomic_u32_lt(atomic_u32* atom, u32 val) {
  return atomic_u32_get(atom) < val;
}
func b32 atomic_u32_gt(atomic_u32* atom, u32 val) {
  return atomic_u32_get(atom) > val;
}
func b32 atomic_u32_lte(atomic_u32* atom, u32 val) {
  return atomic_u32_get(atom) <= val;
}
func b32 atomic_u32_gte(atomic_u32* atom, u32 val) {
  return atomic_u32_get(atom) >= val;
}

// =========================================================================
// atomic_i64
// =========================================================================

func i64 atomic_i64_get(atomic_i64* atom) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i64)atomic_load((_Atomic int64_t*)(void*)atom);
}

func i64 atomic_i64_get_explicit(atomic_i64* atom, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  i64 result = (i64)atomic_load_explicit((_Atomic int64_t*)(void*)atom, atomic_map_order(order));
  return result;
}

func i64 atomic_i64_set(atomic_i64* atom, i64 val) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i64)atomic_exchange((_Atomic int64_t*)(void*)atom, (int64_t)val);
}

func i64 atomic_i64_set_explicit(atomic_i64* atom, i64 val, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  i64 expected = (i64)atomic_load_explicit((_Atomic int64_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic int64_t*)(void*)atom, (int64_t)val, atomic_map_order(order));
  return expected;
}

func b32 atomic_i64_cmpex(atomic_i64* atom, i64* expected, i64 desired) {
  if (atom == NULL || expected == NULL) {
    return false;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  int64_t exp = (int64_t)*expected;
  if (atomic_compare_exchange_strong((_Atomic int64_t*)(void*)atom, &exp, (int64_t)desired)) {
    return true;
  }
  *expected = (i64)exp;
  return false;
}

func i64 atomic_i64_add(atomic_i64* atom, i64 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i64)atomic_fetch_add((_Atomic int64_t*)(void*)atom, (int64_t)delta);
}

func i64 atomic_i64_sub(atomic_i64* atom, i64 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (i64)atomic_fetch_sub((_Atomic int64_t*)(void*)atom, (int64_t)delta);
}

func i64 atomic_i64_and(atomic_i64* atom, i64 mask) {
  if (atom == NULL) {
    return 0;
  }
  i64 result = (i64)atomic_fetch_and((_Atomic int64_t*)(void*)atom, (int64_t)mask);
  return result;
}

func i64 atomic_i64_or(atomic_i64* atom, i64 mask) {
  if (atom == NULL) {
    return 0;
  }
  i64 result = (i64)atomic_fetch_or((_Atomic int64_t*)(void*)atom, (int64_t)mask);
  return result;
}

func i64 atomic_i64_xor(atomic_i64* atom, i64 mask) {
  if (atom == NULL) {
    return 0;
  }
  i64 result = (i64)atomic_fetch_xor((_Atomic int64_t*)(void*)atom, (int64_t)mask);
  return result;
}

func b32 atomic_i64_eq(atomic_i64* atom, i64 val) {
  return atomic_i64_get(atom) == val;
}
func b32 atomic_i64_neq(atomic_i64* atom, i64 val) {
  return atomic_i64_get(atom) != val;
}
func b32 atomic_i64_lt(atomic_i64* atom, i64 val) {
  return atomic_i64_get(atom) < val;
}
func b32 atomic_i64_gt(atomic_i64* atom, i64 val) {
  return atomic_i64_get(atom) > val;
}
func b32 atomic_i64_lte(atomic_i64* atom, i64 val) {
  return atomic_i64_get(atom) <= val;
}
func b32 atomic_i64_gte(atomic_i64* atom, i64 val) {
  return atomic_i64_get(atom) >= val;
}

// =========================================================================
// atomic_u64
// =========================================================================

func u64 atomic_u64_get(atomic_u64* atom) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (u64)atomic_load((_Atomic uint64_t*)(void*)atom);
}

func u64 atomic_u64_get_explicit(atomic_u64* atom, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  u64 result = (u64)atomic_load_explicit((_Atomic uint64_t*)(void*)atom, atomic_map_order(order));
  return result;
}

func u64 atomic_u64_set(atomic_u64* atom, u64 val) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (u64)atomic_exchange((_Atomic uint64_t*)(void*)atom, (uint64_t)val);
}

func u64 atomic_u64_set_explicit(atomic_u64* atom, u64 val, atomic_memory_order order) {
  if (atom == NULL) {
    return 0;
  }
  u64 expected = (u64)atomic_load_explicit((_Atomic uint64_t*)(void*)atom, memory_order_relaxed);
  atomic_store_explicit((_Atomic uint64_t*)(void*)atom, (uint64_t)val, atomic_map_order(order));
  return expected;
}

func b32 atomic_u64_cmpex(atomic_u64* atom, u64* expected, u64 desired) {
  if (atom == NULL || expected == NULL) {
    return false;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  uint64_t exp = (uint64_t)*expected;
  if (atomic_compare_exchange_strong((_Atomic uint64_t*)(void*)atom, &exp, (uint64_t)desired)) {
    return true;
  }
  *expected = (u64)exp;
  return false;
}

func u64 atomic_u64_add(atomic_u64* atom, u64 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (u64)atomic_fetch_add((_Atomic uint64_t*)(void*)atom, (uint64_t)delta);
}

func u64 atomic_u64_sub(atomic_u64* atom, u64 delta) {
  if (atom == NULL) {
    return 0;
  }
  assert(atom != NULL);
  return (u64)atomic_fetch_sub((_Atomic uint64_t*)(void*)atom, (uint64_t)delta);
}

func u64 atomic_u64_and(atomic_u64* atom, u64 mask) {
  if (atom == NULL) {
    return 0;
  }
  u64 result = (u64)atomic_fetch_and((_Atomic uint64_t*)(void*)atom, (uint64_t)mask);
  return result;
}

func u64 atomic_u64_or(atomic_u64* atom, u64 mask) {
  if (atom == NULL) {
    return 0;
  }
  u64 result = (u64)atomic_fetch_or((_Atomic uint64_t*)(void*)atom, (uint64_t)mask);
  return result;
}

func u64 atomic_u64_xor(atomic_u64* atom, u64 mask) {
  if (atom == NULL) {
    return 0;
  }
  u64 result = (u64)atomic_fetch_xor((_Atomic uint64_t*)(void*)atom, (uint64_t)mask);
  return result;
}

func b32 atomic_u64_eq(atomic_u64* atom, u64 val) {
  return atomic_u64_get(atom) == val;
}
func b32 atomic_u64_neq(atomic_u64* atom, u64 val) {
  return atomic_u64_get(atom) != val;
}
func b32 atomic_u64_lt(atomic_u64* atom, u64 val) {
  return atomic_u64_get(atom) < val;
}
func b32 atomic_u64_gt(atomic_u64* atom, u64 val) {
  return atomic_u64_get(atom) > val;
}
func b32 atomic_u64_lte(atomic_u64* atom, u64 val) {
  return atomic_u64_get(atom) <= val;
}
func b32 atomic_u64_gte(atomic_u64* atom, u64 val) {
  return atomic_u64_get(atom) >= val;
}

// =========================================================================
// atomic_ptr
// =========================================================================

func void* atomic_ptr_get(atomic_ptr* atom) {
  if (atom == NULL) {
    return NULL;
  }
  assert(atom != NULL);
  return SDL_GetAtomicPointer(&atom->val);
}

func void* atomic_ptr_set(atomic_ptr* atom, void* val) {
  if (atom == NULL) {
    return NULL;
  }
  assert(atom != NULL);
  return SDL_SetAtomicPointer(&atom->val, val);
}

func b32 atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired) {
  if (atom == NULL || expected == NULL) {
    return false;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicPointer(&atom->val, *expected, desired)) {
    return true;
  }
  *expected = SDL_GetAtomicPointer(&atom->val);
  return false;
}

func b32 atomic_ptr_eq(atomic_ptr* atom, void* val) {
  return atomic_ptr_get(atom) == val;
}
func b32 atomic_ptr_neq(atomic_ptr* atom, void* val) {
  return atomic_ptr_get(atom) != val;
}

// =========================================================================
// Fences
// =========================================================================

func void atomic_fence_acquire(void) {
  SDL_MemoryBarrierAcquireFunction();
}

func void atomic_fence_release(void) {
  SDL_MemoryBarrierReleaseFunction();
}

func void atomic_fence(void) {
  SDL_MemoryBarrierReleaseFunction();
  SDL_MemoryBarrierAcquireFunction();
}

func void atomic_pause(void) {
  SDL_CPUPauseInstruction();
}
