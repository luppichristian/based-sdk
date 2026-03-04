// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_ctx.h"
#include "basic/utility_defines.h"
#include <string.h>

// Default reserved growth size for each thread-local allocator.
const_var sz THREAD_CTX_DEFAULT_BLOCK_SIZE = kb(64);

// One context per OS thread. It remains zero-initialized until thread_ctx_init succeeds.
thread_local global_var thread_ctx tls_thread_ctx = {0};

func thread_ctx* thread_ctx_get(void) {
  if (!tls_thread_ctx.is_initialized) {
    return NULL;
  }
  return &tls_thread_ctx;
}

func b32 thread_ctx_is_initialized(void) {
  return tls_thread_ctx.is_initialized;
}

func allocator thread_ctx_get_allocator(void) {
  thread_ctx* ctx = thread_ctx_get();
  if (!ctx) {
    allocator alloc;
    memset(&alloc, 0, sizeof(alloc));
    return alloc;
  }
  return heap_get_allocator(&ctx->perm_heap);
}

func log_state* thread_ctx_get_log_state(void) {
  thread_ctx* ctx = thread_ctx_get();
  if (!ctx) {
    return log_get_global_state();
  }
  return &ctx->log;
}

func arena* thread_ctx_get_perm_arena(void) {
  thread_ctx* ctx = thread_ctx_get();
  return ctx ? &ctx->perm_arena : NULL;
}

func arena* thread_ctx_get_temp_arena(void) {
  thread_ctx* ctx = thread_ctx_get();
  return ctx ? &ctx->temp_arena : NULL;
}

func heap* thread_ctx_get_perm_heap(void) {
  thread_ctx* ctx = thread_ctx_get();
  return ctx ? &ctx->perm_heap : NULL;
}

func heap* thread_ctx_get_temp_heap(void) {
  thread_ctx* ctx = thread_ctx_get();
  return ctx ? &ctx->temp_heap : NULL;
}

func void* thread_ctx_get_user_data(sz index) {
  thread_ctx* ctx = thread_ctx_get();
  if (!ctx || index >= THREAD_CTX_USER_DATA_COUNT) {
    return NULL;
  }
  return ctx->user_data[index];
}

func b32 thread_ctx_set_user_data(sz index, void* user_data) {
  thread_ctx* ctx = thread_ctx_get();
  if (!ctx || index >= THREAD_CTX_USER_DATA_COUNT) {
    return false;
  }

  ctx->user_data[index] = user_data;
  return true;
}

func void thread_ctx_clear_temp(void) {
  thread_ctx* ctx = thread_ctx_get();
  if (!ctx) {
    return;
  }

  arena_clear(&ctx->temp_arena);
  heap_clear(&ctx->temp_heap);
}

func void thread_ctx_log_set_level(log_level level) {
  log_state_set_level(thread_ctx_get_log_state(), level);
}

func void thread_ctx_log_set_callback(log_callback callback) {
  log_state_set_callback(thread_ctx_get_log_state(), callback);
}

func b32 thread_ctx_log_sync_from_main(void) {
  thread_ctx* ctx = thread_ctx_get();
  if (!ctx) {
    return false;
  }

  log_state_sync(&ctx->log, log_get_global_state());
  return true;
}

func void thread_ctx_log_begin_frame(void) {
  log_state_begin_frame(thread_ctx_get_log_state());
}

func log_frame* thread_ctx_log_end_frame(u32 severity_mask) {
  return log_state_end_frame(thread_ctx_get_log_state(), severity_mask);
}

func b32 thread_ctx_init(allocator main_allocator) {
  if (!main_allocator.alloc_fn || tls_thread_ctx.is_initialized) {
    return false;
  }

  memset(&tls_thread_ctx, 0, sizeof(tls_thread_ctx));
  if (!log_state_init(&tls_thread_ctx.log, false)) {
    memset(&tls_thread_ctx, 0, sizeof(tls_thread_ctx));
    return false;
  }

  tls_thread_ctx.main_allocator = main_allocator;
  tls_thread_ctx.perm_arena = arena_create(main_allocator, NULL, THREAD_CTX_DEFAULT_BLOCK_SIZE);
  tls_thread_ctx.temp_arena = arena_create(main_allocator, NULL, THREAD_CTX_DEFAULT_BLOCK_SIZE);
  tls_thread_ctx.perm_heap = heap_create(main_allocator, NULL, THREAD_CTX_DEFAULT_BLOCK_SIZE);
  tls_thread_ctx.temp_heap = heap_create(main_allocator, NULL, THREAD_CTX_DEFAULT_BLOCK_SIZE);
  tls_thread_ctx.is_initialized = true;
  return true;
}

func void thread_ctx_quit(void) {
  if (!tls_thread_ctx.is_initialized) {
    return;
  }

  heap_destroy(&tls_thread_ctx.temp_heap);
  heap_destroy(&tls_thread_ctx.perm_heap);
  arena_destroy(&tls_thread_ctx.temp_arena);
  arena_destroy(&tls_thread_ctx.perm_arena);
  log_state_quit(&tls_thread_ctx.log);
  memset(&tls_thread_ctx, 0, sizeof(tls_thread_ctx));
}
