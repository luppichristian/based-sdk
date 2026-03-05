// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/log.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "memory/vmem.h"
#include "threads/atomics.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

global_var mutex log_emit_mutex = NULL;
global_var atomic_i32 log_emit_mutex_init = {0};

// =========================================================================
// Internal helpers
// =========================================================================

func mutex log_shared_mutex_get(atomic_i32* init_state, mutex* out_mutex) {
  if (atomic_i32_get(init_state) == 2) {
    return *out_mutex;
  }

  i32 expected = 0;
  if (atomic_i32_cmpex(init_state, &expected, 1)) {
    *out_mutex = mutex_create();
    atomic_fence_release();
    atomic_i32_set(init_state, 2);
    return *out_mutex;
  }

  while (atomic_i32_get(init_state) != 2) {
    atomic_pause();
  }
  atomic_fence_acquire();
  return *out_mutex;
}

func log_state* log_state_resolve(log_state* state) {
  return (state && state->is_init) ? state : NULL;
}

func allocator log_allocator_resolve(void) {
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    return alloc;
  }
  return vmem_get_allocator();
}

func void log_state_lock(log_state* state) {
  if (state && state->mutex_handle) {
    mutex_lock(state->mutex_handle);
  }
}

func void log_state_unlock(log_state* state) {
  if (state && state->mutex_handle) {
    mutex_unlock(state->mutex_handle);
  }
}

func void log_state_lock_pair(log_state* first, log_state* second) {
  if (!first && !second) {
    return;
  }

  if (first == second) {
    log_state_lock(first);
    return;
  }

  if ((up)first < (up)second) {
    log_state_lock(first);
    log_state_lock(second);
  } else {
    log_state_lock(second);
    log_state_lock(first);
  }
}

func void log_state_unlock_pair(log_state* first, log_state* second) {
  if (!first && !second) {
    return;
  }

  if (first == second) {
    log_state_unlock(first);
    return;
  }

  if ((up)first < (up)second) {
    log_state_unlock(second);
    log_state_unlock(first);
  } else {
    log_state_unlock(first);
    log_state_unlock(second);
  }
}

func log_frame* log_frame_create(void) {
  allocator alloc = log_allocator_resolve();
  log_frame* frame = (log_frame*)allocator_alloc(&alloc, size_of(*frame));
  if (!frame) {
    return NULL;
  }
  assert(alloc.alloc_fn != NULL);
  memset(frame, 0, size_of(*frame));
  return frame;
}

func log_msg* log_msg_create(log_level level, callsite site, cstr8 msg) {
  sz text_len = 0;
  sz total_size = 0;
  allocator alloc = log_allocator_resolve();
  if (msg == NULL) {
    return NULL;
  }
  text_len = cstr8_len(msg);
  total_size = size_of(log_msg) + text_len + 1;
  log_msg* message = (log_msg*)allocator_alloc(&alloc, total_size);
  if (!message) {
    return NULL;
  }

  assert(msg != NULL);
  memset(message, 0, size_of(*message));
  message->level = level;
  message->site = site;
  message->text = (cstr8)(message + 1);
  memcpy((void*)message->text, msg, text_len + 1);
  return message;
}

func void log_msg_destroy(log_msg* message) {
  allocator alloc = log_allocator_resolve();
  sz text_len = 0;
  if (message == NULL) {
    return;
  }
  assert(message->text != NULL);
  text_len = cstr8_len(message->text);
  allocator_dealloc(&alloc, message, size_of(*message) + text_len + 1);
}

func void log_msg_list_destroy(log_msg* head) {
  while (head) {
    log_msg* next_message = head->next;
    log_msg_destroy(head);
    head = next_message;
  }
}

func void log_frame_append_message_unsafe(log_frame* frame, log_msg* message) {
  if (!frame || !message) {
    return;
  }

  if (frame->messages_tail) {
    frame->messages_tail->next = message;
    frame->messages_tail = message;
  } else {
    frame->messages_head = message;
    frame->messages_tail = message;
  }
  frame->message_count += 1;
}

func void log_frame_clear_messages_unsafe(log_frame* frame) {
  if (!frame) {
    return;
  }

  log_msg_list_destroy(frame->messages_head);
  frame->messages_head = NULL;
  frame->messages_tail = NULL;
  frame->message_count = 0;
}

func void log_frame_destroy_unsafe(log_frame* frame) {
  allocator alloc = log_allocator_resolve();
  if (!frame) {
    return;
  }

  log_frame_clear_messages_unsafe(frame);
  allocator_dealloc(&alloc, frame, size_of(*frame));
}

