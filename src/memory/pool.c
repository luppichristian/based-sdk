// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/pool.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include <string.h>

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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pool_slot_ref ref;
  ref.ptr = next_ptr;
  memcpy(slot_ptr, ref.bytes, size_of(void*));
  TracyCZoneEnd(__tracy_zone_ctx);
}

// Reads the next-free pointer from the first bytes of the slot at slot_ptr.
func void* pool_slot_read_next(void* slot_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pool_slot_ref ref;
  memcpy(ref.bytes, slot_ptr, size_of(void*));
  TracyCZoneEnd(__tracy_zone_ctx);
  return ref.ptr;
}

// Returns the stride between consecutive slots: large enough for object_size
// bytes and a free-list pointer, aligned to object_align.
func sz pool_slot_stride(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz min_sz = pol->object_size > size_of(void*) ? pol->object_size : size_of(void*);
  TracyCZoneEnd(__tracy_zone_ctx);
  return align_up(min_sz, pol->object_align);
}

// Carves all usable space in blk into free slots and prepends them to the
// pool's free list. Called both from pool_add_block and pool_clear.
func void pool_block_carve(pool* pol, pool_block* blk) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz stride = pool_slot_stride(pol);
  // Ensure each slot is aligned for both the object type and the free-list pointer.
  sz eff_align = pol->object_align > align_of(void*) ? pol->object_align : align_of(void*);
  u8* base = (u8*)(blk + 1);
  sz pad = (sz)(align_up((up)base, eff_align) - (up)base);
  u8* slot = base + pad;
  sz header_used = size_of(pool_block) + pad;
  sz avail = blk->size > header_used ? blk->size - header_used : 0;

  while (avail >= stride) {
    pool_slot_write_next(slot, pol->free_head);
    pol->free_head = slot;
    slot += stride;
    avail -= stride;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

// Appends blk to the pool's block chain.
func void pool_chain_block(pool* pol, pool_block* blk) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol->blocks_tail) {
    pol->blocks_tail->next = blk;
    pol->blocks_tail = blk;
  } else {
    pol->blocks_head = blk;
    pol->blocks_tail = blk;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* pool_alloc_callback(void* user_data, callsite site, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)size;
  pool* pol = (pool*)user_data;
  TracyCZoneEnd(__tracy_zone_ctx);
  return _pool_alloc(pol, site);
}

func void pool_dealloc_callback(void* user_data, callsite site, void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pool* pol = (pool*)user_data;
  _pool_dealloc(pol, ptr, site);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void* pool_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  (void)old_size;
  pool* pol = (pool*)user_data;
  // Pools are fixed-size: realloc only makes sense when the sizes match.
  if (new_size == pol->object_size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return ptr;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

// =========================================================================
// Create / Destroy
// =========================================================================

func pool pool_create(
    allocator parent_alloc,
    mutex opt_mutex,
    sz default_block_sz,
    sz object_size,
    sz object_align) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pool pol;
  memset(&pol, 0, size_of(pol));
  pol.parent = parent_alloc;
  pol.opt_mutex = opt_mutex;
  pol.default_block_sz = default_block_sz;
  pol.object_size = object_size;
  pol.object_align = object_align;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_POOL,
                                                     .object_ptr = &pol,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    memset(&pol, 0, size_of(pol));
  }
  thread_log_trace("pool_create: obj_size=%zu obj_align=%zu", (size_t)object_size, (size_t)object_align);
  TracyCZoneEnd(__tracy_zone_ctx);
  return pol;
}

func pool pool_create_mutexed(
    allocator parent_alloc,
    sz default_block_sz,
    sz object_size,
    sz object_align) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pool pol =
      pool_create(parent_alloc, mutex_create(), default_block_sz, object_size, object_align);
  pol.mutex_owned = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return pol;
}

func void pool_destroy(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(pol != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_POOL,
                                                     .object_ptr = pol,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  pool_block* blk = pol->blocks_head;
  while (blk) {
    pool_block* nxt = blk->next;
    if (blk->owned && pol->parent.alloc_fn) {
      _allocator_dealloc(&pol->parent, blk, blk->size, CALLSITE_HERE);
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
  thread_log_trace("pool_destroy: pol=%p", (void*)pol);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func allocator pool_get_allocator(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc;
  alloc.user_data = pol;
  alloc.alloc_fn = pool_alloc_callback;
  alloc.dealloc_fn = pool_dealloc_callback;
  alloc.realloc_fn = pool_realloc_callback;
  TracyCZoneEnd(__tracy_zone_ctx);
  return alloc;
}

// =========================================================================
// Block Management
// =========================================================================

func void pool_add_block(pool* pol, void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL || ptr == NULL || size <= size_of(pool_block)) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 pool_remove_block(pool* pol, void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL || ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  b32 found = 0;
  pool_block* prev_blk = NULL;
  pool_block* blk = pol->blocks_head;

  while (blk) {
    if ((void*)blk == ptr) {
      if (prev_blk) {
        prev_blk->next = blk->next;
      } else {
        pol->blocks_head = blk->next;
      }
      if (pol->blocks_tail == blk) {
        pol->blocks_tail = prev_blk;
      }
      found = 1;
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

    while (slot) {
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

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _pool_alloc(pool* pol, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
    pool_block* new_blk = (pool_block*)_allocator_alloc(&pol->parent, block_sz, site);
    if (new_blk) {
      new_blk->next = NULL;
      new_blk->size = block_sz;
      new_blk->owned = 1;
      pool_chain_block(pol, new_blk);
      pool_block_carve(pol, new_blk);
      if (pol->free_head) {
        result = pol->free_head;
        pol->free_head = pool_slot_read_next(result);
      }
    }
  }

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func void _pool_dealloc(pool* pol, void* ptr, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  if (pol == NULL || !ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneEnd(__tracy_zone_ctx);
}

// =========================================================================
// Lifecycle
// =========================================================================

func void pool_clear(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }

  pol->free_head = NULL;
  pool_block* blk = pol->blocks_head;
  while (blk) {
    pool_block_carve(pol, blk);
    blk = blk->next;
  }

  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func sz pool_block_count(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }
  sz count = 0;
  for (pool_block* blk = pol->blocks_head; blk != NULL; blk = blk->next) {
    count += 1;
  }
  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return count;
}

func sz pool_slot_size(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  sz result = pol->object_size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func sz pool_free_count(pool* pol) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (pol == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (pol->opt_mutex) {
    mutex_lock(pol->opt_mutex);
  }
  sz count = 0;
  for (void* slot = pol->free_head; slot != NULL; slot = pool_slot_read_next(slot)) {
    count += 1;
  }
  if (pol->opt_mutex) {
    mutex_unlock(pol->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return count;
}
