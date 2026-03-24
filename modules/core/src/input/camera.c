// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/camera.h"
#include "basic/assert.h"
#include "../internal.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

typedef struct camera_state_entry {
  SDL_CameraID camera_id;
  b32 is_open;
  b32 is_started;
} camera_state_entry;

global_var camera_state_entry camera_states[DEVICES_HANDLE_CAP] = {0};

func camera_state_entry* camera_find_state(SDL_CameraID camera_id, b32 create_if_missing) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (camera_states[item_idx].camera_id == camera_id) {
      profile_func_end;
      return &camera_states[item_idx];
    }
  }
  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (camera_states[item_idx].camera_id == 0) {
      camera_states[item_idx].camera_id = camera_id;
      profile_func_end;
      return &camera_states[item_idx];
    }
  }
  if (create_if_missing) {
    thread_log_error("Camera state table is full camera_id=%lld", (long long)camera_id);
  }
  profile_func_end;
  return NULL;
}

func b32 camera_is_valid(camera src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_CAMERA;
}

func camera device_get_camera(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_CAMERA) {
    invalid_code_path;
    return NULL;
  }

  return (camera)src;
}

func camera camera_from_native_id(up native_id) {
  return device_get_camera(devices_make_id(DEVICE_TYPE_CAMERA, (u64)native_id));
}

func up camera_to_native_id(camera src) {
  if (!camera_is_valid(src)) {
    return 0;
  }

  return (up)devices_get_instance(camera_to_device(src));
}

func device camera_to_device(camera src) {
  if (!camera_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func sz camera_get_total_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);

  if (ids == NULL && count != 0) {
    thread_log_warn("Failed to enumerate cameras error=%s", SDL_GetError());
  }

  if (ids) {
    SDL_free(ids);
  }

  sz result = count > 0 ? (sz)count : 0;
  profile_func_end;
  return result;
}

func camera camera_get_from_idx(sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);
  b32 found = ids != NULL && idx < (sz)count;
  camera out_id = NULL;

  if (ids == NULL && count != 0) {
    thread_log_error("Failed to enumerate cameras for id lookup idx=%zu error=%s", (size_t)idx, SDL_GetError());
  } else if (!found) {
    thread_log_warn("Camera id lookup missed idx=%zu count=%d", (size_t)idx, count);
  }

  if (found) {
    out_id = camera_from_native_id((up)ids[idx]);
    devices_update_runtime(camera_to_device(out_id), 1, (void*)(up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return out_id;
}

func camera camera_get_primary(void) {
  return camera_get_from_idx(0);
}

func camera camera_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_CAMERA);
  return device_is_valid(src) ? device_get_camera(src) : NULL;
}

func cstr8 camera_get_name(camera id) {
  profile_func_begin;
  if (!camera_is_valid(id)) {
    thread_log_warn("Rejected camera name query for invalid camera");
    profile_func_end;
    return NULL;
  }

  cstr8 result = SDL_GetCameraName((SDL_CameraID)camera_to_native_id(id));
  if (result == NULL) {
    thread_log_warn("Camera name is unavailable camera=%lld", (long long)camera_to_native_id(id));
  }
  profile_func_end;
  return result;
}

func camera_pos camera_get_pos(camera id) {
  profile_func_begin;
  if (!camera_is_valid(id)) {
    thread_log_warn("Rejected camera pos query for invalid camera");
    profile_func_end;
    return CAMERA_POS_UNKNOWN;
  }

  camera_pos result = CAMERA_POS_UNKNOWN;
  switch (SDL_GetCameraPosition((SDL_CameraID)camera_to_native_id(id))) {
    case SDL_CAMERA_POSITION_FRONT_FACING:
      result = CAMERA_POS_FRONT_FACING;
      break;
    case SDL_CAMERA_POSITION_BACK_FACING:
      result = CAMERA_POS_BACK_FACING;
      break;
    case SDL_CAMERA_POSITION_UNKNOWN:
    default:
      result = CAMERA_POS_UNKNOWN;
      break;
  }
  profile_func_end;
  return result;
}

func b32 camera_open(camera id) {
  profile_func_begin;
  if (!camera_is_valid(id)) {
    thread_log_error("Rejected camera open for invalid camera");
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 1);
  if (state == NULL) {
    thread_log_error("Failed to create camera state camera=%lld", (long long)camera_to_native_id(id));
    profile_func_end;
    return false;
  }
  state->is_open = 1;
  thread_log_info("Opened camera=%lld", (long long)camera_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 camera_close(camera id) {
  profile_func_begin;
  if (!camera_is_valid(id)) {
    thread_log_error("Rejected camera close for invalid camera");
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL) {
    thread_log_warn("Cannot close unknown camera=%lld", (long long)camera_to_native_id(id));
    profile_func_end;
    return false;
  }
  *state = (camera_state_entry) {0};
  thread_log_info("Closed camera=%lld", (long long)camera_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 camera_start(camera id) {
  profile_func_begin;
  if (!camera_is_valid(id)) {
    thread_log_error("Rejected camera start for invalid camera");
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 1);
  if (state == NULL || !state->is_open) {
    thread_log_error("Cannot start camera=%lld because it is not open", (long long)camera_to_native_id(id));
    profile_func_end;
    return false;
  }
  state->is_started = 1;
  thread_log_info("Started camera=%lld", (long long)camera_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 camera_stop(camera id) {
  profile_func_begin;
  if (!camera_is_valid(id)) {
    thread_log_error("Rejected camera stop for invalid camera");
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL) {
    thread_log_warn("Cannot stop unknown camera=%lld", (long long)camera_to_native_id(id));
    profile_func_end;
    return false;
  }
  state->is_started = 0;
  thread_log_info("Stopped camera=%lld", (long long)camera_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 camera_read(camera id, buffer* out_frame) {
  profile_func_begin;
  if (out_frame != NULL) {
    *out_frame = (buffer) {0};
  }
  if (!camera_is_valid(id) || out_frame == NULL) {
    thread_log_error("Rejected camera read camera=%lld out_frame=%p",
                     (long long)camera_to_native_id(id),
                     (void*)out_frame);
    profile_func_end;
    return false;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL || !state->is_open || !state->is_started) {
    thread_log_error("Cannot read camera=%lld open=%u started=%u",
                     (long long)camera_to_native_id(id),
                     (u32)(state != NULL && state->is_open),
                     (u32)(state != NULL && state->is_started));
    profile_func_end;
    return false;
  }
  thread_log_warn("Camera read is not implemented camera=%lld", (long long)camera_to_native_id(id));
  profile_func_end;
  return false;
}
