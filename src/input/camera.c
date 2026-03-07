// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/camera.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

typedef struct camera_state_entry {
  SDL_CameraID camera_id;
  b32 is_open;
  b32 is_started;
} camera_state_entry;

const_var sz CAMERA_STATE_CAP = 64;
global_var camera_state_entry camera_states[CAMERA_STATE_CAP] = {0};

func camera_state_entry* camera_find_state(SDL_CameraID camera_id, b32 create_if_missing) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  for (sz item_idx = 0; item_idx < CAMERA_STATE_CAP; item_idx += 1) {
    if (camera_states[item_idx].camera_id == camera_id) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return &camera_states[item_idx];
    }
  }
  if (!create_if_missing) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  for (sz item_idx = 0; item_idx < CAMERA_STATE_CAP; item_idx += 1) {
    if (camera_states[item_idx].camera_id == 0) {
      camera_states[item_idx].camera_id = camera_id;
      TracyCZoneEnd(__tracy_zone_ctx);
      return &camera_states[item_idx];
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

func b32 camera_id_is_valid(camera src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL;
}

func camera camera_from_native_id(up native_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (camera)(up)native_id;
}

func up camera_to_native_id(camera src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (up)src;
}

func sz camera_get_count(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return count > 0 ? (sz)count : 0;
}

func b32 camera_get_id(sz idx, camera* out_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = camera_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

func cstr8 camera_get_name(camera id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!camera_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetCameraName((SDL_CameraID)camera_to_native_id(id));
}

func camera_position camera_get_position(camera id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!camera_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return CAMERA_POSITION_UNKNOWN;
  }

  switch (SDL_GetCameraPosition((SDL_CameraID)camera_to_native_id(id))) {
    case SDL_CAMERA_POSITION_FRONT_FACING:
      TracyCZoneEnd(__tracy_zone_ctx);
      return CAMERA_POSITION_FRONT_FACING;
    case SDL_CAMERA_POSITION_BACK_FACING:
      TracyCZoneEnd(__tracy_zone_ctx);
      return CAMERA_POSITION_BACK_FACING;
    case SDL_CAMERA_POSITION_UNKNOWN:
    default:
      TracyCZoneEnd(__tracy_zone_ctx);
      return CAMERA_POSITION_UNKNOWN;
  }
}

func b32 camera_open(camera id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!camera_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 1);
  if (state == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  state->is_open = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 camera_close(camera id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!camera_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  *state = (camera_state_entry) {0};
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 camera_start(camera id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!camera_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 1);
  if (state == NULL || !state->is_open) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  state->is_started = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 camera_stop(camera id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!camera_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  state->is_started = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 camera_read(camera id, buffer* out_frame) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_frame != NULL) {
    *out_frame = (buffer) {0};
  }
  if (!camera_id_is_valid(id) || out_frame == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  camera_state_entry* state = camera_find_state((SDL_CameraID)camera_to_native_id(id), 0);
  if (state == NULL || !state->is_open || !state->is_started) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}
