// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/heap.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include <string.h>

// =========================================================================
// Internal Helpers
// =========================================================================

// Bytes reserved before user data to store the back-pointer to heap_chunk.
// Using size_of(heap_chunk*) ensures the stored pointer is naturally aligned.
const_var sz HEAP_BACK_REF_SZ = size_of(heap_chunk*);

// Union used for type-safe pointer serialisation into/from a raw byte buffer.
// This avoids multilevel pointer conversions that would be flagged by clang-tidy.
typedef union heap_chunk_ref {
  heap_chunk* ptr;
  u8 bytes[size_of(heap_chunk*)];
} heap_chunk_ref;

// Writes the chunk back-pointer into the padding bytes just before user_ptr.
// Recoverable with heap_read_back_ref on the same pointer.
func void heap_write_back_ref(void* user_ptr, heap_chunk* chunk) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap_chunk_ref ref;
  ref.ptr = chunk;
  memcpy((u8*)user_ptr - HEAP_BACK_REF_SZ, ref.bytes, HEAP_BACK_REF_SZ);
  TracyCZoneEnd(__tracy_zone_ctx);
}

// Reads the chunk back-pointer from the bytes just before user_ptr.
func heap_chunk* heap_read_back_ref(void* user_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap_chunk_ref ref;
  memcpy(ref.bytes, (u8*)user_ptr - HEAP_BACK_REF_SZ, HEAP_BACK_REF_SZ);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ref.ptr;
}

// Removes chunk from the free list. Returns 1 if found and removed, 0 otherwise.
func b32 heap_free_list_remove(heap* hep, heap_chunk* chunk) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap_chunk* prev = NULL;
  heap_chunk* cur = hep->free_head;
  while (cur) {
    if (cur == chunk) {
      if (prev) {
        prev->next_free = cur->next_free;
      } else {
        hep->free_head = cur->next_free;
      }
      cur->next_free = NULL;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
    }
    prev = cur;
    cur = cur->next_free;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

// Initializes a block header and carves its remaining space into a single free chunk.
func void heap_block_setup(heap* hep, heap_block* blk, sz size, b8 owned) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  blk->next = NULL;
  blk->size = size;
  blk->owned = owned;

  sz body = size - size_of(heap_block);
  if (body <= size_of(heap_chunk)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  heap_chunk* chunk = (heap_chunk*)(blk + 1);
  chunk->next_in_block = NULL;
  chunk->next_free = hep->free_head;
  chunk->size = body - size_of(heap_chunk);
  chunk->align_pad = 0;
  chunk->is_free = 1;
  hep->free_head = chunk;
  TracyCZoneEnd(__tracy_zone_ctx);
}

// Appends a block to the heap's block chain (does not modify the free list).
func void heap_chain_block(heap* hep, heap_block* blk) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep->blocks_tail) {
    hep->blocks_tail->next = blk;
    hep->blocks_tail = blk;
  } else {
    hep->blocks_head = blk;
    hep->blocks_tail = blk;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

// First-fit allocation from the current free list.
// eff_align must be >= HEAP_BACK_REF_SZ so the back-pointer always fits.
// Returns the user pointer on success, NULL on failure.
func void* heap_try_alloc(heap* hep, sz size, sz eff_align) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap_chunk* prev = NULL;
  heap_chunk* chunk = hep->free_head;

  while (chunk) {
    u8* raw = (u8*)(chunk + 1);
    // Advance past the back-reference slot, then align up.
    sz pad = (sz)(align_up((up)(raw + HEAP_BACK_REF_SZ), eff_align) - (up)raw);
    u8* usr = raw + pad;
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
      TracyCZoneEnd(__tracy_zone_ctx);
      return usr;
    }

    prev = chunk;
    chunk = chunk->next_free;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* heap_alloc_callback(void* user_data, callsite site, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap* hap = (heap*)user_data;
  TracyCZoneEnd(__tracy_zone_ctx);
  return _heap_alloc(hap, size, size_of(void*), site);
}

func void heap_dealloc_callback(void* user_data, callsite site, void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap* hap = (heap*)user_data;
  _heap_dealloc(hap, ptr, site);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void* heap_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap* hap = (heap*)user_data;
  TracyCZoneEnd(__tracy_zone_ctx);
  return _heap_realloc(hap, ptr, old_size, new_size, size_of(void*), site);
}

// =========================================================================
// Create / Destroy
// =========================================================================

func heap heap_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap hep;
  memset(&hep, 0, size_of(hep));
  hep.parent = parent_alloc;
  if (hep.parent.alloc_fn == NULL || hep.parent.dealloc_fn == NULL) {
    hep.parent = thread_get_allocator();
  }
  if (hep.parent.alloc_fn == NULL || hep.parent.dealloc_fn == NULL) {
    hep.parent = global_get_allocator();
  }
  hep.opt_mutex = opt_mutex;
  hep.default_block_sz = default_block_sz;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_HEAP,
                                                     .object_ptr = &hep,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("heap_create: block_sz=%zu", (size_t)default_block_sz);
  TracyCZoneEnd(__tracy_zone_ctx);
  return hep;
}

