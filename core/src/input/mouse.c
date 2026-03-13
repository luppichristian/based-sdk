// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/mouse.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 mouse_button_is_valid(mouse_button button) {
  return button >= 0 && button < MOUSE_BUTTON_COUNT;
}

func mouse_state mouse_make_state(SDL_MouseButtonFlags buttons) {
  profile_func_begin;
  mouse_state result = 0;
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0) {
    result |= bit(MOUSE_BUTTON_LEFT);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE)) != 0) {
    result |= bit(MOUSE_BUTTON_MIDDLE);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
    result |= bit(MOUSE_BUTTON_RIGHT);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_X1)) != 0) {
    result |= bit(MOUSE_BUTTON_X1);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_X2)) != 0) {
    result |= bit(MOUSE_BUTTON_X2);
  }
  profile_func_end;
  return result;
}

func b32 mouse_is_available(void) {
  return SDL_HasMouse() ? true : false;
}

func device mouse_get_primary_device(void) {
  return devices_get_device(DEVICE_TYPE_MOUSE, 0);
}

func mouse_state mouse_get_state(void) {
  return mouse_make_state(SDL_GetMouseState(NULL, NULL));
}

func b32 mouse_is_button_down(mouse_button button) {
  profile_func_begin;
  mouse_state state = mouse_get_state();
  if (!mouse_button_is_valid(button)) {
    profile_func_end;
    return false;
  }
  assert(mouse_button_is_valid(button));

  profile_func_end;
  return (state & bit(button)) != 0 ? true : false;
}
