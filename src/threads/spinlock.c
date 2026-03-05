// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/spinlock.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "memory/vmem.h"
#include "../sdl3_include.h"

func spinlock _spinlock_create(callsite site) {
  allocator alloc = thread_get_allocator();
  (void)site;
  if (alloc.alloc_fn == NULL || alloc.dealloc_fn == NULL) {
    alloc = vmem_get_allocator();
  }
  assert(alloc.alloc_fn != NULL);
  assert(alloc.dealloc_fn != NULL);

  SDL_SpinLock* spl = (SDL_SpinLock*)allocator_alloc(&alloc, size_of(SDL_SpinLock));
  if (spl) {
    *spl = 0;
  }
  if (spl != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_SPINLOCK;
    lifecycle_msg.object_lifecycle.object_ptr = spl;
    if (!msg_post(&lifecycle_msg)) {
      allocator_dealloc(&alloc, spl, size_of(SDL_SpinLock));
      return NULL;
    }
    thread_log_trace("spinlock_create: handle=%p", spl);
  }
  return (spinlock)spl;
}

func void _spinlock_destroy(spinlock sl, callsite site) {
  allocator alloc = thread_get_allocator();
  (void)site;
  if (!sl) {
    return;
  }
  if (alloc.dealloc_fn == NULL) {
    alloc = vmem_get_allocator();
  }
  assert(alloc.dealloc_fn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_SPINLOCK;
  lifecycle_msg.object_lifecycle.object_ptr = sl;
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
