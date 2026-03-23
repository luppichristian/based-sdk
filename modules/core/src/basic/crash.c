// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/crash.h"

#include "basic/assert.h"
#include "basic/codespace.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"
#include "basic/safe.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"
#include "processes/process_current.h"
#include "strings/cstrings.h"
#include "threads/atomics.h"
#include "../sdl3_include.h"
#include "../platform_includes.h"

#include <signal.h>

// =========================================================================
// Internal state
// =========================================================================

global_var atomic_u32 crash_is_installed_state = {0};
global_var atomic_u32 crash_is_handling_state = {0};

#if defined(PLATFORM_WINDOWS)
global_var LPTOP_LEVEL_EXCEPTION_FILTER crash_previous_windows_filter = NULL;
#elif defined(PLATFORM_UNIX)
typedef struct crash_signal_slot {
  i32 signal_number;
  struct sigaction previous_action;
} crash_signal_slot;

global_var crash_signal_slot crash_signal_slots[] = {
    {SIGSEGV, {0}},
    { SIGILL, {0}},
    { SIGFPE, {0}},
    {SIGABRT, {0}},
    {SIGTRAP, {0}},
#  if defined(SIGBUS)
    { SIGBUS, {0}},
#  endif
};
#endif

// =========================================================================
// Internal helpers
// =========================================================================

#if defined(PLATFORM_UNIX)
func cstr8 crash_signal_name(i32 signal_number) {
  cstr8 result = "SIGUNKNOWN";

  switch (signal_number) {
    case SIGSEGV:
      result = "SIGSEGV";
      break;
    case SIGILL:
      result = "SIGILL";
      break;
    case SIGFPE:
      result = "SIGFPE";
      break;
    case SIGABRT:
      result = "SIGABRT";
      break;
    case SIGTRAP:
      result = "SIGTRAP";
      break;
#  if defined(SIGBUS)
    case SIGBUS:
      result = "SIGBUS";
      break;
#  endif
    default:
      break;
  }

  return result;
}
#endif

func cstr8 crash_windows_exception_name(u32 exception_code) {
  cstr8 result = "EXCEPTION_UNKNOWN";
#if defined(PLATFORM_WINDOWS)
  switch (exception_code) {
    case EXCEPTION_ACCESS_VIOLATION:
      result = "EXCEPTION_ACCESS_VIOLATION";
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      result = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
      break;
    case EXCEPTION_BREAKPOINT:
      result = "EXCEPTION_BREAKPOINT";
      break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
      result = "EXCEPTION_DATATYPE_MISALIGNMENT";
      break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
      result = "EXCEPTION_FLT_DENORMAL_OPERAND";
      break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      result = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
      break;
    case EXCEPTION_FLT_INEXACT_RESULT:
      result = "EXCEPTION_FLT_INEXACT_RESULT";
      break;
    case EXCEPTION_FLT_INVALID_OPERATION:
      result = "EXCEPTION_FLT_INVALID_OPERATION";
      break;
    case EXCEPTION_FLT_OVERFLOW:
      result = "EXCEPTION_FLT_OVERFLOW";
      break;
    case EXCEPTION_FLT_STACK_CHECK:
      result = "EXCEPTION_FLT_STACK_CHECK";
      break;
    case EXCEPTION_FLT_UNDERFLOW:
      result = "EXCEPTION_FLT_UNDERFLOW";
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      result = "EXCEPTION_ILLEGAL_INSTRUCTION";
      break;
    case EXCEPTION_IN_PAGE_ERROR:
      result = "EXCEPTION_IN_PAGE_ERROR";
      break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      result = "EXCEPTION_INT_DIVIDE_BY_ZERO";
      break;
    case EXCEPTION_INT_OVERFLOW:
      result = "EXCEPTION_INT_OVERFLOW";
      break;
    case EXCEPTION_INVALID_DISPOSITION:
      result = "EXCEPTION_INVALID_DISPOSITION";
      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      result = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
      break;
    case EXCEPTION_PRIV_INSTRUCTION:
      result = "EXCEPTION_PRIV_INSTRUCTION";
      break;
    case EXCEPTION_SINGLE_STEP:
      result = "EXCEPTION_SINGLE_STEP";
      break;
    case EXCEPTION_STACK_OVERFLOW:
      result = "EXCEPTION_STACK_OVERFLOW";
      break;
    default:
      break;
  }
#else
  (void)exception_code;
#endif

  return result;
}

#if defined(PLATFORM_WINDOWS)
func cstr8 crash_windows_access_operation(ULONG_PTR access_kind) {
  cstr8 result = "unknown";

  switch (access_kind) {
    case 0:
      result = "read";
      break;
    case 1:
      result = "write";
      break;
    case 8:
      result = "execute";
      break;
    default:
      break;
  }

  return result;
}
#endif

func no_return void crash_route_unhandled(cstr8 message, callsite site) {
  u32 expected = 0;
  if (!atomic_u32_cmpex(&crash_is_handling_state, &expected, 1)) {
    process_abort();
  }

  crash_uninstall();
  thread_log_fatal("Routing unhandled crash through assert message=%s", message != NULL ? message : "<null>");
  _assert(false, message != NULL ? message : "Unhandled crash", site);
  process_abort();
}

