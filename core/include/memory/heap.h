// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../threads/mutex.h"
#include "allocator.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Heap Chunk
// =========================================================================

// Intrusive header placed immediately before every allocation within a heap block.
// User-visible data starts at (u8*)(chunk + 1) + align_pad.
// align_pad is stored here so heap_dealloc can recover the chunk pointer from
// any arbitrarily aligned user pointer.
typedef struct heap_chunk {
  struct heap_chunk* next_in_block;  // Next chunk (free or used) in the same block.
  struct heap_chunk* next_free;      // Next free chunk in the global free list (valid when is_free).
  sz size;                           // Usable data bytes, excluding this header and align_pad.
  sz align_pad;                      // Padding bytes inserted between this header and user data.
  b8 is_free;
} heap_chunk;

// =========================================================================
// Heap Block
// =========================================================================

// Intrusive header embedded at the start of every memory region managed by a heap.
// The first heap_chunk header follows immediately after this struct.
typedef struct heap_block {
  struct heap_block* next;
  sz size;   // Total byte size of the region, including this header.
  b8 owned;  // True when this block was allocated through the parent allocator.
} heap_block;

// =========================================================================
// Heap
// =========================================================================

// A general-purpose allocator that supports O(1)-amortised alloc, dealloc, and realloc.
// Memory is carved from a chain of blocks; adjacent free chunks are coalesced on dealloc
// to reduce fragmentation.
//
// Thread safety is optional: supply a valid mutex in opt_mutex to enable it,
// or pass NULL to treat the heap as single-threaded.
typedef struct heap {
  heap_block* blocks_head;
  heap_block* blocks_tail;
  heap_chunk* free_head;  // Head of the singly-linked free-chunk list.
  allocator parent;       // Grows by allocating new blocks; zeroed means no parent.
  mutex opt_mutex;        // Thread-safety guard; NULL means no locking.
  sz default_block_sz;    // Byte size of blocks auto-allocated from parent.
  b8 mutex_owned;         // True when opt_mutex was created by heap_create_mutexed.
} heap;

// Creates a new heap.
// parent_alloc     — allocator used to obtain new blocks when all free chunks are exhausted;
//                    pass a zeroed allocator for fixed-buffer-only operation.
// opt_mutex        — mutex that guards every operation; pass NULL to disable locking.
// default_block_sz — byte size for automatically grown blocks (ignored when parent has no alloc_fn).
func heap heap_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz);

// Creates a new heap and internally allocates a dedicated mutex for thread safety.
// The mutex is destroyed automatically by heap_destroy.
// parent_alloc     — same as heap_create.
// default_block_sz — same as heap_create.
func heap heap_create_mutexed(allocator parent_alloc, sz default_block_sz);

// Releases all blocks that were auto-allocated through the parent allocator and
// resets the heap to its initial empty state. Manually added blocks are detached
// but their memory is not freed. If the heap owns its mutex (created via
// heap_create_mutexed), the mutex is also destroyed.
func void heap_destroy(heap* hep);

// Returns an allocator interface backed by hep.
// The returned allocator stores a pointer to hep; hep must outlive the allocator.
func allocator heap_get_allocator(heap* hep);

// =========================================================================
// Block Management
// =========================================================================

// Attaches a caller-owned memory region to the heap's block chain.
// The region must remain valid for the lifetime of the heap (or until removed).
// size_of(heap_block) bytes are consumed by the embedded block header; the remaining
// memory is carved into a single free chunk ready for allocation.
func void heap_add_block(heap* hep, void* ptr, sz size);

// Detaches the manually-added block whose base address equals ptr.
// Any memory previously allocated from that block is invalidated by this call.
// Returns true if the block was found and removed, false otherwise.
func b32 heap_remove_block(heap* hep, void* ptr);

// =========================================================================
// Allocation
// =========================================================================

// Allocates size bytes with the given power-of-two alignment.
// Uses a first-fit free-list strategy; splits oversized free chunks to reduce waste.
// Grows via the parent allocator when no existing free chunk can satisfy the request.
// Returns NULL if the request cannot be satisfied.
func void* _heap_alloc(heap* hep, sz size, sz align, callsite site);

// Returns a previously heap-allocated pointer to the free list.
// Coalesces with adjacent free chunks to reduce fragmentation.
// The chunk size is read from the embedded header; no size argument is required.
func void _heap_dealloc(heap* hep, void* ptr, callsite site);

// Resizes a previous heap allocation, preserving up to min(old_size, new_size) bytes.
// Extends in place when the following chunk is free and large enough; otherwise
// allocates a new region, copies the data, and frees the old chunk.
// Returns NULL if the new allocation cannot be satisfied (the old pointer stays valid).
func void* _heap_realloc(
    heap* hep,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site);

#define heap_alloc(hp, size, align) \
  _heap_alloc(hp, size, align, CALLSITE_HERE)
#define heap_dealloc(hp, ptr) \
  _heap_dealloc(hp, ptr, CALLSITE_HERE)
#define heap_realloc(hp, ptr, old_size, new_size, align) \
  _heap_realloc(hp, ptr, old_size, new_size, align, CALLSITE_HERE)

// Typed helpers — derive size and alignment automatically from the type.
// heap_alloc_type   allocates a single instance of type.
// heap_alloc_array  allocates an array of count elements of type.
// heap_realloc_array resizes a previously allocated array of type.
#define heap_alloc_type(hp, type) \
  ((type*)heap_alloc(hp, size_of(type), align_of(type)))
#define heap_alloc_array(hp, type, count) \
  ((type*)heap_alloc(hp, size_of(type) * (count), align_of(type)))
#define heap_realloc_array(hp, ptr, old_count, new_count, type) \
  ((type*)heap_realloc(hp, ptr, size_of(type) * (old_count), size_of(type) * (new_count), align_of(type)))

// =========================================================================
// Lifecycle
// =========================================================================

// Marks all chunks as free and rebuilds the free list without releasing any blocks.
// This effectively resets the heap to an empty state while keeping reserved memory.
func void heap_clear(heap* hep);

// Aggregate heap statistics.
func sz heap_block_count(heap* hep);
func sz heap_total_size(heap* hep);
func sz heap_total_free(heap* hep);

// =========================================================================
c_end;
// =========================================================================
