// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/arena.h"
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

func void arena_block_setup(arena_block* blk, sz size, b8 owned) {
  profile_func_begin;
  blk->next = NULL;
  blk->size = size;
  blk->used = size_of(arena_block);
  blk->owned = owned;
  profile_func_end;
}

func void arena_chain_block(arena* arn, arena_block* blk) {
  profile_func_begin;
  SINGLY_LIST_PUSH_BACK(arn->blocks_head, arn->blocks_tail, blk);
  profile_func_end;
}

// Attempts to bump-allocate size bytes aligned to align from blk.
// Returns the aligned pointer on success, NULL if the block has no room.
func void* arena_block_alloc(arena_block* blk, sz size, sz align) {
  profile_func_begin;
  u8* base = (u8*)blk + blk->used;
  u8* aligned = (u8*)mem_align_forward(base, align);
  sz pad = (sz)(aligned - base);
  sz avail = blk->size - blk->used;
  if (pad > avail || size > avail - pad) {
    profile_func_end;
    return NULL;
  }
  blk->used += pad + size;
  profile_func_end;
  return aligned;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* arena_alloc_callback(void* user_data, callsite site, sz size) {
  profile_func_begin;
  arena* arn = (arena*)user_data;
  sz total_size = size + size_of(sz);
  u8* raw_ptr = (u8*)_arena_alloc(arn, total_size, size_of(void*), site);
  if (raw_ptr == NULL) {
    profile_func_end;
    return NULL;
  }
  *((sz*)raw_ptr) = size;
  profile_func_end;
  return raw_ptr + size_of(sz);
}

func void arena_dealloc_callback(void* user_data, callsite site, void* ptr) {
  profile_func_begin;
  (void)user_data;
  (void)site;
  (void)ptr;
  profile_func_end;
}

func void* arena_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz new_size) {
  profile_func_begin;
  arena* arn = (arena*)user_data;
  if (ptr == NULL) {
    profile_func_end;
    return arena_alloc_callback(user_data, site, new_size);
  }
  u8* raw_ptr = ((u8*)ptr) - size_of(sz);
  sz old_size = *((sz*)raw_ptr);
  sz old_total_size = old_size + size_of(sz);
  sz new_total_size = new_size + size_of(sz);
  u8* new_raw_ptr = (u8*)_arena_realloc(arn, raw_ptr, old_total_size, new_total_size, size_of(void*), site);
  if (new_raw_ptr == NULL) {
    profile_func_end;
    return NULL;
  }
  *((sz*)new_raw_ptr) = new_size;
  profile_func_end;
  return new_raw_ptr + size_of(sz);
}

// =========================================================================
// Create / Destroy
// =========================================================================

func arena _arena_create(
    allocator parent_alloc,
    mutex opt_mutex,
    sz default_block_sz,
    callsite site) {
  profile_func_begin;
  arena arn;
  mem_zero(&arn, size_of(arn));
  arn.parent = parent_alloc;
  arn.opt_mutex = opt_mutex;
  arn.default_block_sz = default_block_sz;

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_ARENA,
      .object_ptr = &arn,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    mem_zero(&arn, size_of(arn));
    thread_log_trace("Arena creation was suspended");
    profile_func_end;
    return arn;
  }

  thread_log_trace("Created arena default_block_size=%zu", (size_t)default_block_sz);
  profile_func_end;
  return arn;
}

func arena _arena_create_mutexed(
    allocator parent_alloc,
    sz default_block_sz,
    callsite site) {
  profile_func_begin;
  arena arn = _arena_create(parent_alloc, mutex_create(), default_block_sz, site);
  arn.mutex_owned = 1;
  profile_func_end;
  return arn;
}

