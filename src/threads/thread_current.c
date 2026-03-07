// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_current.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"

// Tracks the scheduling priority of the current thread; defaults to NORMAL (OS default).
thread_local global_var thread_priority tls_priority = THREAD_PRIORITY_NORMAL;

// Mapping of our thread_priority enum to SDL_ThreadPriority values.
const_var SDL_ThreadPriority sdl_priorities[] = {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH,
    SDL_THREAD_PRIORITY_TIME_CRITICAL,
};

func u64 thread_id(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)SDL_GetCurrentThreadID();
}

func thread_priority thread_get_priority(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return tls_priority;
}

func b32 thread_set_priority(thread_priority priority) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (priority >= count_of(sdl_priorities)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;  // Invalid priority
  }
  assert(priority < count_of(sdl_priorities));

  b32 ok = SDL_SetCurrentThreadPriority(sdl_priorities[priority]);
  if (ok) {
    tls_priority = priority;
  }
  thread_log_trace("thread_set_priority: priority=%u success=%u", (u32)priority, (u32)ok);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ok;
}

func void thread_sleep(u32 millis) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Delay((Uint32)millis);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void thread_yield(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Delay(0);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void thread_sleep_ns(u64 nanos) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_DelayNS((Uint64)nanos);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void thread_sleep_precise(u64 nanos) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_DelayPrecise((Uint64)nanos);
  TracyCZoneEnd(__tracy_zone_ctx);
}
