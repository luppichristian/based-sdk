// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "arena.h"

// =========================================================================
// Scratch
// =========================================================================

// A lightweight checkpoint into an arena's state. Capturing a scratch saves the
// identity and cursor position of the tail block at that moment. Releasing it
// rewinds the tail block's cursor and frees any blocks that were auto-allocated
// after the checkpoint was taken.
//
// Intended for "allocate, use, discard" patterns (e.g. per-frame temp memory):
//
//   scratch scr = scratch_begin(&arn);
//   void* tmp = arena_alloc(&arn, 1024, 8);
//   // ... use tmp ...
//   scratch_end(&scr);   // tmp is now invalid; arena state is restored
//
// Limitation: only the tail block's cursor (the block active at scratch_begin
// time) is restored. Blocks that existed before the checkpoint and were partially
// filled are not rewound. For correct results, take a scratch when the arena's
// tail block is in a clean state, or use a dedicated scratch arena.
typedef struct scratch {
  arena* arn;               // The arena this scratch was taken from.
  arena_block* saved_tail;  // Tail block at scratch_begin time (NULL if empty).
  sz saved_tail_used;       // saved_tail->used at scratch_begin time.
} scratch;

// Captures the current state of arn's tail block and returns a scratch token.
// Does not perform any allocation. arn must remain valid until scratch_end is called.
func scratch scratch_begin(arena* arn);

// Restores the arena to the state captured by scr.
// Frees any blocks that were auto-allocated after the checkpoint, and rewinds the
// tail block's cursor. After this call the scratch token must not be used again.
func void scratch_end(scratch* scr);
