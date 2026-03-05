// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/gamepads.h"
#include "input/msg.h"
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

func void gamepads_release_slot(sz slot_index) {
  if (slot_index >= GAMEPADS_MAX_COUNT) {
    return;
  }

  if (gamepad_slots[slot_index].handle) {
    SDL_CloseGamepad(gamepad_slots[slot_index].handle);
  }

  gamepad_slots[slot_index] = (gamepad_slot_state) {0};
}

func b32 gamepads_slot_and_button_are_valid(sz slot_index, gamepad_button button) {
  return slot_index < GAMEPADS_MAX_COUNT && button >= 0 && button < GAMEPAD_BUTTON_COUNT;
}

func u32 gamepads_next_generation(u32 value) {
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  return result;
}

func sz gamepads_find_slot_by_instance(SDL_JoystickID joystick_id) {
  for (sz slot_index = 0; slot_index < GAMEPADS_MAX_COUNT; slot_index += 1) {
    if (gamepad_slots[slot_index].handle != NULL && gamepad_slots[slot_index].id == joystick_id) {
      return slot_index;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func void gamepads_sync_slots(void) {
  SDL_JoystickID desired_ids[GAMEPADS_MAX_COUNT] = {0};
  SDL_JoystickID* ids = NULL;
  int count = 0;

  ids = SDL_GetGamepads(&count);
  for (sz slot_index = 0; ids && slot_index < GAMEPADS_MAX_COUNT && slot_index < (sz)count; slot_index += 1) {
    desired_ids[slot_index] = ids[slot_index];
  }

  for (sz slot_index = 0; slot_index < GAMEPADS_MAX_COUNT; slot_index += 1) {
    if (!desired_ids[slot_index]) {
      gamepads_release_slot(slot_index);
      continue;
    }

    if (gamepad_slots[slot_index].id == desired_ids[slot_index] && gamepad_slots[slot_index].handle) {
      continue;
    }

    gamepads_release_slot(slot_index);
    gamepad_slots[slot_index].handle = SDL_OpenGamepad(desired_ids[slot_index]);
    if (gamepad_slots[slot_index].handle) {
      gamepad_slots[slot_index].id = desired_ids[slot_index];
    }
  }

  if (ids) {
    SDL_free(ids);
  }
}

func sz gamepads_get_count(void) {
  gamepads_sync_slots();

  for (sz slot_index = 0; slot_index < GAMEPADS_MAX_COUNT; slot_index += 1) {
    if (!gamepad_slots[slot_index].handle) {
      return slot_index;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func b32 gamepads_is_connected(sz slot_index) {
  gamepads_sync_slots();
  return slot_index < GAMEPADS_MAX_COUNT && gamepad_slots[slot_index].handle != NULL;
}

func b32 gamepads_get_device_id(sz slot_index, device_id* out_id) {
  if (out_id) {
    *out_id = (device_id) {0};
  }

  if (!gamepads_is_connected(slot_index)) {
    return 0;
  }

  if (out_id) {
    out_id->type = DEVICE_TYPE_GAMEPAD;
    out_id->instance = (u64)gamepad_slots[slot_index].id;
  }

  return 1;
}

func cstr8 gamepads_get_name(sz slot_index) {
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle) {
    return NULL;
  }

  return SDL_GetGamepadName(gamepad_slots[slot_index].handle);
}

func b32 gamepads_has_button(sz slot_index, gamepad_button button) {
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || button < 0) {
    return 0;
  }

  return SDL_GamepadHasButton(gamepad_slots[slot_index].handle, (SDL_GamepadButton)button) ? 1 : 0;
}

func b32 gamepads_get_button(input_key key, sz slot_index, gamepad_button button) {
  (void)key;
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || button < 0) {
    return 0;
  }

  return SDL_GetGamepadButton(gamepad_slots[slot_index].handle, (SDL_GamepadButton)button) ? 1 : 0;
}

func b32 gamepads_is_button_pressed(input_key key, sz slot_index, gamepad_button button) {
  if (!gamepads_slot_and_button_are_valid(slot_index, button) || !gamepads_is_connected(slot_index)) {
    return 0;
  }

  sz key_slot_index = input_capture_get_slot(key);
  if (key_slot_index >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_index);
  if (gamepad_pressed_seen_epoch[key_slot_index][slot_index][button] != slot_epoch) {
    gamepad_pressed_seen_epoch[key_slot_index][slot_index][button] = slot_epoch;
    gamepad_pressed_seen[key_slot_index][slot_index][button] = gamepad_slots[slot_index].button_pressed_generation[button];
    return 0;
  }

  u32 generation = gamepad_slots[slot_index].button_pressed_generation[button];
  if (generation == 0 || gamepad_pressed_seen[key_slot_index][slot_index][button] == generation) {
    return 0;
  }

  gamepad_pressed_seen[key_slot_index][slot_index][button] = generation;
  return 1;
}

func b32 gamepads_is_button_released(input_key key, sz slot_index, gamepad_button button) {
  if (!gamepads_slot_and_button_are_valid(slot_index, button) || !gamepads_is_connected(slot_index)) {
    return 0;
  }

  sz key_slot_index = input_capture_get_slot(key);
  if (key_slot_index >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_index);
  if (gamepad_released_seen_epoch[key_slot_index][slot_index][button] != slot_epoch) {
    gamepad_released_seen_epoch[key_slot_index][slot_index][button] = slot_epoch;
    gamepad_released_seen[key_slot_index][slot_index][button] =
      gamepad_slots[slot_index].button_released_generation[button];
    return 0;
  }

  u32 generation = gamepad_slots[slot_index].button_released_generation[button];
  if (generation == 0 || gamepad_released_seen[key_slot_index][slot_index][button] == generation) {
    return 0;
  }

  gamepad_released_seen[key_slot_index][slot_index][button] = generation;
  return 1;
}

func b32 gamepads_has_axis(sz slot_index, gamepad_axis axis) {
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || axis < 0) {
    return 0;
  }

  return SDL_GamepadHasAxis(gamepad_slots[slot_index].handle, (SDL_GamepadAxis)axis) ? 1 : 0;
}

func i16 gamepads_get_axis(input_key key, sz slot_index, gamepad_axis axis) {
  (void)key;
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || axis < 0) {
    return 0;
  }

  return (i16)SDL_GetGamepadAxis(gamepad_slots[slot_index].handle, (SDL_GamepadAxis)axis);
}

func void gamepads_internal_on_msg(const msg* src) {
  if (src == NULL || (src->type != MSG_TYPE_GAMEPAD_BUTTON_DOWN && src->type != MSG_TYPE_GAMEPAD_BUTTON_UP)) {
    return;
  }

  gamepads_sync_slots();

  if (src->gamepad_button.button < 0 || src->gamepad_button.button >= GAMEPAD_BUTTON_COUNT) {
    return;
  }

  sz slot_index = gamepads_find_slot_by_instance((SDL_JoystickID)src->gamepad_button.device.instance);
  if (slot_index >= GAMEPADS_MAX_COUNT) {
    return;
  }

  gamepad_button button = src->gamepad_button.button;
  if (src->type == MSG_TYPE_GAMEPAD_BUTTON_DOWN) {
    gamepad_slots[slot_index].button_pressed_generation[button] =
      gamepads_next_generation(gamepad_slots[slot_index].button_pressed_generation[button]);
  } else {
    gamepad_slots[slot_index].button_released_generation[button] =
      gamepads_next_generation(gamepad_slots[slot_index].button_released_generation[button]);
  }
}
