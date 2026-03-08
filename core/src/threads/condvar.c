// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/condvar.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func condvar _condvar_create(callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  condvar handle = (condvar)SDL_CreateCondition();
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_CONDVAR,
                                                       .object_ptr = handle,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyCondition((SDL_Condition*)handle);
      TracyCZoneEnd(__tracy_zone_ctx);
      return NULL;
    }
    thread_log_trace("condvar_create: handle=%p", handle);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return handle;
}

func b32 _condvar_destroy(condvar cond, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  if (!cond) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_CONDVAR,
                                                     .object_ptr = cond,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  thread_log_trace("condvar_destroy: handle=%p", cond);
  SDL_DestroyCondition((SDL_Condition*)cond);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 condvar_is_valid(condvar cond) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cond != NULL;
}

func void condvar_wait(condvar cond, mutex mtx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (cond == NULL || mtx == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(cond != NULL);
  assert(mtx != NULL);
  SDL_WaitCondition((SDL_Condition*)cond, (SDL_Mutex*)mtx);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 condvar_wait_timeout(condvar cond, mutex mtx, u32 millis) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (cond == NULL || mtx == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(cond != NULL);
  assert(mtx != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_WaitConditionTimeout((SDL_Condition*)cond, (SDL_Mutex*)mtx, (Sint32)millis);
}

func void condvar_signal(condvar cond) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (cond == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(cond != NULL);
  SDL_SignalCondition((SDL_Condition*)cond);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void condvar_broadcast(condvar cond) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (cond == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(cond != NULL);
  SDL_BroadcastCondition((SDL_Condition*)cond);
  TracyCZoneEnd(__tracy_zone_ctx);
}
