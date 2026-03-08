// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/scratch.h"
#include "basic/assert.h"
#include "basic/profiler.h"

// =========================================================================
// Create / Destroy
// =========================================================================

func scratch scratch_begin(arena* arn) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  scratch scr;
  if (arn == NULL) {
    scr = (scratch) {0};
    TracyCZoneEnd(__tracy_zone_ctx);
    return scr;
  }
  assert(arn != NULL);
  scr.arn = arn;

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  scr.saved_tail = arn->blocks_tail;
  scr.saved_tail_used = arn->blocks_tail ? arn->blocks_tail->used : 0;

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return scr;
}

func void scratch_end(scratch* scr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (scr == NULL || scr->arn == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(scr != NULL);
  arena* arn = scr->arn;

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  // Walk all blocks that were allocated after the checkpoint and free owned ones.
  arena_block* blk = scr->saved_tail ? scr->saved_tail->next : arn->blocks_head;
  while (blk) {
    arena_block* nxt = blk->next;
    if (blk->owned && arn->parent.alloc_fn) {
      _allocator_dealloc(&arn->parent, blk, blk->size, CALLSITE_HERE);
    }
    blk = nxt;
  }

  // Restore the chain and the tail block's cursor.
  if (scr->saved_tail) {
    scr->saved_tail->next = NULL;
    scr->saved_tail->used = scr->saved_tail_used;
    arn->blocks_tail = scr->saved_tail;
  } else {
    arn->blocks_head = NULL;
    arn->blocks_tail = NULL;
  }

  if (arn->opt_mutex) {
    mutex_unlock(arn->opt_mutex);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}
