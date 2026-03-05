// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/ctx.h"
#include "basic/utility_defines.h"

#include <string.h>

// Default reserved growth size for each context-local allocator.
const_var sz CTX_DEFAULT_BLOCK_SIZE = kb(64);

func b32 ctx_init(ctx* context, allocator main_allocator, b32 use_log_mutex) {
  if (!context || !main_allocator.alloc_fn || context->is_init) {
    return false;
  }

  memset(context, 0, sizeof(*context));
  if (!log_state_init(&context->log, use_log_mutex)) {
    memset(context, 0, sizeof(*context));
    return false;
  }

  context->main_allocator = main_allocator;
  context->perm_arena = arena_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->temp_arena = arena_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->perm_heap = heap_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->temp_heap = heap_create(main_allocator, NULL, CTX_DEFAULT_BLOCK_SIZE);
  context->is_init = true;
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
  memset(context, 0, sizeof(*context));
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

func void* ctx_get_user_data(ctx* context, ctx_user_data_index index) {
  if (!ctx_is_init(context) || index >= CTX_USER_DATA_COUNT) {
    return NULL;
  }
  return context->user_data[index];
}

func b32 ctx_set_user_data(ctx* context, ctx_user_data_index index, void* user_data) {
  if (!ctx_is_init(context) || index >= CTX_USER_DATA_COUNT) {
    return false;
  }

  context->user_data[index] = user_data;
  return true;
}

func void ctx_clear_temp(ctx* context) {
  if (!ctx_is_init(context)) {
    return;
  }

  arena_clear(&context->temp_arena);
  heap_clear(&context->temp_heap);
}
