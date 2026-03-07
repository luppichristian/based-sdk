// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/semaphore.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"

func semaphore _semaphore_create(u32 initial_count, callsite site) {
  (void)site;
  semaphore handle = (semaphore)SDL_CreateSemaphore((Uint32)initial_count);
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_SEMAPHORE,
                                                       .object_ptr = handle,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroySemaphore((SDL_Semaphore*)handle);
      return NULL;
    }
    thread_log_trace("semaphore_create: handle=%p count=%u", handle, initial_count);
  }
  return handle;
}

func b32 _semaphore_destroy(semaphore sem, callsite site) {
  (void)site;
  if (!sem) {
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_SEMAPHORE,
                                                     .object_ptr = sem,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    return 0;
  }
  thread_log_trace("semaphore_destroy: handle=%p", sem);
  SDL_DestroySemaphore((SDL_Semaphore*)sem);
  return 1;
}

func b32 semaphore_is_valid(semaphore sem) {
  return sem != NULL;
}

func void semaphore_wait(semaphore sem) {
  if (sem == NULL) {
    return;
  }
  assert(sem != NULL);
  SDL_WaitSemaphore((SDL_Semaphore*)sem);
}

func b32 semaphore_try_wait(semaphore sem) {
  if (sem == NULL) {
    return 0;
  }
  assert(sem != NULL);
  return SDL_TryWaitSemaphore((SDL_Semaphore*)sem);
}

func b32 semaphore_wait_timeout(semaphore sem, u32 millis) {
  if (sem == NULL) {
    return 0;
  }
  assert(sem != NULL);
  return SDL_WaitSemaphoreTimeout((SDL_Semaphore*)sem, (Sint32)millis);
}

func void semaphore_signal(semaphore sem) {
  if (sem == NULL) {
    return;
  }
  assert(sem != NULL);
  SDL_SignalSemaphore((SDL_Semaphore*)sem);
}

