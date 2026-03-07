// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/spinlock.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"

func allocator spinlock_allocator_resolve(void) {
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    return alloc;
  }
  return global_get_allocator();
}

func spinlock _spinlock_create(callsite site) {
  allocator alloc = spinlock_allocator_resolve();
  (void)site;
  if (alloc.alloc_fn == NULL || alloc.dealloc_fn == NULL) {
    return NULL;
  }
  assert(alloc.alloc_fn != NULL);
  assert(alloc.dealloc_fn != NULL);

  SDL_SpinLock* spl = (SDL_SpinLock*)allocator_alloc(&alloc, size_of(SDL_SpinLock));
  if (spl) {
    *spl = 0;
  }
  if (spl != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_SPINLOCK,
                                                       .object_ptr = spl,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      allocator_dealloc(&alloc, spl, size_of(SDL_SpinLock));
      return NULL;
    }
    thread_log_trace("spinlock_create: handle=%p", spl);
  }
  return (spinlock)spl;
}

func void _spinlock_destroy(spinlock sl, callsite site) {
  allocator alloc = spinlock_allocator_resolve();
  (void)site;
  if (!sl) {
    return;
  }
  if (alloc.dealloc_fn == NULL) {
    return;
  }
  assert(alloc.dealloc_fn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_SPINLOCK,
                                                     .object_ptr = sl,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    return;
  }
  thread_log_trace("spinlock_destroy: handle=%p", sl);
  allocator_dealloc(&alloc, sl, size_of(SDL_SpinLock));
}

func b32 spinlock_is_valid(spinlock sl) {
  return sl != NULL;
}

func void spinlock_lock(spinlock sl) {
  if (sl == NULL) {
    return;
  }
  assert(sl != NULL);
  SDL_LockSpinlock((SDL_SpinLock*)sl);
}

func void spinlock_unlock(spinlock sl) {
  if (sl == NULL) {
    return;
  }
  assert(sl != NULL);
  SDL_UnlockSpinlock((SDL_SpinLock*)sl);
}

func b32 spinlock_try_lock(spinlock sl) {
  if (sl == NULL) {
    return 0;
  }
  assert(sl != NULL);
  return SDL_TryLockSpinlock((SDL_SpinLock*)sl);
}