func void log_state_store_message(log_state* state, log_level level, callsite site, cstr8 msg) {
  if (!state || !state->is_init || !state->root_frame) {
    return;
  }
  assert(msg != NULL);
  assert(level < LOG_LEVEL_MAX);

  log_state_lock(state);

  log_msg* root_message = log_msg_create(level, site, msg);
  if (root_message) {
    log_frame_append_message_unsafe(state->root_frame, root_message);
  }

  for (log_frame* frame = state->active_frame; frame != NULL; frame = frame->prev_active) {
    log_msg* frame_message = log_msg_create(level, site, msg);
    if (!frame_message) {
      continue;
    }
    log_frame_append_message_unsafe(frame, frame_message);
  }

  log_state_unlock(state);
}

func void log_state_clear_frames_unsafe(log_state* state) {
  assert(state != NULL);
  while (state->active_frame) {
    log_frame* frame = state->active_frame;
    state->active_frame = frame->prev_active;
    log_frame_destroy_unsafe(frame);
  }

  if (state->root_frame) {
    log_frame_clear_messages_unsafe(state->root_frame);
  }
}

func b32 log_level_matches_mask(log_level level, u32 severity_mask) {
  if (severity_mask == 0) {
    return true;
  }
  return (severity_mask & log_level_mask(level)) != 0;
}

// Returns the ANSI foreground-color escape sequence for the given log level.
func cstr8 log_level_to_color(log_level level) {
  switch (level) {
    case LOG_LEVEL_FATAL:
      return "\033[1;31m";  // bold red
    case LOG_LEVEL_ERROR:
      return "\033[0;31m";  // red
    case LOG_LEVEL_WARN:
      return "\033[0;33m";  // yellow
    case LOG_LEVEL_INFO:
      return "\033[0;32m";  // green
    case LOG_LEVEL_DEBUG:
      return "\033[0;36m";  // cyan
    case LOG_LEVEL_VERBOSE:
      return "\033[0;34m";  // blue
    case LOG_LEVEL_TRACE:
      return "\033[0;37m";  // gray
    case LOG_LEVEL_MAX:
      return "\033[0m";
  }
  return "\033[0m";
}

// Emits a single formatted log line to stderr.
func void log_emit(log_level level, callsite site, cstr8 msg) {
  mutex emit_lock = log_shared_mutex_get(&log_emit_mutex_init, &log_emit_mutex);
  assert(level < LOG_LEVEL_MAX);
  assert(msg != NULL);
  if (emit_lock) {
    mutex_lock(emit_lock);
  }

  cstr8 color = log_level_to_color(level);
  cstr8 label = log_level_to_str(level);
  fprintf(stderr, "%s[%s]\033[0m %s  \033[0;90m(%s() %s:%u)\033[0m\n", color, label, msg, site.function, site.filename, site.line);
  fflush(stderr);

  if (emit_lock) {
    mutex_unlock(emit_lock);
  }
}

// =========================================================================
// Public API
// =========================================================================

func cstr8 log_level_to_str(log_level level) {
  switch (level) {
    case LOG_LEVEL_FATAL:
      return "FATAL";
    case LOG_LEVEL_ERROR:
      return "ERROR";
    case LOG_LEVEL_WARN:
      return "WARN ";
    case LOG_LEVEL_INFO:
      return "INFO ";
    case LOG_LEVEL_DEBUG:
      return "DEBUG";
    case LOG_LEVEL_VERBOSE:
      return "VERB ";
    case LOG_LEVEL_TRACE:
      return "TRACE";
    case LOG_LEVEL_MAX:
      return "?????";
  }
  return NULL;
}

func b32 log_state_init(log_state* state, b32 use_mutex) {
  if (!state) {
    return false;
  }
  assert(use_mutex == 0 || use_mutex == 1);

  memset(state, 0, size_of(*state));
  state->level = LOG_LEVEL_DEFAULT;
  state->root_frame = log_frame_create();
  if (!state->root_frame) {
    return false;
  }
  if (use_mutex) {
    state->mutex_handle = mutex_create();
    if (!state->mutex_handle) {
      log_frame_destroy_unsafe(state->root_frame);
      state->root_frame = NULL;
      return false;
    }
  }
  state->is_init = true;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_LOG_STATE;
  lifecycle_msg.object_lifecycle.object_ptr = state;
  if (!msg_post(&lifecycle_msg)) {
    log_state_quit(state);
    return false;
  }
  return true;
}

func void log_state_quit(log_state* state) {
  if (!state) {
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_LOG_STATE;
  lifecycle_msg.object_lifecycle.object_ptr = state;
  if (!msg_post(&lifecycle_msg)) {
    return;
  }

  mutex state_mutex = state->mutex_handle;
  log_frame* root_frame = state->root_frame;
  if (state_mutex) {
    mutex_lock(state_mutex);
  }
  log_state_clear_frames_unsafe(state);
  memset(state, 0, size_of(*state));
  if (state_mutex) {
    mutex_unlock(state_mutex);
    mutex_destroy(state_mutex);
  }
  log_frame_destroy_unsafe(root_frame);
}

func b32 log_state_is_init(log_state* state) {
  return state != NULL && state->is_init;
}

func void log_state_set_level(log_state* state, log_level level) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return;
  }

  log_state_lock(resolved);
  resolved->level = level;
  log_state_unlock(resolved);
}

