// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_current.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"

// Tracks the scheduling priority of the current thread; defaults to MEDIUM (OS default).
thread_local global_var thread_priority tls_priority = THREAD_PRIORITY_MEDIUM;

// Mapping of our thread_priority enum to SDL_ThreadPriority values.
const_var SDL_ThreadPriority SDL_THREAD_PRIORITIES[] = {
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
  profile_func_begin;
  if (priority >= count_of(SDL_THREAD_PRIORITIES)) {
    thread_log_error("Passed invalid priority in thread_set_priority");
    profile_func_end;
    return false;  // Invalid priority
  }
  assert(priority < count_of(SDL_THREAD_PRIORITIES));

  b32 ok = SDL_SetCurrentThreadPriority(SDL_THREAD_PRIORITIES[priority]);
  if (ok) {
    tls_priority = priority;
  }
  thread_log_trace("thread_set_priority: priority=%u success=%u", (u32)priority, (u32)ok);
  profile_func_end;
  return ok;
}

func void thread_sleep(u32 millis) {
  profile_func_begin;
  SDL_Delay((Uint32)millis);
  profile_func_end;
}

func void thread_yield(void) {
  profile_func_begin;
  SDL_Delay(0);
  profile_func_end;
}

func void thread_sleep_ns(u64 nanos) {
  profile_func_begin;
  SDL_DelayNS((Uint64)nanos);
  profile_func_end;
}

func void thread_sleep_precise(u64 nanos) {
  profile_func_begin;
  SDL_DelayPrecise((Uint64)nanos);
  profile_func_end;
}
