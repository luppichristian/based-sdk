// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/window.h"
#include "../sdl3_include.h"

func b32 window_id_is_valid(window_id src) {
  return src.opaque != 0;
}

func window_id window_from_native_id(u64 native_id) {
  window_id result = {0};
  result.opaque = native_id;
  return result;
}

func u64 window_to_native_id(window_id src) {
  return src.opaque;
}

func sz window_get_count(void) {
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);

  if (windows) {
    SDL_free(windows);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 window_get_id(sz index, window_id* out_id) {
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);
  b32 found = windows != NULL && index < (sz)count && windows[index] != NULL;

  if (out_id) {
    *out_id = (window_id) {0};
  }

  if (found && out_id) {
    *out_id = window_from_native_id((u64)SDL_GetWindowID(windows[index]));
  }

  if (windows) {
    SDL_free(windows);
  }

  return found;
}

func b32 window_is_valid(window_id id) {
  if (!window_id_is_valid(id)) {
    return 0;
  }

  return SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id)) != NULL;
}

func cstr8 window_get_title(window_id id) {
  SDL_Window* window_ptr = NULL;

  if (!window_id_is_valid(id)) {
    return NULL;
  }

  window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  if (window_ptr == NULL) {
    return NULL;
  }

  return SDL_GetWindowTitle(window_ptr);
}

func b32 window_get_display_id(window_id id, display_id* out_display_id) {
  SDL_Window* window_ptr = NULL;
  SDL_DisplayID native_display_id = 0;

  if (out_display_id) {
    *out_display_id = (display_id) {0};
  }

  if (!window_id_is_valid(id)) {
    return 0;
  }

  window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  if (window_ptr == NULL) {
    return 0;
  }

  native_display_id = SDL_GetDisplayForWindow(window_ptr);
  if (native_display_id == 0) {
    return 0;
  }

  if (out_display_id) {
    *out_display_id = display_from_native_id((u64)native_display_id);
  }

  return 1;
}
