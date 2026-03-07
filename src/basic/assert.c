// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/assert.h"
#include "basic/log.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "threads/atomics.h"
#include "../sdl3_include.h"
#include "basic/utility_defines.h"
#include <stdlib.h>

// =========================================================================
// Internal state
// =========================================================================

global_var assert_mode assert_mode_current = ASSERT_MODE_DEFAULT;
global_var mutex assert_mutex = NULL;
global_var atomic_i32 assert_mutex_init = {0};

// =========================================================================
// Internal helpers
// =========================================================================

func mutex assert_lock_get(void) {
  if (atomic_i32_get(&assert_mutex_init) == 2) {
    return assert_mutex;
  }

  i32 expected = 0;
  if (atomic_i32_cmpex(&assert_mutex_init, &expected, 1)) {
    assert_mutex = mutex_create();
    atomic_fence_release();
    atomic_i32_set(&assert_mutex_init, 2);
    return assert_mutex;
  }

  while (atomic_i32_get(&assert_mutex_init) != 2) {
    atomic_pause();
  }
  atomic_fence_acquire();
  return assert_mutex;
}

func void assert_log_msg(cstr8 msg, callsite site) {
  _log(thread_get_log_state(), LOG_LEVEL_FATAL, site, "Assertion failed: %s", msg);
}

// Returns: 0 = ignore, 1 = breakpoint, 2 = quit.
// Defaults to quit if the message box cannot be displayed.
func i32 assert_dialog(cstr8 msg, callsite site) {
  str8_long buf = {0};
  cstr8_format(buf, size_of(buf), "Assertion failed: %s\n\nin %s() at %s:%u", msg, site.function, site.filename, site.line);

  SDL_MessageBoxButtonData buttons[] = {
      {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Breakpoint"},
      {                                      0, 0,     "Ignore"},
      {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2,       "Quit"},
  };

  SDL_MessageBoxData data = {
      SDL_MESSAGEBOX_ERROR,
      NULL,
      "Assertion Failed",
      buf,
      count_of(buttons),
      buttons,
      NULL,
  };

  int btn_id = 2;
  SDL_ShowMessageBox(&data, &btn_id);
  return (i32)btn_id;
}

// =========================================================================
// Public API
// =========================================================================

func void assert_set_mode(assert_mode mode) {
  if (mode < ASSERT_MODE_DEBUG || mode > ASSERT_MODE_IGNORE) {
    return;
  }
  assert(mode >= ASSERT_MODE_DEBUG && mode <= ASSERT_MODE_IGNORE);
  mutex lock = assert_lock_get();
  if (lock) {
    mutex_lock(lock);
  }
  assert_mode_current = mode;
  if (lock) {
    mutex_unlock(lock);
  }
}

func void _assert(b32 condition, cstr8 cond_msg, callsite site) {
  if (condition) {
    return;
  }
  if (cond_msg == NULL) {
    cond_msg = "<null assertion message>";
  }
  assert(cond_msg != NULL);

  assert_mode mode = ASSERT_MODE_DEFAULT;
  mutex lock = assert_lock_get();
  if (lock) {
    mutex_lock(lock);
  }
  mode = assert_mode_current;
  if (lock) {
    mutex_unlock(lock);
  }

  msg assert_msg = {0};
  assert_msg.type = MSG_CORE_TYPE_ASSERT;
  msg_core_assert_data assert_data = {
      .mode = mode,
      .source_site = site,
  };
  strncpy(assert_data.text, cond_msg != NULL ? cond_msg : "", MSG_ASSERT_TEXT_CAP);
  msg_core_fill_assert(&assert_msg, &assert_data);
  if (!msg_post(&assert_msg)) {
    return;
  }

  switch (mode) {
    case ASSERT_MODE_DEBUG: {
      assert_log_msg(cond_msg, site);
      i32 action = assert_dialog(cond_msg, site);
      if (action == 1) {
        SDL_TriggerBreakpoint();
      } else if (action == 2) {
        exit(1);
      }
      break;
    }
    case ASSERT_MODE_QUIT:
      assert_log_msg(cond_msg, site);
      exit(1);
    case ASSERT_MODE_LOG:
      assert_log_msg(cond_msg, site);
      break;
    case ASSERT_MODE_IGNORE:
      break;
  }
}

void _lm2_custom_assert(
    int expression,
    const char* msg,
    const char* file,
    const char* function,
    int line) {
  callsite site = {file, function, (u32)line};
  _assert(expression, msg, site);
}
