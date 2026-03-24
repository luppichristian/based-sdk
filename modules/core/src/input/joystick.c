// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/joystick.h"
#include "../internal.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func SDL_Joystick* joystick_open_for_query(joystick joy_id) {
  profile_func_begin;
  if (!joystick_is_valid(joy_id)) {
    profile_func_end;
    return NULL;
  }

  SDL_Joystick* handle = SDL_OpenJoystick((SDL_JoystickID)joystick_to_native_id(joy_id));
  if (handle == NULL) {
    thread_log_warn("Failed to open joystick=%lld error=%s", (long long)joystick_to_native_id(joy_id), SDL_GetError());
  }

  profile_func_end;
  return handle;
}

func battery_state joystick_battery_state_from_native(i32 native_state) {
  switch ((SDL_PowerState)native_state) {
    case SDL_POWERSTATE_ERROR:
      return BATTERY_STATE_ERROR;
    case SDL_POWERSTATE_ON_BATTERY:
      return BATTERY_STATE_ON_BATTERY;
    case SDL_POWERSTATE_NO_BATTERY:
      return BATTERY_STATE_NO_BATTERY;
    case SDL_POWERSTATE_CHARGING:
      return BATTERY_STATE_CHARGING;
    case SDL_POWERSTATE_CHARGED:
      return BATTERY_STATE_CHARGED;
    case SDL_POWERSTATE_UNKNOWN:
    default:
      return BATTERY_STATE_UNKNOWN;
  }
}

func i32 joystick_battery_state_to_native(battery_state state) {
  switch (state) {
    case BATTERY_STATE_ERROR:
      return SDL_POWERSTATE_ERROR;
    case BATTERY_STATE_ON_BATTERY:
      return SDL_POWERSTATE_ON_BATTERY;
    case BATTERY_STATE_NO_BATTERY:
      return SDL_POWERSTATE_NO_BATTERY;
    case BATTERY_STATE_CHARGING:
      return SDL_POWERSTATE_CHARGING;
    case BATTERY_STATE_CHARGED:
      return SDL_POWERSTATE_CHARGED;
    case BATTERY_STATE_UNKNOWN:
    default:
      return SDL_POWERSTATE_UNKNOWN;
  }
}

func b32 joystick_is_valid(joystick src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_JOYSTICK;
}

func joystick device_get_joystick(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_JOYSTICK) {
    invalid_code_path;
    return NULL;
  }

  return (joystick)src;
}

func joystick joystick_from_native_id(up native_id) {
  return device_get_joystick(devices_make_id(DEVICE_TYPE_JOYSTICK, (u64)native_id));
}

func up joystick_to_native_id(joystick src) {
  return (up)devices_get_instance(joystick_to_device(src));
}

func device joystick_to_device(joystick src) {
  if (!joystick_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func sz joystick_get_total_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_JoystickID* ids = SDL_GetJoysticks(&count);

  if (ids == NULL && count != 0) {
    thread_log_warn("Failed to enumerate joysticks error=%s", SDL_GetError());
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func joystick joystick_get_from_idx(sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_JoystickID* ids = SDL_GetJoysticks(&count);
  b32 found = ids != NULL && idx < (sz)count;
  joystick out_id = NULL;

  if (ids == NULL && count != 0) {
    thread_log_error("Failed to enumerate joysticks for id lookup idx=%zu error=%s", (size_t)idx, SDL_GetError());
  } else if (!found) {
    thread_log_warn("Joystick id lookup missed idx=%zu count=%d", (size_t)idx, count);
  }

  if (found) {
    out_id = joystick_from_native_id((up)ids[idx]);
    devices_update_runtime(joystick_to_device(out_id), 1, (void*)(up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return out_id;
}

func joystick joystick_get_primary(void) {
  return joystick_get_from_idx(0);
}

func joystick joystick_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_JOYSTICK);
  return device_is_valid(src) ? device_get_joystick(src) : NULL;
}

func cstr8 joystick_get_name(joystick joy_id) {
  profile_func_begin;
  if (!joystick_is_valid(joy_id)) {
    thread_log_warn("Rejected joystick name query for invalid joystick");
    profile_func_end;
    return NULL;
  }

  cstr8 result = SDL_GetJoystickNameForID((SDL_JoystickID)joystick_to_native_id(joy_id));
  if (result == NULL) {
    thread_log_warn("Joystick name is unavailable joystick=%lld", (long long)joystick_to_native_id(joy_id));
  }
  profile_func_end;
  return result;
}

func b32 joystick_is_connected(joystick joy_id) {
  profile_func_begin;
  if (!joystick_is_valid(joy_id)) {
    profile_func_end;
    return false;
  }

  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_GetJoystickConnectionState(handle) == SDL_JOYSTICK_CONNECTION_WIRED ||
               SDL_GetJoystickConnectionState(handle) == SDL_JOYSTICK_CONNECTION_WIRELESS;
  SDL_CloseJoystick(handle);
  profile_func_end;
  return result;
}

func sz joystick_get_axis_count(joystick joy_id) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return 0;
  }

  int count = SDL_GetNumJoystickAxes(handle);
  SDL_CloseJoystick(handle);
  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func sz joystick_get_ball_count(joystick joy_id) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return 0;
  }

  int count = SDL_GetNumJoystickBalls(handle);
  SDL_CloseJoystick(handle);
  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func sz joystick_get_hat_count(joystick joy_id) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return 0;
  }

  int count = SDL_GetNumJoystickHats(handle);
  SDL_CloseJoystick(handle);
  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func sz joystick_get_button_count(joystick joy_id) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return 0;
  }

  int count = SDL_GetNumJoystickButtons(handle);
  SDL_CloseJoystick(handle);
  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func i16 joystick_get_axis(joystick joy_id, sz axis_idx) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return 0;
  }

  if (axis_idx >= (sz)SDL_GetNumJoystickAxes(handle)) {
    SDL_CloseJoystick(handle);
    profile_func_end;
    return 0;
  }

  i16 result = (i16)SDL_GetJoystickAxis(handle, (int)axis_idx);
  SDL_CloseJoystick(handle);
  profile_func_end;
  return result;
}

