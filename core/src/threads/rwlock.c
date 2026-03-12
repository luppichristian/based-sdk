// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/rwlock.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "threads/atomics.h"
#include "basic/safe.h"

func rwlock _rwlock_create(callsite site) {
  profile_func_begin;
  rwlock handle = (rwlock)SDL_CreateRWLock();
  if (handle == NULL) {
    thread_log_error("Failed to create rwlock error=%s", SDL_GetError());
    profile_func_end;
    return NULL;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_RWLOCK,
      .object_ptr = handle,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("RWLock creation was suspended handle=%p", handle);
    SDL_DestroyRWLock((SDL_RWLock*)handle);
    profile_func_end;
    return NULL;
  }

  thread_log_trace("Created rwlock handle=%p", handle);
  profile_func_end;
  return handle;
}

func b32 _rwlock_destroy(rwlock rw, callsite site) {
  profile_func_begin;
  if (!rw) {
    thread_log_warn("Skipping rwlock destroy for invalid handle");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_RWLOCK,
      .object_ptr = rw,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("RWLock destruction was suspended handle=%p", rw);
    profile_func_end;
    return false;
  }

  thread_log_trace("Destroyed rwlock handle=%p", rw);
  SDL_DestroyRWLock((SDL_RWLock*)rw);
  profile_func_end;
  return true;
}

func b32 rwlock_is_valid(rwlock rw) {
  return rw != NULL;
}

func void rwlock_read_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    thread_log_error("Rejected rwlock read lock for invalid handle");
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_LockRWLockForReading((SDL_RWLock*)rw);
  profile_func_end;
}

func void rwlock_read_unlock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    thread_log_error("Rejected rwlock read unlock for invalid handle");
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_UnlockRWLock((SDL_RWLock*)rw);
  profile_func_end;
}

func void rwlock_write_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    thread_log_error("Rejected rwlock write lock for invalid handle");
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_LockRWLockForWriting((SDL_RWLock*)rw);
  profile_func_end;
}

func void rwlock_write_unlock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    thread_log_error("Rejected rwlock write unlock for invalid handle");
    profile_func_end;
    return;
  }
  assert(rw != NULL);
  SDL_UnlockRWLock((SDL_RWLock*)rw);
  profile_func_end;
}

func b32 rwlock_try_read_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    thread_log_error("Rejected rwlock try read lock for invalid handle");
    profile_func_end;
    return false;
  }
  assert(rw != NULL);
  b32 res = SDL_TryLockRWLockForReading((SDL_RWLock*)rw);
  profile_func_end;
  return res;
}

func b32 rwlock_try_write_lock(rwlock rw) {
  profile_func_begin;
  if (rw == NULL) {
    thread_log_error("Rejected rwlock try write lock for invalid handle");
    profile_func_end;
    return false;
  }
  assert(rw != NULL);
  b32 res = SDL_TryLockRWLockForWriting((SDL_RWLock*)rw);
  profile_func_end;
  return res;
}

func b32 rwlock_timed_read_lock(rwlock rw, i32 timeout_ms) {
  profile_func_begin;
  if (rw == NULL || timeout_ms < 0) {
    thread_log_error("Rejected rwlock timed read lock handle=%p timeout_ms=%d", rw, timeout_ms);
    profile_func_end;
    return false;
  }

  u64 start_ticks = SDL_GetTicks();
  safe_while (!rwlock_try_read_lock(rw)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      thread_log_warn("Rwlock timed read lock expired handle=%p timeout_ms=%d", rw, timeout_ms);
      profile_func_end;
      return false;
    }

    atomic_pause();
  }

  profile_func_end;
  return true;
}

func b32 rwlock_timed_write_lock(rwlock rw, i32 timeout_ms) {
  profile_func_begin;
  if (rw == NULL || timeout_ms < 0) {
    thread_log_error("Rejected rwlock timed write lock handle=%p timeout_ms=%d", rw, timeout_ms);
    profile_func_end;
    return false;
  }

  u64 start_ticks = SDL_GetTicks();
  safe_while (!rwlock_try_write_lock(rw)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      thread_log_warn("Rwlock timed write lock expired handle=%p timeout_ms=%d", rw, timeout_ms);
      profile_func_end;
      return false;
    }

    atomic_pause();
  }

  profile_func_end;
  return true;
}
