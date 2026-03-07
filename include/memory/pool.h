// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../threads/mutex.h"
#include "allocator.h"

// =========================================================================
// Pool Block
// =========================================================================

// Intrusive header embedded at the start of every memory region managed by a pool.
// Immediately after this struct, slots are laid out at a fixed stride.
typedef struct pool_block {
  struct pool_block* next;
  sz size;   // Total byte size of the region, including this header.
  b8 owned;  // True when this block was allocated through the parent allocator.
} pool_block;

// =========================================================================
// Pool
// =========================================================================

// A fixed-size object pool allocator. Every allocation returns a slot of exactly
// object_size bytes aligned to object_align. Alloc and dealloc are O(1) via an
// intrusive singly-linked free list stored within each unused slot. Individual frees
// are supported; bulk reclaim is available via pool_clear.
//
// Thread safety is optional: supply a valid mutex in opt_mutex to enable it,
// or pass NULL to treat the pool as single-threaded.
typedef struct pool {
  pool_block* blocks_head;
  pool_block* blocks_tail;
  void* free_head;      // Head of the intrusive free list (stored within free slots).
  allocator parent;     // Grows by allocating new blocks; zeroed means no parent.
  mutex opt_mutex;      // Thread-safety guard; NULL means no locking.
  sz default_block_sz;  // Byte size of blocks auto-allocated from parent.
  sz object_size;       // Fixed allocation size for every slot.
  sz object_align;      // Required alignment for every slot; must be a power of two.
  b8 mutex_owned;       // True when opt_mutex was created by pool_create_mutexed.
} pool;

// Creates a new pool.
// parent_alloc     — allocator used to obtain new blocks when the free list is empty;
//                    pass a zeroed allocator for fixed-buffer-only operation.
// opt_mutex        — mutex that guards every operation; pass NULL to disable locking.
// default_block_sz — byte size for automatically grown blocks (ignored when parent has no alloc_fn).
// object_size      — byte size of every allocation; must be > 0.
// object_align     — power-of-two alignment for every slot; must be > 0.
func pool pool_create(
    allocator parent_alloc,
    mutex opt_mutex,
    sz default_block_sz,
    sz object_size,
    sz object_align);

// Creates a new pool and internally allocates a dedicated mutex for thread safety.
// The mutex is destroyed automatically by pool_destroy.
// parent_alloc     — same as pool_create.
// default_block_sz — same as pool_create.
// object_size      — same as pool_create.
// object_align     — same as pool_create.
func pool pool_create_mutexed(
    allocator parent_alloc,
    sz default_block_sz,
    sz object_size,
    sz object_align);

// Releases all blocks that were auto-allocated through the parent allocator and
// resets the pool to its initial empty state. Manually added blocks are detached
// but their memory is not freed. If the pool owns its mutex (created via
// pool_create_mutexed), the mutex is also destroyed.
func void pool_destroy(pool* pol);

// Returns an allocator interface backed by pol.
// The returned allocator stores a pointer to pol; pol must outlive the allocator.
// alloc_fn ignores the size argument and always returns one object_size slot.
// realloc_fn is a no-op: returns ptr if new_size equals object_size, NULL otherwise.
func allocator pool_get_allocator(pool* pol);

// =========================================================================
// Block Management
// =========================================================================

// Attaches a caller-owned memory region to the pool's block chain and carves it
// into free slots. The region must remain valid for the lifetime of the pool (or
// until removed). size_of(pool_block) bytes are consumed by the embedded header.
func void pool_add_block(pool* pol, void* ptr, sz size);

// Detaches the manually-added block whose base address equals ptr.
// Any memory previously allocated from that block is invalidated by this call.
// Returns true if the block was found and removed, false otherwise.
func b32 pool_remove_block(pool* pol, void* ptr);

// =========================================================================
// Allocation
// =========================================================================

// Pops one slot from the free list and returns it.
// When the free list is empty, tries to grow via the parent allocator.
// Returns NULL if the request cannot be satisfied.
func void* _pool_alloc(pool* pol, callsite site);

// Returns a slot to the free list. ptr must have been returned by pool_alloc on the
// same pool. No-op if ptr is NULL.
func void _pool_dealloc(pool* pol, void* ptr, callsite site);

// Convenience macros that automatically capture the callsite for better diagnostics.
#define pool_alloc(pol) \
  _pool_alloc(pol, CALLSITE_HERE)
#define pool_dealloc(pol, ptr) \
  _pool_dealloc(pol, ptr, CALLSITE_HERE)

// Typed helpers — no size or alignment needed; they are baked into the pool.
// pool_alloc_type  allocates one slot and casts to the requested pointer type.
// pool_dealloc_type returns a slot to the pool's free list.
#define pool_alloc_type(pol, type) \
  ((type*)pool_alloc(pol))
#define pool_dealloc_type(pol, ptr) \
  pool_dealloc(pol, ptr)

// =========================================================================
// Lifecycle
// =========================================================================

// Rebuilds the free list from all blocks, making every slot available again.
// No blocks are released to the parent allocator.
func void pool_clear(pool* pol);

// Aggregate pool statistics.
func sz pool_block_count(pool* pol);
func sz pool_slot_size(pool* pol);
func sz pool_free_count(pool* pol);
