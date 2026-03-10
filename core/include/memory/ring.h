// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../threads/mutex.h"
#include "allocator.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Ring Buffer
// =========================================================================

// A byte-oriented circular (ring) buffer backed by a contiguous memory region.
// Writes advance a write cursor; reads advance a read cursor. Both wrap around
// at capacity, so the buffer can be used continuously without compaction.
//
// The ring is not typed: it stores raw bytes. Callers are responsible for
// framing (e.g. fixed-size records, length-prefixed messages).
//
// Thread safety is optional: supply a valid mutex in opt_mutex to enable it,
// or pass NULL to treat the ring as single-threaded.
typedef struct ring {
  u8* ptr;           // Base pointer to the backing buffer.
  sz capacity;       // Total byte capacity of the backing buffer.
  sz read_pos;       // Read cursor (byte offset within [0, capacity)).
  sz write_pos;      // Write cursor (byte offset within [0, capacity)).
  sz count;          // Number of bytes currently stored.
  allocator parent;  // Used when the backing buffer was auto-allocated.
  mutex opt_mutex;   // Thread-safety guard; NULL means no locking.
  b8 buf_owned;      // True when ptr was allocated through parent.
  b8 mutex_owned;    // True when opt_mutex was created by ring_create_mutexed.
} ring;

// Creates a ring backed by an existing caller-owned buffer.
// ptr      — base of the backing buffer; must remain valid for the lifetime of the ring.
// capacity — byte size of the buffer; must be > 0.
// opt_mutex — mutex that guards every operation; pass NULL to disable locking.
func ring ring_create(void* ptr, sz capacity, mutex opt_mutex);

// Creates a ring backed by an existing buffer and allocates a dedicated mutex.
// The mutex is destroyed automatically by ring_destroy.
func ring ring_create_mutexed(void* ptr, sz capacity);

// Creates a ring and allocates its backing buffer from parent_alloc.
// capacity  — byte size of the buffer to allocate; must be > 0.
// opt_mutex — mutex that guards every operation; pass NULL to disable locking.
func ring ring_create_alloc(allocator parent_alloc, sz capacity, mutex opt_mutex);

// Creates a ring, allocates its backing buffer from parent_alloc, and allocates
// a dedicated mutex. Both are destroyed automatically by ring_destroy.
func ring ring_create_alloc_mutexed(allocator parent_alloc, sz capacity);

// Releases the backing buffer if it was auto-allocated and destroys the mutex
// if it was owned. Resets the ring to a zeroed state.
func void ring_destroy(ring* rng);

// =========================================================================
// Capacity Queries
// =========================================================================

// Returns the number of bytes currently available to read.
func sz ring_size(ring* rng);

// Returns the number of bytes that can be written before the ring is full.
func sz ring_space(ring* rng);
// Returns the total ring capacity in bytes.
func sz ring_capacity(ring* rng);

// =========================================================================
// I/O
// =========================================================================

// Writes up to size bytes from data into the ring.
// Returns the number of bytes actually written (may be less than size if the
// ring does not have enough space).
func sz ring_write(ring* rng, void* data, sz size);

// Reads and consumes up to size bytes from the ring into out.
// Returns the number of bytes actually read (may be less than size if the
// ring holds fewer bytes than requested).
func sz ring_read(ring* rng, void* out, sz size);

// Copies up to size bytes from the ring into out without advancing the read
// cursor. Repeated calls to ring_peek return the same bytes.
// Returns the number of bytes peeked.
func sz ring_peek(ring* rng, void* out, sz size);

// Advances the read cursor by up to size bytes without copying any data.
// Returns the number of bytes actually skipped.
func sz ring_skip(ring* rng, sz size);

// Reserves a contiguous writable span and returns its pointer/size.
func void* ring_reserve_write(ring* rng, sz* out_size);
// Commits bytes previously reserved by ring_reserve_write.
func b32 ring_commit_write(ring* rng, sz size);

// Reserves a contiguous readable span and returns its pointer/size.
func const void* ring_reserve_read(ring* rng, sz* out_size);
// Commits bytes previously reserved by ring_reserve_read.
func b32 ring_commit_read(ring* rng, sz size);

// =========================================================================
// Lifecycle
// =========================================================================

// Resets both cursors and the byte count to zero, making the ring appear empty.
// Does not release the backing buffer.
func void ring_clear(ring* rng);

// =========================================================================
c_end;
// =========================================================================
