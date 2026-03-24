// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/gamepads.h"
#include "basic/assert.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

typedef struct gamepad_state_entry {
  gamepad gamepad;
  SDL_JoystickID id;
  SDL_Gamepad* handle;
  i16 axis_deadzone[GAMEPAD_AXIS_COUNT];
  i16 axis_state[GAMEPAD_AXIS_COUNT];
  b32 button_state[GAMEPAD_BUTTON_COUNT];
} gamepad_state_entry;

global_var gamepad_state_entry gamepad_states[DEVICES_HANDLE_CAP] = {0};

func gamepad_state_entry* gamepad_find_state(gamepad src, b32 create_if_missing) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (gamepad_states[item_idx].gamepad == src) {
      profile_func_end;
      return &gamepad_states[item_idx];
    }
  }

  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (gamepad_states[item_idx].gamepad == NULL) {
      gamepad_states[item_idx].gamepad = src;
      gamepad_states[item_idx].id = (SDL_JoystickID)devices_get_instance(gamepad_to_device(src));
      profile_func_end;
      return &gamepad_states[item_idx];
    }
  }

  thread_log_error("Gamepad state table is full");
  profile_func_end;
  return NULL;
}

func void gamepad_sync_handle(gamepad_state_entry* state) {
  profile_func_begin;
  if (state == NULL) {
    profile_func_end;
    return;
  }

  if (state->handle == NULL && state->id != 0) {
    state->handle = SDL_OpenGamepad(state->id);
    devices_update_runtime(gamepad_to_device(state->gamepad), state->handle != NULL, state->handle ? (void*)state->handle : (void*)(up)state->id);
  }
  profile_func_end;
}

func void gamepad_release_state(gamepad_state_entry* state) {
  profile_func_begin;
  if (state == NULL) {
    profile_func_end;
    return;
  }

  if (state->handle != NULL) {
    SDL_CloseGamepad(state->handle);
  }
  *state = (gamepad_state_entry) {0};
  profile_func_end;
}

func b32 gamepad_is_valid(gamepad src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_GAMEPAD;
}

func gamepad device_get_gamepad(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_GAMEPAD) {
    invalid_code_path;
    return NULL;
  }

  return (gamepad)src;
}

func device gamepad_to_device(gamepad src) {
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func sz gamepad_get_total_count(void) {
  return devices_get_count(DEVICE_TYPE_GAMEPAD);
}

func gamepad gamepad_get_from_idx(sz idx) {
  device src = devices_get_device(DEVICE_TYPE_GAMEPAD, idx);
  return device_is_valid(src) ? device_get_gamepad(src) : NULL;
}

func gamepad gamepad_get_primary(void) {
  return gamepad_get_from_idx(0);
}

func gamepad gamepad_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_GAMEPAD);
  return device_is_valid(src) ? device_get_gamepad(src) : NULL;
}

func b32 gamepad_is_connected(gamepad src) {
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    return false;
  }

  return devices_is_connected(gamepad_to_device(src));
}

func cstr8 gamepad_get_name(gamepad src) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return NULL;
  }

  cstr8 result = SDL_GetGamepadNameForID((SDL_JoystickID)devices_get_instance(gamepad_to_device(src)));
  profile_func_end;
  return result;
}

func b32 gamepad_has_button(gamepad src, gamepad_button button) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  if (state == NULL || button < 0 || button >= GAMEPAD_BUTTON_COUNT) {
    profile_func_end;
    return false;
  }

  gamepad_sync_handle(state);
  if (state->handle == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_GamepadHasButton(state->handle, (SDL_GamepadButton)button) ? true : false;
  profile_func_end;
  return result;
}

func b32 gamepad_get_button(gamepad src, gamepad_button button) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  if (state == NULL || button < 0 || button >= GAMEPAD_BUTTON_COUNT) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return state->button_state[button];
}

func b32 gamepad_has_axis(gamepad src, gamepad_axis axis) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  if (state == NULL || axis < 0 || axis >= GAMEPAD_AXIS_COUNT) {
    profile_func_end;
    return false;
  }

  gamepad_sync_handle(state);
  if (state->handle == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_GamepadHasAxis(state->handle, (SDL_GamepadAxis)axis) ? true : false;
  profile_func_end;
  return result;
}

func i16 gamepad_get_axis(gamepad src, gamepad_axis axis) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return 0;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  if (state == NULL || axis < 0 || axis >= GAMEPAD_AXIS_COUNT) {
    profile_func_end;
    return 0;
  }

  i16 value = state->axis_state[axis];
  i16 deadzone = state->axis_deadzone[axis];
  if (value < deadzone && value > -deadzone) {
    value = 0;
  }
  profile_func_end;
  return value;
}

