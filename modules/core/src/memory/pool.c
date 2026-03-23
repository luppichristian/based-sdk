// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/pool.h"
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

// Union for type-safe pointer serialisation into/from raw slot memory.
// Avoids strict-aliasing violations when reading/writing the free-list link.
typedef union pool_slot_ref {
  void* ptr;
  u8 bytes[size_of(void*)];
} pool_slot_ref;

// Writes the next-free pointer into the first bytes of the slot at slot_ptr.
func void pool_slot_write_next(void* slot_ptr, void* next_ptr) {
  profile_func_begin;
  pool_slot_ref ref;
  ref.ptr = next_ptr;
  mem_cpy(slot_ptr, ref.bytes, size_of(void*));
  profile_func_end;
}

// Reads the next-free pointer from the first bytes of the slot at slot_ptr.
func void* pool_slot_read_next(void* slot_ptr) {
  profile_func_begin;
  pool_slot_ref ref;
  mem_cpy(ref.bytes, slot_ptr, size_of(void*));
  profile_func_end;
  return ref.ptr;
}

// Returns the stride between consecutive slots: large enough for object_size
// bytes and a free-list pointer, aligned to object_align.
func sz pool_slot_stride(pool* pol) {
  profile_func_begin;
  sz min_sz = pol->object_size > size_of(void*) ? pol->object_size : size_of(void*);
  profile_func_end;
  return align_up(min_sz, pol->object_align);
}

// Carves all usable space in blk into free slots and prepends them to the
// pool's free list. Called both from pool_add_block and pool_clear.
func void pool_block_carve(pool* pol, pool_block* blk) {
  profile_func_begin;
  sz stride = pool_slot_stride(pol);
  // Ensure each slot is aligned for both the object type and the free-list pointer.
  sz eff_align = pol->object_align > align_of(void*) ? pol->object_align : align_of(void*);
  u8* base = (u8*)(blk + 1);
  u8* slot = (u8*)mem_align_forward(base, eff_align);
  sz pad = (sz)(slot - base);
  sz header_used = size_of(pool_block) + pad;
  sz avail = blk->size > header_used ? blk->size - header_used : 0;

  safe_while (avail >= stride) {
    pool_slot_write_next(slot, pol->free_head);
    pol->free_head = slot;
    slot += stride;
    avail -= stride;
  }
  profile_func_end;
}

// Appends blk to the pool's block chain.
func void pool_chain_block(pool* pol, pool_block* blk) {
  profile_func_begin;
  SINGLY_LIST_PUSH_BACK(pol->blocks_head, pol->blocks_tail, blk);
  profile_func_end;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* pool_alloc_callback(void* user_data, callsite site, sz size) {
  profile_func_begin;
  (void)size;
  pool* pol = (pool*)user_data;
  profile_func_end;
  return _pool_alloc(pol, site);
}

func void pool_dealloc_callback(void* user_data, callsite site, void* ptr) {
  profile_func_begin;
  pool* pol = (pool*)user_data;
  _pool_dealloc(pol, ptr, site);
  profile_func_end;
}

func void* pool_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz new_size) {
  profile_func_begin;
  (void)site;
  pool* pol = (pool*)user_data;
  // Pools are fixed-size: realloc only makes sense when the sizes match.
  if (new_size == pol->object_size) {
    profile_func_end;
    return ptr;
  }
  profile_func_end;
  return NULL;
}

// =========================================================================
// Create / Destroy
// =========================================================================

func pool _pool_create(
    allocator parent_alloc,
    mutex opt_mutex,
    sz default_block_sz,
    sz object_size,
    sz object_align,
    callsite site) {
  profile_func_begin;

  if (parent_alloc.alloc_fn == NULL) {
    parent_alloc = thread_get_allocator();
    if (parent_alloc.alloc_fn != NULL) {
      thread_log_verbose("Using thread allocator as pool parent");
    }
  }

  pool pol;
  mem_zero(&pol, size_of(pol));
  pol.parent = parent_alloc;
  pol.opt_mutex = opt_mutex;
  pol.default_block_sz = default_block_sz;
  pol.object_size = object_size;
  pol.object_align = object_align;
  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_POOL,
      .object_ptr = &pol,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    mem_zero(&pol, size_of(pol));
    thread_log_trace("Pool creation was suspended");
    profile_func_end;
    return pol;
  }
  thread_log_trace("Created pool object_size=%zu object_align=%zu", (size_t)object_size, (size_t)object_align);
  profile_func_end;
  return pol;
}

func pool _pool_create_mutexed(
    allocator parent_alloc,
    sz default_block_sz,
    sz object_size,
    sz object_align,
    callsite site) {
  profile_func_begin;
  pool pol =
      _pool_create(parent_alloc, mutex_create(), default_block_sz, object_size, object_align, site);
  pol.mutex_owned = 1;
  profile_func_end;
  return pol;
}

func void _pool_destroy(pool* pol, callsite site) {
  profile_func_begin;
  if (pol == NULL) {
    profile_func_end;
    return;
  }
  assert(pol != NULL);

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_POOL,
      .object_ptr = pol,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Pool destruction was suspended handle=%p", (void*)pol);
    profile_func_end;
    return;
  }

  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  pool_block* blk = pol->blocks_head;
  safe_while (blk) {
    pool_block* nxt = blk->next;
    if (blk->owned && pol->parent.alloc_fn) {
      _allocator_dealloc(pol->parent, blk, CALLSITE_HERE);
    }
    blk = nxt;
  }

  pol->blocks_head = NULL;
  pol->blocks_tail = NULL;
  pol->free_head = NULL;

  mutex mtx_owned = pol->mutex_owned ? pol->opt_mutex : NULL;

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  if (mtx_owned) {
    mutex_destroy(mtx_owned);
  }

  pol->opt_mutex = NULL;
  pol->mutex_owned = 0;
  thread_log_trace("Destroyed pool handle=%p", (void*)pol);
  profile_func_end;
}

