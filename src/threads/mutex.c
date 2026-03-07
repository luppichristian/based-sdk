// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/mutex.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"

func mutex _mutex_create(callsite site) {
  (void)site;
  mutex handle = (mutex)SDL_CreateMutex();
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_MUTEX,
                                                       .object_ptr = handle,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyMutex((SDL_Mutex*)handle);
      return NULL;
    }
    thread_log_trace("mutex_create: handle=%p", handle);
  }
  return handle;
}

func b32 _mutex_destroy(mutex mtx, callsite site) {
  (void)site;
  if (!mtx) {
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_MUTEX,
                                                     .object_ptr = mtx,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    return 0;
  }
  thread_log_trace("mutex_destroy: handle=%p", mtx);
  SDL_DestroyMutex((SDL_Mutex*)mtx);
  return 1;
}

func b32 mutex_is_valid(mutex mtx) {
  return mtx != NULL;
}

func void mutex_lock(mutex mtx) {
  if (mtx == NULL) {
    return;
  }
  assert(mtx != NULL);
  SDL_LockMutex((SDL_Mutex*)mtx);
}

func b32 mutex_trylock(mutex mtx) {
  if (mtx == NULL) {
    return 0;
  }
  assert(mtx != NULL);
  return SDL_TryLockMutex((SDL_Mutex*)mtx);
}

func void mutex_unlock(mutex mtx) {
  if (mtx == NULL) {
    return;
  }
  assert(mtx != NULL);
  SDL_UnlockMutex((SDL_Mutex*)mtx);
}

