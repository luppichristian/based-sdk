// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/pipe.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func pipe pipe_stdin(process prc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(prc != NULL);

  pipe pip = (pipe)SDL_GetProcessInput((SDL_Process*)prc);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                       .object_ptr = pip,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return NULL;
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return pip;
}

func pipe pipe_stdout(process prc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(prc != NULL);

  pipe pip = (pipe)SDL_GetProcessOutput((SDL_Process*)prc);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                       .object_ptr = pip,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return NULL;
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return pip;
}

func pipe pipe_stderr(process prc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!prc) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(prc != NULL);

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  pipe pip = (pipe)SDL_GetPointerProperty(props, SDL_PROP_PROCESS_STDERR_POINTER, NULL);
  if (pip != NULL) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
    msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                       .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                       .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                       .object_ptr = pip,
                                                   });
    if (!msg_post(&lifecycle_msg)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return NULL;
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return pip;
}

func b32 pipe_is_valid(pipe pip) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return pip != NULL;
}

func sz pipe_read(pipe pip, void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!pip || !ptr || !size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(size > 0);

  TracyCZoneEnd(__tracy_zone_ctx);
  return (sz)SDL_ReadIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func sz pipe_write(pipe pip, const void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!pip || !ptr || !size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(size > 0);

  TracyCZoneEnd(__tracy_zone_ctx);
  return (sz)SDL_WriteIO((SDL_IOStream*)pip, ptr, (size_t)size);
}

func b32 pipe_flush(pipe pip) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!pip) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_FlushIO((SDL_IOStream*)pip);
}

func void pipe_close(pipe pip) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!pip) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_PIPE,
                                                     .object_ptr = pip,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  thread_log_trace("pipe_close: pipe=%p", pip);
  SDL_CloseIO((SDL_IOStream*)pip);
  TracyCZoneEnd(__tracy_zone_ctx);
}
