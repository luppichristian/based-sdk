// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/camera.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

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
