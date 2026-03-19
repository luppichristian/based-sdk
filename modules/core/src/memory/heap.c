// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/heap.h"
#include "basic/assert.h"
#include "containers/singly_list.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include <string.h>
#include "basic/safe.h"

// =========================================================================
// Internal Helpers
// =========================================================================

// Bytes reserved before user data to store the back-pointer to heap_chunk.
// Using size_of(heap_chunk*) ensures the stored pointer is naturally aligned.
static const sz HEAP_BACK_REF_SZ = size_of(heap_chunk*);

// Union used for type-safe pointer serialisation into/from a raw byte buffer.
// This avoids multilevel pointer conversions that would be flagged by clang-tidy.
typedef union heap_chunk_ref {
  heap_chunk* ptr;
  u8 bytes[size_of(heap_chunk*)];
} heap_chunk_ref;

// Writes the chunk back-pointer into the padding bytes just before user_ptr.
// Recoverable with heap_read_back_ref on the same pointer.
func void heap_write_back_ref(void* user_ptr, heap_chunk* chunk) {
  profile_func_begin;
  heap_chunk_ref ref;
  ref.ptr = chunk;
  mem_cpy((u8*)user_ptr - HEAP_BACK_REF_SZ, ref.bytes, HEAP_BACK_REF_SZ);
  profile_func_end;
}

// Reads the chunk back-pointer from the bytes just before user_ptr.
func heap_chunk* heap_read_back_ref(void* user_ptr) {
  profile_func_begin;
  heap_chunk_ref ref;
  mem_cpy(ref.bytes, (u8*)user_ptr - HEAP_BACK_REF_SZ, HEAP_BACK_REF_SZ);
  profile_func_end;
  return ref.ptr;
}

// Removes chunk from the free list. Returns 1 if found and removed, 0 otherwise.
func b32 heap_free_list_remove(heap* hep, heap_chunk* chunk) {
  profile_func_begin;
  heap_chunk* prev = NULL;
  heap_chunk* cur = hep->free_head;
  safe_while (cur) {
    if (cur == chunk) {
      if (prev) {
        prev->next_free = cur->next_free;
      } else {
        hep->free_head = cur->next_free;
      }
      cur->next_free = NULL;
      profile_func_end;
      return true;
    }
    prev = cur;
    cur = cur->next_free;
  }
  profile_func_end;
  return false;
}

// Initializes a block header and carves its remaining space into a single free chunk.
func void heap_block_setup(heap* hep, heap_block* blk, sz size, b8 owned) {
  profile_func_begin;
  blk->next = NULL;
  blk->size = size;
  blk->owned = owned;

  sz body = size - size_of(heap_block);
  if (body <= size_of(heap_chunk)) {
    profile_func_end;
    return;
  }

  heap_chunk* chunk = (heap_chunk*)(blk + 1);
  chunk->next_in_block = NULL;
  chunk->next_free = hep->free_head;
  chunk->size = body - size_of(heap_chunk);
  chunk->align_pad = 0;
  chunk->is_free = 1;
  hep->free_head = chunk;
  profile_func_end;
}

// Appends a block to the heap's block chain (does not modify the free list).
func void heap_chain_block(heap* hep, heap_block* blk) {
  profile_func_begin;
  SINGLY_LIST_PUSH_BACK(hep->blocks_head, hep->blocks_tail, blk);
  profile_func_end;
}

// First-fit allocation from the current free list.
// eff_align must be >= HEAP_BACK_REF_SZ so the back-pointer always fits.
// Returns the user pointer on success, NULL on failure.
func void* heap_try_alloc(heap* hep, sz size, sz eff_align) {
  profile_func_begin;
  heap_chunk* prev = NULL;
  heap_chunk* chunk = hep->free_head;

  safe_while (chunk) {
    u8* raw = (u8*)(chunk + 1);
    // Advance past the back-reference slot, then align up.
    u8* usr = (u8*)mem_align_forward(raw + HEAP_BACK_REF_SZ, eff_align);
    sz pad = (sz)(usr - raw);
    sz avail = chunk->size;

    if (pad <= avail && size <= avail - pad) {
      // Remove chunk from free list.
      if (prev) {
        prev->next_free = chunk->next_free;
      } else {
        hep->free_head = chunk->next_free;
      }

      sz remaining = avail - pad - size;
      // Split if the remainder can hold at least one minimal future allocation.
      sz split_min = size_of(heap_chunk) + HEAP_BACK_REF_SZ;
      if (remaining >= split_min) {
        heap_chunk* split = (heap_chunk*)(usr + size);
        split->next_in_block = chunk->next_in_block;
        split->next_free = hep->free_head;
        split->size = remaining - size_of(heap_chunk);
        split->align_pad = 0;
        split->is_free = 1;
        hep->free_head = split;
        chunk->next_in_block = split;
        chunk->size = size;
      } else {
        // Absorb the leftover into this chunk to avoid a tiny unusable fragment.
        chunk->size = avail - pad;
      }

      chunk->align_pad = pad;
      chunk->is_free = 0;
      chunk->next_free = NULL;
      heap_write_back_ref(usr, chunk);
      profile_func_end;
      return usr;
    }

    prev = chunk;
    chunk = chunk->next_free;
  }
  profile_func_end;
  return NULL;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* heap_alloc_callback(void* user_data, callsite site, sz size) {
  profile_func_begin;
  heap* hap = (heap*)user_data;
  profile_func_end;
  return _heap_alloc(hap, size, size_of(void*), site);
}

func void heap_dealloc_callback(void* user_data, callsite site, void* ptr) {
  profile_func_begin;
  heap* hap = (heap*)user_data;
  _heap_dealloc(hap, ptr, site);
  profile_func_end;
}

func void* heap_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz new_size) {
  profile_func_begin;
  heap* hap = (heap*)user_data;
  sz old_size = 0;
  if (ptr != NULL) {
    heap_chunk* chunk = heap_read_back_ref(ptr);
    old_size = chunk->size;
  }
  profile_func_end;
  return _heap_realloc(hap, ptr, old_size, new_size, size_of(void*), site);
}

