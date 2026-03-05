// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../threads/mutex.h"
#include "allocator.h"

// =========================================================================
// Arena Block
// =========================================================================

// Intrusive header embedded at the start of every memory region managed by an arena.
// Usable space begins immediately after this struct; arena_alloc advances the cursor
// within [ptr + size_of(arena_block), ptr + size).
typedef struct arena_block {
  struct arena_block* next;
  sz size;   // Total byte size of the region, including this header.
  sz used;   // Bytes consumed so far, counting from the start of the header.
  b8 owned;  // True when this block was allocated through the parent allocator.
} arena_block;

// =========================================================================
// Arena
// =========================================================================

// A linear (bump-pointer) allocator. Allocations are O(1) and advance a cursor
// within a chain of memory blocks. Individual frees are not supported; reclaim
// all memory at once with arena_clear, or release everything with arena_destroy.
//
// Thread safety is optional: supply a valid mutex in opt_mutex to enable it,
// or pass NULL to treat the arena as single-threaded.
typedef struct arena {
  arena_block* blocks_head;
  arena_block* blocks_tail;
  allocator parent;     // Grows by allocating new blocks; zeroed means no parent.
  mutex opt_mutex;      // Thread-safety guard; NULL means no locking.
  sz default_block_sz;  // Byte size of blocks auto-allocated from parent.
  b8 mutex_owned;       // True when opt_mutex was created by arena_create_mutexed.
} arena;

// Creates a new arena.
// parent_alloc     — allocator used to obtain new blocks when the current ones are full;
//                    pass a zeroed allocator for fixed-buffer-only operation.
// opt_mutex        — mutex that guards every operation; pass NULL to disable locking.
// default_block_sz — byte size for automatically grown blocks (ignored when parent has no alloc_fn).
func arena arena_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz);

// Creates a new arena and internally allocates a dedicated mutex for thread safety.
// The mutex is destroyed automatically by arena_destroy.
// parent_alloc     — same as arena_create.
// default_block_sz — same as arena_create.
func arena arena_create_mutexed(allocator parent_alloc, sz default_block_sz);

// Releases all blocks that were auto-allocated through the parent allocator and
// resets the arena to its initial empty state. Manually added blocks are detached
// but their memory is not freed. If the arena owns its mutex (created via
// arena_create_mutexed), the mutex is also destroyed.
func void arena_destroy(arena* arn);

// Returns an allocator interface backed by arn.
// The returned allocator stores a pointer to arn; arn must outlive the allocator.
// dealloc_fn is a no-op (arenas do not support individual frees).
func allocator arena_get_allocator(arena* arn);

// =========================================================================
// Block Management
// =========================================================================

// Attaches a caller-owned memory region to the arena's block chain.
// The region must remain valid for the lifetime of the arena (or until removed).
// size_of(arena_block) bytes are consumed by the embedded header; the remaining
// size - size_of(arena_block) bytes are available for allocations.
func void arena_add_block(arena* arn, void* ptr, sz size);

// Detaches the manually-added block whose base address equals ptr.
// Any memory previously allocated from that block is invalidated by this call.
// Returns true if the block was found and removed, false otherwise.
func b32 arena_remove_block(arena* arn, void* ptr);

// =========================================================================
// Allocation
// =========================================================================

// Allocates size bytes with the given power-of-two alignment from the arena.
// Walks the block chain for a block with enough remaining space; when none exists,
// tries to grow by allocating a new block from the parent allocator.
// Returns NULL if the request cannot be satisfied.
func void* _arena_alloc(arena* arn, sz size, sz align, callsite site);

// Resizes a previous arena allocation.
// When the allocation sits at the cursor of its block and there is room, it is
// extended in place. Otherwise a fresh region is allocated and old_size bytes
// are copied into it (the old region is not reclaimed).
// Returns NULL if the new allocation cannot be satisfied.
func void* _arena_realloc(
    arena* arn,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site);

// Convenience macros that automatically capture the callsite for better diagnostics.
#define arena_alloc(arn, size, align) \
  _arena_alloc(arn, size, align, CALLSITE_HERE)
#define arena_realloc(arn, ptr, old_size, new_size, align) \
  _arena_realloc(arn, ptr, old_size, new_size, align, CALLSITE_HERE)

// Typed helpers — derive size and alignment automatically from the type.
// arena_alloc_type  allocates a single instance of type.
// arena_alloc_array allocates an array of count elements of type.
// arena_realloc_array resizes a previously allocated array of type.
#define arena_alloc_type(arn, type) \
  ((type*)arena_alloc(arn, size_of(type), align_of(type)))
#define arena_alloc_array(arn, type, count) \
  ((type*)arena_alloc(arn, size_of(type) * (count), align_of(type)))
#define arena_realloc_array(arn, ptr, old_count, new_count, type) \
  ((type*)arena_realloc(arn, ptr, size_of(type) * (old_count), size_of(type) * (new_count), align_of(type)))

// =========================================================================
// Lifecycle
// =========================================================================

// Resets the cursor of every block back to its post-header position, making all
// memory available for reuse. No blocks are released to the parent allocator.
func void arena_clear(arena* arn);
