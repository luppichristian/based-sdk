// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

// Returns true if options matches process_options_default().
func b32 process_options_is_default(process_options options) {
  return !options.pipe_stdin && !options.pipe_stdout && !options.pipe_stderr &&
         !options.stderr_to_stdout && !options.background && options.cwd == NULL &&
         options.envp == NULL && options.timeout_ms == 0;
}

// Returns true if options matches process_options_captured().
func b32 process_options_is_captured(process_options options) {
  return !options.pipe_stdin && options.pipe_stdout && !options.pipe_stderr &&
         options.stderr_to_stdout && !options.background && options.cwd == NULL &&
         options.envp == NULL && options.timeout_ms == 0;
}

func process_options process_options_default(void) {
  profile_func_begin;
  process_options options = {0};
  profile_func_end;
  return options;
}

func process_options process_options_captured(void) {
  profile_func_begin;
  process_options options = {0};
  options.pipe_stdout = 1;
  options.stderr_to_stdout = 1;
  profile_func_end;
  return options;
}

func process _process_create(cstr8 const* args, callsite site) {
  profile_func_begin;
  process res = _process_create_with(args, process_options_default(), site);
  profile_func_end;
  return res;
}

func process _process_create_with(
    cstr8 const* args,
    process_options options,
    callsite site) {
  profile_func_begin;

  if (!args || !args[0]) {
    thread_log_error("Rejected process create without arguments");
    profile_func_end;
    return NULL;
  }
  assert(args[0][0] != '\0');

  if (process_options_is_default(options)) {
    process prc = (process)SDL_CreateProcess(args, false);
    if (prc == NULL) {
      thread_log_warn("Default process create failed command=%s error=%s", args[0], SDL_GetError());
      thread_log_verbose("Retrying process create with explicit null stdio");

      SDL_PropertiesID fallback_props = SDL_CreateProperties();
      b32 fallback_ok = fallback_props != 0;
      if (fallback_ok) {
        fallback_ok = SDL_SetPointerProperty(fallback_props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, (void*)args);
      }
      if (fallback_ok) {
        fallback_ok = SDL_SetNumberProperty(
            fallback_props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_NULL);
      }
      if (fallback_ok) {
        fallback_ok = SDL_SetNumberProperty(
            fallback_props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_NULL);
      }
      if (fallback_ok) {
        fallback_ok = SDL_SetNumberProperty(
            fallback_props, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_NULL);
      }

      prc = fallback_ok ? (process)SDL_CreateProcessWithProperties(fallback_props) : NULL;
      if (fallback_props != 0) {
        SDL_DestroyProperties(fallback_props);
      }
    }

    if (prc == NULL) {
      thread_log_error("Failed to create process command=%s error=%s", args[0], SDL_GetError());
      profile_func_end;
      return NULL;
    }

    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
        .object_ptr = prc,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyProcess((SDL_Process*)prc);
      thread_log_trace("Process creation was suspended command=%s", args[0]);
      profile_func_end;
      return NULL;
    }

    thread_log_trace("Created process command=%s options=default", args[0]);
    profile_func_end;
    return prc;
  }

  if (process_options_is_captured(options)) {
    process prc = (process)SDL_CreateProcess(args, true);
    if (prc == NULL) {
      thread_log_error("Failed to create captured process command=%s error=%s", args[0], SDL_GetError());
      profile_func_end;
      return NULL;
    }

    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
        .object_ptr = prc,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyProcess((SDL_Process*)prc);
      thread_log_trace("Captured process creation was suspended command=%s", args[0]);
      profile_func_end;
      return NULL;
    }

    thread_log_trace("Created process command=%s options=captured", args[0]);
    profile_func_end;
    return prc;
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  if (!props) {
    thread_log_error("Failed to create process properties command=%s error=%s", args[0], SDL_GetError());
    profile_func_end;
    return NULL;
  }

  b32 ok = SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, (void*)args);
  if (ok && options.pipe_stdin) {
    ok = SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_APP);
  }
  if (ok && options.pipe_stdout) {
    ok = SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
  }
  if (ok && options.pipe_stderr) {
    ok = SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_APP);
  }
  if (ok && options.stderr_to_stdout) {
    ok = SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_TO_STDOUT_BOOLEAN, true);
  }
  if (ok && options.background) {
    ok = SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN, true);
  }

  process prc = ok ? (process)SDL_CreateProcessWithProperties(props) : NULL;
  SDL_DestroyProperties(props);
  if (!ok) {
    thread_log_error("Failed to configure process properties command=%s", args[0]);
  }
  if (prc == NULL) {
    thread_log_error("Failed to create configured process command=%s error=%s", args[0], SDL_GetError());
  } else {
    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
        .object_ptr = prc,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyProcess((SDL_Process*)prc);
      thread_log_trace("Process creation was suspended command=%s", args[0]);
      profile_func_end;
      return NULL;
    }

    thread_log_trace("Created configured process command=%s", args[0]);
  }
  profile_func_end;
  return prc;
}

