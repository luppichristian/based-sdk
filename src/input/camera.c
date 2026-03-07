// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/camera.h"
#include "../sdl3_include.h"

func b32 camera_id_is_valid(camera_id src) {
  return src.opaque != 0;
}

func camera_id camera_from_native_id(u64 native_id) {
  camera_id result = {0};
  result.opaque = native_id;
  return result;
}

func u64 camera_to_native_id(camera_id src) {
  return src.opaque;
}

func sz camera_get_count(void) {
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);

  if (ids) {
    SDL_free(ids);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 camera_get_id(sz idx, camera_id* out_id) {
  int count = 0;
  SDL_CameraID* ids = SDL_GetCameras(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = (camera_id) {0};
  }

  if (found && out_id) {
    *out_id = camera_from_native_id((u64)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  return found;
}

func cstr8 camera_get_name(camera_id id) {
  if (!camera_id_is_valid(id)) {
    return NULL;
  }

  return SDL_GetCameraName((SDL_CameraID)camera_to_native_id(id));
}

func camera_position camera_get_position(camera_id id) {
  if (!camera_id_is_valid(id)) {
    return CAMERA_POSITION_UNKNOWN;
  }

  switch (SDL_GetCameraPosition((SDL_CameraID)camera_to_native_id(id))) {
    case SDL_CAMERA_POSITION_FRONT_FACING:
      return CAMERA_POSITION_FRONT_FACING;
    case SDL_CAMERA_POSITION_BACK_FACING:
      return CAMERA_POSITION_BACK_FACING;
    case SDL_CAMERA_POSITION_UNKNOWN:
    default:
      return CAMERA_POSITION_UNKNOWN;
  }
}
