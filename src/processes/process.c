// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process.h"
#include "../sdl3_include.h"

// Returns true if options matches process_options_default().
func b32 process_options_is_default(process_options options) {
  return !options.pipe_stdin && !options.pipe_stdout && !options.pipe_stderr &&
         !options.stderr_to_stdout && !options.background;
}

func process_options process_options_default(void) {
  process_options options = {0};
  return options;
}

func process_options process_options_captured(void) {
  process_options options = {0};
  options.pipe_stdout = 1;
  options.stderr_to_stdout = 1;
  return options;
}

func process _process_create(const c8* const* args, callsite site) {
  return _process_create_with(args, process_options_default(), site);
}

func process _process_create_with(const c8* const* args, process_options options, callsite site) {
  (void)site;

  if (!args || !args[0]) {
    return NULL;
  }

  if (process_options_is_default(options)) {
    return (process)SDL_CreateProcess(args, false);
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  if (!props) {
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
  if (!prc) {
    return NULL;
  }

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

  return data;
}

func void process_read_free(void* ptr) {
  SDL_free(ptr);
}

func b32 process_wait(process prc, b32 block, i32* out_exit_code) {
  if (!prc) {
    return 0;
  }

  i32 exit_code = 0;
  b32 finished = SDL_WaitProcess(
      (SDL_Process*)prc,
      block != 0,
      out_exit_code ? (int*)&exit_code : NULL);
  if (finished && out_exit_code) {
    *out_exit_code = (i32)exit_code;
  }

  return finished;
}

func b32 process_kill(process prc, b32 force) {
  if (!prc) {
    return 0;
  }

  return SDL_KillProcess((SDL_Process*)prc, force != 0);
}

func void process_destroy(process prc) {
  if (!prc) {
    return;
  }

  SDL_DestroyProcess((SDL_Process*)prc);
}