func void log_state_sync(log_state* dst, log_state* src) {
  log_state* resolved_dst = log_state_resolve(dst);
  log_state* resolved_src = log_state_resolve(src);
  if (!resolved_dst || !resolved_src) {
    return;
  }
  assert(resolved_dst->root_frame != NULL);
  assert(resolved_src->root_frame != NULL);

  if (resolved_dst == resolved_src || !resolved_dst->is_init || !resolved_src->is_init) {
    return;
  }

  log_state_lock_pair(resolved_dst, resolved_src);
  if (resolved_src->root_frame->messages_head) {
    if (resolved_dst->root_frame->messages_tail) {
      resolved_dst->root_frame->messages_tail->next = resolved_src->root_frame->messages_head;
      resolved_dst->root_frame->messages_tail = resolved_src->root_frame->messages_tail;
    } else {
      resolved_dst->root_frame->messages_head = resolved_src->root_frame->messages_head;
      resolved_dst->root_frame->messages_tail = resolved_src->root_frame->messages_tail;
    }
    resolved_dst->root_frame->message_count += resolved_src->root_frame->message_count;

    resolved_src->root_frame->messages_head = NULL;
    resolved_src->root_frame->messages_tail = NULL;
    resolved_src->root_frame->message_count = 0;
  }
  log_state_unlock_pair(resolved_dst, resolved_src);
}

func void log_state_begin_frame(log_state* state) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return;
  }
  assert(resolved->root_frame != NULL);

  log_frame* frame = log_frame_create();
  if (!frame) {
    return;
  }

  log_state_lock(resolved);
  frame->prev_active = resolved->active_frame;
  resolved->active_frame = frame;
  log_state_unlock(resolved);
}

func log_frame* log_state_end_frame(log_state* state, u32 severity_mask) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return NULL;
  }
  assert(resolved->root_frame != NULL);

  log_state_lock(resolved);
  log_frame* frame = resolved->active_frame;
  if (!frame) {
    log_state_unlock(resolved);
    return NULL;
  }

  resolved->active_frame = frame->prev_active;
  frame->prev_active = NULL;

  log_msg* prev_message = NULL;
  log_msg* message = frame->messages_head;
  while (message) {
    log_msg* next_message = message->next;
    if (!log_level_matches_mask(message->level, severity_mask)) {
      if (prev_message) {
        prev_message->next = next_message;
      } else {
        frame->messages_head = next_message;
      }
      if (frame->messages_tail == message) {
        frame->messages_tail = prev_message;
      }
      log_msg_destroy(message);
      frame->message_count -= 1;
    } else {
      prev_message = message;
    }
    message = next_message;
  }
  log_state_unlock(resolved);

  if (!frame->message_count) {
    log_frame_destroy(frame);
    return NULL;
  }
  return frame;
}

func void log_frame_destroy(log_frame* frame) {
  if (!frame) {
    return;
  }

  log_frame_destroy_unsafe(frame);
}

func b32 log_frame_has_messages(log_frame* frame) {
  return frame != NULL && frame->message_count > 0;
}

func sz log_frame_message_count(log_frame* frame) {
  return frame ? frame->message_count : 0;
}

func log_msg* log_frame_first(log_frame* frame) {
  return frame ? frame->messages_head : NULL;
}

func log_msg* log_frame_last(log_frame* frame) {
  return frame ? frame->messages_tail : NULL;
}

func log_msg* log_msg_next(log_msg* message) {
  return message ? message->next : NULL;
}

func log_level log_msg_level(log_msg* message) {
  return message ? message->level : LOG_LEVEL_MAX;
}

func callsite log_msg_site(log_msg* message) {
  callsite empty_site = {0};
  return message ? message->site : empty_site;
}

func cstr8 log_msg_text(log_msg* message) {
  return message ? message->text : NULL;
}

func void _log(log_state* state, log_level level, callsite site, cstr8 fmt, ...) {
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    return;
  }
  if (fmt == NULL) {
    return;
  }
  assert(level < LOG_LEVEL_MAX);
  assert(resolved->root_frame != NULL);

  log_level active_level = LOG_LEVEL_DEFAULT;

  log_state_lock(resolved);
  active_level = resolved->level;
  log_state_unlock(resolved);

  if (level > active_level) {
    return;
  }

  str8_large buf = {0};
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, size_of(buf), fmt, args);
  va_end(args);

  msg log_msg = {0};
  log_msg.type = MSG_TYPE_LOG;
  log_msg.log.state_ptr = resolved;
  log_msg.log.level = level;
  log_msg.log.source_site = site;
  snprintf(log_msg.log.text, size_of(log_msg.log.text), "%s", buf);
  if (!msg_post(&log_msg)) {
    return;
  }

  log_state_store_message(resolved, level, site, buf);
  log_emit(level, site, buf);
}
