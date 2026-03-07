// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/atomics.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include <stdatomic.h>

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

// =========================================================================
// atomic_i32
// =========================================================================

func i32 atomic_i32_get(atomic_i32* atom) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetAtomicInt((SDL_AtomicInt*)(void*)atom);
}

func i32 atomic_i32_set(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)SDL_SetAtomicInt((SDL_AtomicInt*)(void*)atom, (int)val);
}

func b32 atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL || expected == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicInt((SDL_AtomicInt*)(void*)atom, (int)*expected, (int)desired)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  *expected = (i32)SDL_GetAtomicInt((SDL_AtomicInt*)(void*)atom);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func i32 atomic_i32_add(atomic_i32* atom, i32 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)SDL_AddAtomicInt((SDL_AtomicInt*)(void*)atom, (int)delta);
}

func i32 atomic_i32_sub(atomic_i32* atom, i32 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)SDL_AddAtomicInt((SDL_AtomicInt*)(void*)atom, -(int)delta);
}

func b32 atomic_i32_eq(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i32_get(atom) == val;
}
func b32 atomic_i32_neq(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i32_get(atom) != val;
}
func b32 atomic_i32_lt(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i32_get(atom) < val;
}
func b32 atomic_i32_gt(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i32_get(atom) > val;
}
func b32 atomic_i32_lte(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i32_get(atom) <= val;
}
func b32 atomic_i32_gte(atomic_i32* atom, i32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i32_get(atom) >= val;
}

// =========================================================================
// atomic_u32
// =========================================================================

func u32 atomic_u32_get(atomic_u32* atom) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u32)SDL_GetAtomicU32((SDL_AtomicU32*)(void*)atom);
}

func u32 atomic_u32_set(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u32)SDL_SetAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)val);
}

func b32 atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL || expected == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)*expected, (Uint32)desired)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  *expected = (u32)SDL_GetAtomicU32((SDL_AtomicU32*)(void*)atom);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

// SDL3 has no SDL_AddAtomicU32 — implement via CAS loop.
func u32 atomic_u32_add(atomic_u32* atom, u32 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  u32 old;
  do {
    old = atomic_u32_get(atom);
  } while (!SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)old, (Uint32)(old + delta)));
  TracyCZoneEnd(__tracy_zone_ctx);
  return old;
}

func u32 atomic_u32_sub(atomic_u32* atom, u32 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  u32 old;
  do {
    old = atomic_u32_get(atom);
  } while (!SDL_CompareAndSwapAtomicU32((SDL_AtomicU32*)(void*)atom, (Uint32)old, (Uint32)(old - delta)));
  TracyCZoneEnd(__tracy_zone_ctx);
  return old;
}

func b32 atomic_u32_eq(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u32_get(atom) == val;
}
func b32 atomic_u32_neq(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u32_get(atom) != val;
}
func b32 atomic_u32_lt(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u32_get(atom) < val;
}
func b32 atomic_u32_gt(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u32_get(atom) > val;
}
func b32 atomic_u32_lte(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u32_get(atom) <= val;
}
func b32 atomic_u32_gte(atomic_u32* atom, u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u32_get(atom) >= val;
}

// =========================================================================
// atomic_i64
// =========================================================================

func i64 atomic_i64_get(atomic_i64* atom) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i64)atomic_load((_Atomic int64_t*)(void*)atom);
}

func i64 atomic_i64_set(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i64)atomic_exchange((_Atomic int64_t*)(void*)atom, (int64_t)val);
}

func b32 atomic_i64_cmpex(atomic_i64* atom, i64* expected, i64 desired) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL || expected == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  int64_t exp = (int64_t)*expected;
  if (atomic_compare_exchange_strong((_Atomic int64_t*)(void*)atom, &exp, (int64_t)desired)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  *expected = (i64)exp;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func i64 atomic_i64_add(atomic_i64* atom, i64 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i64)atomic_fetch_add((_Atomic int64_t*)(void*)atom, (int64_t)delta);
}

func i64 atomic_i64_sub(atomic_i64* atom, i64 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i64)atomic_fetch_sub((_Atomic int64_t*)(void*)atom, (int64_t)delta);
}

func b32 atomic_i64_eq(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i64_get(atom) == val;
}
func b32 atomic_i64_neq(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i64_get(atom) != val;
}
func b32 atomic_i64_lt(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i64_get(atom) < val;
}
func b32 atomic_i64_gt(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i64_get(atom) > val;
}
func b32 atomic_i64_lte(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i64_get(atom) <= val;
}
func b32 atomic_i64_gte(atomic_i64* atom, i64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_i64_get(atom) >= val;
}

// =========================================================================
// atomic_u64
// =========================================================================

func u64 atomic_u64_get(atomic_u64* atom) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)atomic_load((_Atomic uint64_t*)(void*)atom);
}

func u64 atomic_u64_set(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)atomic_exchange((_Atomic uint64_t*)(void*)atom, (uint64_t)val);
}

func b32 atomic_u64_cmpex(atomic_u64* atom, u64* expected, u64 desired) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL || expected == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  uint64_t exp = (uint64_t)*expected;
  if (atomic_compare_exchange_strong((_Atomic uint64_t*)(void*)atom, &exp, (uint64_t)desired)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  *expected = (u64)exp;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func u64 atomic_u64_add(atomic_u64* atom, u64 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)atomic_fetch_add((_Atomic uint64_t*)(void*)atom, (uint64_t)delta);
}

func u64 atomic_u64_sub(atomic_u64* atom, u64 delta) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)atomic_fetch_sub((_Atomic uint64_t*)(void*)atom, (uint64_t)delta);
}

func b32 atomic_u64_eq(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u64_get(atom) == val;
}
func b32 atomic_u64_neq(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u64_get(atom) != val;
}
func b32 atomic_u64_lt(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u64_get(atom) < val;
}
func b32 atomic_u64_gt(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u64_get(atom) > val;
}
func b32 atomic_u64_lte(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u64_get(atom) <= val;
}
func b32 atomic_u64_gte(atomic_u64* atom, u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_u64_get(atom) >= val;
}

// =========================================================================
// atomic_ptr
// =========================================================================

func void* atomic_ptr_get(atomic_ptr* atom) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetAtomicPointer(&atom->val);
}

func void* atomic_ptr_set(atomic_ptr* atom, void* val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(atom != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_SetAtomicPointer(&atom->val, val);
}

func b32 atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (atom == NULL || expected == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(atom != NULL);
  assert(expected != NULL);
  if (SDL_CompareAndSwapAtomicPointer(&atom->val, *expected, desired)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  *expected = SDL_GetAtomicPointer(&atom->val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func b32 atomic_ptr_eq(atomic_ptr* atom, void* val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_ptr_get(atom) == val;
}
func b32 atomic_ptr_neq(atomic_ptr* atom, void* val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return atomic_ptr_get(atom) != val;
}

// =========================================================================
// Memory Fences
// =========================================================================

func void atomic_fence_acquire(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_MemoryBarrierAcquireFunction();
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void atomic_fence_release(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_MemoryBarrierReleaseFunction();
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void atomic_fence(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_MemoryBarrierReleaseFunction();
  SDL_MemoryBarrierAcquireFunction();
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void atomic_pause(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_CPUPauseInstruction();
  TracyCZoneEnd(__tracy_zone_ctx);
}