func void _arena_destroy(arena* arn, callsite site) {
  profile_func_begin;
  if (arn == NULL) {
    profile_func_end;
    return;
  }
  assert(arn != NULL);

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_ARENA,
      .object_ptr = arn,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Arena destruction was suspended handle=%p", (void*)arn);
    profile_func_end;
    return;
  }

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    arena_block* nxt = blk->next;
    if (blk->owned && arn->parent.alloc_fn) {
      _allocator_dealloc(arn->parent, blk, CALLSITE_HERE);
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
  thread_log_trace("Destroyed arena handle=%p", (void*)arn);
  profile_func_end;
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
  profile_func_begin;
  if (arn == NULL || ptr == NULL || size <= size_of(arena_block)) {
    profile_func_end;
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
  profile_func_end;
}

func b32 arena_remove_block(arena* arn, void* ptr) {
  profile_func_begin;
  if (arn == NULL || ptr == NULL) {
    profile_func_end;
    return false;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  b32 found = false;
  arena_block* prev = NULL;
  arena_block* blk = arn->blocks_head;

  safe_while (blk) {
    if ((void*)blk == ptr) {
      if (prev) {
        prev->next = blk->next;
      } else {
        arn->blocks_head = blk->next;
      }
      if (arn->blocks_tail == blk) {
        arn->blocks_tail = prev;
      }
      found = true;
      break;
    }
    prev = blk;
    blk = blk->next;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  profile_func_end;
  return found;
}

// =========================================================================
// Allocation
// =========================================================================

func void* _arena_alloc(arena* arn, sz size, sz align, callsite site) {
  profile_func_begin;
  if (arn == NULL || size == 0 || align == 0) {
    profile_func_end;
    return NULL;
  }
  assert(align > 0);
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  void* result = NULL;

  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    result = arena_block_alloc(blk, size, align);
    if (result != NULL) {
      break;
    }
  }

  if (!result && arn->parent.alloc_fn) {
    sz needed = size_of(arena_block) + align + size;
    sz block_sz = arn->default_block_sz > needed ? arn->default_block_sz : needed;
    arena_block* new_blk = (arena_block*)_allocator_alloc(arn->parent, block_sz, site);
    if (new_blk) {
      arena_block_setup(new_blk, block_sz, 1);
      arena_chain_block(arn, new_blk);
      thread_log_verbose("Added arena block size=%zu request=%zu", (size_t)block_sz, (size_t)size);
      result = arena_block_alloc(new_blk, size, align);
    } else {
      thread_log_error("Failed to allocate arena block size=%zu request=%zu", (size_t)block_sz, (size_t)size);
    }
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  profile_func_end;
  return result;
}

func void* _arena_realloc(
    arena* arn,
    void* ptr,
    sz old_size,
    sz new_size,
    sz align,
    callsite site) {
  profile_func_begin;
  if (!ptr) {
    profile_func_end;
    return _arena_alloc(arn, new_size, align, site);
  }

  b32 done = false;
  void* result = ptr;

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  // Try to extend the last allocation in-place by moving the cursor.
  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
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
    if (done) {
      break;
    }
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  if (!done) {
    result = _arena_alloc(arn, new_size, align, site);
    if (result) {
      sz cpy_sz = old_size < new_size ? old_size : new_size;
      mem_cpy(result, ptr, cpy_sz);
    }
  }

  profile_func_end;
  return result;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void arena_clear(arena* arn) {
  profile_func_begin;
  if (arn == NULL) {
    profile_func_end;
    return;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  sz cleared_blocks = 0;
  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    blk->used = size_of(arena_block);
    cleared_blocks += 1;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  thread_log_verbose("Cleared arena blocks=%zu", (size_t)cleared_blocks);
  profile_func_end;
}

func sz arena_block_count(arena* arn) {
  if (arn == NULL) {
    return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz count = 0;
  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    count += 1;
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return count;
}

func sz arena_total_size(arena* arn) {
  if (arn == NULL) {
    return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz total = 0;
  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    total += blk->size;
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return total;
}

func sz arena_total_used(arena* arn) {
  if (arn == NULL) {
    return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz total = 0;
  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    total += blk->used > size_of(arena_block) ? blk->used - size_of(arena_block) : 0;
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return total;
}

func sz arena_total_free(arena* arn) {
  if (arn == NULL) {
    return 0;
  }
  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }
  sz total = 0;
  SINGLY_LIST_FOREACH(arn->blocks_head, arn->blocks_tail, blk) {
    if (blk->size > blk->used) {
      total += blk->size - blk->used;
    }
  }
  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  return total;
}
