// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/arena.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "basic/utility_defines.h"
#include <string.h>

// =========================================================================
// Internal Helpers
// =========================================================================

func void arena_block_setup(arena_block* blk, sz size, b8 owned) {
  blk->next = NULL;
  blk->size = size;
  blk->used = size_of(arena_block);
  blk->owned = owned;
}

func void arena_chain_block(arena* arn, arena_block* blk) {
  if (arn->blocks_tail) {
    arn->blocks_tail->next = blk;
    arn->blocks_tail = blk;
  } else {
    arn->blocks_head = blk;
    arn->blocks_tail = blk;
  }
}

// Attempts to bump-allocate size bytes aligned to align from blk.
// Returns the aligned pointer on success, NULL if the block has no room.
func void* arena_block_alloc(arena_block* blk, sz size, sz align) {
  u8* base = (u8*)blk + blk->used;
  sz pad = (sz)(align_up((up)base, align) - (up)base);
  u8* aligned = base + pad;
  sz avail = blk->size - blk->used;
  if (pad > avail || size > avail - pad) {
    return NULL;
  }
  blk->used += pad + size;
  return aligned;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* arena_alloc_callback(void* user_data, callsite site, sz size) {
  arena* arn = (arena*)user_data;
  return _arena_alloc(arn, size, size_of(void*), site);
}

func void arena_dealloc_callback(void* user_data, callsite site, void* ptr) {
  (void)user_data;
  (void)site;
  (void)ptr;
}

func void* arena_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size) {
  arena* arn = (arena*)user_data;
  return _arena_realloc(arn, ptr, old_size, new_size, size_of(void*), site);
}

// =========================================================================
// Create / Destroy
// =========================================================================

func arena arena_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz) {
  arena arn;
  memset(&arn, 0, size_of(arn));
  arn.parent = parent_alloc;
  arn.opt_mutex = opt_mutex;
  arn.default_block_sz = default_block_sz;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_ARENA;
  lifecycle_msg.object_lifecycle.object_ptr = &arn;
  if (!msg_post(&lifecycle_msg)) {
    memset(&arn, 0, size_of(arn));
  }
  thread_log_trace("arena_create: block_sz=%zu", (size_t)default_block_sz);
  return arn;
}

func arena arena_create_mutexed(allocator parent_alloc, sz default_block_sz) {
  arena arn = arena_create(parent_alloc, mutex_create(), default_block_sz);
  arn.mutex_owned = 1;
  return arn;
}

func void arena_destroy(arena* arn) {
  if (arn == NULL) {
    return;
  }
  assert(arn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_ARENA;
  lifecycle_msg.object_lifecycle.object_ptr = arn;
  if (!msg_post(&lifecycle_msg)) {
    return;
  }

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  arena_block* blk = arn->blocks_head;
  while (blk) {
    arena_block* nxt = blk->next;
    if (blk->owned && arn->parent.alloc_fn) {
      _allocator_dealloc(&arn->parent, blk, blk->size, CALLSITE_HERE);
    }
    blk = nxt;
  }

  arn->blocks_head = NULL;
  arn->blocks_tail = NULL;

  mutex mtx_owned = arn->mutex_owned ? arn->opt_mutex : NULL;

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  if (mtx_owned) {
    mutex_destroy(mtx_owned);
  }

  arn->opt_mutex = NULL;
  arn->mutex_owned = 0;
  thread_log_trace("arena_destroy: arn=%p", (void*)arn);
}

func allocator arena_get_allocator(arena* arn) {
  allocator alloc;
  alloc.user_data = arn;
  alloc.alloc_fn = arena_alloc_callback;
  alloc.dealloc_fn = arena_dealloc_callback;
  alloc.realloc_fn = arena_realloc_callback;
  return alloc;
}

// =========================================================================
// Block Management
// =========================================================================

func void arena_add_block(arena* arn, void* ptr, sz size) {
  if (arn == NULL || ptr == NULL || size <= size_of(arena_block)) {
    return;
  }
  assert(arn != NULL);
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  arena_block* blk = (arena_block*)ptr;
  arena_block_setup(blk, size, 0);
  arena_chain_block(arn, blk);

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
}

func b32 arena_remove_block(arena* arn, void* ptr) {
  if (arn == NULL || ptr == NULL) {
    return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  b32 found = 0;
  arena_block* prev = NULL;
  arena_block* blk = arn->blocks_head;

  while (blk) {
    if ((void*)blk == ptr) {
      if (prev) {
        prev->next = blk->next;
      } else {
        arn->blocks_head = blk->next;
      }
      if (arn->blocks_tail == blk) {
        arn->blocks_tail = prev;
      }
      found = 1;
      break;
    }
    prev = blk;
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _arena_alloc(arena* arn, sz size, sz align, callsite site) {
  if (arn == NULL || size == 0 || align == 0) {
    return NULL;
  }
  assert(align > 0);
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  void* result = NULL;

  arena_block* blk = arn->blocks_head;
  while (blk && !result) {
    result = arena_block_alloc(blk, size, align);
    blk = blk->next;
  }

  if (!result && arn->parent.alloc_fn) {
    sz needed = size_of(arena_block) + align + size;
    sz block_sz = arn->default_block_sz > needed ? arn->default_block_sz : needed;
    arena_block* new_blk = (arena_block*)_allocator_alloc(&arn->parent, block_sz, site);
    if (new_blk) {
      arena_block_setup(new_blk, block_sz, 1);
      arena_chain_block(arn, new_blk);
      result = arena_block_alloc(new_blk, size, align);
    }
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  return result;
}

func void* _arena_realloc(
    arena* arn,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site) {
  if (!ptr) {
    return _arena_alloc(arn, new_size, align, site);
  }

  b32 done = 0;
  void* result = ptr;

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  // Try to extend the last allocation in-place by moving the cursor.
  arena_block* blk = arn->blocks_head;
  while (blk && !done) {
    u8* cursor = (u8*)blk + blk->used;
    if ((u8*)ptr + old_size == cursor) {
      if (new_size <= old_size) {
        blk->used -= old_size - new_size;
        done = 1;
      } else {
        sz extra = new_size - old_size;
        sz avail = blk->size - blk->used;
        if (extra <= avail) {
          blk->used += extra;
          done = 1;
        }
      }
    }
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  if (!done) {
    result = _arena_alloc(arn, new_size, align, site);
    if (result) {
      sz copy_sz = old_size < new_size ? old_size : new_size;
      memcpy(result, ptr, copy_sz);
    }
  }

  return result;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void arena_clear(arena* arn) {
  if (arn == NULL) {
    return;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  arena_block* blk = arn->blocks_head;
  while (blk) {
    blk->used = size_of(arena_block);
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
}
