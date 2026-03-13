// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/gamepads.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

typedef struct gamepad_slot_state {
  SDL_JoystickID id;
  SDL_Gamepad* handle;
} gamepad_slot_state;

global_var gamepad_slot_state gamepad_slots[GAMEPADS_MAX_COUNT];
global_var i16 gamepad_axis_deadzone[GAMEPADS_MAX_COUNT][GAMEPAD_AXIS_COUNT] = {{0}};

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

func device gamepads_get_device(sz slot_idx) {
  profile_func_begin;
  if (!gamepads_is_connected(slot_idx)) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)gamepad_slots[slot_idx].id);
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

func b32 gamepads_get_button(sz slot_idx, gamepad_button button) {
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || button < 0) {
    return false;
  }

  return SDL_GetGamepadButton(gamepad_slots[slot_idx].handle, (SDL_GamepadButton)button) ? true : false;
}

func b32 gamepads_has_axis(sz slot_idx, gamepad_axis axis) {
  gamepads_sync_slots();

  if (slot_idx >= GAMEPADS_MAX_COUNT || !gamepad_slots[slot_idx].handle || axis < 0) {
    return false;
  }

  return SDL_GamepadHasAxis(gamepad_slots[slot_idx].handle, (SDL_GamepadAxis)axis) ? true : false;
}

func i16 gamepads_get_axis(sz slot_idx, gamepad_axis axis) {
  profile_func_begin;
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