// =========================================================================
// Create / Destroy
// =========================================================================

func heap _heap_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz, callsite site) {
  profile_func_begin;

  if (parent_alloc.alloc_fn == NULL) {
    parent_alloc = thread_get_allocator();
    if (parent_alloc.alloc_fn != NULL) {
      thread_log_verbose("Using thread allocator as heap parent");
    }
  }

  heap hep;
  mem_zero(&hep, size_of(hep));
  hep.parent = parent_alloc;
  hep.opt_mutex = opt_mutex;
  hep.default_block_sz = default_block_sz;
  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_HEAP,
      .object_ptr = &hep,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    mem_zero(&hep, size_of(hep));
    thread_log_trace("Heap creation was suspended");
    profile_func_end;
    return hep;
  }
  thread_log_trace("Created heap default_block_size=%zu", (size_t)default_block_sz);
  profile_func_end;
  return hep;
}

func heap _heap_create_mutexed(allocator parent_alloc, sz default_block_sz, callsite site) {
  profile_func_begin;
  heap hep = _heap_create(parent_alloc, mutex_create(), default_block_sz, site);
  hep.mutex_owned = 1;
  profile_func_end;
  return hep;
}

func void _heap_destroy(heap* hep, callsite site) {
  profile_func_begin;
  if (hep == NULL) {
    profile_func_end;
    return;
  }
  assert(hep != NULL);

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_HEAP,
      .object_ptr = hep,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Heap destruction was suspended handle=%p", (void*)hep);
    profile_func_end;
    return;
  }

  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  heap_block* blk = hep->blocks_head;
  safe_while (blk) {
    heap_block* nxt = blk->next;
    if (blk->owned && hep->parent.alloc_fn) {
      _allocator_dealloc(hep->parent, blk, CALLSITE_HERE);
    }
    blk = nxt;
  }

  hep->blocks_head = NULL;
  hep->blocks_tail = NULL;
  hep->free_head = NULL;

  mutex mtx_owned = hep->mutex_owned ? hep->opt_mutex : NULL;

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  if (mtx_owned) {
    mutex_destroy(mtx_owned);
  }

  hep->opt_mutex = NULL;
  hep->mutex_owned = 0;
  thread_log_trace("Destroyed heap handle=%p", (void*)hep);
  profile_func_end;
}

func allocator heap_get_allocator(heap* hep) {
  allocator alloc;
  alloc.user_data = hep;
  alloc.alloc_fn = heap_alloc_callback;
  alloc.dealloc_fn = heap_dealloc_callback;
  alloc.realloc_fn = heap_realloc_callback;
  return alloc;
}

// =========================================================================
// Block Management
// =========================================================================

