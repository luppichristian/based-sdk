// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// SDL_ThreadFunction is `int (SDLCALL *)(void*)`.
// SDLCALL is __cdecl on Windows, which is the default C calling convention,
// so casting thread_func (i32 (*)(void*)) to SDL_ThreadFunction is safe on all targets.

typedef struct thread_entry_payload {
  thread_func entry;
  void* arg;
  allocator payload_allocator;
  allocator main_allocator;
  b32 should_init_thread_ctx;
} thread_entry_payload;

func allocator thread_allocator_resolve(allocator preferred_alloc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (preferred_alloc.alloc_fn != NULL && preferred_alloc.dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return preferred_alloc;
  }

  allocator thread_alloc = thread_get_allocator();
  if (thread_alloc.alloc_fn != NULL && thread_alloc.dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return thread_alloc;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return global_get_allocator();
}

func i32 thread_entry_wrapper(void* raw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  thread_entry_payload* payload = (thread_entry_payload*)raw;
  b32 has_thread_ctx = false;
  i32 exit_code = 0;

  if (payload == NULL || payload->entry == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(payload->payload_allocator.dealloc_fn != NULL);

  if (payload->should_init_thread_ctx) {
    has_thread_ctx = thread_ctx_init(payload->main_allocator);
    thread_log_trace("thread_entry_wrapper: thread_ctx_init=%u", (u32)has_thread_ctx);
  }

  exit_code = payload->entry(payload->arg);

  if (has_thread_ctx) {
    thread_ctx_quit();
  }

  allocator_dealloc(&payload->payload_allocator, payload, size_of(*payload));
  TracyCZoneEnd(__tracy_zone_ctx);
  return exit_code;
}

func thread thread_create_impl(thread_func entry, void* arg, cstr8 name, allocator main_allocator) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator payload_allocator = {0};
  if (!entry) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  payload_allocator = thread_allocator_resolve(main_allocator);
  assert(payload_allocator.alloc_fn != NULL);
  assert(payload_allocator.dealloc_fn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
                                                     .object_ptr = NULL,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  thread_entry_payload* payload = (thread_entry_payload*)allocator_alloc(&payload_allocator, size_of(*payload));
  if (!payload) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  SDL_zero(*payload);
  payload->entry = entry;
  payload->arg = arg;
  payload->payload_allocator = payload_allocator;
  if (main_allocator.alloc_fn) {
    payload->main_allocator = main_allocator;
    payload->should_init_thread_ctx = true;
  }

  thread thd = (thread)SDL_CreateThread(thread_entry_wrapper, name, payload);
  if (!thd) {
    allocator_dealloc(&payload_allocator, payload, size_of(*payload));
    thread_log_error("thread_create_impl: SDL_CreateThread failed name=%s", name != NULL ? name : "<null>");
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  thread_log_trace("thread_create_impl: created thread=%p name=%s", thd, name != NULL ? name : "<null>");
  TracyCZoneEnd(__tracy_zone_ctx);
  return thd;
}

func thread _thread_create(thread_func entry, void* arg, allocator main_allocator, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  TracyCZoneEnd(__tracy_zone_ctx);
  return thread_create_impl(entry, arg, NULL, main_allocator);
}

func thread _thread_create_named(
    thread_func entry,
    void* arg,
    cstr8 name,
    allocator main_allocator,
    callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  TracyCZoneEnd(__tracy_zone_ctx);
  return thread_create_impl(entry, arg, name, main_allocator);
}

func b32 thread_is_valid(thread thd) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return thd != NULL;
}

func b32 thread_join(thread thd, i32* out_exit_code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!thd) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(thread_is_valid(thd));
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
                                                     .object_ptr = thd,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  SDL_WaitThread((SDL_Thread*)thd, (int*)out_exit_code);
  thread_log_trace("thread_join: thread=%p", thd);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func void thread_detach(thread thd) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!thd) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD,
                                                     .object_ptr = thd,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  SDL_DetachThread((SDL_Thread*)thd);
  thread_log_trace("thread_detach: thread=%p", thd);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func u64 thread_get_id(thread thd) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)SDL_GetThreadID((SDL_Thread*)thd);
}

func cstr8 thread_get_name(thread thd) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetThreadName((SDL_Thread*)thd);
}