func b32 process_is_valid(process prc) {
  return prc != NULL;
}

func u64 process_get_id(process prc) {
  if (!prc) {
    return 0;
  }

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    return 0;
  }

  return (u64)SDL_GetNumberProperty(props, SDL_PROP_PROCESS_PID_NUMBER, 0);
}

func void* process_read(process prc, sz* out_size, i32* out_exit_code) {
  profile_func_begin;
  if (!prc) {
    thread_log_error("Cannot read invalid process handle");
    profile_func_end;
    return NULL;
  }
  assert(process_is_valid(prc));

  sz read_size = 0;
  i32 exit_code = 0;
  void* data = SDL_ReadProcess(
      (SDL_Process*)prc,
      out_size ? (size_t*)&read_size : NULL,
      out_exit_code ? (int*)&exit_code : NULL);

  if (out_size) {
    *out_size = (sz)read_size;
  }
  if (out_exit_code) {
    *out_exit_code = (i32)exit_code;
  }

  if (data == NULL) {
    thread_log_warn("Process read returned no data handle=%p", prc);
  } else {
    thread_log_trace("Read process output handle=%p size=%zu", prc, (size_t)read_size);
  }

  profile_func_end;
  return data;
}

func void process_read_free(void* ptr) {
  profile_func_begin;
  SDL_free(ptr);
  profile_func_end;
}

func b32 process_wait(process prc, b32 block, i32* out_exit_code) {
  profile_func_begin;
  if (!prc) {
    thread_log_error("Cannot wait on invalid process");
    profile_func_end;
    return false;
  }
  assert(block == 0 || block == 1);

  i32 exit_code = 0;
  b32 finished = SDL_WaitProcess(
      (SDL_Process*)prc,
      block != 0,
      out_exit_code ? (int*)&exit_code : NULL);
  if (finished && out_exit_code) {
    *out_exit_code = (i32)exit_code;
  }
  thread_log_trace("Waited on process handle=%p finished=%u block=%u", prc, (u32)finished, (u32)block);

  profile_func_end;
  return finished;
}

func b32 process_wait_timeout(process prc, i32 timeout_ms, i32* out_exit_code) {
  profile_func_begin;
  if (!prc || timeout_ms < 0) {
    thread_log_error("Rejected process wait timeout handle=%p timeout_ms=%d", prc, timeout_ms);
    profile_func_end;
    return false;
  }

  u64 start_ticks = SDL_GetTicks();
  i32 exit_code = 0;
  safe_while (!process_wait(prc, 0, &exit_code)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      thread_log_warn("Process wait timed out handle=%p timeout_ms=%d", prc, timeout_ms);
      profile_func_end;
      return false;
    }
    SDL_Delay(1);
  }

  if (out_exit_code != NULL) {
    *out_exit_code = exit_code;
  }
  thread_log_trace("Process wait completed within timeout handle=%p timeout_ms=%d", prc, timeout_ms);
  profile_func_end;
  return true;
}

func b32 process_kill(process prc, b32 force) {
  profile_func_begin;
  if (!prc) {
    thread_log_error("Cannot kill invalid process");
    profile_func_end;
    return false;
  }
  assert(force == 0 || force == 1);

  b32 success = SDL_KillProcess((SDL_Process*)prc, force != 0);
  if (!success) {
    thread_log_error("Failed to kill process handle=%p force=%u error=%s", prc, (u32)force, SDL_GetError());
  } else {
    thread_log_info("Killed process handle=%p force=%u", prc, (u32)force);
  }
  profile_func_end;
  return success;
}

func void _process_destroy(process prc, callsite site) {
  profile_func_begin;
  if (!prc) {
    thread_log_warn("Skipping process destroy for invalid handle");
    profile_func_end;
    return;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
      .object_ptr = prc,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Process destruction was suspended handle=%p", prc);
    profile_func_end;
    return;
  }

  thread_log_trace("Destroyed process handle=%p", prc);
  SDL_DestroyProcess((SDL_Process*)prc);
  profile_func_end;
}