func void heap_add_block(heap* hep, void* ptr, sz size) {
  profile_func_begin;
  if (hep == NULL || ptr == NULL || size <= size_of(heap_block)) {
    profile_func_end;
    return;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  heap_block* blk = (heap_block*)ptr;
  heap_block_setup(hep, blk, size, 0);
  heap_chain_block(hep, blk);

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  profile_func_end;
}

func b32 heap_remove_block(heap* hep, void* ptr) {
  profile_func_begin;
  if (hep == NULL || ptr == NULL) {
    profile_func_end;
    return false;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  b32 found = false;
  heap_block* prev = NULL;
  heap_block* blk = hep->blocks_head;

  safe_while (blk) {
    if ((void*)blk == ptr) {
      // Purge this block's free chunks from the global free list.
      heap_chunk* chunk = (heap_chunk*)(blk + 1);
      safe_while (chunk) {
        if (chunk->is_free) {
          heap_free_list_remove(hep, chunk);
        }
        chunk = chunk->next_in_block;
      }

      if (prev) {
        prev->next = blk->next;
      } else {
        hep->blocks_head = blk->next;
      }
      if (hep->blocks_tail == blk) {
        hep->blocks_tail = prev;
      }
      found = true;
      break;
    }
    prev = blk;
    blk = blk->next;
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }

  profile_func_end;
  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _heap_alloc(heap* hep, sz size, sz align, callsite site) {
  profile_func_begin;
  if (hep == NULL || size == 0 || align == 0) {
    profile_func_end;
    return NULL;
  }
  assert(align > 0);
  sz eff_align = align < HEAP_BACK_REF_SZ ? HEAP_BACK_REF_SZ : align;

  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  void* result = heap_try_alloc(hep, size, eff_align);

  if (!result && hep->parent.alloc_fn) {
    sz overhead = size_of(heap_block) + size_of(heap_chunk) + HEAP_BACK_REF_SZ;
    sz needed = overhead + size;
    sz block_sz = hep->default_block_sz > needed ? hep->default_block_sz : needed;
    heap_block* new_blk = (heap_block*)_allocator_alloc(hep->parent, block_sz, site);
    if (new_blk) {
      heap_block_setup(hep, new_blk, block_sz, 1);
      heap_chain_block(hep, new_blk);
      thread_log_verbose("Added heap block size=%zu request=%zu", (size_t)block_sz, (size_t)size);
      result = heap_try_alloc(hep, size, eff_align);
    } else {
      thread_log_error("Failed to allocate heap block size=%zu request=%zu", (size_t)block_sz, (size_t)size);
    }
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }

  profile_func_end;
  return result;
}

func void _heap_dealloc(heap* hep, void* ptr, callsite site) {
  profile_func_begin;
  (void)site;
  if (hep == NULL || !ptr) {
    profile_func_end;
    return;
  }

  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  heap_chunk* chunk = heap_read_back_ref(ptr);

  // Reclaim alignment padding into the chunk's usable size.
  chunk->size = chunk->align_pad + chunk->size;
  chunk->align_pad = 0;
  chunk->is_free = 1;

  // Forward coalesce: absorb the next chunk if it is also free.
  heap_chunk* nxt = chunk->next_in_block;
  if (nxt && nxt->is_free) {
    heap_free_list_remove(hep, nxt);
    chunk->size += size_of(heap_chunk) + nxt->size;
    chunk->next_in_block = nxt->next_in_block;
  }

  chunk->next_free = hep->free_head;
  hep->free_head = chunk;

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  profile_func_end;
}

func void* _heap_realloc(
    heap* hep,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site) {
  profile_func_begin;
  if (!ptr) {
    profile_func_end;
    return _heap_alloc(hep, new_size, align, site);
  }

  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  heap_chunk* chunk = heap_read_back_ref(ptr);
  void* result = NULL;

  if (new_size <= chunk->size) {
    // Fits in the existing chunk without any modification.
    result = ptr;
  } else {
    // Try to extend in place by absorbing the immediately following free chunk.
    heap_chunk* nxt = chunk->next_in_block;
    if (nxt && nxt->is_free) {
      sz combined = chunk->size + size_of(heap_chunk) + nxt->size;
      if (combined >= new_size) {
        heap_free_list_remove(hep, nxt);
        chunk->size = combined;
        chunk->next_in_block = nxt->next_in_block;
        result = ptr;
      }
    }
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }

  if (!result) {
    // No in-place option: allocate fresh, cpy data, free the old chunk.
    result = _heap_alloc(hep, new_size, align, site);
    if (result) {
      sz cpy_sz = old_size < new_size ? old_size : new_size;
      mem_cpy(result, ptr, cpy_sz);
      _heap_dealloc(hep, ptr, site);
    }
  }

  profile_func_end;
  return result;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void heap_clear(heap* hep) {
  profile_func_begin;
  if (hep == NULL) {
    profile_func_end;
    return;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  hep->free_head = NULL;

  sz rebuilt_blocks = 0;
  SINGLY_LIST_FOREACH(hep->blocks_head, hep->blocks_tail, blk) {
    sz body = blk->size - size_of(heap_block);
    if (body > size_of(heap_chunk)) {
      heap_chunk* chunk = (heap_chunk*)(blk + 1);
      chunk->next_in_block = NULL;
      chunk->next_free = hep->free_head;
      chunk->size = body - size_of(heap_chunk);
      chunk->align_pad = 0;
      chunk->is_free = 1;
      hep->free_head = chunk;
    }
    rebuilt_blocks += 1;
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  thread_log_verbose("Cleared heap blocks=%zu", (size_t)rebuilt_blocks);
  profile_func_end;
}

func sz heap_block_count(heap* hep) {
  if (hep == NULL) {
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }
  sz count = 0;
  SINGLY_LIST_FOREACH(hep->blocks_head, hep->blocks_tail, blk) {
    count += 1;
  }
  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  return count;
}

func sz heap_total_size(heap* hep) {
  if (hep == NULL) {
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }
  sz total = 0;
  SINGLY_LIST_FOREACH(hep->blocks_head, hep->blocks_tail, blk) {
    total += blk->size;
  }
  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  return total;
}

func sz heap_total_free(heap* hep) {
  profile_func_begin;
  if (hep == NULL) {
    profile_func_end;
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }
  sz total = 0;
  safe_for (heap_chunk* chunk = hep->free_head; chunk != NULL; chunk = chunk->next_free) {
    total += chunk->size;
  }
  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  profile_func_end;
  return total;
}