func heap heap_create_mutexed(allocator parent_alloc, sz default_block_sz) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  heap hep = heap_create(parent_alloc, mutex_create(), default_block_sz);
  hep.mutex_owned = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return hep;
}

func void heap_destroy(heap* hep) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(hep != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_HEAP,
                                                     .object_ptr = hep,
                                                 });
  (void)msg_post(&lifecycle_msg);

  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  heap_block* blk = hep->blocks_head;
  while (blk) {
    heap_block* nxt = blk->next;
    if (blk->owned && hep->parent.alloc_fn) {
      _allocator_dealloc(&hep->parent, blk, blk->size, CALLSITE_HERE);
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
  thread_log_trace("heap_destroy: hep=%p", (void*)hep);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func allocator heap_get_allocator(heap* hep) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc;
  alloc.user_data = hep;
  alloc.alloc_fn = heap_alloc_callback;
  alloc.dealloc_fn = heap_dealloc_callback;
  alloc.realloc_fn = heap_realloc_callback;
  TracyCZoneEnd(__tracy_zone_ctx);
  return alloc;
}

// =========================================================================
// Block Management
// =========================================================================

func void heap_add_block(heap* hep, void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL || ptr == NULL || size <= size_of(heap_block)) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 heap_remove_block(heap* hep, void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL || ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  b32 found = 0;
  heap_block* prev = NULL;
  heap_block* blk = hep->blocks_head;

  while (blk) {
    if ((void*)blk == ptr) {
      // Purge this block's free chunks from the global free list.
      heap_chunk* chunk = (heap_chunk*)(blk + 1);
      while (chunk) {
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
      found = 1;
      break;
    }
    prev = blk;
    blk = blk->next;
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _heap_alloc(heap* hep, sz size, sz align, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL || size == 0 || align == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
    heap_block* new_blk = (heap_block*)_allocator_alloc(&hep->parent, block_sz, site);
    if (new_blk) {
      heap_block_setup(hep, new_blk, block_sz, 1);
      heap_chain_block(hep, new_blk);
      result = heap_try_alloc(hep, size, eff_align);
    }
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func void _heap_dealloc(heap* hep, void* ptr, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  if (hep == NULL || !ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void* _heap_realloc(
    heap* hep,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
    // No in-place option: allocate fresh, copy data, free the old chunk.
    result = _heap_alloc(hep, new_size, align, site);
    if (result) {
      sz copy_sz = old_size < new_size ? old_size : new_size;
      memcpy(result, ptr, copy_sz);
      _heap_dealloc(hep, ptr, site);
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void heap_clear(heap* hep) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }

  hep->free_head = NULL;

  heap_block* blk = hep->blocks_head;
  while (blk) {
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
    blk = blk->next;
  }

  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func sz heap_block_count(heap* hep) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }
  sz count = 0;
  for (heap_block* blk = hep->blocks_head; blk != NULL; blk = blk->next) {
    count += 1;
  }
  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return count;
}

func sz heap_total_size(heap* hep) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }
  sz total = 0;
  for (heap_block* blk = hep->blocks_head; blk != NULL; blk = blk->next) {
    total += blk->size;
  }
  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return total;
}

func sz heap_total_free(heap* hep) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (hep == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (hep->opt_mutex) {
    mutex_lock(hep->opt_mutex);
  }
  sz total = 0;
  for (heap_chunk* chunk = hep->free_head; chunk != NULL; chunk = chunk->next_free) {
    total += chunk->size;
  }
  if (hep->opt_mutex) {
    mutex_unlock(hep->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return total;
}
