// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/window.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 window_id_is_valid(window src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL;
}

func window window_from_native_id(up native_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (window)(up)native_id;
}

func up window_to_native_id(window src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (up)src;
}

func sz window_get_count(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);

  if (windows) {
    SDL_free(windows);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return count > 0 ? (sz)count : 0;
}

func b32 window_get_id(sz idx, window* out_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);
  b32 found = windows != NULL && idx < (sz)count && windows[idx] != NULL;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = window_from_native_id((up)SDL_GetWindowID(windows[idx]));
  }

  if (windows) {
    SDL_free(windows);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

func b32 window_is_valid(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!window_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id)) != NULL;
}

func cstr8 window_get_title(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = NULL;

  if (!window_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetWindowTitle(window_ptr);
}

func b32 window_get_display_id(window id, display* out_display_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = NULL;
  SDL_DisplayID native_display_id = 0;

  if (out_display_id) {
    *out_display_id = NULL;
  }

  if (!window_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  native_display_id = SDL_GetDisplayForWindow(window_ptr);
  if (native_display_id == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (out_display_id) {
    *out_display_id = display_from_native_id((up)native_display_id);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}
