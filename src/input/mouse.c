// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/mouse.h"
#include "../sdl3_include.h"

func mouse_state mouse_make_state(SDL_MouseButtonFlags buttons, f32 xpos, f32 ypos) {
  mouse_state result = {0};
  result.x = xpos;
  result.y = ypos;
  result.button_mask = (u32)buttons;
  return result;
}

func b32 mouse_is_available(void) {
  return SDL_HasMouse() ? 1 : 0;
}

func b32 mouse_get_primary_device_id(device_id* out_id) {
  return devices_get_device(DEVICE_TYPE_MOUSE, 0, out_id);
}

func mouse_state mouse_get_state(void) {
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetMouseState(&xpos, &ypos), xpos, ypos);
}

func mouse_state mouse_get_global_state(void) {
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetGlobalMouseState(&xpos, &ypos), xpos, ypos);
}

func mouse_state mouse_get_relative_state(void) {
  f32 xpos = 0.0f;
  f32 ypos = 0.0f;
  return mouse_make_state(SDL_GetRelativeMouseState(&xpos, &ypos), xpos, ypos);
}

func b32 mouse_is_button_down(u8 button) {
  u32 mask = mouse_get_state().button_mask;
  if (!button) {
    return 0;
  }

  return (mask & (1u << (button - 1))) != 0 ? 1 : 0;
}