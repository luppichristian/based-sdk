// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/gamepads.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"

typedef struct gamepad_slot_state {
  SDL_JoystickID id;
  SDL_Gamepad* handle;
  u32 button_pressed_generation[GAMEPAD_BUTTON_COUNT];
  u32 button_released_generation[GAMEPAD_BUTTON_COUNT];
} gamepad_slot_state;

global_var gamepad_slot_state gamepad_slots[GAMEPADS_MAX_COUNT];
global_var u32 gamepad_pressed_seen[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};
global_var u32 gamepad_released_seen[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};
global_var u32 gamepad_pressed_seen_epoch[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};
global_var u32 gamepad_released_seen_epoch[INPUT_CAPTURE_MAX_KEYS][GAMEPADS_MAX_COUNT][GAMEPAD_BUTTON_COUNT] = {0};

func void gamepads_release_slot(sz slot_idx) {
  if (slot_idx >= GAMEPADS_MAX_COUNT) {
    return;
  }

  if (gamepad_slots[slot_idx].handle) {
    SDL_CloseGamepad(gamepad_slots[slot_idx].handle);
  }

  gamepad_slots[slot_idx] = (gamepad_slot_state) {0};
}

func b32 gamepads_slot_and_button_are_valid(sz slot_idx, gamepad_button button) {
  return slot_idx < GAMEPADS_MAX_COUNT && button >= 0 && button < GAMEPAD_BUTTON_COUNT;
}

func u32 gamepads_next_generation(u32 value) {
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  return result;
}

func sz gamepads_find_slot_by_instance(SDL_JoystickID joystick_id) {
  for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    if (gamepad_slots[slot_idx].handle != NULL && gamepad_slots[slot_idx].id == joystick_id) {
      return slot_idx;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func void gamepads_sync_slots(void) {
  SDL_JoystickID desired_ids[GAMEPADS_MAX_COUNT] = {0};
  SDL_JoystickID* ids = NULL;
  int count = 0;

  ids = SDL_GetGamepads(&count);
  for (sz slot_idx = 0; ids && slot_idx < GAMEPADS_MAX_COUNT && slot_idx < (sz)count; slot_idx += 1) {
    desired_ids[slot_idx] = ids[slot_idx];
  }

  for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
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
}

func sz gamepads_get_count(void) {
  gamepads_sync_slots();

  for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    if (!gamepad_slots[slot_idx].handle) {
      return slot_idx;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func b32 gamepads_is_connected(sz slot_idx) {
  if (slot_idx >= GAMEPADS_MAX_COUNT) {
    return 0;
  }
  assert(slot_idx < GAMEPADS_MAX_COUNT);
  gamepads_sync_slots();
  return gamepad_slots[slot_idx].handle != NULL;
}

func b32 gamepads_get_device_id(sz slot_idx, device_id* out_id) {
  if (out_id) {
    *out_id = (device_id) {0};
  }

  if (!gamepads_is_connected(slot_idx)) {
    return 0;
  }

  if (out_id) {
    out_id->type = DEVICE_TYPE_GAMEPAD;
    out_id->instance = (u64)gamepad_slots[slot_idx].id;
  }

  return 1;
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
    return 0;
  }
  assert(button < GAMEPAD_BUTTON_COUNT);

  return SDL_GamepadHasButton(gamepad_slots[slot_idx].handle, (SDL_GamepadButton)button) ? 1 : 0;
}

func b32 gamepads_get_button(input_key key, sz slot_idx, gamepad_button button) {
  (void)key;
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || button < 0) {
    return 0;
  }

  return SDL_GetGamepadButton(gamepad_slots[slot_idx].handle, (SDL_GamepadButton)button) ? 1 : 0;
}

func b32 gamepads_is_button_pressed(input_key key, sz slot_idx, gamepad_button button) {
  if (!gamepads_slot_and_button_are_valid(slot_idx, button) || !gamepads_is_connected(slot_idx)) {
    return 0;
  }

  sz key_slot_idx = input_capture_get_slot(key);
  if (key_slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_idx);
  if (gamepad_pressed_seen_epoch[key_slot_idx][slot_idx][button] != slot_epoch) {
    gamepad_pressed_seen_epoch[key_slot_idx][slot_idx][button] = slot_epoch;
    gamepad_pressed_seen[key_slot_idx][slot_idx][button] = gamepad_slots[slot_idx].button_pressed_generation[button];
    return 0;
  }

  u32 generation = gamepad_slots[slot_idx].button_pressed_generation[button];
  if (generation == 0 || gamepad_pressed_seen[key_slot_idx][slot_idx][button] == generation) {
    return 0;
  }

  gamepad_pressed_seen[key_slot_idx][slot_idx][button] = generation;
  return 1;
}

func b32 gamepads_is_button_released(input_key key, sz slot_idx, gamepad_button button) {
  if (!gamepads_slot_and_button_are_valid(slot_idx, button) || !gamepads_is_connected(slot_idx)) {
    return 0;
  }

  sz key_slot_idx = input_capture_get_slot(key);
  if (key_slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_idx);
  if (gamepad_released_seen_epoch[key_slot_idx][slot_idx][button] != slot_epoch) {
    gamepad_released_seen_epoch[key_slot_idx][slot_idx][button] = slot_epoch;
    gamepad_released_seen[key_slot_idx][slot_idx][button] =
        gamepad_slots[slot_idx].button_released_generation[button];
    return 0;
  }

  u32 generation = gamepad_slots[slot_idx].button_released_generation[button];
  if (generation == 0 || gamepad_released_seen[key_slot_idx][slot_idx][button] == generation) {
    return 0;
  }

  gamepad_released_seen[key_slot_idx][slot_idx][button] = generation;
  return 1;
}

func b32 gamepads_has_axis(sz slot_idx, gamepad_axis axis) {
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || axis < 0) {
    return 0;
  }

  return SDL_GamepadHasAxis(gamepad_slots[slot_idx].handle, (SDL_GamepadAxis)axis) ? 1 : 0;
}

func i16 gamepads_get_axis(input_key key, sz slot_idx, gamepad_axis axis) {
  (void)key;
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || axis < 0) {
    return 0;
  }
  assert(axis < GAMEPAD_AXIS_COUNT);

  return (i16)SDL_GetGamepadAxis(gamepad_slots[slot_idx].handle, (SDL_GamepadAxis)axis);
}

func void gamepads_internal_on_msg(msg* src) {
  if (src == NULL || (src->type != MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN && src->type != MSG_CORE_TYPE_GAMEPAD_BUTTON_UP)) {
    return;
  }

  gamepads_sync_slots();

  if (msg_core_get_gamepad_button(src)->button < 0 || msg_core_get_gamepad_button(src)->button >= GAMEPAD_BUTTON_COUNT) {
    return;
  }

  sz slot_idx = gamepads_find_slot_by_instance((SDL_JoystickID)msg_core_get_gamepad_button(src)->device.instance);
  if (slot_idx >= GAMEPADS_MAX_COUNT) {
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
}
