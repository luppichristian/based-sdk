// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/condvar.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"

func condvar _condvar_create(callsite site) {
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
      return NULL;
    }
    thread_log_trace("condvar_create: handle=%p", handle);
  }
  return handle;
}

func b32 _condvar_destroy(condvar cond, callsite site) {
  (void)site;
  if (!cond) {
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
    return 0;
  }
  thread_log_trace("condvar_destroy: handle=%p", cond);
  SDL_DestroyCondition((SDL_Condition*)cond);
  return 1;
}

func b32 condvar_is_valid(condvar cond) {
  return cond != NULL;
}

func void condvar_wait(condvar cond, mutex mtx) {
  if (cond == NULL || mtx == NULL) {
    return;
  }
  assert(cond != NULL);
  assert(mtx != NULL);
  SDL_WaitCondition((SDL_Condition*)cond, (SDL_Mutex*)mtx);
}

func b32 condvar_wait_timeout(condvar cond, mutex mtx, u32 millis) {
  if (cond == NULL || mtx == NULL) {
    return 0;
  }
  assert(cond != NULL);
  assert(mtx != NULL);
  return SDL_WaitConditionTimeout((SDL_Condition*)cond, (SDL_Mutex*)mtx, (Sint32)millis);
}

func void condvar_signal(condvar cond) {
  if (cond == NULL) {
    return;
  }
  assert(cond != NULL);
  SDL_SignalCondition((SDL_Condition*)cond);
}

func void condvar_broadcast(condvar cond) {
  if (cond == NULL) {
    return;
  }
  assert(cond != NULL);
  SDL_BroadcastCondition((SDL_Condition*)cond);
}

