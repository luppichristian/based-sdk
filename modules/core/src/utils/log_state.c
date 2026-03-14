// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/log_state.h"
#include "basic/assert.h"
#include "containers/singly_list.h"
#include "containers/stack_list.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"
#include "memory/memops.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "basic/safe.h"

// =========================================================================
// Internal helpers
// =========================================================================

func log_state* log_state_resolve(log_state* state) {
  return (state && state->is_init) ? state : NULL;
}

func void log_frame_reset(log_frame* frame) {
  if (frame) {
    mem_zero(frame, size_of(*frame));
  }
}

func void log_state_lock(log_state* state) {
  profile_func_begin;
  if (state && state->mutex_handle) {
    mutex_lock(state->mutex_handle);
  }
  profile_func_end;
}

func void log_state_unlock(log_state* state) {
  profile_func_begin;
  if (state && state->mutex_handle) {
    mutex_unlock(state->mutex_handle);
  }
  profile_func_end;
}

func log_frame* log_frame_create(log_state* state) {
  profile_func_begin;
  log_frame* frame = arena_alloc_type(&state->arena_alloc, log_frame);
  if (!frame) {
    profile_func_end;
    return NULL;
  }
  log_frame_reset(frame);
  profile_func_end;
  return frame;
}

func log_msg* log_msg_create(log_state* state, log_level level, callsite site, cstr8 text) {
  profile_func_begin;
  sz text_len = 0;
  sz total_size = 0;
  if (text == NULL) {
    profile_func_end;
    return NULL;
  }

  text_len = cstr8_len(text);
  total_size = size_of(log_msg) + text_len + 1;
  log_msg* msg = arena_alloc(&state->arena_alloc, total_size, align_of(log_msg));
  if (!msg) {
    profile_func_end;
    return NULL;
  }

  msg->next = NULL;
  msg->level = level;
  msg->site = site;
  msg->text = (cstr8)(msg + 1);
  mem_cpy((void*)msg->text, text, text_len + 1);
  profile_func_end;
  return msg;
}

func void log_frame_append_msg(log_frame* frame, log_msg* msg) {
  profile_func_begin;
  if (!frame || !msg) {
    profile_func_end;
    return;
  }

  SINGLY_LIST_PUSH_BACK(frame->msgs_head, frame->msgs_tail, msg);
  frame->msg_count += 1;
  profile_func_end;
}

func void log_state_reset_frames(log_state* state) {
  profile_func_begin;
  if (!state) {
    profile_func_end;
    return;
  }

  arena_clear(&state->arena_alloc);
  log_frame_reset(&state->root_frame_storage);
  state->root_frame = &state->root_frame_storage;
  state->active_frame = NULL;
  profile_func_end;
}

func void log_state_store_msg(log_state* state, log_level level, callsite site, cstr8 msg) {
  profile_func_begin;
  if (!state || !state->is_init || !state->root_frame) {
    profile_func_end;
    return;
  }
  assert(msg != NULL);
  assert(level < LOG_LEVEL_MAX);

  log_state_lock(state);

  log_msg* root_msg = log_msg_create(state, level, site, msg);
  if (root_msg) {
    log_frame_append_msg(state->root_frame, root_msg);
  }

  STACK_LIST_FOREACH(state->active_frame, frame) {
    log_msg* frame_msg = log_msg_create(state, level, site, msg);
    if (!frame_msg) {
      continue;
    }
    log_frame_append_msg(frame, frame_msg);
  }

  log_state_unlock(state);
  profile_func_end;
}

func b32 log_level_matches_mask(log_level level, u32 severity_mask) {
  profile_func_begin;
  if (severity_mask == 0) {
    profile_func_end;
    return true;
  }

  b32 res = (severity_mask & bit(level)) != 0;
  profile_func_end;
  return res;
}

// Emits a single formatted log line.
func void log_emit(log_level level, callsite site, cstr8 msg) {
  profile_func_begin;
  assert(level < LOG_LEVEL_MAX);
  assert(msg != NULL);
  cstr8 label = log_level_to_str(level);

  FILE* out = stdout;
  switch (level) {
    case LOG_LEVEL_ERROR:
    case LOG_LEVEL_FATAL:
    case LOG_LEVEL_WARN:
      out = stderr;
      break;
    default:
      break;
  }

  fprintf(out, "[%s] %s (%s() %s:%u)\n", label, msg, site.function, site.filename, site.line);
  fflush(out);
  profile_func_end;
}

// =========================================================================
// Public API
// =========================================================================

