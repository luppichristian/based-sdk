// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/ctx.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"

#include <string.h>

// Default reserved growth size for each context-local allocator.
const_var sz CTX_DEFAULT_BLOCK_SIZE = kb(64);

func b32 ctx_init(ctx* context, allocator main_allocator, b32 use_log_mutex) {
  if (!context || !main_allocator.alloc_fn || context->is_init) {
    return false;
  }
  assert(main_allocator.dealloc_fn != NULL);
  assert(use_log_mutex == 0 || use_log_mutex == 1);

  memset(context, 0, size_of(*context));
  context->main_allocator = main_allocator;
  if (!log_state_init(&context->log, use_log_mutex)) {
    memset(context, 0, size_of(*context));
    return false;
  }

  context->perm_arena = arena_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->temp_arena = arena_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->perm_heap = heap_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->temp_heap = heap_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->is_init = true;
  thread_log_trace("ctx_init: context=%p use_log_mutex=%u", (void*)context, (u32)use_log_mutex);
  return true;
}

func void ctx_quit(ctx* context) {
  if (!context || !context->is_init) {
    return;
  }

  heap_destroy(&context->temp_heap);
  heap_destroy(&context->perm_heap);
  arena_destroy(&context->temp_arena);
  arena_destroy(&context->perm_arena);
  log_state_quit(&context->log);
  thread_log_trace("ctx_quit: context=%p", (void*)context);
  memset(context, 0, size_of(*context));
}

func b32 ctx_is_init(ctx* context) {
  return context != NULL && context->is_init;
}

func allocator ctx_get_allocator(ctx* context) {
  allocator alloc = {0};
  if (!ctx_is_init(context)) {
    return alloc;
  }
  return heap_get_allocator(&context->perm_heap);
}

func log_state* ctx_get_log_state(ctx* context) {
  if (!ctx_is_init(context)) {
    return NULL;
  }
  return &context->log;
}

func arena* ctx_get_perm_arena(ctx* context) {
  return ctx_is_init(context) ? &context->perm_arena : NULL;
}

func arena* ctx_get_temp_arena(ctx* context) {
  return ctx_is_init(context) ? &context->temp_arena : NULL;
}

func heap* ctx_get_perm_heap(ctx* context) {
  return ctx_is_init(context) ? &context->perm_heap : NULL;
}

func heap* ctx_get_temp_heap(ctx* context) {
  return ctx_is_init(context) ? &context->temp_heap : NULL;
}

func void* ctx_get_user_data(ctx* context, ctx_user_data_idx idx) {
  if (!ctx_is_init(context) || idx >= CTX_USER_DATA_COUNT) {
    return NULL;
  }
  assert(idx < CTX_USER_DATA_COUNT);
  return context->user_data[idx];
}

func b32 ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data) {
  if (!ctx_is_init(context) || idx >= CTX_USER_DATA_COUNT) {
    return false;
  }
  assert(idx < CTX_USER_DATA_COUNT);

  context->user_data[idx] = user_data;
  return true;
}

func void ctx_clear_temp(ctx* context) {
  if (!ctx_is_init(context)) {
    return;
  }

  arena_clear(&context->temp_arena);
  heap_clear(&context->temp_heap);
}
