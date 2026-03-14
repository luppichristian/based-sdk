// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../basic/primitive_types.h"
#include "../basic/utility_defines.h"
#include "../memory/arena.h"
#include "../threads/mutex.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct log_state log_state;

// =========================================================================
// Log Level
// =========================================================================

// Define default log level if not defined by the user.
#ifndef LOG_LEVEL_DEFAULT
#  ifdef BUILD_DEBUG
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#  else
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO
#  endif
#endif

// Log levels for logging messages.
// These levels can be used to filter logs based on their severity.
typedef enum log_level {
  LOG_LEVEL_FATAL,    // A fatal error that causes the application to trigger an assert.
  LOG_LEVEL_ERROR,    // An error that should be fixed, but does not cause an assert.
  LOG_LEVEL_WARN,     // A warning that should be fixed, but does not cause an assert.
  LOG_LEVEL_INFO,     // Info message that does not indicate a problem.
  LOG_LEVEL_DEBUG,    // Debug message that is only logged in debug builds.
  LOG_LEVEL_VERBOSE,  // Verbose message that is only logged in debug builds and is more detailed.
  LOG_LEVEL_TRACE,    // Trace message that is only logged in debug builds and is very detailed, used for tracing.
  LOG_LEVEL_MAX,
} log_level;

// Returns label string for the given log level.
func cstr8 log_level_to_str(log_level level);

// Sets the minimum enabled level for the given state.
func void log_state_set_level(log_state* state, log_level level);

// =========================================================================
// Log message
// =========================================================================

// One retained log message stored in a linked list owned by a log_state or log_frame.
typedef struct log_msg {
  struct log_msg* next;
  log_level level;
  callsite site;
  cstr8 text;
} log_msg;

// Helpers functions
func log_msg* log_msg_next(log_msg* msg);
func log_level log_msg_level(log_msg* msg);
func callsite log_msg_site(log_msg* msg);
func cstr8 log_msg_text(log_msg* msg);

// =========================================================================
// Log frame
// =========================================================================

// One log frame. log_state owns a persistent root frame for retained history,
// and may also hold a stack of active nested frames.
typedef struct log_frame {
  struct log_frame* next;
  log_msg* msgs_head;
  log_msg* msgs_tail;
  sz msg_count;
} log_frame;

// Frame helpers.
func b32 log_frame_has_msgs(log_frame* frame);
func sz log_frame_msg_count(log_frame* frame);
func log_msg* log_frame_first(log_frame* frame);
func log_msg* log_frame_last(log_frame* frame);

// =========================================================================
// Log State
// =========================================================================

static const sz LOG_STATE_ARENA_MIN_SIZE = kb(4);

// The mutex is optional. When present, it serializes access to the state.
typedef struct log_state {
  b32 is_init;
  mutex mutex_handle;
  log_level level;
  log_frame root_frame_storage;
  log_frame* root_frame;
  log_frame* active_frame;
  arena arena_alloc;
} log_state;

// Initializes a log state with LOG_LEVEL_DEFAULT.
// When mutex_handle is non-NULL the state uses it for synchronization.
func b32 _log_state_init(log_state* state, mutex mutex_handle, allocator alloc, callsite site);

// Releases resources owned by the log state and zeroes it.
// Safe to call on NULL or zero-initialized states.
func void _log_state_quit(log_state* state, callsite site);

#define log_state_init(state, mutex_handle, alloc) \
  _log_state_init(state, mutex_handle, alloc, CALLSITE_HERE)
#define log_state_quit(state) \
  _log_state_quit(state, CALLSITE_HERE)

// Returns true if the state is non-NULL and has been initialized.
func b32 log_state_is_init(log_state* state);

// Clears all retained messages and ended/active frame storage in one arena reset.
// Any frame pointer returned by log_state_end_frame becomes invalid after this call.
func b32 log_state_clear(log_state* state);

// Copies retained root-frame messages from src into dst, then clears src root
// frame metadata. Active frame stacks are not modified.
func void log_state_sync(log_state* dst, log_state* src);

// =========================================================================
// Log frames
// =========================================================================

// Begins a nested log frame on the given state.
// Messages logged after this call are included in the frame until it is ended.
func void log_state_begin_frame(log_state* state);

// Ends the most recent log frame and returns a snapshot of messages collected
// since the matching log_state_begin_frame call.
// severity_mask filters the returned messages; pass 0 to include all severities.
// Returns NULL when there is no active frame or when no messages matched.
// Returned frames live until the state is cleared or quit.
func log_frame* log_state_end_frame(log_state* state, u32 severity_mask);

// =========================================================================
// Log function
// =========================================================================

// Log function called by the logging macros.
func void _log(log_state* state, log_level level, callsite site, const char* msg, ...);

// Convenience macros for logging with an explicit state.
#define log_state_fatal(state, ...)   _log((state), LOG_LEVEL_FATAL, CALLSITE_HERE, __VA_ARGS__)
#define log_state_error(state, ...)   _log((state), LOG_LEVEL_ERROR, CALLSITE_HERE, __VA_ARGS__)
#define log_state_warn(state, ...)    _log((state), LOG_LEVEL_WARN, CALLSITE_HERE, __VA_ARGS__)
#define log_state_info(state, ...)    _log((state), LOG_LEVEL_INFO, CALLSITE_HERE, __VA_ARGS__)
#define log_state_debug(state, ...)   _log((state), LOG_LEVEL_DEBUG, CALLSITE_HERE, __VA_ARGS__)
#define log_state_verbose(state, ...) _log((state), LOG_LEVEL_VERBOSE, CALLSITE_HERE, __VA_ARGS__)
#define log_state_trace(state, ...)   _log((state), LOG_LEVEL_TRACE, CALLSITE_HERE, __VA_ARGS__)

// =========================================================================
c_end;
// =========================================================================