func cstr8 log_level_to_str(log_level level) {
  profile_func_begin;
  switch (level) {
    case LOG_LEVEL_FATAL:
      profile_func_end;
      return "FATAL";
    case LOG_LEVEL_ERROR:
      profile_func_end;
      return "ERROR";
    case LOG_LEVEL_WARN:
      profile_func_end;
      return "WARN ";
    case LOG_LEVEL_INFO:
      profile_func_end;
      return "INFO ";
    case LOG_LEVEL_DEBUG:
      profile_func_end;
      return "DEBUG";
    case LOG_LEVEL_VERBOSE:
      profile_func_end;
      return "VERB ";
    case LOG_LEVEL_TRACE:
      profile_func_end;
      return "TRACE";
    case LOG_LEVEL_MAX:
      profile_func_end;
      return "?????";
  }
  profile_func_end;
  return NULL;
}

func b32 _log_state_init(log_state* state, b32 use_mutex, allocator alloc, callsite site) {
  profile_func_begin;
  if (!state) {
    profile_func_end;
    return false;
  }
  assert(use_mutex == 0 || use_mutex == 1);

  mem_zero(state, size_of(*state));
  state->level = LOG_LEVEL_DEFAULT;
  state->arena_alloc = arena_create(alloc, NULL, LOG_STATE_ARENA_MIN_SIZE);
  state->root_frame = &state->root_frame_storage;
  if (use_mutex) {
    state->mutex_handle = mutex_create();
    if (!state->mutex_handle) {
      arena_destroy(&state->arena_alloc);
      state->root_frame = NULL;
      profile_func_end;
      return false;
    }
  }
  state->is_init = true;
  msg lifecycle_msg = {0};
  msg_core_object_lifecycle_data msg_data = {
      .object_type = MSG_CORE_OBJECT_TYPE_LOG_STATE,
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_ptr = state,
      .site = site,
  };
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    if (state->mutex_handle) {
      mutex_destroy(state->mutex_handle);
    }
    arena_destroy(&state->arena_alloc);
    mem_zero(state, size_of(*state));
    thread_log_trace("Log state initialization was suspended state=%p", (void*)state);
    profile_func_end;
    return false;
  }
  profile_func_end;
  return true;
}

func void _log_state_quit(log_state* state, callsite site) {
  profile_func_begin;
  if (!state) {
    profile_func_end;
    return;
  }

  msg lifecycle_msg = {0};
  msg_core_object_lifecycle_data msg_data = {
      .object_type = MSG_CORE_OBJECT_TYPE_LOG_STATE,
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_ptr = state,
      .site = site,
  };
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Log state shutdown was suspended state=%p", (void*)state);
    profile_func_end;
    return;
  }

  mutex state_mutex = state->mutex_handle;
  log_state_lock(state);
  state->is_init = false;
  state->root_frame = NULL;
  state->active_frame = NULL;
  log_state_unlock(state);

  arena_destroy(&state->arena_alloc);
  log_frame_reset(&state->root_frame_storage);
  if (state_mutex) {
    mutex_destroy(state_mutex);
  }
  mem_zero(state, size_of(*state));
  profile_func_end;
}

func b32 log_state_is_init(log_state* state) {
  return state != NULL && state->is_init;
}

func b32 log_state_clear(log_state* state) {
  profile_func_begin;
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    profile_func_end;
    return false;
  }

  log_state_lock(resolved);
  log_state_reset_frames(resolved);
  log_state_unlock(resolved);
  profile_func_end;
  return true;
}

func void log_state_set_level(log_state* state, log_level level) {
  profile_func_begin;
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    profile_func_end;
    return;
  }

  log_state_lock(resolved);
  resolved->level = level;
  log_state_unlock(resolved);
  profile_func_end;
}

func void log_state_sync(log_state* dst, log_state* src) {
  profile_func_begin;
  log_state* resolved_dst = log_state_resolve(dst);
  log_state* resolved_src = log_state_resolve(src);
  if (!resolved_dst || !resolved_src) {
    profile_func_end;
    return;
  }
  assert(resolved_dst->root_frame != NULL);
  assert(resolved_src->root_frame != NULL);

  if (resolved_dst == resolved_src || !resolved_dst->is_init || !resolved_src->is_init) {
    profile_func_end;
    return;
  }

  log_state_lock(resolved_dst);
  log_state_lock(resolved_src);
  if (resolved_src->root_frame->msgs_head) {
    b32 cpy_ok = true;
    log_frame copied_frame = {0};
    SINGLY_LIST_FOREACH(resolved_src->root_frame->msgs_head, resolved_src->root_frame->msgs_tail, src_msg) {
      log_msg* dst_msg = log_msg_create(resolved_dst, src_msg->level, src_msg->site, src_msg->text);
      if (!dst_msg) {
        cpy_ok = false;
        break;
      }
      SINGLY_LIST_PUSH_BACK(copied_frame.msgs_head, copied_frame.msgs_tail, dst_msg);
      copied_frame.msg_count += 1;
    }

    if (cpy_ok) {
      if (resolved_dst->root_frame->msgs_tail) {
        resolved_dst->root_frame->msgs_tail->next = copied_frame.msgs_head;
      } else {
        resolved_dst->root_frame->msgs_head = copied_frame.msgs_head;
      }
      resolved_dst->root_frame->msgs_tail = copied_frame.msgs_tail;
      resolved_dst->root_frame->msg_count += copied_frame.msg_count;
      log_frame_reset(resolved_src->root_frame);
    }
  }
  log_state_unlock(resolved_dst);
  log_state_unlock(resolved_src);

  profile_func_end;
}

