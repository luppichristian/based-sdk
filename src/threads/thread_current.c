// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_current.h"
#include "../sdl3_include.h"
#include "basic/utility_defines.h"

// Tracks the scheduling priority of the current thread; defaults to NORMAL (OS default).
thread_local global_var thread_priority tls_priority = THREAD_PRIORITY_NORMAL;

// Mapping of our thread_priority enum to SDL_ThreadPriority values.
read_only global_var SDL_ThreadPriority sdl_priorities[] = {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH,
    SDL_THREAD_PRIORITY_TIME_CRITICAL,
};

func u64 thread_id(void) {
  return (u64)SDL_GetCurrentThreadID();
}

func thread_priority thread_get_priority(void) {
  return tls_priority;
}

func b32 thread_set_priority(thread_priority priority) {
  if (priority >= count_of(sdl_priorities)) {
    return false;  // Invalid priority
  }

  b32 ok = SDL_SetCurrentThreadPriority(sdl_priorities[priority]);
  if (ok) {
    tls_priority = priority;
  }
  return ok;
}

func void thread_sleep(u32 millis) {
  SDL_Delay((Uint32)millis);
}

func void thread_yield(void) {
  SDL_Delay(0);
}

func void thread_sleep_ns(u64 nanos) {
  SDL_DelayNS((Uint64)nanos);
}

func void thread_sleep_precise(u64 nanos) {
  SDL_DelayPrecise((Uint64)nanos);
}