// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "allocator.h"

// =========================================================================
// Virtual Memory
// =========================================================================

// Low-level OS virtual-memory primitives. These sit below the allocator layer
// and are the natural backing for arena and heap blocks on desktop platforms.
//
// On Windows the implementation uses VirtualAlloc / VirtualFree.
// On POSIX (Linux, macOS, Android, iOS) it uses mmap / mprotect / munmap.
// On other platforms (consoles, web) a malloc-based fallback is provided;
// reserve/commit/decommit are collapsed into a single allocation and the
// distinction between reserved and committed memory does not exist.

typedef struct vmem_stats {
  sz page_size;

  u64 reserve_calls;
  u64 commit_calls;
  u64 decommit_calls;
  u64 release_calls;

  sz reserved_bytes;
  sz committed_bytes;
  sz decommitted_bytes;
  sz released_bytes;

  u64 alloc_calls;
  u64 calloc_calls;
  u64 realloc_calls;
  u64 free_calls;

  u64 live_allocations;
  sz live_allocated_bytes;
  sz peak_live_allocated_bytes;
  sz total_allocated_bytes;
  sz total_freed_bytes;
} vmem_stats;

// Returns the OS memory page size in bytes.
func sz vmem_page_size(void);

// Reserves a contiguous virtual address range of at least size bytes without
// committing physical memory. Accessing the reserved pages before committing
// them is undefined behaviour (likely a hardware fault).
// Returns NULL on failure.
func void* vmem_reserve(sz size);

// Commits physical memory for the range [ptr, ptr + size).
// The range must lie entirely within a previously reserved region.
// Returns non-zero on success, zero on failure.
func b32 vmem_commit(void* ptr, sz size);

// Returns the physical memory backing [ptr, ptr + size) to the OS while keeping
// the virtual address reservation intact. The pages may be re-committed later
// with vmem_commit.
// Returns non-zero on success, zero on failure.
func b32 vmem_decommit(void* ptr, sz size);

// Releases a previously reserved region entirely, returning both the physical
// memory (if any was committed) and the virtual address space to the OS.
// ptr must be the original base address returned by vmem_reserve.
// Returns non-zero on success, zero on failure.
func b32 vmem_release(void* ptr, sz size);

// Allocates size bytes of committed virtual memory in a single call
// (equivalent to vmem_reserve + vmem_commit). Useful for large block backing
// when the reserve/commit split is not needed.
// Returns NULL on failure.
func void* vmem_alloc(sz size);

// Allocates count * size bytes of committed virtual memory and zero-initialises
// the returned range.
// Returns NULL on failure or on size overflow.
func void* vmem_calloc(sz count, sz size);

// Resizes memory obtained from vmem_alloc/vmem_calloc/vmem_realloc, preserving
// up to min(previous_size, new_size) bytes.
// old_size is accepted for allocator-interface symmetry and may be ignored.
// Returns NULL if the new allocation cannot be satisfied; the old pointer stays
// valid in that case.
func void* vmem_realloc(void* ptr, sz old_size, sz new_size);

// Releases memory obtained from vmem_alloc/vmem_calloc/vmem_realloc.
// Returns non-zero on success, zero on failure.
func b32 vmem_free(void* ptr, sz size);

// Returns an allocator backed by vmem_alloc/vmem_free/vmem_realloc.
// The returned allocator has no user data and is valid for the lifetime of the
// program.
func allocator vmem_get_allocator(void);

// Returns best-effort process-local virtual-memory counters tracked by this
// module.
func vmem_stats vmem_get_stats(void);