func allocator pool_get_allocator(pool* pol) {
  allocator alloc;
  alloc.user_data = pol;
  alloc.tracker = NULL;
  alloc.alloc_fn = pool_alloc_callback;
  alloc.dealloc_fn = pool_dealloc_callback;
  alloc.realloc_fn = pool_realloc_callback;
  return alloc;
}

// =========================================================================
// Block Management
// =========================================================================

func void pool_add_block(pool* pol, void* ptr, sz size) {
  profile_func_begin;
  if (pol == NULL || ptr == NULL || size <= size_of(pool_block)) {
    profile_func_end;
    return;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  pool_block* blk = (pool_block*)ptr;
  blk->next = NULL;
  blk->size = size;
  blk->owned = 0;
  pool_chain_block(pol, blk);
  pool_block_carve(pol, blk);

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  profile_func_end;
}

func b32 pool_remove_block(pool* pol, void* ptr) {
  profile_func_begin;
  if (pol == NULL || ptr == NULL) {
    profile_func_end;
    return false;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  b32 found = false;
  pool_block* prev_blk = NULL;
  pool_block* blk = pol->blocks_head;

  safe_while (blk) {
    if ((void*)blk == ptr) {
      if (prev_blk) {
        prev_blk->next = blk->next;
      } else {
        pol->blocks_head = blk->next;
      }
      if (pol->blocks_tail == blk) {
        pol->blocks_tail = prev_blk;
      }
      found = true;
      break;
    }
    prev_blk = blk;
    blk = blk->next;
  }

  if (found) {
    // Purge any free slots belonging to the removed block from the free list.
    u8* blk_start = (u8*)ptr;
    u8* blk_end = blk_start + ((pool_block*)ptr)->size;
    void* prev_slot = NULL;
    void* slot = pol->free_head;

    safe_while (slot) {
      void* nxt = pool_slot_read_next(slot);
      u8* slot_bytes = (u8*)slot;

      if (slot_bytes >= blk_start && slot_bytes < blk_end) {
        if (prev_slot) {
          pool_slot_write_next(prev_slot, nxt);
        } else {
          pol->free_head = nxt;
        }
      } else {
        prev_slot = slot;
      }
      slot = nxt;
    }
  }

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }

  profile_func_end;
  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _pool_alloc(pool* pol, callsite site) {
  profile_func_begin;
  if (pol == NULL) {
    profile_func_end;
    return NULL;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  void* result = NULL;

  if (pol->free_head) {
    result = pol->free_head;
    pol->free_head = pool_slot_read_next(result);
  } else if (pol->parent.alloc_fn) {
    sz stride = pool_slot_stride(pol);
    sz overhead = size_of(pool_block) + pol->object_align;
    sz needed = overhead + stride;
    sz block_sz = pol->default_block_sz > needed ? pol->default_block_sz : needed;
    pool_block* new_blk = (pool_block*)_allocator_alloc(pol->parent, block_sz, site);
    if (new_blk) {
      new_blk->next = NULL;
      new_blk->size = block_sz;
      new_blk->owned = true;
      pool_chain_block(pol, new_blk);
      pool_block_carve(pol, new_blk);
      thread_log_verbose("Added pool block size=%zu object_size=%zu", (size_t)block_sz, (size_t)pol->object_size);
      if (pol->free_head) {
        result = pol->free_head;
        pol->free_head = pool_slot_read_next(result);
      }
    } else {
      thread_log_error("Failed to allocate pool block size=%zu object_size=%zu", (size_t)block_sz, (size_t)pol->object_size);
    }
  }

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }

  profile_func_end;
  return result;
}

func void _pool_dealloc(pool* pol, void* ptr, callsite site) {
  profile_func_begin;
  (void)site;
  if (pol == NULL || !ptr) {
    profile_func_end;
    return;
  }

  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  pool_slot_write_next(ptr, pol->free_head);
  pol->free_head = ptr;

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  profile_func_end;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void pool_clear(pool* pol) {
  profile_func_begin;
  if (pol == NULL) {
    profile_func_end;
    return;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  pol->free_head = NULL;
  sz rebuilt_blocks = 0;
  SINGLY_LIST_FOREACH(pol->blocks_head, pol->blocks_tail, blk) {
    pool_block_carve(pol, blk);
    rebuilt_blocks += 1;
  }

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  thread_log_verbose("Cleared pool blocks=%zu", (size_t)rebuilt_blocks);
  profile_func_end;
}

func sz pool_block_count(pool* pol) {
  if (pol == NULL) {
    return 0;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }
  sz count = 0;
  SINGLY_LIST_FOREACH(pol->blocks_head, pol->blocks_tail, blk) {
    count += 1;
  }
  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  return count;
}

func sz pool_slot_size(pool* pol) {
  if (pol == NULL) {
    return 0;
  }
  sz result = pol->object_size;
  return result;
}

func sz pool_free_count(pool* pol) {
  if (pol == NULL) {
    return 0;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }
  sz count = 0;
  safe_for (void* slot = pol->free_head; slot != NULL; slot = pool_slot_read_next(slot)) {
    count += 1;
  }
  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  return count;
}
