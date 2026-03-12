// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/assert.h"
#include "utils/log_state.h"
#include "utils/stacktrace.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "threads/atomics.h"
#include "../sdl3_include.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include "processes/process_current.h"
#include <stdio.h>
#include <stdlib.h>
#include "basic/safe.h"

// =========================================================================
// Internal state
// =========================================================================

global_var atomic_u32 assert_mode_current = {ASSERT_MODE_DEFAULT};

// =========================================================================
// Internal helpers
// =========================================================================

func void assert_log_msg(cstr8 msg, callsite site) {
  profile_func_begin;
  _log(thread_get_log_state(), LOG_LEVEL_FATAL, site, "Assertion failed: %s", msg);
  profile_func_end;
}

func void assert_log_stacktrace(callsite site) {
  profile_func_begin;
  if (ASSERT_STACKTRACE_DEPTH == 0) {
    profile_func_end;
    return;
  }

  stacktrace_frame frames[ASSERT_STACKTRACE_DEPTH] = {0};
  sz captured_count = stacktrace_capture(frames, ASSERT_STACKTRACE_DEPTH, 1);
  if (captured_count == 0) {
    _log(thread_get_log_state(), LOG_LEVEL_FATAL, site, "Stack trace unavailable");
    profile_func_end;
    return;
  }

  _log(thread_get_log_state(), LOG_LEVEL_FATAL, site, "Stack trace:");
  safe_for (sz frame_idx = 0; frame_idx < captured_count; ++frame_idx) {
    _log(
        thread_get_log_state(),
        LOG_LEVEL_FATAL,
        site,
        "  #%u 0x%p %s",
        (u32)frame_idx,
        (void*)frames[frame_idx].address,
        frames[frame_idx].symbol);
  }
  profile_func_end;
}

// Action requested by user with assert_dialog
typedef enum assert_action {
  ASSERT_ACTION_IGNORE = 0,
  ASSERT_ACTION_BREAKPOINT = 1,
  ASSERT_ACTION_QUIT = 2,
  ASSERT_ACTION_ABORT = 3,
} assert_action;