func b32 joystick_get_ball(joystick joy_id, sz ball_idx, i32* out_xrel, i32* out_yrel) {
  profile_func_begin;
  if (out_xrel) {
    *out_xrel = 0;
  }
  if (out_yrel) {
    *out_yrel = 0;
  }

  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL || out_xrel == NULL || out_yrel == NULL) {
    if (handle != NULL) {
      SDL_CloseJoystick(handle);
    }
    profile_func_end;
    return false;
  }

  if (ball_idx >= (sz)SDL_GetNumJoystickBalls(handle)) {
    SDL_CloseJoystick(handle);
    profile_func_end;
    return false;
  }

  b32 result = SDL_GetJoystickBall(handle, (int)ball_idx, out_xrel, out_yrel) ? true : false;
  SDL_CloseJoystick(handle);
  profile_func_end;
  return result;
}

func joystick_hat_state joystick_get_hat(joystick joy_id, sz hat_idx) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return JOYSTICK_HAT_STATE_CENTERED;
  }

  if (hat_idx >= (sz)SDL_GetNumJoystickHats(handle)) {
    SDL_CloseJoystick(handle);
    profile_func_end;
    return JOYSTICK_HAT_STATE_CENTERED;
  }

  joystick_hat_state result = (joystick_hat_state)SDL_GetJoystickHat(handle, (int)hat_idx);
  SDL_CloseJoystick(handle);
  profile_func_end;
  return result;
}

func b32 joystick_get_button(joystick joy_id, sz button_idx) {
  profile_func_begin;
  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return false;
  }

  if (button_idx >= (sz)SDL_GetNumJoystickButtons(handle)) {
    SDL_CloseJoystick(handle);
    profile_func_end;
    return false;
  }

  b32 result = SDL_GetJoystickButton(handle, (int)button_idx) ? true : false;
  SDL_CloseJoystick(handle);
  profile_func_end;
  return result;
}

func battery_state joystick_get_battery(joystick joy_id, i32* out_percent) {
  profile_func_begin;
  if (out_percent) {
    *out_percent = -1;
  }

  SDL_Joystick* handle = joystick_open_for_query(joy_id);
  if (handle == NULL) {
    profile_func_end;
    return BATTERY_STATE_UNKNOWN;
  }

  int native_percent = -1;
  battery_state result = joystick_battery_state_from_native((i32)SDL_GetJoystickPowerInfo(handle, &native_percent));
  devices_set_battery(joystick_to_device(joy_id), result);
  devices_update_runtime(joystick_to_device(joy_id), 1, (void*)handle);
  if (out_percent) {
    *out_percent = native_percent;
  }

  SDL_CloseJoystick(handle);
  profile_func_end;
  return result;
}

func b32 joystick_internal_on_msg(msg* src, void* user_data) {
  profile_func_begin;
  (void)user_data;
  if (src == NULL) {
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_JOYSTICK_ADDED || src->type == MSG_CORE_TYPE_JOYSTICK_REMOVED ||
      src->type == MSG_CORE_TYPE_JOYSTICK_UPDATE_COMPLETE) {
    joystick joy_id = msg_core_get_joystick_device(src)->joystick;
    b32 connected = src->type != MSG_CORE_TYPE_JOYSTICK_REMOVED;
    devices_update_runtime(joystick_to_device(joy_id), connected, connected ? (void*)(up)joystick_to_native_id(joy_id) : NULL);
  } else if (src->type == MSG_CORE_TYPE_JOYSTICK_AXIS_MOTION) {
    devices_set_focus(joystick_to_device(msg_core_get_joystick_axis(src)->joystick));
  } else if (src->type == MSG_CORE_TYPE_JOYSTICK_BALL_MOTION) {
    devices_set_focus(joystick_to_device(msg_core_get_joystick_ball(src)->joystick));
  } else if (src->type == MSG_CORE_TYPE_JOYSTICK_HAT_MOTION) {
    devices_set_focus(joystick_to_device(msg_core_get_joystick_hat(src)->joystick));
  } else if (src->type == MSG_CORE_TYPE_JOYSTICK_BUTTON_DOWN || src->type == MSG_CORE_TYPE_JOYSTICK_BUTTON_UP) {
    devices_set_focus(joystick_to_device(msg_core_get_joystick_button(src)->joystick));
  } else if (src->type == MSG_CORE_TYPE_JOYSTICK_BATTERY_UPDATED) {
    devices_set_battery(joystick_to_device(msg_core_get_joystick_battery(src)->joystick), msg_core_get_joystick_battery(src)->state);
    devices_set_focus(joystick_to_device(msg_core_get_joystick_battery(src)->joystick));
  }
  profile_func_end;
  return true;
}
