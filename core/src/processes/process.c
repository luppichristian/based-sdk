// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// Returns true if options matches process_options_default().
func b32 process_options_is_default(process_options options) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return !options.pipe_stdin && !options.pipe_stdout && !options.pipe_stderr &&
         !options.stderr_to_stdout && !options.background && options.cwd == NULL &&
         options.envp == NULL && options.timeout_ms == 0;
}

func process_options process_options_default(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  process_options options = {0};
  TracyCZoneEnd(__tracy_zone_ctx);
  return options;
}

func process_options process_options_captured(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  process_options options = {0};
  options.pipe_stdout = 1;
  options.stderr_to_stdout = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return options;
}

func process _process_create(cstr8 const* args, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return _process_create_with(args, process_options_default(), site);
}

func process _process_create_with(cstr8 const* args, process_options options, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;

  if (!args || !args[0]) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(args[0][0] != '\0');

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
                                                     .object_ptr = NULL,
                                                 });
  (void)msg_post(&lifecycle_msg);

  if (process_options_is_default(options)) {
    process prc = (process)SDL_CreateProcess(args, false);
    if (prc == NULL) {
      thread_log_error("process_create: SDL_CreateProcess failed for '%s'", args[0]);
    } else {
      thread_log_trace("process_create: '%s' options=default", args[0]);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return prc;
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  if (!props) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
  if (prc == NULL) {
    thread_log_error("process_create_with: failed for '%s'", args[0]);
  } else {
    thread_log_trace("process_create_with: '%s'", args[0]);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return prc;
}

func b32 process_is_valid(process prc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return prc != NULL;
}

func u64 process_get_id(process prc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return (u64)SDL_GetNumberProperty(props, SDL_PROP_PROCESS_PID_NUMBER, 0);
}

func void* process_read(process prc, sz* out_size, i32* out_exit_code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
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

  TracyCZoneEnd(__tracy_zone_ctx);
  return data;
}

func void process_read_free(void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_free(ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 process_wait(process prc, b32 block, i32* out_exit_code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
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
  thread_log_trace("process_wait: prc=%p finished=%u", prc, (u32)finished);

  TracyCZoneEnd(__tracy_zone_ctx);
  return finished;
}

func b32 process_wait_timeout(process prc, i32 timeout_ms, i32* out_exit_code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc || timeout_ms < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  u64 start_ticks = SDL_GetTicks();
  i32 exit_code = 0;
  while (!process_wait(prc, 0, &exit_code)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    SDL_Delay(1);
  }

  if (out_exit_code != NULL) {
    *out_exit_code = exit_code;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 process_kill(process prc, b32 force) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(force == 0 || force == 1);

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_KillProcess((SDL_Process*)prc, force != 0);
}

func void process_destroy(process prc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
                                                     .object_ptr = prc,
                                                 });
  (void)msg_post(&lifecycle_msg);

  thread_log_trace("process_destroy: prc=%p", prc);
  SDL_DestroyProcess((SDL_Process*)prc);
  TracyCZoneEnd(__tracy_zone_ctx);
}
