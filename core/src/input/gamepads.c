// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/gamepads.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

typedef struct gamepad_slot_state {
  SDL_JoystickID id;
  SDL_Gamepad* handle;
  u32 button_pressed_generation[GAMEPAD_BUTTON_COUNT];
  u32 button_released_generation[GAMEPAD_BUTTON_COUNT];
} gamepad_slot_state;

global_var gamepad_slot_state gamepad_slots[GAMEPADS_MAX_COUNT];
global_var i16 gamepad_axis_deadzone[GAMEPADS_MAX_COUNT][GAMEPAD_AXIS_COUNT] = {{0}};
global_var u32 gamepad_pressed_seen[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};
global_var u32 gamepad_released_seen[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};
global_var u32 gamepad_pressed_seen_epoch[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};
global_var u32 gamepad_released_seen_epoch[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};

func void gamepads_release_slot(sz slot_idx) {
  profile_func_begin;
  if (slot_idx >= GAMEPADS_MAX_COUNT) {
    profile_func_end;
    return;
  }

  if (gamepad_slots[slot_idx].handle) {
    SDL_CloseGamepad(gamepad_slots[slot_idx].handle);
  }

  gamepad_slots[slot_idx] = (gamepad_slot_state) {0};
  profile_func_end;
}

func b32 gamepads_slot_and_button_are_valid(sz slot_idx, gamepad_button button) {
  return slot_idx < GAMEPADS_MAX_COUNT && button >= 0 && button < GAMEPAD_BUTTON_COUNT;
}

func u32 gamepads_next_generation(u32 value) {
  profile_func_begin;
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  profile_func_end;
  return result;
}

func sz gamepads_find_slot_by_instance(SDL_JoystickID joystick_id) {
  profile_func_begin;
  safe_for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    if (gamepad_slots[slot_idx].handle != NULL && gamepad_slots[slot_idx].id == joystick_id) {
      profile_func_end;
      return slot_idx;
    }
  }

  profile_func_end;
  return GAMEPADS_MAX_COUNT;
}

func void gamepads_sync_slots(void) {
  profile_func_begin;
  SDL_JoystickID desired_ids[GAMEPADS_MAX_COUNT] = {0};
  SDL_JoystickID* ids = NULL;
  int count = 0;

  ids = SDL_GetGamepads(&count);
  safe_for (sz slot_idx = 0; ids && slot_idx < GAMEPADS_MAX_COUNT && slot_idx < (sz)count; slot_idx += 1) {
    desired_ids[slot_idx] = ids[slot_idx];
  }

  safe_for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    if (!desired_ids[slot_idx]) {
      gamepads_release_slot(slot_idx);
      continue;
    }

    if (gamepad_slots[slot_idx].id == desired_ids[slot_idx] && gamepad_slots[slot_idx].handle) {
      continue;
    }

    gamepads_release_slot(slot_idx);
    gamepad_slots[slot_idx].handle = SDL_OpenGamepad(desired_ids[slot_idx]);
    if (gamepad_slots[slot_idx].handle) {
      gamepad_slots[slot_idx].id = desired_ids[slot_idx];
    }
  }

  if (ids) {
    SDL_free(ids);
  }
  profile_func_end;
}

func sz gamepads_get_count(void) {
  gamepads_sync_slots();

  safe_for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    if (!gamepad_slots[slot_idx].handle) {
          return slot_idx;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func b32 gamepads_is_connected(sz slot_idx) {
  if (slot_idx >= GAMEPADS_MAX_COUNT) {
      return false;
  }
  assert(slot_idx < GAMEPADS_MAX_COUNT);
  gamepads_sync_slots();
  return gamepad_slots[slot_idx].handle != NULL;
}

func b32 gamepads_get_device_id(sz slot_idx, device_id* out_id) {
  profile_func_begin;
  if (out_id) {
    *out_id = (device_id) {0};
  }

  if (!gamepads_is_connected(slot_idx)) {
    profile_func_end;
    return false;
  }

  if (out_id) {
    out_id->type = DEVICE_TYPE_GAMEPAD;
    out_id->instance = (u64)gamepad_slots[slot_idx].id;
  }

  profile_func_end;
  return true;
}

func cstr8 gamepads_get_name(sz slot_idx) {
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle) {
      return NULL;
  }

  return SDL_GetGamepadName(gamepad_slots[slot_idx].handle);
}

func b32 gamepads_has_button(sz slot_idx, gamepad_button button) {
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || button < 0) {
      return false;
  }
  assert(button < GAMEPAD_BUTTON_COUNT);

  return SDL_GamepadHasButton(gamepad_slots[slot_idx].handle, (SDL_GamepadButton)button) ? true : false;
}

func b32 gamepads_get_button(input_key key, sz slot_idx, gamepad_button button) {
  (void)key;
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || button < 0) {
      return false;
  }

  return SDL_GetGamepadButton(gamepad_slots[slot_idx].handle, (SDL_GamepadButton)button) ? true : false;
}

func b32 gamepads_is_button_pressed(input_key key, sz slot_idx, gamepad_button button) {
  profile_func_begin;
  if (!gamepads_slot_and_button_are_valid(slot_idx, button) || !gamepads_is_connected(slot_idx)) {
    profile_func_end;
    return false;
  }

  sz key_slot_idx = input_capture_get_slot(key);
  if (key_slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_idx);
  if (gamepad_pressed_seen_epoch[key_slot_idx][slot_idx][button] != slot_epoch) {
    gamepad_pressed_seen_epoch[key_slot_idx][slot_idx][button] = slot_epoch;
    gamepad_pressed_seen[key_slot_idx][slot_idx][button] = gamepad_slots[slot_idx].button_pressed_generation[button];
    profile_func_end;
    return false;
  }

  u32 generation = gamepad_slots[slot_idx].button_pressed_generation[button];
  if (generation == 0 || gamepad_pressed_seen[key_slot_idx][slot_idx][button] == generation) {
    profile_func_end;
    return false;
  }

  gamepad_pressed_seen[key_slot_idx][slot_idx][button] = generation;
  profile_func_end;
  return true;
}

