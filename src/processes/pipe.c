// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/pipe.h"
#include "../sdl3_include.h"

func pipe process_get_stdin(process prc) {
  if (!prc) {
    return NULL;
  }

  return (pipe)SDL_GetProcessInput((SDL_Process*)prc);
}

func pipe process_get_stdout(process prc) {
  if (!prc) {
    return NULL;
  }

  return (pipe)SDL_GetProcessOutput((SDL_Process*)prc);
}

func pipe process_get_stderr(process prc) {
  if (!prc) {
    return NULL;
  }

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    return NULL;
  }

  return (pipe)SDL_GetPointerProperty(props, SDL_PROP_PROCESS_STDERR_POINTER, NULL);
}

func b32 pipe_is_valid(pipe pip) {
  return pip != NULL;
}

func sz pipe_read(pipe pip, void* ptr, sz size) {
  if (!pip || !ptr || !size) {
    return 0;
  }

  return (sz)SDL_ReadIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func sz pipe_write(pipe pip, const void* ptr, sz size) {
  if (!pip || !ptr || !size) {
    return 0;
  }

  return (sz)SDL_WriteIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func b32 pipe_flush(pipe pip) {
  if (!pip) {
    return 0;
  }

  return SDL_FlushIO((SDL_IOStream*)pip);
}

func void pipe_close(pipe pip) {
  if (!pip) {
    return;
  }

  SDL_CloseIO((SDL_IOStream*)pip);
}
