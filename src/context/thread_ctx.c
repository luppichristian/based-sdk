// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/thread_ctx.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "threads/thread_current.h"
#include "basic/profiler.h"

#include <string.h>

// One context per OS thread. It remains zero-initialized until thread_ctx_init succeeds.
thread_local global_var ctx thread_ctx = {0};

func ctx* thread_ctx_get(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!thread_ctx.is_init) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return &thread_ctx;
}

func b32 thread_ctx_is_init(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return thread_ctx.is_init;
}

func allocator thread_get_allocator(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  ctx* context = thread_ctx_get();
  if (context == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return global_get_allocator();
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_get_allocator(context);
}

func log_state* thread_get_log_state(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  log_state* state = ctx_get_log_state(thread_ctx_get());
  if (!state) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return global_get_log_state();
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return state;
}

func arena* thread_get_perm_arena(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_get_perm_arena(thread_ctx_get());
}

func arena* thread_get_temp_arena(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_get_temp_arena(thread_ctx_get());
}

func heap* thread_get_perm_heap(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_get_perm_heap(thread_ctx_get());
}

func heap* thread_get_temp_heap(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_get_temp_heap(thread_ctx_get());
}

func void* thread_get_user_data(ctx_user_data_idx idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_get_user_data(thread_ctx_get(), idx);
}

func b32 thread_set_user_data(ctx_user_data_idx idx, void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ctx_set_user_data(thread_ctx_get(), idx, user_data);
}

func void thread_clear_temp(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  ctx_clear_temp(thread_ctx_get());
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void thread_log_set_level(log_level level) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  log_state_set_level(thread_get_log_state(), level);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 thread_log_sync(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  ctx* context = thread_ctx_get();
  if (!context) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  log_state_sync(&context->log, global_get_log_state());
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func void thread_log_begin_frame(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  log_state_begin_frame(thread_get_log_state());
  TracyCZoneEnd(__tracy_zone_ctx);
}

func log_frame* thread_log_end_frame(u32 severity_mask) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return log_state_end_frame(thread_get_log_state(), severity_mask);
}

func b32 thread_ctx_init(allocator main_allocator) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!main_allocator.alloc_fn || thread_ctx.is_init) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }
  assert(main_allocator.dealloc_fn != NULL);

  msg thread_ctx_msg = {0};
  thread_ctx_msg.type = MSG_CORE_TYPE_THREAD_CTX;
  msg_core_thread_ctx_data thread_ctx_data = {
      .event_kind = MSG_CORE_THREAD_CTX_EVENT_INIT,
      .thread_id = thread_id(),
      .ctx_ptr = &thread_ctx,
  };
  msg_core_fill_thread_ctx(&thread_ctx_msg, &thread_ctx_data);
  if (!msg_post(&thread_ctx_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  memset(&thread_ctx, 0, size_of(thread_ctx));
  if (!ctx_init(&thread_ctx, main_allocator, NULL, false)) {
    memset(&thread_ctx, 0, size_of(thread_ctx));
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  thread_log_trace("thread_ctx_init: thread_id=%llu", (unsigned long long)thread_id());
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func void thread_ctx_quit(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!thread_ctx.is_init) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  msg thread_ctx_msg = {0};
  thread_ctx_msg.type = MSG_CORE_TYPE_THREAD_CTX;
  msg_core_thread_ctx_data thread_ctx_data = {
      .event_kind = MSG_CORE_THREAD_CTX_EVENT_QUIT,
      .thread_id = thread_id(),
      .ctx_ptr = &thread_ctx,
  };
  msg_core_fill_thread_ctx(&thread_ctx_msg, &thread_ctx_data);
  if (!msg_post(&thread_ctx_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  thread_log_trace("thread_ctx_quit: thread_id=%llu", (unsigned long long)thread_id());
  ctx_quit(&thread_ctx);
  TracyCZoneEnd(__tracy_zone_ctx);
}
