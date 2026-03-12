// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/mutex.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "threads/atomics.h"
#include "basic/safe.h"

func mutex _mutex_create(callsite site) {
  profile_func_begin;
  mutex handle = (mutex)SDL_CreateMutex();
  if (handle == NULL) {
    thread_log_error("Failed to create mutex error=%s", SDL_GetError());
    profile_func_end;
    return NULL;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_MUTEX,
      .object_ptr = handle,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Mutex creation was suspended handle=%p", handle);
    SDL_DestroyMutex((SDL_Mutex*)handle);
    profile_func_end;
    return NULL;
  }

  thread_log_trace("Created mutex handle=%p", handle);
  profile_func_end;
  return handle;
}

func b32 _mutex_destroy(mutex mtx, callsite site) {
  profile_func_begin;
  if (!mtx) {
    thread_log_warn("Skipping mutex destroy for invalid handle");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_MUTEX,
      .object_ptr = mtx,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Mutex destruction was suspended handle=%p", mtx);
    profile_func_end;
    return false;
  }

  thread_log_trace("Destroyed mutex handle=%p", mtx);
  SDL_DestroyMutex((SDL_Mutex*)mtx);
  profile_func_end;
  return true;
}

func b32 mutex_is_valid(mutex mtx) {
  return mtx != NULL;
}

func void mutex_lock(mutex mtx) {
  profile_func_begin;
  if (mtx == NULL) {
    thread_log_error("Rejected mutex lock for invalid handle");
    profile_func_end;
    return;
  }
  assert(mtx != NULL);
  SDL_LockMutex((SDL_Mutex*)mtx);
  profile_func_end;
}

func b32 mutex_trylock(mutex mtx) {
  profile_func_begin;
  if (mtx == NULL) {
    thread_log_error("Rejected mutex try lock for invalid handle");
    profile_func_end;
    return false;
  }
  assert(mtx != NULL);
  profile_func_end;
  return SDL_TryLockMutex((SDL_Mutex*)mtx);
}

func b32 mutex_timed_lock(mutex mtx, i32 timeout_ms) {
  profile_func_begin;
  if (mtx == NULL || timeout_ms < 0) {
    thread_log_error("Rejected mutex timed lock handle=%p timeout_ms=%d", mtx, timeout_ms);
    profile_func_end;
    return false;
  }

  u64 start_ticks = SDL_GetTicks();
  safe_while (!mutex_trylock(mtx)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      thread_log_warn("Mutex timed lock expired handle=%p timeout_ms=%d", mtx, timeout_ms);
      profile_func_end;
      return false;
    }

    atomic_pause();
  }

  profile_func_end;
  return true;
}

func void mutex_unlock(mutex mtx) {
  profile_func_begin;
  if (mtx == NULL) {
    thread_log_error("Rejected mutex unlock for invalid handle");
    profile_func_end;
    return;
  }
  assert(mtx != NULL);
  SDL_UnlockMutex((SDL_Mutex*)mtx);
  profile_func_end;
}
