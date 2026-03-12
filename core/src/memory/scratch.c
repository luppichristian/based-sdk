// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/scratch.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "basic/safe.h"

// =========================================================================
// Create / Destroy
// =========================================================================

func scratch scratch_begin(arena* arn) {
  profile_func_begin;
  scratch scr;
  if (arn == NULL) {
    scr = (scratch) {0};
    profile_func_end;
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

  profile_func_end;
  return scr;
}

func void scratch_end(scratch* scr) {
  profile_func_begin;
  if (scr == NULL || scr->arn == NULL) {
    profile_func_end;
    return;
  }
  assert(scr != NULL);
  arena* arn = scr->arn;

  if (arn->opt_mutex) {
    mutex_lock(arn->opt_mutex);
  }

  // Walk all blocks that were allocated after the checkpoint and free owned ones.
  arena_block* blk = scr->saved_tail ? scr->saved_tail->next : arn->blocks_head;
  safe_while (blk) {
    arena_block* nxt = blk->next;
    if (blk->owned && arn->parent.alloc_fn) {
      _allocator_dealloc(arn->parent, blk, CALLSITE_HERE);
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
  profile_func_end;
}
