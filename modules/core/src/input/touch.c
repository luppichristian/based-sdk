// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/touch.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 touch_is_valid(touch src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_TOUCH;
}

func touch device_get_touch(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_TOUCH) {
    invalid_code_path;
    return NULL;
  }

  return (touch)src;
}

func device touch_to_device(touch src) {
  if (!touch_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func b32 touch_is_available(void) {
  return touch_get_total_count() > 0;
}

func sz touch_get_total_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);

  if (ids == NULL && count != 0) {
    thread_log_warn("Failed to enumerate touch devices error=%s", SDL_GetError());
  }

  if (ids) {
    SDL_free(ids);
  }

  sz result = count > 0 ? (sz)count : 0;
  profile_func_end;
  return result;
}

func touch touch_get_from_idx(sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);
  touch result = NULL;
  b32 found = ids && idx < (sz)count;

  if (ids == NULL && count != 0) {
    thread_log_error("Failed to enumerate touch devices for id lookup idx=%zu error=%s", (size_t)idx, SDL_GetError());
  } else if (!found) {
    thread_log_warn("Touch device lookup missed idx=%zu count=%d", (size_t)idx, count);
  }

  if (found) {
    result = device_get_touch(devices_make_id(DEVICE_TYPE_TOUCH, (u64)ids[idx]));
    devices_update_runtime(touch_to_device(result), 1, (void*)(up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return result;
}

func touch touch_get_primary(void) {
  return touch_get_from_idx(0);
}

func touch touch_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_TOUCH);
  return device_is_valid(src) ? device_get_touch(src) : NULL;
}

func touch_device_kind touch_get_device_kind(touch src) {
  profile_func_begin;
  if (!touch_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return TOUCH_DEVICE_INVALID;
  }

  touch_device_kind result = (touch_device_kind)SDL_GetTouchDeviceType((SDL_TouchID)devices_get_instance(touch_to_device(src)));
  profile_func_end;
  return result;
}

func sz touch_get_finger_count(touch src) {
  profile_func_begin;
  int count = 0;
  SDL_Finger** fingers = NULL;

  if (!touch_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return 0;
  }

  fingers = SDL_GetTouchFingers((SDL_TouchID)devices_get_instance(touch_to_device(src)), &count);
  if (fingers) {
    SDL_free(fingers);
  }

  sz result = count > 0 ? (sz)count : 0;
  profile_func_end;
  return result;
}

func b32 touch_get_finger(touch src, sz idx, touch_finger_state* out_finger) {
  profile_func_begin;
  int count = 0;
  SDL_Finger** fingers = NULL;
  b32 found = false;

  if (out_finger) {
    *out_finger = (touch_finger_state) {0};
  }

  if (!touch_is_valid(src) || !out_finger) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  fingers = SDL_GetTouchFingers((SDL_TouchID)devices_get_instance(touch_to_device(src)), &count);
  if (fingers == NULL && count != 0) {
    thread_log_error("Failed to enumerate touch fingers device=%llu error=%s",
                     (unsigned long long)devices_get_instance(touch_to_device(src)),
                     SDL_GetError());
  }
  if (fingers && idx < (sz)count && fingers[idx]) {
    out_finger->id = (finger_id)fingers[idx]->id;
    out_finger->x = fingers[idx]->x;
    out_finger->y = fingers[idx]->y;
    out_finger->pressure = fingers[idx]->pressure;
    found = 1;
  } else {
    thread_log_warn("Touch finger lookup missed device=%llu idx=%zu count=%d",
                    (unsigned long long)devices_get_instance(touch_to_device(src)),
                    (size_t)idx,
                    count);
  }

  if (fingers) {
    SDL_free(fingers);
  }

  profile_func_end;
  return found;
}