#if defined(PLATFORM_WINDOWS)
func void crash_format_windows_message(c8* dst, sz dst_cap, EXCEPTION_POINTERS* exception_ptrs) {
  if (dst == NULL || dst_cap == 0) {
    return;
  }

  dst[0] = '\0';
  if (exception_ptrs == NULL || exception_ptrs->ExceptionRecord == NULL) {
    (void)cstr8_format(dst, dst_cap, "Unhandled exception <missing exception record>");
    return;
  }

  EXCEPTION_RECORD* record = exception_ptrs->ExceptionRecord;
  (void)cstr8_format(dst,
                     dst_cap,
                     "Unhandled exception 0x%08lX: %s at %p",
                     (unsigned long)record->ExceptionCode,
                     crash_windows_exception_name((u32)record->ExceptionCode),
                     record->ExceptionAddress);

  if ((record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION ||
       record->ExceptionCode == EXCEPTION_IN_PAGE_ERROR) &&
      record->NumberParameters >= 2) {
    (void)cstr8_append_format(dst,
                              dst_cap,
                              " during %s at %p",
                              crash_windows_access_operation(record->ExceptionInformation[0]),
                              (void*)record->ExceptionInformation[1]);
  }
}

func LONG WINAPI crash_windows_exception_handler(EXCEPTION_POINTERS* exception_ptrs) {
  str8_long message = {0};
  crash_format_windows_message(message, size_of(message), exception_ptrs);
  crash_route_unhandled(message, (callsite) {"basic/crash.c", "crash_windows_exception_handler", 0});
}
#elif defined(PLATFORM_UNIX)
func void crash_format_unix_message(c8* dst, sz dst_cap, i32 signal_number, siginfo_t* signal_info) {
  if (dst == NULL || dst_cap == 0) {
    return;
  }

  dst[0] = '\0';

  (void)cstr8_format(dst,
                     dst_cap,
                     "Unhandled signal %d: %s",
                     signal_number,
                     crash_signal_name(signal_number));

  if (signal_info != NULL) {
    (void)cstr8_append_format(dst, dst_cap, " code=%d", signal_info->si_code);
    if (signal_info->si_addr != NULL) {
      (void)cstr8_append_format(dst, dst_cap, " addr=%p", signal_info->si_addr);
    }
  }
}

func void crash_unix_signal_handler(i32 signal_number, siginfo_t* signal_info, void* user_data) {
  (void)user_data;
  str8_long message = {0};
  crash_format_unix_message(message, size_of(message), signal_number, signal_info);
  crash_route_unhandled(message, (callsite) {"basic/crash.c", "crash_unix_signal_handler", (u32)signal_number});
}
#endif

// =========================================================================
// Public API
// =========================================================================

func b32 crash_install(void) {
  if (crash_is_installed()) {
    thread_log_trace("Crash handler already installed");
    return true;
  }

#if defined(PLATFORM_WINDOWS)
  thread_log_verbose("Installing Windows unhandled exception filter");
  crash_previous_windows_filter = SetUnhandledExceptionFilter(crash_windows_exception_handler);
#elif defined(PLATFORM_UNIX)
  struct sigaction action = {0};
  action.sa_sigaction = crash_unix_signal_handler;
  action.sa_flags = SA_SIGINFO | SA_RESETHAND;
  sigemptyset(&action.sa_mask);

  thread_log_verbose("Installing Unix crash signal handlers count=%zu", (size_t)count_of(crash_signal_slots));
  safe_for (sz slot_idx = 0; slot_idx < count_of(crash_signal_slots); ++slot_idx) {
    if (sigaction(crash_signal_slots[slot_idx].signal_number, &action, &crash_signal_slots[slot_idx].previous_action) != 0) {
      thread_log_error("Failed installing crash signal handler signal=%d", crash_signal_slots[slot_idx].signal_number);
      safe_for (sz rollback_idx = 0; rollback_idx < slot_idx; ++rollback_idx) {
        (void)sigaction(crash_signal_slots[rollback_idx].signal_number,
                        &crash_signal_slots[rollback_idx].previous_action,
                        NULL);
      }
      return false;
    }
  }
#else
  thread_log_error("Crash handlers are unavailable on this platform");
  return false;
#endif

  atomic_u32_set(&crash_is_handling_state, 0);
  atomic_u32_set(&crash_is_installed_state, 1);
  thread_log_info("Installed crash handler");
  return true;
}

func void crash_uninstall(void) {
  if (!crash_is_installed()) {
    return;
  }

#if defined(PLATFORM_WINDOWS)
  thread_log_verbose("Restoring Windows unhandled exception filter");
  (void)SetUnhandledExceptionFilter(crash_previous_windows_filter);
  crash_previous_windows_filter = NULL;
#elif defined(PLATFORM_UNIX)
  thread_log_verbose("Restoring Unix crash signal handlers count=%zu", (size_t)count_of(crash_signal_slots));
  safe_for (sz slot_idx = 0; slot_idx < count_of(crash_signal_slots); ++slot_idx) {
    (void)sigaction(crash_signal_slots[slot_idx].signal_number, &crash_signal_slots[slot_idx].previous_action, NULL);
  }
#endif

  atomic_u32_set(&crash_is_installed_state, 0);
  thread_log_info("Uninstalled crash handler");
}

func b32 crash_is_installed(void) {
  b32 result = atomic_u32_get(&crash_is_installed_state) != 0;
  return result;
}
