// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/touch.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

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

func device touch_get_device(sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);
  device result = NULL;
  b32 found = ids && idx < (sz)count;

  if (ids == NULL && count != 0) {
    thread_log_error("Failed to enumerate touch devices for id lookup idx=%zu error=%s", (size_t)idx, SDL_GetError());
  } else if (!found) {
    thread_log_warn("Touch device lookup missed idx=%zu count=%d", (size_t)idx, count);
  }

  if (found) {
    result = devices_make_id(DEVICE_TYPE_TOUCH, (u64)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return result;
}

func touch_device_kind touch_get_device_kind(device dev_id) {
  profile_func_begin;
  if (devices_get_type(dev_id) != DEVICE_TYPE_TOUCH) {
    thread_log_warn("Rejected touch device kind query for invalid device type=%u", (u32)devices_get_type(dev_id));
    profile_func_end;
    return TOUCH_DEVICE_INVALID;
  }

  touch_device_kind result = (touch_device_kind)SDL_GetTouchDeviceType((SDL_TouchID)devices_get_instance(dev_id));
  profile_func_end;
  return result;
}

func sz touch_get_finger_count(device dev_id) {
  profile_func_begin;
  int count = 0;
  SDL_Finger** fingers = NULL;

  if (devices_get_type(dev_id) != DEVICE_TYPE_TOUCH) {
    thread_log_warn("Rejected touch finger count query for invalid device type=%u", (u32)devices_get_type(dev_id));
    profile_func_end;
    return 0;
  }

  fingers = SDL_GetTouchFingers((SDL_TouchID)devices_get_instance(dev_id), &count);
  if (fingers) {
    SDL_free(fingers);
  }

  sz result = count > 0 ? (sz)count : 0;
  profile_func_end;
  return result;
}

func b32 touch_get_finger(device dev_id, sz idx, touch_finger_state* out_finger) {
  profile_func_begin;
  int count = 0;
  SDL_Finger** fingers = NULL;
  b32 found = false;

  if (out_finger) {
    *out_finger = (touch_finger_state) {0};
  }

  if (devices_get_type(dev_id) != DEVICE_TYPE_TOUCH || !out_finger) {
    thread_log_error("Rejected touch finger query type=%u out_finger=%p",
                     (u32)devices_get_type(dev_id),
                     (void*)out_finger);
    profile_func_end;
    return false;
  }
  assert(out_finger != NULL);

  fingers = SDL_GetTouchFingers((SDL_TouchID)devices_get_instance(dev_id), &count);
  if (fingers == NULL && count != 0) {
    thread_log_error("Failed to enumerate touch fingers device=%llu error=%s",
                     (unsigned long long)devices_get_instance(dev_id),
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
                    (unsigned long long)devices_get_instance(dev_id),
                    (size_t)idx,
                    count);
  }

  if (fingers) {
    SDL_free(fingers);
  }

  profile_func_end;
  return found;
}
