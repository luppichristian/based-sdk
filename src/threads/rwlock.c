// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/rwlock.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func rwlock _rwlock_create(callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  rwlock handle = (rwlock)SDL_CreateRWLock();
  if (handle != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_RWLOCK,
                                                       .object_ptr = handle,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyRWLock((SDL_RWLock*)handle);
      TracyCZoneEnd(__tracy_zone_ctx);
      return NULL;
    }
    thread_log_trace("rwlock_create: handle=%p", handle);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return handle;
}

func b32 _rwlock_destroy(rwlock rw, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  if (!rw) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_RWLOCK,
                                                     .object_ptr = rw,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  thread_log_trace("rwlock_destroy: handle=%p", rw);
  SDL_DestroyRWLock((SDL_RWLock*)rw);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 rwlock_is_valid(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return rw != NULL;
}

func void rwlock_read_lock(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (rw == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(rw != NULL);
  SDL_LockRWLockForReading((SDL_RWLock*)rw);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void rwlock_read_unlock(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (rw == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(rw != NULL);
  SDL_UnlockRWLock((SDL_RWLock*)rw);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void rwlock_write_lock(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (rw == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(rw != NULL);
  SDL_LockRWLockForWriting((SDL_RWLock*)rw);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void rwlock_write_unlock(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (rw == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(rw != NULL);
  SDL_UnlockRWLock((SDL_RWLock*)rw);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 rwlock_try_read_lock(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (rw == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(rw != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_TryLockRWLockForReading((SDL_RWLock*)rw);
}

func b32 rwlock_try_write_lock(rwlock rw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (rw == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(rw != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_TryLockRWLockForWriting((SDL_RWLock*)rw);
}
