// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process_current.h"
#include "basic/entry.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "processes/process.h"
#include "basic/profiler.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#elif defined(PLATFORM_UNIX)
#  include <fcntl.h>
#  include <sys/file.h>
#  include <sys/resource.h>
#  include <unistd.h>
#endif

#if defined(PLATFORM_WINDOWS)
// Mapping of our process_priority enum to Win32 priority classes.
const_var DWORD win32_process_priorities[] = {
    BELOW_NORMAL_PRIORITY_CLASS,
    NORMAL_PRIORITY_CLASS,
    HIGH_PRIORITY_CLASS,
    REALTIME_PRIORITY_CLASS,
};
#endif

global_var b32 process_unique_init_done = false;
global_var b32 process_unique_is_single = false;

#if defined(PLATFORM_WINDOWS)
global_var HANDLE process_unique_handle = NULL;
#elif defined(PLATFORM_UNIX)
global_var i32 process_unique_fd = -1;
#endif

func u64 process_hash_str(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  u64 hash_value = 1469598103934665603ULL;
  for (sz idx = 0; src[idx] != '\0'; idx += 1) {
    hash_value ^= (u8)src[idx];
    hash_value *= 1099511628211ULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return hash_value;
}

func b32 process_unique_init_once(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (process_unique_init_done) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return process_unique_is_single;
  }

  cmdline cmdl = entry_get_cmdline();
  cstr8 exe_name = cmdline_get_program(cmdl);
  u64 key_hash = process_hash_str(exe_name != NULL ? exe_name : "based");

#if defined(PLATFORM_WINDOWS)
  c8 mutex_name[96] = {0};
  (void)snprintf(mutex_name, size_of(mutex_name), "based-process-%llu", (unsigned long long)key_hash);
  process_unique_handle = CreateMutexA(NULL, TRUE, mutex_name);
  process_unique_is_single = process_unique_handle != NULL && GetLastError() != ERROR_ALREADY_EXISTS;
#elif defined(PLATFORM_UNIX)
  c8 lock_path[128] = {0};
  (void)snprintf(lock_path, size_of(lock_path), "/tmp/based-process-%llu.lock", (unsigned long long)key_hash);
  process_unique_fd = open(lock_path, O_RDWR | O_CREAT, 0666);
  if (process_unique_fd < 0) {
    process_unique_is_single = false;
  } else {
    process_unique_is_single = flock(process_unique_fd, LOCK_EX | LOCK_NB) == 0;
  }
#else
  process_unique_is_single = false;
#endif

  process_unique_init_done = true;
  TracyCZoneEnd(__tracy_zone_ctx);
  return process_unique_is_single;
}

func process_priority process_get_priority(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS)
  DWORD priority_class = GetPriorityClass(GetCurrentProcess());

  switch (priority_class) {
    case IDLE_PRIORITY_CLASS:
    case BELOW_NORMAL_PRIORITY_CLASS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return PROCESS_PRIORITY_LOW;

    case ABOVE_NORMAL_PRIORITY_CLASS:
    case HIGH_PRIORITY_CLASS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return PROCESS_PRIORITY_HIGH;

    case REALTIME_PRIORITY_CLASS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return PROCESS_PRIORITY_REALTIME;

    case NORMAL_PRIORITY_CLASS:
    default:
      TracyCZoneEnd(__tracy_zone_ctx);
      return PROCESS_PRIORITY_NORMAL;
  }
#elif defined(PLATFORM_UNIX)
  errno = 0;
  i32 nice_value = (i32)getpriority(PRIO_PROCESS, 0);
  if (errno != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PROCESS_PRIORITY_NORMAL;
  }

  if (nice_value <= -15) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PROCESS_PRIORITY_REALTIME;
  }
  if (nice_value <= -5) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PROCESS_PRIORITY_HIGH;
  }
  if (nice_value >= 10) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PROCESS_PRIORITY_LOW;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return PROCESS_PRIORITY_NORMAL;
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return PROCESS_PRIORITY_NORMAL;
#endif
}

func b32 process_set_priority(process_priority priority) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (priority < PROCESS_PRIORITY_LOW || priority > PROCESS_PRIORITY_REALTIME) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(priority >= PROCESS_PRIORITY_LOW && priority <= PROCESS_PRIORITY_REALTIME);

#if defined(PLATFORM_WINDOWS)
  b32 success = SetPriorityClass(GetCurrentProcess(), win32_process_priorities[priority]) != 0;
  thread_log_trace("process_set_priority: priority=%u success=%u", (u32)priority, (u32)success);
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
#elif defined(PLATFORM_UNIX)
  i32 nice_value = 0;

  switch (priority) {
    case PROCESS_PRIORITY_LOW:
      nice_value = 10;
      break;

    case PROCESS_PRIORITY_HIGH:
      nice_value = -10;
      break;

    case PROCESS_PRIORITY_REALTIME:
      nice_value = -20;
      break;

    case PROCESS_PRIORITY_NORMAL:
    default:
      nice_value = 0;
      break;
  }

  b32 success = setpriority(PRIO_PROCESS, 0, (int)nice_value) == 0;
  thread_log_trace("process_set_priority: priority=%u success=%u", (u32)priority, (u32)success);
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
#else
  (void)priority;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}

func b32 process_is_unique(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  b32 result = process_unique_init_once();
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 process_restart(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  cmdline cmdl = entry_get_cmdline();
  if (cmdl.count == 0 || cmdl.args == NULL || cmdline_get_program(cmdl) == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  process_options options = process_options_default();
  options.background = true;
  process spawned = process_create_with((cstr8 const*)cmdl.args, options);
  if (!process_is_valid(spawned)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  process_destroy(spawned);
  TracyCZoneEnd(__tracy_zone_ctx);
  process_exit(0);
  return true;
}

func u64 process_id(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS)
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)GetCurrentProcessId();
#elif defined(PLATFORM_UNIX)
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)getpid();
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}

func no_return void process_exit(i32 exit_code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  thread_log_fatal("process_exit: code=%d", exit_code);
  TracyCZoneEnd(__tracy_zone_ctx);
#if defined(PLATFORM_WINDOWS)
  ExitProcess((UINT)exit_code);
#else
  exit(exit_code);
#endif
}

func no_return void process_abort(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  thread_log_fatal("process_abort");
  TracyCZoneEnd(__tracy_zone_ctx);
  abort();
}

