// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/ctx.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"
#include "basic/profiler.h"

#include <string.h>

// Default reserved growth size for each context-local allocator.
const_var sz CTX_DEFAULT_BLOCK_SIZE = kb(64);

func b32 ctx_init(ctx* context, allocator main_allocator, mutex allocator_mutex, b32 use_log_mutex) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!context || !main_allocator.alloc_fn || context->is_init) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }
  assert(main_allocator.dealloc_fn != NULL);
  assert(use_log_mutex == 0 || use_log_mutex == 1);

  memset(context, 0, size_of(*context));
  context->main_allocator = main_allocator;
  if (!log_state_init(&context->log, use_log_mutex)) {
    memset(context, 0, size_of(*context));
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  context->perm_arena = arena_create(main_allocator, allocator_mutex, CTX_DEFAULT_BLOCK_SIZE);
  context->temp_arena = arena_create(main_allocator, allocator_mutex, CTX_DEFAULT_BLOCK_SIZE);
  context->perm_heap = heap_create(main_allocator, allocator_mutex, CTX_DEFAULT_BLOCK_SIZE);
  context->temp_heap = heap_create(main_allocator, allocator_mutex, CTX_DEFAULT_BLOCK_SIZE);
  context->is_init = true;
  thread_log_trace("ctx_init: context=%p use_log_mutex=%u", (void*)context, (u32)use_log_mutex);
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func void ctx_quit(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!context || !context->is_init) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  log_state_quit(&context->log);
  heap_destroy(&context->temp_heap);
  heap_destroy(&context->perm_heap);
  arena_destroy(&context->temp_arena);
  arena_destroy(&context->perm_arena);
  thread_log_trace("ctx_quit: context=%p", (void*)context);
  memset(context, 0, size_of(*context));
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 ctx_is_init(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return context != NULL && context->is_init;
}

func allocator ctx_get_allocator(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = {0};
  if (!ctx_is_init(context)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return alloc;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return heap_get_allocator(&context->perm_heap);
}

func log_state* ctx_get_log_state(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!ctx_is_init(context)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return &context->log;
}

func arena* ctx_get_perm_arena(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_is_init(context) ? &context->perm_arena : NULL;
}

func arena* ctx_get_temp_arena(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_is_init(context) ? &context->temp_arena : NULL;
}

func heap* ctx_get_perm_heap(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_is_init(context) ? &context->perm_heap : NULL;
}

func heap* ctx_get_temp_heap(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_is_init(context) ? &context->temp_heap : NULL;
}

func void* ctx_get_user_data(ctx* context, ctx_user_data_idx idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!ctx_is_init(context) || idx >= CTX_USER_DATA_COUNT) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(idx < CTX_USER_DATA_COUNT);
  TracyCZoneEnd(__tracy_zone_ctx);
  return context->user_data[idx];
}

func b32 ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!ctx_is_init(context) || idx >= CTX_USER_DATA_COUNT) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }
  assert(idx < CTX_USER_DATA_COUNT);

  context->user_data[idx] = user_data;
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func void ctx_clear_temp(ctx* context) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!ctx_is_init(context)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  arena_clear(&context->temp_arena);
  heap_clear(&context->temp_heap);
  TracyCZoneEnd(__tracy_zone_ctx);
}
