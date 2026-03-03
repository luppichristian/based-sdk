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

func b32 mouse_get_primary_device_id(input_device_id* out_id) {
  return devices_get_device(INPUT_DEVICE_TYPE_MOUSE, 0, out_id);
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

func b32 mouse_window_to_normalized(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y) {
  int width = 0;
  int height = 0;

  if (!window_handle || !out_x || !out_y || !SDL_GetWindowSize((SDL_Window*)window_handle, &width, &height) || width <= 0 || height <= 0) {
    return 0;
  }

  *out_x = src_x / (f32)width;
  *out_y = src_y / (f32)height;
  return 1;
}

func b32 mouse_normalized_to_window(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y) {
  int width = 0;
  int height = 0;

  if (!window_handle || !out_x || !out_y || !SDL_GetWindowSize((SDL_Window*)window_handle, &width, &height) || width <= 0 || height <= 0) {
    return 0;
  }

  *out_x = src_x * (f32)width;
  *out_y = src_y * (f32)height;
  return 1;
}

func b32 mouse_window_to_pixels(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y) {
  int width = 0;
  int height = 0;
  int pixel_width = 0;
  int pixel_height = 0;

  if (!window_handle || !out_x || !out_y || !SDL_GetWindowSize((SDL_Window*)window_handle, &width, &height) ||
      !SDL_GetWindowSizeInPixels((SDL_Window*)window_handle, &pixel_width, &pixel_height) || width <= 0 || height <= 0) {
    return 0;
  }

  *out_x = src_x * ((f32)pixel_width / (f32)width);
  *out_y = src_y * ((f32)pixel_height / (f32)height);
  return 1;
}

func b32 mouse_pixels_to_window(void* window_handle, f32 src_x, f32 src_y, f32* out_x, f32* out_y) {
  int width = 0;
  int height = 0;
  int pixel_width = 0;
  int pixel_height = 0;

  if (!window_handle || !out_x || !out_y || !SDL_GetWindowSize((SDL_Window*)window_handle, &width, &height) ||
      !SDL_GetWindowSizeInPixels((SDL_Window*)window_handle, &pixel_width, &pixel_height) || pixel_width <= 0 || pixel_height <= 0) {
    return 0;
  }

  *out_x = src_x * ((f32)width / (f32)pixel_width);
  *out_y = src_y * ((f32)height / (f32)pixel_height);
  return 1;
}
