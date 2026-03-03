// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/gamepads.h"
#include "../sdl3_include.h"

typedef struct gamepad_slot_state {
  SDL_JoystickID id;
  SDL_Gamepad* handle;
} gamepad_slot_state;

global_var gamepad_slot_state gamepad_slots[GAMEPADS_MAX_COUNT];

func void gamepads_release_slot(sz slot_index) {
  if (slot_index >= GAMEPADS_MAX_COUNT) {
    return;
  }

  if (gamepad_slots[slot_index].handle) {
    SDL_CloseGamepad(gamepad_slots[slot_index].handle);
  }

  gamepad_slots[slot_index] = (gamepad_slot_state){0};
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

func b32 gamepads_get_device_id(sz slot_index, input_device_id* out_id) {
  if (out_id) {
    *out_id = (input_device_id){0};
  }

  if (!gamepads_is_connected(slot_index)) {
    return 0;
  }

  if (out_id) {
    out_id->type = INPUT_DEVICE_TYPE_GAMEPAD;
    out_id->instance = (u64)gamepad_slots[slot_index].id;
  }

  return 1;
}

func const c8* gamepads_get_name(sz slot_index) {
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

func b32 gamepads_get_button(sz slot_index, gamepad_button button) {
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || button < 0) {
    return 0;
  }

  return SDL_GetGamepadButton(gamepad_slots[slot_index].handle, (SDL_GamepadButton)button) ? 1 : 0;
}

func b32 gamepads_has_axis(sz slot_index, gamepad_axis axis) {
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || axis < 0) {
    return 0;
  }

  return SDL_GamepadHasAxis(gamepad_slots[slot_index].handle, (SDL_GamepadAxis)axis) ? 1 : 0;
}

func i16 gamepads_get_axis(sz slot_index, gamepad_axis axis) {
  gamepads_sync_slots();

  if (slot_index >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_index].handle || axis < 0) {
    return 0;
  }

  return (i16)SDL_GetGamepadAxis(gamepad_slots[slot_index].handle, (SDL_GamepadAxis)axis);
}
