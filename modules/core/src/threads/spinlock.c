// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/spinlock.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func spinlock _spinlock_create(callsite site) {
  profile_func_begin;

  heap* hp = thread_get_perm_heap();
  if (!hp) {
    thread_log_error("Thread ctx heap allocator is not available");
    profile_func_end;
    return NULL;
  }

  SDL_SpinLock* spl = heap_alloc_type(hp, SDL_SpinLock);
  if (spl == NULL) {
    thread_log_error("Failed to create spinlock");
    profile_func_end;
    return NULL;
  }

  *spl = 0;

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_SPINLOCK,
      .object_ptr = spl,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    heap_dealloc(hp, spl);
    thread_log_trace("Spinlock creation was suspended");
    profile_func_end;
    return NULL;
  }

  thread_log_trace("Created spinlock handle=%p", spl);
  profile_func_end;
  return (spinlock)spl;
}

func b32 _spinlock_destroy(spinlock sl, callsite site) {
  profile_func_begin;

  heap* hp = thread_get_perm_heap();
  if (!hp) {
    thread_log_error("Thread ctx heap allocator is not available");
    profile_func_end;
    return false;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_SPINLOCK,
      .object_ptr = sl,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Spinlock destruction was suspended handle=%p", sl);
    profile_func_end;
    return false;
  }

  thread_log_trace("Destroyed spinlock handle=%p", sl);
  heap_dealloc(hp, sl);
  profile_func_end;
  return true;
}

func b32 spinlock_is_valid(spinlock sl) {
  return sl != 0;
}

func void spinlock_lock(spinlock sl) {
  profile_func_begin;
  if (sl == NULL) {
    thread_log_error("Rejected spinlock lock for invalid handle");
    profile_func_end;
    return;
  }
  assert(sl != NULL);
  SDL_LockSpinlock((SDL_SpinLock*)(sl));
  profile_func_end;
}

func void spinlock_unlock(spinlock sl) {
  profile_func_begin;
  if (sl == NULL) {
    thread_log_error("Rejected spinlock unlock for invalid handle");
    profile_func_end;
    return;
  }
  assert(sl != NULL);
  SDL_UnlockSpinlock((SDL_SpinLock*)(sl));
  profile_func_end;
}

func b32 spinlock_try_lock(spinlock sl) {
  profile_func_begin;
  if (sl == NULL) {
    thread_log_error("Rejected spinlock try lock for invalid handle");
    profile_func_end;
    return false;
  }
  assert(sl != NULL);
  b32 res = SDL_TryLockSpinlock((SDL_SpinLock*)(sl));
  profile_func_end;
  return res;
}