// Defaults to quit if the message box cannot be displayed.
func assert_action assert_dialog(cstr8 msg, callsite site) {
  profile_func_begin;
  str8_long buf = {0};
  stacktrace_frame frames[ASSERT_STACKTRACE_DEPTH] = {0};
  sz captured_count = stacktrace_capture(frames, ASSERT_STACKTRACE_DEPTH, 2);

  i32 text_len = cstr8_format(
      buf,
      size_of(buf),
      "Assertion failed\n"
      "----------------\n"
      "Message : %s\n"
      "Function: %s\n"
      "Location: %s:%u\n"
      "\n"
      "Stack trace:\n",
      msg,
      site.function,
      site.filename,
      site.line);

  if (text_len < 0) {
    text_len = 0;
  }

  sz write_offset = (sz)text_len;
  if (write_offset >= size_of(buf)) {
    write_offset = size_of(buf) - 1;
  }

  if (captured_count == 0) {
    cstr8_format(buf + write_offset, size_of(buf) - write_offset, "  <unavailable>\n");
  } else {
    safe_for (sz frame_idx = 0; frame_idx < captured_count; ++frame_idx) {
      i32 line_len = cstr8_format(
          buf + write_offset,
          size_of(buf) - write_offset,
          "  #%02u %s\n",
          (u32)frame_idx,
          frames[frame_idx].symbol);
      if (line_len < 0) {
        break;
      }
      write_offset += (sz)line_len;
      if (write_offset >= size_of(buf) - 1) {
        break;
      }
    }
  }

  SDL_MessageBoxButtonData buttons[] = {
      {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Breakpoint"},
      {                                      0, 0,     "Ignore"},
      {                                      0, 3,      "Abort"},
      {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2,       "Quit"},
  };

  SDL_MessageBoxData data = {
      SDL_MESSAGEBOX_ERROR,
      NULL,
      "Assertion Triggered",
      buf,
      count_of(buttons),
      buttons,
      NULL,
  };

  assert_action action = ASSERT_ACTION_IGNORE;

  int btn_id = 2;
  SDL_ShowMessageBox(&data, &btn_id);
  switch (btn_id) {
    case 0: {
      action = ASSERT_ACTION_IGNORE;
      break;
    }
    case 1: {
      action = ASSERT_ACTION_BREAKPOINT;
      break;
    }
    case 2: {
      action = ASSERT_ACTION_QUIT;
      break;
    }
    case 3: {
      action = ASSERT_ACTION_ABORT;
      break;
    }
    default: {
      break;
    }
  }

  profile_func_end;
  return action;
}

func void assert_do_action(assert_action action) {
  profile_func_begin;
  switch (action) {
    case ASSERT_ACTION_IGNORE: {
      break;
    }
    case ASSERT_ACTION_BREAKPOINT: {
      SDL_TriggerBreakpoint();
      break;
    }
    case ASSERT_ACTION_QUIT: {
      process_exit(1);
      break;
    }
    case ASSERT_ACTION_ABORT: {
      process_abort();
      break;
    }
    default: {
      break;
    }
  }
  profile_func_end;
}

// =========================================================================
// Public API
// =========================================================================

func void assert_set_mode(assert_mode mode) {
  profile_func_begin;
  if (mode < ASSERT_MODE_DEBUG || mode > ASSERT_MODE_IGNORE) {
    profile_func_end;
    return;
  }
  assert(mode >= ASSERT_MODE_DEBUG && mode <= ASSERT_MODE_IGNORE);
  atomic_u32_set(&assert_mode_current, mode);
  profile_func_end;
}

func assert_mode assert_get_mode(void) {
  profile_func_begin;
  assert_mode mode = (assert_mode)atomic_u32_get(&assert_mode_current);
  profile_func_end;
  return mode;
}

func void _assert(b32 condition, cstr8 cond_msg, callsite site) {
  profile_func_begin;
  if (condition) {
    profile_func_end;
    return;
  }
  if (cond_msg == NULL) {
    cond_msg = "<null assertion message>";
  }
  assert(cond_msg != NULL);
  assert_mode mode = assert_get_mode();

  msg assert_msg = {0};
  assert_msg.type = MSG_CORE_TYPE_ASSERT;
  msg_core_assert_data assert_data = {
      .mode = mode,
      .source_site = site,
      .text = cond_msg != NULL ? cond_msg : "",
  };
  msg_core_fill_assert(&assert_msg, &assert_data);
  if (!msg_post(&assert_msg)) {
    profile_func_end;
    return;
  }

  assert_action action = ASSERT_ACTION_IGNORE;
  switch (mode) {
    case ASSERT_MODE_DEBUG: {
      assert_log_msg(cond_msg, site);
      assert_log_stacktrace(site);
      action = assert_dialog(cond_msg, site);
      break;
    }
    case ASSERT_MODE_QUIT: {
      assert_log_msg(cond_msg, site);
      assert_log_stacktrace(site);
      action = ASSERT_ACTION_QUIT;
      break;
    }
    case ASSERT_MODE_ABORT: {
      assert_log_msg(cond_msg, site);
      assert_log_stacktrace(site);
      action = ASSERT_ACTION_ABORT;
      break;
    }
    case ASSERT_MODE_LOG: {
      assert_log_msg(cond_msg, site);
      assert_log_stacktrace(site);
      break;
    }
    case ASSERT_MODE_IGNORE: {
      break;
    }
  }

  assert_do_action(action);
  profile_func_end;
}

// Make libmath2 use our custom assert...
void _lm2_custom_assert(
    int expression,
    const char* msg,
    const char* file,
    const char* function,
    int line) {
  callsite site = {file, function, (u32)line};
  _assert(expression, msg, site);
}
