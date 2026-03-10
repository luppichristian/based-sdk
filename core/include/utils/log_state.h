// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../basic/primitive_types.h"
#include "../threads/mutex.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Log function
// =========================================================================

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

// Bitmask helper for filtering one or more severities.
#define log_level_mask(level) (1u << (level))

// One retained log message stored in a linked list owned by a log_state or log_frame.
typedef struct log_msg {
  struct log_msg* next;
  log_level level;
  callsite site;
  cstr8 text;
} log_msg;

// One log frame. log_state owns a persistent root frame for retained history,
// and may also hold a stack of active nested frames.
typedef struct log_frame {
  struct log_frame* prev_active;
  log_msg* msgs_head;
  log_msg* msgs_tail;
  sz msg_count;
} log_frame;

// Per-thread (or caller-owned) log configuration.
// The mutex is optional. When present, it serializes access to level.
typedef struct log_state {
  b32 is_init;
  mutex mutex_handle;
  log_level level;
  log_frame* root_frame;
  log_frame* active_frame;
} log_state;

// Returns label string for the given log level.
func cstr8 log_level_to_str(log_level level);

// Initializes a log state with LOG_LEVEL_DEFAULT and no callback.
// When use_mutex is true the state creates and owns a mutex; otherwise it is lock-free.
func b32 log_state_init(log_state* state, b32 use_mutex);

// Releases resources owned by the log state and zeroes it.
// Safe to call on NULL or zero-initialized states.
func void log_state_quit(log_state* state);

// Returns true if the state is non-NULL and has been initialized.
func b32 log_state_is_init(log_state* state);

// Sets the minimum enabled level for the given state.
func void log_state_set_level(log_state* state, log_level level);

// Moves the retained root-frame messages from src into dst by splicing the
// linked list nodes directly. Messages are appended to dst->root_frame and
// removed from src->root_frame.
// This does not modify either state's level, mutex ownership,
// or active nested frame stack.
func void log_state_sync(log_state* dst, log_state* src);

// Begins a nested log frame on the given state.
// Messages logged after this call are included in the frame until it is ended.
func void log_state_begin_frame(log_state* state);

// Ends the most recent log frame and returns a snapshot of messages collected
// since the matching log_state_begin_frame call.
// severity_mask filters the returned messages; pass 0 to include all severities.
// Returns NULL when there is no active frame or when no messages matched.
func log_frame* log_state_end_frame(log_state* state, u32 severity_mask);

// Frame helpers.
func void log_frame_destroy(log_frame* frame);
func b32 log_frame_has_msgs(log_frame* frame);
func sz log_frame_msg_count(log_frame* frame);
func log_msg* log_frame_first(log_frame* frame);
func log_msg* log_frame_last(log_frame* frame);
func log_msg* log_msg_next(log_msg* msg);
func log_level log_msg_level(log_msg* msg);
func callsite log_msg_site(log_msg* msg);
func cstr8 log_msg_text(log_msg* msg);

// Iterates over every message in a log frame from first to last.
#define LOG_FRAME_FOREACH(frame, it) \
  for (log_msg* it = log_frame_first(frame); (it) != NULL; (it) = log_msg_next(it))

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

// Define default log level if not defined by the user.
#ifndef LOG_LEVEL_DEFAULT
#  ifdef BUILD_DEBUG
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#  else
#    define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO
#  endif
#endif

// =========================================================================
c_end;
// =========================================================================