func b32 gamepads_is_button_released(input_key key, sz slot_idx, gamepad_button button) {
  profile_func_begin;
  if (!gamepads_slot_and_button_are_valid(slot_idx, button) || !gamepads_is_connected(slot_idx)) {
    profile_func_end;
    return false;
  }

  sz key_slot_idx = input_capture_get_slot(key);
  if (key_slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_idx);
  if (gamepad_released_seen_epoch[key_slot_idx][slot_idx][button] != slot_epoch) {
    gamepad_released_seen_epoch[key_slot_idx][slot_idx][button] = slot_epoch;
    gamepad_released_seen[key_slot_idx][slot_idx][button] =
        gamepad_slots[slot_idx].button_released_generation[button];
    profile_func_end;
    return false;
  }

  u32 generation = gamepad_slots[slot_idx].button_released_generation[button];
  if (generation == 0 || gamepad_released_seen[key_slot_idx][slot_idx][button] == generation) {
    profile_func_end;
    return false;
  }

  gamepad_released_seen[key_slot_idx][slot_idx][button] = generation;
  profile_func_end;
  return true;
}

func b32 gamepads_has_axis(sz slot_idx, gamepad_axis axis) {
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || axis < 0) {
      return false;
  }

  return SDL_GamepadHasAxis(gamepad_slots[slot_idx].handle, (SDL_GamepadAxis)axis) ? true : false;
}

func i16 gamepads_get_axis(input_key key, sz slot_idx, gamepad_axis axis) {
  profile_func_begin;
  (void)key;
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || axis < 0) {
    profile_func_end;
    return 0;
  }
  assert(axis < GAMEPAD_AXIS_COUNT);

  i16 value = (i16)SDL_GetGamepadAxis(gamepad_slots[slot_idx].handle, (SDL_GamepadAxis)axis);
  i16 deadzone = gamepad_axis_deadzone[slot_idx][axis];
  if (value < deadzone && value > -deadzone) {
    value = 0;
  }
  profile_func_end;
  return value;
}

func b32 gamepads_set_rumble(sz slot_idx, u16 low_freq, u16 high_freq, u32 duration_ms) {
  profile_func_begin;
  gamepads_sync_slots();
  if (slot_idx >= GAMEPADS_MAX_COUNT || gamepad_slots[slot_idx].handle == NULL) {
    profile_func_end;
    return false;
  }
  b32 result = SDL_RumbleGamepad(gamepad_slots[slot_idx].handle, low_freq, high_freq, duration_ms);
  profile_func_end;
  return result;
}

func b32 gamepads_set_led(sz slot_idx, u8 red, u8 green, u8 blue) {
  profile_func_begin;
  gamepads_sync_slots();
  if (slot_idx >= GAMEPADS_MAX_COUNT || gamepad_slots[slot_idx].handle == NULL) {
    profile_func_end;
    return false;
  }
  b32 result = SDL_SetGamepadLED(gamepad_slots[slot_idx].handle, red, green, blue);
  profile_func_end;
  return result;
}

func b32 gamepads_set_axis_deadzone(sz slot_idx, gamepad_axis axis, i16 deadzone) {
  profile_func_begin;
  if (slot_idx >= GAMEPADS_MAX_COUNT || axis < 0 || axis >= GAMEPAD_AXIS_COUNT) {
    profile_func_end;
    return false;
  }
  gamepad_axis_deadzone[slot_idx][axis] = deadzone < 0 ? -deadzone : deadzone;
  profile_func_end;
  return true;
}

func i16 gamepads_get_axis_deadzone(sz slot_idx, gamepad_axis axis) {
  profile_func_begin;
  if (slot_idx >= GAMEPADS_MAX_COUNT || axis < 0 || axis >= GAMEPAD_AXIS_COUNT) {
    profile_func_end;
    return 0;
  }
  i16 value = gamepad_axis_deadzone[slot_idx][axis];
  profile_func_end;
  return value;
}

func void gamepads_internal_on_msg(msg* src) {
  profile_func_begin;
  if (src == NULL || (src->type != MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN && src->type != MSG_CORE_TYPE_GAMEPAD_BUTTON_UP)) {
    profile_func_end;
    return;
  }

  gamepads_sync_slots();

  if (msg_core_get_gamepad_button(src)->button < 0 || msg_core_get_gamepad_button(src)->button >= GAMEPAD_BUTTON_COUNT) {
    profile_func_end;
    return;
  }

  sz slot_idx = gamepads_find_slot_by_instance((SDL_JoystickID)msg_core_get_gamepad_button(src)->device.instance);
  if (slot_idx >= GAMEPADS_MAX_COUNT) {
    profile_func_end;
    return;
  }

  gamepad_button button = msg_core_get_gamepad_button(src)->button;
  if (src->type == MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN) {
    gamepad_slots[slot_idx].button_pressed_generation[button] =
        gamepads_next_generation(gamepad_slots[slot_idx].button_pressed_generation[button]);
  } else {
    gamepad_slots[slot_idx].button_released_generation[button] =
        gamepads_next_generation(gamepad_slots[slot_idx].button_released_generation[button]);
  }
  profile_func_end;
}