func b32 gamepad_set_rumble(gamepad src, u16 low_freq, u16 high_freq, u32 duration_ms) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  gamepad_sync_handle(state);
  if (state == NULL || state->handle == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_RumbleGamepad(state->handle, low_freq, high_freq, duration_ms);
  profile_func_end;
  return result;
}

func b32 gamepad_set_led(gamepad src, u8 red, u8 green, u8 blue) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  gamepad_sync_handle(state);
  if (state == NULL || state->handle == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetGamepadLED(state->handle, red, green, blue);
  profile_func_end;
  return result;
}

func b32 gamepad_set_axis_deadzone(gamepad src, gamepad_axis axis, i16 deadzone) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  if (state == NULL || axis < 0 || axis >= GAMEPAD_AXIS_COUNT) {
    profile_func_end;
    return false;
  }

  state->axis_deadzone[axis] = deadzone < 0 ? -deadzone : deadzone;
  profile_func_end;
  return true;
}

func i16 gamepad_get_axis_deadzone(gamepad src, gamepad_axis axis) {
  profile_func_begin;
  if (!gamepad_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return 0;
  }

  gamepad_state_entry* state = gamepad_find_state(src, 1);
  if (state == NULL || axis < 0 || axis >= GAMEPAD_AXIS_COUNT) {
    profile_func_end;
    return 0;
  }

  i16 value = state->axis_deadzone[axis];
  profile_func_end;
  return value;
}

func b32 gamepad_internal_on_msg(msg* src, void* user_data) {
  profile_func_begin;
  (void)user_data;
  if (src == NULL) {
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_GAMEPAD_ADDED || src->type == MSG_CORE_TYPE_GAMEPAD_REMOVED ||
      src->type == MSG_CORE_TYPE_GAMEPAD_REMAPPED || src->type == MSG_CORE_TYPE_GAMEPAD_UPDATE_COMPLETE ||
      src->type == MSG_CORE_TYPE_GAMEPAD_STEAM_HANDLE_UPDATED) {
    gamepad pad = device_get_gamepad(msg_core_get_gamepad_device(src)->device);
    gamepad_state_entry* state = gamepad_find_state(pad, 1);
    b32 connected = src->type != MSG_CORE_TYPE_GAMEPAD_REMOVED;
    if (state != NULL) {
      state->id = (SDL_JoystickID)devices_get_instance(gamepad_to_device(pad));
      if (!connected) {
        gamepad_release_state(state);
      }
    }
    devices_update_runtime(gamepad_to_device(pad), connected, connected ? (void*)(up)devices_get_instance(gamepad_to_device(pad)) : NULL);
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_GAMEPAD_AXIS_MOTION) {
    gamepad pad = device_get_gamepad(msg_core_get_gamepad_axis(src)->device);
    gamepad_state_entry* state = gamepad_find_state(pad, 1);
    if (state != NULL && msg_core_get_gamepad_axis(src)->axis >= 0 && msg_core_get_gamepad_axis(src)->axis < GAMEPAD_AXIS_COUNT) {
      state->axis_state[msg_core_get_gamepad_axis(src)->axis] = msg_core_get_gamepad_axis(src)->value;
    }
    devices_update_runtime(gamepad_to_device(pad), 1, (void*)(up)devices_get_instance(gamepad_to_device(pad)));
    devices_set_focus(gamepad_to_device(pad));
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN || src->type == MSG_CORE_TYPE_GAMEPAD_BUTTON_UP) {
    gamepad pad = device_get_gamepad(msg_core_get_gamepad_button(src)->device);
    gamepad_state_entry* state = gamepad_find_state(pad, 1);
    if (state != NULL && msg_core_get_gamepad_button(src)->button >= 0 &&
        msg_core_get_gamepad_button(src)->button < GAMEPAD_BUTTON_COUNT) {
      state->button_state[msg_core_get_gamepad_button(src)->button] = msg_core_get_gamepad_button(src)->down;
    }
    devices_update_runtime(gamepad_to_device(pad), 1, (void*)(up)devices_get_instance(gamepad_to_device(pad)));
    devices_set_focus(gamepad_to_device(pad));
    profile_func_end;
    return true;
  }

  if ((src->type >= MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_DOWN && src->type <= MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_UP) ||
      src->type == MSG_CORE_TYPE_GAMEPAD_SENSOR_UPDATE) {
    gamepad pad = (src->type >= MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_DOWN && src->type <= MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_UP)
                      ? device_get_gamepad(msg_core_get_gamepad_touchpad(src)->device)
                      : device_get_gamepad(msg_core_get_gamepad_sensor(src)->device);
    devices_update_runtime(gamepad_to_device(pad), 1, (void*)(up)devices_get_instance(gamepad_to_device(pad)));
    devices_set_focus(gamepad_to_device(pad));
  }
  profile_func_end;
  return true;
}
