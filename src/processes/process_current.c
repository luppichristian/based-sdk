// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process_current.h"
#include "basic/env_defines.h"

#include <errno.h>
#include <stdlib.h>

#if defined(PLATFORM_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
#  include <sys/resource.h>
#  include <unistd.h>
#endif

#if defined(PLATFORM_WINDOWS)
// Mapping of our process_priority enum to Win32 priority classes.
read_only global_var DWORD win32_process_priorities[] = {
    BELOW_NORMAL_PRIORITY_CLASS,
    NORMAL_PRIORITY_CLASS,
    HIGH_PRIORITY_CLASS,
    REALTIME_PRIORITY_CLASS,
};
#endif

func process_priority process_get_priority(void) {
#if defined(PLATFORM_WINDOWS)
  DWORD priority_class = GetPriorityClass(GetCurrentProcess());

  switch (priority_class) {
    case IDLE_PRIORITY_CLASS:
    case BELOW_NORMAL_PRIORITY_CLASS:
      return PROCESS_PRIORITY_LOW;

    case ABOVE_NORMAL_PRIORITY_CLASS:
    case HIGH_PRIORITY_CLASS:
      return PROCESS_PRIORITY_HIGH;

    case REALTIME_PRIORITY_CLASS:
      return PROCESS_PRIORITY_REALTIME;

    case NORMAL_PRIORITY_CLASS:
    default:
      return PROCESS_PRIORITY_NORMAL;
  }
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  errno = 0;
  i32 nice_value = (i32)getpriority(PRIO_PROCESS, 0);
  if (errno != 0) {
    return PROCESS_PRIORITY_NORMAL;
  }

  if (nice_value <= -15) {
    return PROCESS_PRIORITY_REALTIME;
  }
  if (nice_value <= -5) {
    return PROCESS_PRIORITY_HIGH;
  }
  if (nice_value >= 10) {
    return PROCESS_PRIORITY_LOW;
  }
  return PROCESS_PRIORITY_NORMAL;
#else
  return PROCESS_PRIORITY_NORMAL;
#endif
}

func b32 process_set_priority(process_priority priority) {
  if (priority < PROCESS_PRIORITY_LOW || priority > PROCESS_PRIORITY_REALTIME) {
    return 0;
  }

#if defined(PLATFORM_WINDOWS)
  return SetPriorityClass(GetCurrentProcess(), win32_process_priorities[priority]) != 0;
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
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

  return setpriority(PRIO_PROCESS, 0, (int)nice_value) == 0;
#else
  (void)priority;
  return 0;
#endif
}

func u64 process_id(void) {
#if defined(PLATFORM_WINDOWS)
  return (u64)GetCurrentProcessId();
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  return (u64)getpid();
#else
  return 0;
#endif
}

func no_return void process_exit(i32 exit_code) {
#if defined(PLATFORM_WINDOWS)
  ExitProcess((UINT)exit_code);
#else
  exit(exit_code);
#endif
}

func no_return void process_abort(void) {
  abort();
}