func void log_state_begin_frame(log_state* state) {
  profile_func_begin;
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    profile_func_end;
    return;
  }
  assert(resolved->root_frame != NULL);

  log_frame* frame = log_frame_create(state);
  if (!frame) {
    profile_func_end;
    return;
  }

  log_state_lock(resolved);
  STACK_LIST_PUSH(resolved->active_frame, frame);
  log_state_unlock(resolved);
  profile_func_end;
}

func log_frame* log_state_end_frame(log_state* state, u32 severity_mask) {
  profile_func_begin;
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    profile_func_end;
    return NULL;
  }
  assert(resolved->root_frame != NULL);

  log_state_lock(resolved);
  log_frame* frame = NULL;
  STACK_LIST_POP(resolved->active_frame, frame);
  if (!frame) {
    log_state_unlock(resolved);
    profile_func_end;
    return NULL;
  }

  log_msg* kept_head = NULL;
  log_msg* kept_tail = NULL;
  sz kept_count = 0;
  log_msg* msg = NULL;
  safe_while (frame->msgs_head) {
    SINGLY_LIST_POP_FRONT(frame->msgs_head, frame->msgs_tail, msg);
    if (!log_level_matches_mask(msg->level, severity_mask)) {
      continue;
    }

    SINGLY_LIST_PUSH_BACK(kept_head, kept_tail, msg);
    kept_count += 1;
  }
  frame->msgs_head = kept_head;
  frame->msgs_tail = kept_tail;
  frame->msg_count = kept_count;
  log_state_unlock(resolved);

  if (!frame->msg_count) {
    profile_func_end;
    return NULL;
  }
  profile_func_end;
  return frame;
}

func b32 log_frame_has_msgs(log_frame* frame) {
  return frame != NULL && frame->msg_count > 0;
}

func sz log_frame_msg_count(log_frame* frame) {
  return frame ? frame->msg_count : 0;
}

func log_msg* log_frame_first(log_frame* frame) {
  return frame ? frame->msgs_head : NULL;
}

func log_msg* log_frame_last(log_frame* frame) {
  return frame ? frame->msgs_tail : NULL;
}

func log_msg* log_msg_next(log_msg* msg) {
  return msg ? msg->next : NULL;
}

func log_level log_msg_level(log_msg* msg) {
  return msg ? msg->level : LOG_LEVEL_MAX;
}

func callsite log_msg_site(log_msg* msg) {
  callsite empty_site = {0};
  return msg ? msg->site : empty_site;
}

func cstr8 log_msg_text(log_msg* msg) {
  return msg ? msg->text : NULL;
}

// =========================================================================
// Log function
// =========================================================================

func void _log(log_state* state, log_level level, callsite site, cstr8 msg, ...) {
  profile_func_begin;
  log_state* resolved = log_state_resolve(state);
  if (!resolved) {
    profile_func_end;
    return;
  }
  if (msg == NULL) {
    profile_func_end;
    return;
  }
  assert(level < LOG_LEVEL_MAX);
  assert(resolved->root_frame != NULL);

  log_level active_level = LOG_LEVEL_DEFAULT;

  log_state_lock(resolved);
  active_level = resolved->level;
  log_state_unlock(resolved);

  if (level > active_level) {
    profile_func_end;
    return;
  }

  str8_large buf = {0};
  va_list args;
  va_start(args, msg);
  cstr8_vformat(buf, size_of(buf), msg, args);
  va_end(args);

  struct msg log_msg = {0};
  log_msg.type = MSG_CORE_TYPE_LOG;
  msg_core_log_data log_data = {
      .state_ptr = resolved,
      .level = level,
      .source_site = site,
  };
  cstr8_format(log_data.text, size_of(log_data.text), "%s", buf);
  msg_core_fill_log(&log_msg, &log_data);
  (void)msg_post(&log_msg);

  log_state_store_msg(resolved, level, site, buf);
  log_emit(level, site, buf);
  profile_func_end;
}
