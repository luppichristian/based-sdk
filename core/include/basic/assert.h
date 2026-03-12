// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "codespace.h"
#include "primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Assert function
// =========================================================================

// If assert is already defined, undefine it to avoid conflicts with our own implementation.
#ifdef assert
#  undef assert
#endif

// Assert function called by the ASSERT macro.
// It will act depending on the internal setup.
func void _assert(b32 condition, const char* msg, callsite site);

// Convenience macro for assertions. It will call the _assert function.
#define assert(condition) _assert((condition), #condition, CALLSITE_HERE)

// Convenience macro for assertions with a custom message. It will call the _assert function.
#define assert_msg(condition, msg) _assert((condition), msg, CALLSITE_HERE)

// Invalid codepath helper macro
#define invalid_code_path assert(!"Invalid code path")

// =========================================================================
// Assert mode
// =========================================================================

// Set stacktrace depth logged and showed to the user.
// If it's 0, stacktrace will be disabled.
#ifndef ASSERT_STACKTRACE_DEPTH
#  define ASSERT_STACKTRACE_DEPTH 8
#endif

// Different modes settable at runtime.
typedef enum assert_mode {
  // On Desktop: Opens a debug window and waits for user input, in
  // the window you can choose to break point, ignore or quit the application.
  // The message is also logged.
  ASSERT_MODE_DEBUG,

  // Quits the application immediately.
  // Also logs a message.
  ASSERT_MODE_QUIT,

  // Aborts the application immediately.
  // Also logs a message.
  ASSERT_MODE_ABORT,

  // Just log a message.
  ASSERT_MODE_LOG,

  // Ignores the assertion, does nothing.
  ASSERT_MODE_IGNORE,
} assert_mode;

// Define default assert mode if not defined by the user.
#ifndef ASSERT_MODE_DEFAULT
#  ifdef BUILD_DEBUG
#    define ASSERT_MODE_DEFAULT ASSERT_MODE_DEBUG
#  else
#    define ASSERT_MODE_DEFAULT ASSERT_MODE_LOG
#  endif
#endif

// Set the desired assert mode at runtime. By default, it is set to ASSERT_MODE_DEFAULT.
func void assert_set_mode(assert_mode mode);

// Returns the current runtime assertion mode.
func assert_mode assert_get_mode(void);

// =========================================================================
c_end;
// =========================================================================